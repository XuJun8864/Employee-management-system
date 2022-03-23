#include "./server_tcp.h"
int main(int argc, const char *argv[])
{	
	int sockfd,optval = 1,newfd,epfd,ret,i;
	char buf[32] = {0};
	ssize_t res;
	struct epoll_event event;
	struct epoll_event events[MAXEVENTS];
	info_t info;
	/*创建并打开数据库*/
	sqlite3 *db = NULL;
	if(sqlite3_open("./my.db",&db) != SQLITE_OK){
		printf("打开数据库失败\n");
		ERR_MSG("sqlite3_open");
		return -1;
	}
	printf("打开数据库\n");
	/*创建管理员密码表*/
	char sql[256] = "create table if not exists Administrator_password(password char primary key)";
	char *errmsg = NULL;
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK){
		fprintf(stderr,"__%d__ %s\n",__LINE__,errmsg);
		return -1;
	}
	/*创建账号密码表*/
	bzero(sql,sizeof(sql));
	strcpy(sql,"create table if not exists account_number (name_number char primary key,password char,power char)");
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK){
		fprintf(stderr,"__%d__ %s\n",__LINE__,errmsg);
		return -1;
	}
	printf("创建账号密码表\n");
	/*创建员工信息表*/
	bzero(sql,sizeof(sql));
	strcpy(sql,"create table if not exists Employee_information(job_number char primary \
		key,name char,sex char,age char,address char,phone char,department char,wages char)");
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK){
		fprintf(stderr,"__%d__ %s\n",__LINE__,errmsg);
		return -1;
	}
	printf("创建员工信息表\n");
	/*填充地址信息结构体*/
	struct sockaddr_in sin = {
		.sin_family = AF_INET,
		.sin_port = htons(PORT),
		.sin_addr.s_addr = inet_addr(IP),
	};
	/*储存客户端地址信息结构体*/
	struct sockaddr_in cin;
	socklen_t addrlen = sizeof(cin);
	/*创建流式套接字*/
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0){
		ERR_MSG("socket");
		return -1;
	}
	/*允许端口快速重用*/
	if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval))<0){
		ERR_MSG("setsockopt");
		return -1;
	}
	/*绑定*/
	if(bind(sockfd,(struct sockaddr *)&sin,sizeof(sin))<0){
		ERR_MSG("bind");
		return -1;
	}
	/*被动监听*/
	if(listen(sockfd,BACKLOG)<0){
		ERR_MSG("listen");
		return -1;
	}
	/*创建一个epoll*/
	epfd = epoll_create(10);
	if(epfd<0){
		ERR_MSG("epoll_create");
		return -1;
	}
	/*将0号和sockfd添加到epoll表中*/
	event.events = EPOLLIN;
	event.data.fd = sockfd;
	if(epoll_ctl(epfd,EPOLL_CTL_ADD,sockfd,&event)<0){
		ERR_MSG("epoll_ctl");
		return -1;
	}
	event.data.fd = 0;
	if(epoll_ctl(epfd,EPOLL_CTL_ADD,0,&event)<0){
		ERR_MSG("epoll_ctl");
		return -1;
	}
	printf("员工管理系统服务器启动\n");
	while(1){
		/*等待准备好的文件描述符事件*/
		ret = epoll_wait(epfd,events,MAXEVENTS,-1);
		if(ret < 0){
			ERR_MSG("epoll_wait");
			return -1;
		}
		for(i=0;i<ret;i++){
			if(events[i].data.fd == 0){
				printf("终端输入事件\n");
				bzero(buf,sizeof(buf));
				fgets(buf,sizeof(buf),stdin);
				buf[strlen(buf)-1] = '\0';
				printf("%s\n",buf);
			}else if(events[i].data.fd == sockfd){
				printf("客户端连接事件\n");
				/*阻塞，从队列接收客户端连接，返回新的套接字*/
				newfd = accept(sockfd,(struct sockaddr *)&cin,&addrlen);
				if(newfd < 0){
					ERR_MSG("accept");
					return -1;
				}
				/*将cin中的网络字节序转化为本地字节序*/
				printf("[%s:%d] newfd=%d 已连接\n",inet_ntoa(cin.sin_addr),ntohs(cin.sin_port),newfd);
				/*将newfd添加到epoll监听表中*/
				event.data.fd = newfd;
				if(epoll_ctl(epfd,EPOLL_CTL_ADD,newfd,&event)<0){
					ERR_MSG("epoll_ctl");
					return -1;
				}
			}else{
				printf("客户端交互事件\n");
				/*接收消息*/
				bzero(&info,sizeof(info));
				res = recv(events[i].data.fd,&info,sizeof(info),0);
				if(res < 0){
					ERR_MSG("recv");
					return -1;
				}else if(res == 0){
					printf("newfd = %d客户端断开\n",events[i].data.fd);
					/*删除epoll池中的文件描述符*/
					event.data.fd = events[i].data.fd;
					if(epoll_ctl(epfd,EPOLL_CTL_DEL,events[i].data.fd,&event)<0){
						ERR_MSG("epoll_ctl");
						return -1;
					}
					printf("删除newfd成功\n");
					close(events[i].data.fd);
				}else{
					printf("newfd = %d: name_number:%s\n",
							events[i].data.fd,info.name_number);
					switch(info.flag_1){
						case 'Z':	/*注册*/
							register_(db,&info,events[i].data.fd);
							break;
						case 'D':	/*登录*/
							/*登录失败或者没有登录*/
							if(info.flag_3 != '1')
								log_on(db,&info,events[i].data.fd);
							else{
								switch(info.flag_2){
									case 'Z':increase(db,&info,events[i].data.fd);break;
									case 'S':delete(db,&info,events[i].data.fd);break;
									case 'G':modify(db,&info,events[i].data.fd);break;
									case 'C':query(db,&info,events[i].data.fd);break;
									default:printf("未知符号\n");break;
								}
							}
							break;
						default:
							printf("未知符号\n");
							break;
					}
				}
			}
		}
	}
	sqlite3_close(db);
	close(sockfd);
	return 0;
}
