#include "./server_tcp.h"
/*注册*/
int register_(sqlite3 *db,info_t *info,int newfd)
{
	char sql[128] = {0};
	char *errmsg = NULL;
	char **pazResult = NULL;
	int pnRow,pnColumn;
	switch(info->power){
		case '1':
			/*注册成为普通员工*/
			sprintf(sql,"insert into account_number values(\"%s\",\"%s\",\"%c\")",\
					info->name_number,info->password,info->power);
			if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK){
				fprintf(stderr,"__%d__ %s\n",__LINE__,errmsg);
				return -1;
			}
			/*发送信息给客户端*/
			bzero(info,sizeof(*info));
			info->flag_1 = 'Z';
			info->flag_3 = '1';
			strcpy(info->buf,"注册成功");
			if(send(newfd,info,sizeof(*info),0) < 0){
				ERR_MSG("send");
				return -1;
			}
			break;
		case '*':
			/*判断管理员密码，查询管理员密码表*/
			bzero(sql,sizeof(sql));
			sprintf(sql,"select * from Administrator_password where password=\"%s\"",info->ad_password);
			if(sqlite3_get_table(db,sql,&pazResult,&pnRow,&pnColumn,&errmsg) != SQLITE_OK){
				fprintf(stderr,"__%d__ %s\n",__LINE__,errmsg);
				return -1;
			}
			if(pnRow != 0){
				printf("密码正确\n");
				/*注册成为管理员*/
				bzero(sql,sizeof(sql));
				sprintf(sql,"insert into account_number values(\"%s\",\"%s\",\"%c\")",\
						info->name_number,info->password,info->power);
				if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK){
					fprintf(stderr,"__%d__ %s\n",__LINE__,errmsg);
					return -1;
				}
				/*发送信息给客户端*/
				bzero(info,sizeof(*info));
				info->flag_1 = 'Z';
				info->flag_3 = '1';
				strcpy(info->buf,"注册成功");
				if(send(newfd,info,sizeof(*info),0) < 0){
					ERR_MSG("send");
					return -1;
				}
			}else{
				printf("密码错误\n");
				/*发送信息给客户端*/
				bzero(info,sizeof(*info));
				info->flag_1 = 'Z';
				info->flag_3 = '0';
				strcpy(info->buf,"注册失败,管理员密码错误");
				if(send(newfd,info,sizeof(*info),0) < 0){
					ERR_MSG("send");
					return -1;
				}
			}
			/*释放查询结果*/
			sqlite3_free_table(pazResult);
			break;
	}
	return 0;
}
/*登录*/
int log_on(sqlite3 *db,info_t *info,int newfd)
{
	char sql[128] = {0};
	char *errmsg = NULL;
	char **pazResult = NULL;
	int pnRow,pnColumn;	
	/*查询账号表确定账号密码*/
	sprintf(sql,"select * from account_number where name_number=\"%s\" and \
			password=\"%s\"",info->name_number,info->password);
	if(sqlite3_get_table(db,sql,&pazResult,&pnRow,&pnColumn,&errmsg) != SQLITE_OK){
		fprintf(stderr,"__%d__ %s\n",__LINE__,errmsg);
		return -1;
	}
	if(pnRow != 0){
		printf("账号密码正确\n");
		/*发送信息给客户端*/
		bzero(info->buf,sizeof(info->buf));
		/*拿出权限标志*/
		info->power = *pazResult[5];
		info->flag_1 = 'D';
		info->flag_3 = '1';
		strcpy(info->buf,"登录成功");
		if(send(newfd,info,sizeof(*info),0) < 0){
			ERR_MSG("send");
			return -1;
		}
	}else{
		/*发送信息给客户端*/
		bzero(info,sizeof(*info));
		info->flag_1 = 'D';
		info->flag_3 = '0';
		strcpy(info->buf,"登录失败,账户名或密码错误");
		if(send(newfd,info,sizeof(*info),0) < 0){
			ERR_MSG("send");
			return -1;
		}
	}
	return 0;
}
/*增*/
int increase(sqlite3 *db,info_t *info,int newfd)
{
	char sql[256] = {0};
	char *errmsg = NULL;
	sprintf(sql,"insert into Employee_information values(\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\")",\
			info->job_number,info->name,info->sex,info->age,info->address,info->phone,info->department,info->wages);
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK){
		fprintf(stderr,"__%d__ %s\n",__LINE__,errmsg);
		return -1;
	}
	printf("增加成功\n");
	/*发送信息给客户端*/
	bzero(info->buf,sizeof(info->buf));
	strcpy(info->buf,"增加成功");
	if(send(newfd,info,sizeof(*info),0) < 0){
		ERR_MSG("send");
		return -1;
	}

	return 0;
}
/*删*/
int delete(sqlite3 *db,info_t *info,int newfd)
{
	char sql[128] = {0};
	char *errmsg = NULL;
	sprintf(sql,"delete from Employee_information where job_number=\"%s\"",info->job_number);
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK){
		fprintf(stderr,"__%d__ %s\n",__LINE__,errmsg);
		return -1;
	}
	printf("删除成功\n");
	/*发送信息给客户端*/
	bzero(info->buf,sizeof(info->buf));
	strcpy(info->buf,"删除成功");
	if(send(newfd,info,sizeof(*info),0) < 0){
		ERR_MSG("send");
		return -1;
	}
	return 0;
}
/*改*/
int modify(sqlite3 *db,info_t *info,int newfd)
{
	char sql[128] = {0};
	char *errmsg = NULL;
	switch(info->flag_4){
	case '1':
		sprintf(sql,"update Employee_information set job_number=\"%s\" where job_number=\"%s\"",info->job_number,info->buf);
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK){
			fprintf(stderr,"__%d__ %s\n",__LINE__,errmsg);
			return -1;
		}
		break;
	case '2':
		sprintf(sql,"update Employee_information set name=\"%s\" where job_number=\"%s\"",info->name,info->buf);
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK){
			fprintf(stderr,"__%d__ %s\n",__LINE__,errmsg);
			return -1;
		}
		break;
	case '3':
		sprintf(sql,"update Employee_information set sex=\"%s\" where job_number=\"%s\"",info->sex,info->buf);
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK){
			fprintf(stderr,"__%d__ %s\n",__LINE__,errmsg);
			return -1;
		}
		break;
	case '4':
		sprintf(sql,"update Employee_information set age=\"%s\" where job_number=\"%s\"",info->age,info->buf);
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK){
			fprintf(stderr,"__%d__ %s\n",__LINE__,errmsg);
			return -1;
		}
		break;
	case '5':
		sprintf(sql,"update Employee_information set address=\"%s\" where job_number=\"%s\"",info->address,info->buf);
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK){
			fprintf(stderr,"__%d__ %s\n",__LINE__,errmsg);
			return -1;
		}
		break;
	case '6':
		sprintf(sql,"update Employee_information set phone=\"%s\" where job_number=\"%s\"",info->phone,info->buf);
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK){
			fprintf(stderr,"__%d__ %s\n",__LINE__,errmsg);
			return -1;
		}
		break;
	case '7':
		sprintf(sql,"update Employee_information set department=\"%s\" where job_number=\"%s\"",info->department,info->buf);
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK){
			fprintf(stderr,"__%d__ %s\n",__LINE__,errmsg);
			return -1;
		}
		break;
	case '8':
		sprintf(sql,"update Employee_information set wages=\"%s\" where job_number=\"%s\"",info->wages,info->buf);
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK){
			fprintf(stderr,"__%d__ %s\n",__LINE__,errmsg);
			return -1;
		}
		break;
	default:
		printf("未知字符\n");
		break;
	}
	printf("修改成功\n");
	/*发送信息给客户端*/
	bzero(info->buf,sizeof(info->buf));
	strcpy(info->buf,"修改成功");
	if(send(newfd,info,sizeof(*info),0) < 0){
		ERR_MSG("send");
		return -1;
	}

	return 0;
}
/*查询*/
int query(sqlite3 *db,info_t *info,int newfd)
{
	char sql[128] = {0};
	char **pazResult = NULL;
	char *errmsg = NULL;
	int pnRow = 0,pnColumn = 0;
	int i = 8;
	sprintf(sql,"select * from Employee_information where job_number=\"%s\"",info->buf);
	if(sqlite3_get_table(db,sql,&pazResult,&pnRow,&pnColumn,&errmsg) != SQLITE_OK){
		fprintf(stderr,"__%d__ %s\n",__LINE__,errmsg);
		return -1;
	}
	if(pnRow == 0){
		printf("查无此人\n");
		/*发送*/
		bzero(info->buf,sizeof(info->buf));
		strcpy(info->buf,"查无此人");
		if(send(newfd,info,sizeof(*info),0) < 0){
			ERR_MSG("send");
			return -1;
		}
	}else{
		bzero(info->job_number,sizeof(info->job_number));
		strcpy(info->job_number,pazResult[i++]);
		bzero(info->name,sizeof(info->name));
		strcpy(info->name,pazResult[i++]);
		bzero(info->sex,sizeof(info->sex));
		strcpy(info->sex,pazResult[i++]);
		bzero(info->age,sizeof(info->age));
		strcpy(info->age,pazResult[i++]);
		bzero(info->address,sizeof(info->address));
		strcpy(info->address,pazResult[i++]);
		bzero(info->phone,sizeof(info->phone));
		strcpy(info->phone,pazResult[i++]);
		bzero(info->department,sizeof(info->department));
		strcpy(info->department,pazResult[i++]);
		bzero(info->wages,sizeof(info->wages));
		strcpy(info->wages,pazResult[i++]);
		/*发送*/
		info->flag_5 = '1';
		strcpy(info->buf,"已查到");
		if(send(newfd,info,sizeof(*info),0) < 0){
			ERR_MSG("send");
			return -1;
		}
	}
	sqlite3_free_table(pazResult);
	return 0;
}
