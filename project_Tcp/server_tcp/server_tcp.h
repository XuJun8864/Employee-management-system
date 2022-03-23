#ifndef __SERVER_TCP_H__
#define __SERVER_TCP_H__

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <sqlite3.h>
#define ERR_MSG(msg) do{\
	fprintf(stderr,"__%d__",__LINE__);\
	perror(msg);\
}while(0)
#define IP "192.168.1.123"
#define PORT 7788
#define BACKLOG 10
#define MAXEVENTS 10
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
int register_(sqlite3 *db,info_t *info,int newfd);	/*注册*/
int log_on(sqlite3 *db,info_t *info,int newfd);		/*登录*/
int increase(sqlite3 *db,info_t *info,int newfd);	/*增加员工信息*/
int delete(sqlite3 *db,info_t *info,int newfd);		/*删除员工信息*/
int modify(sqlite3 *db,info_t *info,int newfd);		/*修改*/
int query(sqlite3 *db,info_t *info,int newfd);		/*查询*/

#endif
