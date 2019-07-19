/**
 * @file main.c
 * @brief 主函数文件
 * @mainpage Mesh SDK 开发文档
 * @author mudlife \n
 * @email mudlifez@wearlinks.com
 * @par Copyright (c):
 * 			上海晶曦微电子科技有限公司
 * @version 1.0.0
 * @date 2019-06-02
 * @details 该文档帮助客户快速进行二次开发 
 * @par 程序架构：
 * @code
 *  
 |		|		 	|		 |
 |		|	 APP	|		 |
 |		|___________|		 |
 |	 D	|		  	|		 |
 |		|	 NET	|	O	 |
 |		|___________|		 |
 |	 E	|			|		 |
 |		|	 MAC	|	S	 |
 |		|___________|		 |
 |	 V	|			|		 |
 |		|	 PHY	|		 |
 |		|        	|        |
 * @endcode
 * PHY,MAC,NET,OS层不对用户开放，用户主要在APP层进行开发 @see netstack_app.c  \n
 * netstack_app.c文件中有三个重要的函数，分别是 app_init,app_input,app_output. \n
 * app_init函数是应用层的初始化函数， \n
 * app_input函数由net层调用，用来处理指令， \n
 * app_output函数是应用层用来发送数据的。 \n
 */
#define ALLOCATE_EXTERN
#include "HC89S003F4.h"
#include "network_conf.h"
#include "netstack_app.h"

#include "system.h"
#include <string.h>
#include "os.h"
#include "spi.h"

/**
 * @brief 主函数
 *
 * @return 无
 */
void main()
{
	system_init(); //系统初始化
	spi_init();   //SPI初始化
	phy_init();  //物理层初始化
	os_init();    //OS初始化
	mac_init();  //MAC层初始化
	net_init();  //网络层初始化
	app_init();  //应用层初始化
	wdt_init();   //看门狗初始化
	
	while(1){
		WDTC |= 0x10;    //清狗
		mac_process();
		os_task_remark(); //任务更新
		os_task_process();//任务进程
		app_process();//应用层进程

	
	}
		
}

