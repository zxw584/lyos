/*  This file is part of Lyos.

    Lyos is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Lyos is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Lyos.  If not, see <http://www.gnu.org/licenses/>. */

#include <signal.h>
#include <lyos/list.h>
#include "stackframe.h"
#include "page.h"
#include <lyos/spinlock.h>
#include <lyos/endpoint.h>

/* Process State */
#define PST_BOOTINHIBIT   0x01 	/* this proc is not runnable until SERVMAN has made it */
#define PST_SENDING   0x02	/* set when proc trying to send */
#define PST_RECEIVING 0x04	/* set when proc trying to recv */
#define PST_PAGEFAULT 0x20  /* set when proc has encounted a page fault */
#define PST_MMINHIBIT 0x40  /* this proc is not runnable until MM has prepared mem regions for it */
#define PST_STOPPED   0x80 	/* set when proc is stopped */
#define PST_FREE_SLOT 0x100	/* set when proc table entry is not used
			 * (ok to allocated to a new process)
			 */

#define proc_addr(n)	(&proc_table[n])
			 
#define lock_proc(proc) spinlock_lock(&((proc)->lock))
#define unlock_proc(proc) spinlock_unlock(&((proc)->lock))

#define pst_is_runnable(pst) ((pst) == 0)
#define proc_is_runnable(proc) (pst_is_runnable((proc)->state))

#define PST_IS_SET(proc, pst) ((proc)->state & pst)
#define PST_SET(proc, pst)  do { \
			 					lock_proc(proc);	\
			 					int prev_pst = (proc)->state; \
			 					(proc)->state |= (pst); \
			 					if (pst_is_runnable(prev_pst) && !proc_is_runnable(proc))	\
			 						dequeue_proc(proc);	\
			 					unlock_proc(proc);	\
							} while(0)
#define PST_SETFLAGS(proc, flags)  do { \
			 					lock_proc(proc);	\
			 					int prev_pst = (proc)->state; \
			 					(proc)->state = (flags); \
			 					if (pst_is_runnable(prev_pst) && !proc_is_runnable(proc))	\
			 						dequeue_proc(proc);	\
			 					unlock_proc(proc);	\
							} while(0)
#define PST_UNSET(proc, pst)  do { \
								lock_proc(proc);	\
								int prev_pst = (proc)->state; \
			 					(proc)->state &= ~(pst); \
			 					if (!pst_is_runnable(prev_pst) && proc_is_runnable(proc))	\
			 						enqueue_proc(proc);	\
			 					unlock_proc(proc);	\
							} while(0)

#define PROC_NAME_LEN	16

struct proc {
	struct stackframe regs;    /* process registers saved in stack frame */
	struct segframe seg;

	spinlock_t lock;

	int priority;
    int counter;                 /* remained ticks */
    int quantum_ms;

    struct proc * next_ready;

	endpoint_t endpoint;
	char name[PROC_NAME_LEN];		   /* name of the process */

	int state;              /**
				    * process flags.
				    * A proc is runnable if state==0
				    */
	int cpu;			/* on which cpu this proc runs */

    sigset_t sig_pending;	/* signals to be handled */
	sigset_t sig_mask;
	struct sigaction sigaction[NSIG];
	unsigned long alarm;
	long blocked;
    
	MESSAGE * send_msg;
	MESSAGE * recv_msg;
	int recvfrom;
	int sendto;

	int special_msg;           /**
				    * nonzero if an INTERRUPT occurred when
				    * the task is not ready to deal with it.
				    */

	struct proc * q_sending;   /**
				    * queue of procs sending messages to
				    * this proc
				    */
	struct proc * next_sending;/**
				    * next proc in the sending
				    * queue (q_sending)
				    */

	int p_parent; /**< pid of parent process */

	/* user id,group id etc. */
	u16 uid, euid, suid;
	u16 gid, egid, sgid;
	
	struct inode * pwd;			/* working directory */
	struct inode * root;		/* root directory */

	int umask;

	int brk;

	struct file_desc * filp[NR_FILES];
};

struct task {
	task_f	initial_eip;
	int	stacksize;
	char	name[32];
};

#define proc2pid(x) (x - proc_table)

#define FIRST_PROC		proc_table[0]
#define LAST_PROC		proc_table[NR_TASKS + NR_PROCS - 1]

#define NR_HOLES		((NR_PROCS * 2) + 4)

/**
 * All forked proc will use memory above PROCS_BASE.
 *
 * @attention make sure PROCS_BASE is higher than any buffers, such as
 *            fsbuf, mmbuf, etc
 * @see global.c
 * @see global.h
 */
//#define	PROCS_BASE		0x1000000 /* 16 MB */
#define	PROC_ORIGIN_STACK	0x80000    /*  32 KB */

/* stacks of tasks */
#define	STACK_SIZE_DEFAULT	0x80000 /* 32 KB */
#define STACK_SIZE_TTY		STACK_SIZE_DEFAULT
#define STACK_SIZE_SYS		STACK_SIZE_DEFAULT
#define STACK_SIZE_FS		STACK_SIZE_DEFAULT
#define STACK_SIZE_SERVMAN	STACK_SIZE_DEFAULT
#define STACK_SIZE_MM		STACK_SIZE_DEFAULT
#define STACK_SIZE_RD		STACK_SIZE_DEFAULT
#define STACK_SIZE_INITFS	STACK_SIZE_DEFAULT
#define STACK_SIZE_INIT		STACK_SIZE_DEFAULT

#define STACK_SIZE_TOTAL	(STACK_SIZE_TTY + \
				STACK_SIZE_SYS + \
				STACK_SIZE_FS + \
				STACK_SIZE_SERVMAN + \
				STACK_SIZE_MM + \
				STACK_SIZE_RD + \
				STACK_SIZE_INIT)
