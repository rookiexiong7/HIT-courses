/**
 * @author See Contributors.txt for code contributors and overview of BadgerDB.
 *
 * @section LICENSE
 * Copyright (c) 2012 Database Group, Computer Sciences Department, University of Wisconsin-Madison.
 */

#include <memory>
#include <iostream>
#include "buffer.h"
#include "exceptions/buffer_exceeded_exception.h"
#include "exceptions/page_not_pinned_exception.h"
#include "exceptions/page_pinned_exception.h"
#include "exceptions/bad_buffer_exception.h"
#include "exceptions/hash_not_found_exception.h"

namespace badgerdb { 

/**
 * Constructor of BufMgr class
 * @param bufs 	The number of buffers to be managed by BufMgr
 */
BufMgr::BufMgr(std::uint32_t bufs)
	: numBufs(bufs) { 
	
	bufDescTable = new BufDesc[bufs];	// 申请 bufs 个 BufDesc 对象的数组 bufDescTable

	for (FrameId i = 0; i < bufs; i++){
		bufDescTable[i].frameNo = i;	// 为每个帧初始化frameNo 为循环索引 i
		bufDescTable[i].valid = false;	// 为每个帧初始化 valid为 false，表示这些帧初始时都是无效的
	}

	bufPool = new Page[bufs]; 			// 实际的缓冲池,包含 bufs 个 Page 对象的数组

	int htsize = ((((int) (bufs * 1.2)) * 2) / 2) + 1;	// 计算哈希表的大小 htsize
	hashTable = new BufHashTbl (htsize); 				// allocate the buffer hash table

	clockHand = bufs - 1; 				// 初始化时钟算法的 clockHand为缓冲池中的最后一个帧的索引
}

/**
 * Destructor of BufMgr class
 */
BufMgr::~BufMgr() {
		for (FrameId i = 0; i < numBufs; i++) { 
			// 将缓冲池中的所有脏页写回磁盘
			if (bufDescTable[i].dirty==1)
				bufDescTable[i].file->writePage(bufPool[i]);
		}
		delete[] bufPool; 		// 释放缓冲池
		delete[] bufDescTable;	// 释放bufDescTable
		delete hashTable; 		// 释放哈希表
}

/**
 * Advance clock to next frame in the buffer pool
 */
void BufMgr::advanceClock()
{
	clockHand = (clockHand + 1) % numBufs;	//顺时针旋转表针，指向下一个页框
}


/**
 * Allocate a free frame.  
 *
 * @param frame		Frame reference, frame ID of allocated frame returned via this variable
 * @throws 		BufferExceededException If no such buffer is found which can be allocated
 */
void BufMgr::allocBuf(FrameId & frame) 
{
		std::uint32_t pincount = 0;
		while(pincount<numBufs)
		{
			advanceClock();
			// 此页框无效, 通过frame返回该页框号
			if (!bufDescTable[clockHand].valid){
				frame = clockHand; 
				return ; 
			}
			// 若页框有效
			else {
				//若引用位为1, refbit置零
				if(bufDescTable[clockHand].refbit){
					bufDescTable[clockHand].refbit=false; 
					continue; 
				}
				else 
				{
					//若引用位为0, pinCnt>0, 则被固定页面数+1
					if(bufDescTable[clockHand].pinCnt>0){
						pincount++; 
						continue;
					}
					//若引用位为0, pinCnt=0, 引用位为0, 页脏, 则将该页写回磁盘, 并将dirty位置0
					else if(bufDescTable[clockHand].dirty){
							bufDescTable[clockHand].dirty = false; 
							bufDescTable[clockHand].file->writePage(bufPool[clockHand]);
					}
				
					try{
						hashTable->remove(bufDescTable[clockHand].file,bufDescTable[clockHand].pageNo);	// 从哈希表中移除当前缓冲帧对应的页
						bufDescTable[clockHand].Clear();		// 初始化当前缓冲帧
						frame = clockHand;						// 将当前帧号通过 frame 参数返回
					}
					
					// 处理哈希表未找到异常，如果有需要的话
					catch(HashNotFoundException e){
					
					}
						return;
					}

				}
			}
			// 如果 pincount 超过了 numBufs，说明所有的页框都被固定住了, 抛出缓冲区超出限制异常
			if(pincount >= numBufs) 
				throw BufferExceededException();
}



/**
 * Reads the given page from the file into a frame and returns the pointer to page.
 * If the requested page is already present in the buffer pool pointer to that frame is returned
 * otherwise a new frame is allocated from the buffer pool for reading the page.
 *
 * @param file   	File object
 * @param PageNo  	Page number in the file to be read
 * @param page  	Reference to page pointer. Used to fetch the Page object in which requested page from file is read in.
 */
void BufMgr::readPage(File* file, const PageId pageNo, Page*& page)
{
		FrameId frame;
		try{
			hashTable->lookup(file, pageNo, frame);
			bufDescTable[frame].refbit = true;
			bufDescTable[frame].pinCnt++;
		}
		// 页面不在缓冲池
		catch(HashNotFoundException&){ 
			allocBuf(frame);  				// 分配一个新的空闲页框
			bufPool[frame] = file->readPage(pageNo);  	// 从磁盘读入到这个页框
			hashTable->insert(file, pageNo, frame);  	// 该页面插入哈希表
			bufDescTable[frame].Set(file, pageNo);  	// 设置页框状态
		}
		page = bufPool + frame; 				// 通过page返回指向该页框的指针
}

/**
 * Unpin a page from memory since it is no longer required for it to remain in memory.
 *
 * @param file		File object
 * @param PageNo	Page number
 * @param dirty		True if the page to be unpinned needs to be marked dirty	
 * @throws  		PageNotPinnedException If the page is not already pinned
 */
void BufMgr::unPinPage(File* file, const PageId pageNo, const bool dirty) 
{
		FrameId frame;
		try
		{
			hashTable->lookup(file, pageNo, frame);		// 找到(file,PageNo)所在页框
			if (!bufDescTable[frame].pinCnt)
				throw PageNotPinnedException(file->filename(),pageNo, frame);
			bufDescTable[frame].pinCnt--;  				// 表示页面所在的页框的pinCnt减一
			if (dirty)
				bufDescTable[frame].dirty = dirty;  	// 将页框的dirty设置true
		}
		catch(HashNotFoundException&){
		
		}
}

/**
 * Writes out all dirty pages of the file to disk.
 * All the frames assigned to the file need to be unpinned from buffer pool before this function can be successfully called.
 * Otherwise Error returned.
 *
 * @param file 	File object
 * @throws 	PagePinnedException If any page of the file is pinned in the buffer pool 
 * @throws 	BadBufferException If any frame allocated to the file is found to be invalid
 */
void BufMgr::flushFile(const File* file) 
{
		for (uint32_t i = 0; i < numBufs; ++i){
			if (bufDescTable[i].file == file)  // 属于file的页面
			{
				if (!bufDescTable[i].valid)  // 无效页
					throw BadBufferException(i, bufDescTable[i].dirty,bufDescTable[i].valid, bufDescTable[i].refbit);
				if (bufDescTable[i].pinCnt)  // 被固定
					throw PagePinnedException(file->filename(),bufDescTable[i].pageNo, i);
				if (bufDescTable[i].dirty)  // 如果页面是脏的，则写回磁盘
				{
					bufDescTable[i].file->writePage(bufPool[i]);
					bufDescTable[i].dirty = false;
				}
				hashTable->remove(file, bufDescTable[i].pageNo);
				bufDescTable[i].Clear();
			}
		}
}

/**
 * Allocates a new, empty page in the file and returns the Page object.
 * The newly allocated page is also assigned a frame in the buffer pool.
 *
 * @param file   	File object
 * @param PageNo  	Page number. The number assigned to the page in the file is returned via this reference.
 * @param page  	Reference to page pointer. The newly allocated in-memory Page object is returned via this reference.
 */
void BufMgr::allocPage(File* file, PageId &pageNo, Page*& page) 
{
		FrameId frame;
		allocBuf(frame);  				// 分配一个新的页框
		bufPool[frame] = file->allocatePage(); 		// 返回一个空闲页面
		pageNo = bufPool[frame].page_number(); 		
		hashTable->insert(file, pageNo, frame); 	// 哈希表中插入该页面
		bufDescTable[frame].Set(file, pageNo);  	// 设置页框状态
		page = bufPool + frame; 		 	// 通过page参数返回指向缓冲池中包含该页面的页框的指针
	
}

/**
 * Delete page from file and also from buffer pool if present.
 * Since the page is entirely deleted from file, its unnecessary to see if the page is dirty.
 *
 * @param file   	File object
 * @param PageNo  	Page number
 */
void BufMgr::disposePage(File* file, const PageId PageNo)
{
		FrameId frame;
		try {
			hashTable->lookup(file, PageNo, frame); 	// 如果页面在缓冲池中
			hashTable->remove(file, PageNo);  			// 将页框清空并从哈希表中删除页面
			bufDescTable[frame].Clear();  
		}
		catch(HashNotFoundException&){
		
		}
		file->deletePage(PageNo);  					    // 删除页号为PageNo的页面
}

/**
 * Print member variable values. 
 */
void BufMgr::printSelf(void) 
{
	BufDesc* tmpbuf;
	int validFrames = 0;

	for (std::uint32_t i = 0; i < numBufs; i++) {
		tmpbuf = &(bufDescTable[i]);
		std::cout << "FrameNo:" << i << " ";
		tmpbuf->Print();

		if (tmpbuf->valid == true)
			validFrames++;
	}

	std::cout << "Total Number of Valid Frames:" << validFrames << "\n";
}

}

