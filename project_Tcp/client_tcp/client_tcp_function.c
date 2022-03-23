#include "./client_tcp.h"
/*界面功能1*/
int interface(info_t *info,int sockfd)
{
	char ch;
	ssize_t res;
	printf("----1.注册----\n");
	printf("----2.登录----\n");
	printf("----3.退出----\n");
	ch = getchar();
	while(getchar()	!= '\n');
	switch(ch){
		case '1':register_(info);break;
		case '2':log_on(info);break;
		case '3':exit(0);
		default:printf("无效字符\n");
	}
	/*发送信息*/
	res = send(sockfd,info,sizeof(*info),0);
	if(res < 0){
		ERR_MSG("send");
		return -1;
	}

	return 0;
}
/*界面功能2*/
int interface_2(info_t *info,int sockfd)
{
	char ch;
	if(info->power == '1'){
		printf("----1.查询员工信息----\n");
		printf("----2.退出----\n");
		ch = getchar();
		while(getchar() != '\n');
		switch(ch){
		case '1':
			query(info,sockfd);
			break;
		case '2':
			exit(0);
		default:
			printf("未知字符\n");
		}		
	}
	if(info->power == '*'){
		printf("----1.增加员工信息----\n");
		printf("----2.删除员工信息----\n");
		printf("----3.修改员工信息----\n");
		printf("----4.查询员工信息----\n");
		printf("----5.退出----\n");
		ch = getchar();
		while(getchar() != '\n');
		switch(ch){
		case '1':increase(info,sockfd);break;
		case '2':delete(info,sockfd);break;
		case '3':modify(info,sockfd);break;
		case '4':query(info,sockfd);break;
		case '5':exit(0);
		default:printf("未知字符\n");break;
		}
	}
	
	return 0;
}
/*注册*/
int register_(info_t *info)
{
	/*写入信息*/
	bzero(info,sizeof(*info));
	info->flag_1 = 'Z'; /*'Z'代表注册*/
	char ch;
	while(1){
		printf("请选择注册为普通员工(1) or 管理员(*)\n");
		ch = getchar();
		while(getchar()!='\n');
		switch(ch){
		case '1':
			info->power = '1';
			printf("请输入你的名字：");
			fgets(info->name_number,sizeof(info->name_number),stdin);
			info->name_number[strlen(info->name_number)-1] = '\0';
			printf("请输入你的密码：");
			fgets(info->password,sizeof(info->password),stdin);
			info->password[strlen(info->password)-1] = '\0';
			goto END1;
		case '*':
			info->power = '*';
			printf("请输入你的名字：");
			fgets(info->name_number,sizeof(info->name_number),stdin);
			info->name_number[strlen(info->name_number)-1] = '\0';
			printf("请输入你的密码：");
			fgets(info->password,sizeof(info->password),stdin);
			info->password[strlen(info->password)-1] = '\0';
			printf("请输入管理员密码：");
			fgets(info->ad_password,sizeof(info->ad_password),stdin);
			info->ad_password[strlen(info->ad_password)-1] = '\0';
			goto END1;
		default:
			printf("无效字符\n");
			break;
		}
	}
END1:
	return 0;
}
/*登录*/
int log_on(info_t *info)
{
	/*写入信息*/
	bzero(info,sizeof(*info));
	info->flag_1 = 'D'; /*'D'代表登录*/
	printf("请输入你的名字：");
	fgets(info->name_number,sizeof(info->name_number),stdin);
	info->name_number[strlen(info->name_number)-1] = '\0';
	printf("请输入你的密码：");
	fgets(info->password,sizeof(info->password),stdin);
	info->password[strlen(info->password)-1] = '\0';

	return 0;
}
/*增加员工信息*/
int increase(info_t *info,int sockfd)
{
	info->flag_2 = 'Z';
	printf("请输入员工工号:\n");
	fgets(info->job_number,sizeof(info->job_number),stdin);
	info->job_number[strlen(info->job_number)-1] = '\0';
	printf("请输入员工姓名:\n");
	fgets(info->name,sizeof(info->name),stdin);
	info->name[strlen(info->name)-1] = '\0';
	printf("请输入员工性别:\n");
	fgets(info->sex,sizeof(info->sex),stdin);
	info->sex[strlen(info->sex)-1] = '\0';
	printf("请输入员工年龄:\n");
	fgets(info->age,sizeof(info->age),stdin);
	info->age[strlen(info->age)-1] = '\0';
	printf("请输入员工地址:\n");
	fgets(info->address,sizeof(info->address),stdin);
	info->address[strlen(info->address)-1] = '\0';
	printf("请输入员工电话:\n");
	fgets(info->phone,sizeof(info->phone),stdin);
	info->phone[strlen(info->phone)-1] = '\0';
	printf("请输入员工部门:\n");
	fgets(info->department,sizeof(info->department),stdin);
	info->department[strlen(info->department)-1] = '\0';
	printf("请输入员工工资:\n");
	fgets(info->wages,sizeof(info->wages),stdin);
	info->wages[strlen(info->wages)-1] = '\0';
	/*发送信息*/
	if(send(sockfd,info,sizeof(*info),0) < 0){
		ERR_MSG("send");
		return -1;
	}

	return 0;
}
/*删除员工信息*/
int delete(info_t *info,int sockfd)
{
	info->flag_2 = 'S';
	printf("请输入要删除的员工工号\n");
	bzero(info->buf,sizeof(info->buf));
	fgets(info->buf,sizeof(info->buf),stdin);
	info->buf[strlen(info->buf)-1] = '\0';
	/*发送信息*/
	if(send(sockfd,info,sizeof(*info),0) < 0){
		ERR_MSG("send");
		return -1;
	}

	return 0;
}
/*修改员工信息*/
int modify(info_t *info,int sockfd)
{
	char ch;
	info->flag_2 = 'G';
	printf("请输入要修改的员工工号\n");
	bzero(info->buf,sizeof(info->buf));
	fgets(info->buf,sizeof(info->buf),stdin);
	info->buf[strlen(info->buf)-1] = '\0';
	printf("请输入要修改哪一项\n");
	printf("----1.修改工号----\n");
	printf("----2.修改姓名----\n");
	printf("----3.修改性别----\n");
	printf("----4.修改年龄----\n");
	printf("----5.修改地址----\n");
	printf("----6.修改电话----\n");
	printf("----7.修改部门----\n");
	printf("----8.修改工资----\n");
	ch = getchar();
	while(getchar() != '\n');
	switch(ch)
	{
		case '1':
			info->flag_4 = '1';
			printf("请输入修改后的员工工号\n");
			bzero(info->job_number,sizeof(info->job_number));
			fgets(info->job_number,sizeof(info->job_number),stdin);
			info->job_number[strlen(info->job_number)-1] = '\0';
			break;
		case '2':
			info->flag_4 = '2';
			printf("请输入修改后的员工姓名\n");
			bzero(info->name,sizeof(info->name));
			fgets(info->name,sizeof(info->name),stdin);
			info->name[strlen(info->name)-1] = '\0';
			break;
		case '3':
			info->flag_4 = '3';
			printf("请输入修改后的员工性别\n");
			bzero(info->sex,sizeof(info->sex));
			fgets(info->sex,sizeof(info->sex),stdin);
			info->sex[strlen(info->sex)-1] = '\0';
			break;
		case '4':
			info->flag_4 = '4';
			printf("请输入修改后的员工年龄\n");
			bzero(info->age,sizeof(info->age));
			fgets(info->age,sizeof(info->age),stdin);
			info->age[strlen(info->age)-1] = '\0';
			break;
		case '5':
			info->flag_4 = '5';
			printf("请输入修改后的员工地址\n");
			bzero(info->address,sizeof(info->address));
			fgets(info->address,sizeof(info->address),stdin);
			info->address[strlen(info->address)-1] = '\0';
			break;
		case '6':
			info->flag_4 = '6';
			printf("请输入修改后的员工电话\n");
			bzero(info->phone,sizeof(info->phone));
			fgets(info->phone,sizeof(info->phone),stdin);
			info->phone[strlen(info->phone)-1] = '\0';
			break;
		case '7':
			info->flag_4 = '7';
			printf("请输入修改后的员工部门\n");
			bzero(info->department,sizeof(info->department));
			fgets(info->department,sizeof(info->department),stdin);
			info->department[strlen(info->department)-1] = '\0';
			break;
		case '8':
			info->flag_4 = '8';
			printf("请输入修改后的员工工资\n");
			bzero(info->wages,sizeof(info->wages));
			fgets(info->wages,sizeof(info->wages),stdin);
			info->wages[strlen(info->wages)-1] = '\0';
			break;
		default:
			printf("无效字符\n");
			break;
	}
	/*发送信息*/
	if(send(sockfd,info,sizeof(*info),0) < 0){
		ERR_MSG("send");
		return -1;
	}
	return 0;
}
/*查询*/
int query(info_t *info,int sockfd)
{
	info->flag_2 = 'C';
	printf("请输入要查询的员工工号:\n");
	bzero(info->buf,sizeof(info->buf));
	fgets(info->buf,sizeof(info->buf),stdin);
	info->buf[strlen(info->buf)-1] = '\0';
	/*发送信息*/
	if(send(sockfd,info,sizeof(*info),0) < 0){
		ERR_MSG("send");
		return -1;
	}
	return 0;
}
