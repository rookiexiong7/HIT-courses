/*
 *  linux/kernel/fork.c
 *
 *  (C) 1991  Linus Torvalds
 */

/*
 *  'fork.c' contains the help-routines for the 'fork' system call
 * (see also system_call.s), and some misc functions ('verify_area').
 * Fork is rather simple, once you get the hang of it, but the memory
 * management can be a bitch. See 'mm/mm.c': 'copy_page_tables()'
 */
#include <string.h>
#include <errno.h>

#include <linux/sched.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <asm/system.h>

extern void write_verify(unsigned long address);

long last_pid=0;

void verify_area(void * addr,int size)
{
	unsigned long start;

	start = (unsigned long) addr;
	size += start & 0xfff;
	start &= 0xfffff000;
	start += get_base(current->ldt[2]);
	while (size>0) {
		size -= 4096;
		write_verify(start);
		start += 4096;
	}
}

int copy_mem(int nr,struct task_struct * p)
{
	unsigned long old_data_base,new_data_base,data_limit;
	unsigned long old_code_base,new_code_base,code_limit;

	code_limit=get_limit(0x0f);
	data_limit=get_limit(0x17);
	old_code_base = get_base(current->ldt[1]);
	old_data_base = get_base(current->ldt[2]);
	if (old_data_base != old_code_base)
		panic("We don't support separate I&D");
	if (data_limit < code_limit)
		panic("Bad data_limit");
	new_data_base = new_code_base = nr * 0x4000000;
	p->start_code = new_code_base;
	set_base(p->ldt[1],new_code_base);
	set_base(p->ldt[2],new_data_base);
	if (copy_page_tables(old_data_base,new_data_base,data_limit)) {
		printk("free_page_tables: from copy_mem\n");
		free_page_tables(new_data_base,data_limit);
		return -ENOMEM;
	}
	return 0;
}

/*
 *  Ok, this is the main fork-routine. It copies the system process
 * information (task[nr]) and sets up the necessary registers. It
 * also copies the data segment in it's entirety.
 */
int copy_process(int nr,long ebp,long edi,long esi,long gs,long none,
		long ebx,long ecx,long edx,
		long fs,long es,long ds,
		long eip,long cs,long eflags,long esp,long ss)
{
	struct task_struct *p;
	int i;
	struct file *f;

	p = (struct task_struct *) get_free_page();
	if (!p)
		return -EAGAIN;
	task[nr] = p;
	
	// NOTE!: the following statement now work with gcc 4.3.2 now, and you
	// must compile _THIS_ memcpy without no -O of gcc.#ifndef GCC4_3
	*p = *current;	/* NOTE! this doesn't copy the supervisor stack */
	p->state = TASK_UNINTERRUPTIBLE;
	p->pid = last_pid;
	p->father = current->pid;
	p->counter = p->priority;
	p->signal = 0;
	p->alarm = 0;
	p->leader = 0;		/* process leadership doesn't inherit */
	p->utime = p->stime = 0;
	p->cutime = p->cstime = 0;
	p->start_time = jiffies;
	fprintk(3, "%ld\t%c\t%ld\n", last_pid, 'N', jiffies);
	p->tss.back_link = 0;
	p->tss.esp0 = PAGE_SIZE + (long) p;
	p->tss.ss0 = 0x10;
	p->tss.eip = eip;
	p->tss.eflags = eflags;
	p->tss.eax = 0;
	p->tss.ecx = ecx;
	p->tss.edx = edx;
	p->tss.ebx = ebx;
	p->tss.esp = esp;
	p->tss.ebp = ebp;
	p->tss.esi = esi;
	p->tss.edi = edi;
	p->tss.es = es & 0xffff;
	p->tss.cs = cs & 0xffff;
	p->tss.ss = ss & 0xffff;
	p->tss.ds = ds & 0xffff;
	p->tss.fs = fs & 0xffff;
	p->tss.gs = gs & 0xffff;
	p->tss.ldt = _LDT(nr);
	p->tss.trace_bitmap = 0x80000000;
	if (last_task_used_math == current)
		__asm__("clts ; fnsave %0"::"m" (p->tss.i387));
	if (copy_mem(nr,p)) {
		task[nr] = NULL;
		free_page((long) p);
		return -EAGAIN;
	}
	for (i=0; i<NR_OPEN;i++)
		if ((f=p->filp[i]))
			f->f_count++;
	if (current->pwd)
		current->pwd->i_count++;
	if (current->root)
		current->root->i_count++;
	if (current->executable)
		current->executable->i_count++;
	set_tss_desc(gdt+(nr<<1)+FIRST_TSS_ENTRY,&(p->tss));
	set_ldt_desc(gdt+(nr<<1)+FIRST_LDT_ENTRY,&(p->ldt));
	print_parent_child(current,p);
	p->state = TASK_RUNNING;	/* do this last, just in case */
	fprintk(3, "%ld\t%c\t%ld\n", last_pid, 'J', jiffies);
	return last_pid;
}

