#ifndef __OS_H__
#define __OS_H__

#include <HC_DEFINE.H>
/** 任务函数类型说明 */
typedef void (*taskfun)(void);

/** 任务结构体 */
struct task{
	s8 sta;			///<任务块状态
	u16 timer;	///<剩余时间
	taskfun fun; ///<任务函数
};

void os_init(void);
u8 os_task_add(u16 time,taskfun fun);
u8 os_task_remark(void);
u8 os_task_process(void);
u8 os_task_update_time(taskfun fun,u16 time);
u8 os_task_delete(taskfun fun);
#endif