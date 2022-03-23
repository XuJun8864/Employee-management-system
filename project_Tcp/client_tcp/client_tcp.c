#include "./client_tcp.h"
int main(int argc, const char *argv[])
{
	int sockfd,ret;
	ssize_t res;
	info_t info;
	/*创建流式套接字*/
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0){
		ERR_MSG("socket");
		return -1;
	}
	/*填充地址信息结构体*/
	struct sockaddr_in sin = {
		.sin_family = AF_INET,
		.sin_port = htons(PORT),
		.sin_addr.s_addr = inet_addr(IP),
	};
	/*连接服务器*/
	if(connect(sockfd,(struct sockaddr *)&sin,sizeof(sin))<0){
		ERR_MSG("connect");
		return -1;
	}
	/*创建文件描述符集合*/
	struct pollfd fds;
	/*将sockfd放入集合中*/
	fds.fd = sockfd;
	fds.events = POLLIN;
	printf("员工管理系统客户端启动\n");
	/*界面1功能选择*/
	interface(&info,sockfd);
	while(1){
		/*阻塞监听*/
		ret = poll(&fds,1,-1);
		if(ret < 0){
			ERR_MSG("poll");
			return -1;
		}else if(0 == ret){
			printf("超时\n");
			break;
		}

		if(fds.revents & POLLIN){
			printf("sockfd事件\n");
			/*接收消息*/
			bzero(&info.buf,sizeof(info.buf));
			res = recv(sockfd,&info,sizeof(info),0);
			if(res < 0){
				ERR_MSG("recv");
				return -1;
			}else if(0 == res){
				printf("服务器关闭\n");
				break;
			}else{
				printf("%s\n",info.buf);
				/*查询成功*/
				if(info.flag_5 == '1'){
					/*判断标志位*/
					if(info.flag_2 == 'C'){
						/*根据权限打印员工信息*/
						if(info.power == '1'){
							printf("工号:%s 姓名:%s 电话:%s\n",info.job_number,info.name,info.phone);
						}else{
							printf("工号:%s 姓名:%s 性别:%s 年龄:%s 地址:%s 电话:%s 部门:%s 工资:%s\n",\
									info.job_number,info.name,info.sex,info.age,info.address,info.phone,info.department,info.wages);
						}
					}
				}
				switch(info.flag_1){
				case 'Z':
					/*调界面1*/
					interface(&info,sockfd);
					break;
				case 'D':
					switch(info.flag_3){
						case '0':		/*失败*/
							/*调界面1*/
							interface(&info,sockfd);
							break;
						case '1':		/*成功*/
							/*调界面2*/
							interface_2(&info,sockfd);
							break;
						default:
							break;
					}
					break;
				default:
					printf("未知字符\n");
					break;
				}
			}
		}
	}

	close(sockfd);
	return 0;
}