int find_empty_process(void)
{
	int i;

	repeat:
		if ((++last_pid)<0) last_pid=1;
		for(i=0 ; i<NR_TASKS ; i++)
			if (task[i] && task[i]->pid == last_pid) goto repeat;
	for(i=1 ; i<NR_TASKS ; i++)
		if (!task[i]){
			fprintk(4, "\n+-------------------------------------------------------+\n");
			fprintk(4, "fork: find a empty space %d in task\n", i);
			return i;
		}
	return -EAGAIN;
}

void print_parent_child(struct task_struct *parent, struct task_struct *child) {

	fprintk(4, "Parent Process:(hex)\tChild Process:(hex)\n");
	fprintk(4, "state:\t\t%lx\t\t%lx\n", parent->state, child->state);
	fprintk(4, "counter:\t%lx\t\t%lx\n", parent->counter, child->counter);
	fprintk(4, "priority:\t%lx\t\t%lx\n", parent->priority, child->priority);
	fprintk(4, "exit_code:\t%x\t\t%x\n", parent->exit_code, child->exit_code);
	fprintk(4, "start_code:\t%lx\t\t%lx\n", parent->start_code, child->start_code);
	fprintk(4, "end_code:\t%lx\t\t%lx\n", parent->end_code, child->end_code);
	fprintk(4, "end_data:\t%lx\t\t%lx\n", parent->end_data, child->end_data);
	fprintk(4, "brk:\t\t%lx\t\t%lx\n", parent->brk, child->brk);
	fprintk(4, "start_stack:\t%lx\t\t%lx\n", parent->start_stack, child->start_stack);
	fprintk(4, "pid:\t\t%lx\t\t%lx\n", parent->pid, child->pid);
	fprintk(4, "father:\t\t%lx\t\t%lx\n", parent->father, child->father);
	fprintk(4, "pgrp:\t\t%lx\t\t%lx\n", parent->pgrp, child->pgrp);
	fprintk(4, "session:\t%lx\t\t%lx\n", parent->session, child->session);
	fprintk(4, "leader:\t\t%lx\t\t%lx\n", parent->leader, child->leader);
	fprintk(4, "uid:\t\t%x\t\t%x\n", parent->uid, child->uid);
	fprintk(4, "euid:\t\t%x\t\t%x\n", parent->euid, child->euid);
	fprintk(4, "suid:\t\t%x\t\t%x\n", parent->suid, child->suid);
	fprintk(4, "gid:\t\t%x\t\t%x\n", parent->gid, child->gid);
	fprintk(4, "egid:\t\t%x\t\t%x\n", parent->egid, child->egid);
	fprintk(4, "sgid:\t\t%x\t\t%x\n", parent->sgid, child->sgid);
	fprintk(4, "alarm:\t\t%lx\t\t%lx\n", parent->alarm, child->alarm);
	fprintk(4, "utime:\t\t%lx\t\t%lx\n", parent->utime, child->utime);
	fprintk(4, "stime:\t\t%lx\t\t%lx\n", parent->stime, child->stime);
	fprintk(4, "cutime:\t\t%lx\t\t%lx\n", parent->cutime, child->cutime);
	fprintk(4, "cstime:\t\t%lx\t\t%lx\n", parent->cstime, child->cstime);
	fprintk(4, "start_time:\t%lx\t\t%lx\n", parent->start_time, child->start_time);
	fprintk(4, "used_math:\t%x\t\t%x\n", parent->used_math, child->used_math);
	fprintk(4, "tty:\t\t%d\t\t%d\n", parent->tty, child->tty);
	fprintk(4, "umask:\t\t%x\t\t%x\n", parent->umask, child->umask);
	fprintk(4, "pwd:\t\t%lx\t\t%lx\n", parent->pwd, child->pwd);
	fprintk(4, "root:\t\t%lx\t\t%lx\n", parent->root, child->root);
	fprintk(4, "executable:\t%lx\t%lx\n", parent->executable, child->executable);
	fprintk(4, "close_on_exec:\t%lx\t\t%lx\n", parent->close_on_exec, child->close_on_exec);
	for (int i = 0; i < 3; ++i) {
	    fprintk(4, "ldt[%d]( 0~31):\t%lx\t\t%lx\n", i, parent->ldt[i].a, child->ldt[i].a);
	    fprintk(4, "ldt[%d](32~63):\t%lx\t\t%lx\n", i, parent->ldt[i].b, child->ldt[i].b);
	}
	fprintk(4, "============================================================\n");
	fprintk(4, "tss.back_link:\t%lx\t\t%lx\n", parent->tss.back_link, child->tss.back_link);
	fprintk(4, "tss.esp0:\t%lx\t\t%lx\n", parent->tss.esp0, child->tss.esp0);
	fprintk(4, "tss.ss0:\t%lx\t\t%lx\n", parent->tss.ss0, child->tss.ss0);
	fprintk(4, "tss.esp1:\t%lx\t\t%lx\n", parent->tss.esp1, child->tss.esp1);
	fprintk(4, "tss.ss1:\t%lx\t\t%lx\n", parent->tss.ss1, child->tss.ss1);
	fprintk(4, "tss.esp2:\t%lx\t\t%lx\n", parent->tss.esp2, child->tss.esp2);
	fprintk(4, "tss.ss2:\t%lx\t\t%lx\n", parent->tss.ss2, child->tss.ss2);
	fprintk(4, "tss.cr3:\t%lx\t\t%lx\n", parent->tss.cr3, child->tss.cr3);
	fprintk(4, "tss.eip:\t%lx\t\t%lx\n", parent->tss.eip, child->tss.eip);
	fprintk(4, "tss.eflags:\t%lx\t\t%lx\n", parent->tss.eflags, child->tss.eflags);
	fprintk(4, "tss.eax:\t%lx\t\t%lx\n", parent->tss.eax, child->tss.eax);
	fprintk(4, "tss.ecx:\t%lx\t\t%lx\n", parent->tss.ecx, child->tss.ecx);
	fprintk(4, "tss.edx:\t%lx\t\t%lx\n", parent->tss.edx, child->tss.edx);
	fprintk(4, "tss.ebx:\t%lx\t\t%lx\n", parent->tss.ebx, child->tss.ebx);
	fprintk(4, "tss.esp:\t%lx\t\t%lx\n", parent->tss.esp, child->tss.esp);
	fprintk(4, "tss.ebp:\t%lx\t\t%lx\n", parent->tss.ebp, child->tss.ebp);
	fprintk(4, "tss.esi:\t%lx\t\t%lx\n", parent->tss.esi, child->tss.esi);
	fprintk(4, "tss.edi:\t%lx\t\t%lx\n", parent->tss.edi, child->tss.edi);
	fprintk(4, "tss.es:\t\t%lx\t\t%lx\n", parent->tss.es, child->tss.es);
	fprintk(4, "tss.cs:\t\t%lx\t\t%lx\n", parent->tss.cs, child->tss.cs);
	fprintk(4, "tss.ss:\t\t%lx\t\t%lx\n", parent->tss.ss, child->tss.ss);
	fprintk(4, "tss.ds:\t\t%lx\t\t%lx\n", parent->tss.ds, child->tss.ds);
	fprintk(4, "tss.fs:\t\t%lx\t\t%lx\n", parent->tss.fs, child->tss.fs);
	fprintk(4, "tss.gs:\t\t%lx\t\t%lx\n", parent->tss.gs, child->tss.gs);
	fprintk(4, "tss.ldt:\t%lx\t\t%lx\n", parent->tss.ldt, child->tss.ldt);
	fprintk(4, "tss.trace_bitmap:\t%lx\t\t%lx\n", parent->tss.trace_bitmap, child->tss.trace_bitmap);
	fprintk(4, "+-------------------------------------------------------+\n");

}
