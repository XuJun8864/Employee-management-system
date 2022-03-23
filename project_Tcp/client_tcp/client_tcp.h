#ifndef __CLIENT_TCP_H__
#define __CLIENT_TCP_H__

#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <poll.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#define ERR_MSG(msg) do{\
	fprintf(stderr,"__%d__",__LINE__);\
	perror(msg);\
}while(0)
#define IP "192.168.1.123"
#define PORT 7788
typedef struct node{
	char flag_1;			/*登录或注册*/
	char flag_2;			/*增删改查*/
	char flag_3; 			/*0失败，1成功*/
	char flag_4; 			/*1-8代表修改的项*/
	char flag_5;			/*0代表没查到，1代表查到*/
	char power; 			/*1代表普通用户，*代表管理员*/
	char name_number[16];	/*账号名*/
	char password[16];		/*密码*/
	char ad_password[16];	/*管理员密码*/
	char buf[32];			/*发送信息*/
	char job_number[16];	/*工号*/
	char name[16];			/*名字*/
	char sex[8];			/*性别*/
	char age[8];			/*年龄*/
	char address[32];		/*地址*/
	char phone[16];			/*电话*/
	char department[32];	/*部门*/
	char wages[16];			/*工资*/
}info_t;
int register_(info_t *info); 				/*注册*/
int log_on(info_t *info);					/*登录*/
int interface(info_t *info,int sockfd);	 	/*界面功能1*/
int interface_2(info_t *info,int sockfd);	/*界面功能2*/
int increase(info_t *info,int sockfd);		/*增加员工信息*/
int delete(info_t *info,int sockfd);		/*删除员工信息*/
int modify(info_t *info,int sockfd);		/*修改员工信息*/
int query(info_t *info,int sockfd);			/*查询员工信息*/

#endif
