#ifndef __OS_H__
#define __OS_H__

#include <HC_DEFINE.H>
/** ����������˵�� */
typedef void (*taskfun)(void);

/** ����ṹ�� */
struct task{
	s8 sta;			///<�����״̬
	u16 timer;	///<ʣ��ʱ��
	taskfun fun; ///<������
};

void os_init(void);
u8 os_task_add(u16 time,taskfun fun);
u8 os_task_remark(void);
u8 os_task_process(void);
u8 os_task_update_time(taskfun fun,u16 time);
u8 os_task_delete(taskfun fun);
#endif