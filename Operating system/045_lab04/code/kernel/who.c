#include <string.h>
#include <errno.h>
#include <asm/segment.h>

char msg[24];
int len=0;

/*
 * 将字符串参数 name 的内容拷贝到内核中保存下来
 * 要求 name 的长度不能超过 23 个字符。
 * 返回值是拷贝的字符数
 * 如果 name 的字符个数超过了 23 ，则返回 -1
 * 并置 errno 为 EINVAL 。
 */
int sys_iam(const char * name){
    char temp[24];
    int i;
    for(i = 0; i < 24; i++){
        temp[i] = get_fs_byte(name+i);
        if(temp[i] == '\0')
            break;
    }
    if(i == 24 && temp[i-1] != '\0')
        return -(EINVAL);
    else{
        len = i;
    	strcpy(msg, temp);
    	return i;
    }
}

/*
 * 将内核中由 iam() 保存的名字拷贝到 name 指向的用户地址空间中，
 * 同时确保不会对 name 越界访存（name 的大小由 size 说明）。
 * 返回值是拷贝的字符数。如果 size 小于需要的空间，则返回 -1
 * 并置 errno 为 EINVAL 。
 */
int sys_whoami(char* name, unsigned int size){
    if(len > size)
        return -(EINVAL);
    int i = 0;
    for(int i = 0; i < len; i++)
    	put_fs_byte(msg[i], name+i);
    return len;
}
