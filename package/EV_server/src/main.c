
#include  "main.h"
#include <sys/types.h>
#include <signal.h>
void sig_handler(int signo)
{
	printf("SIGPIPE  \n");

}

int sock_conn_server(struct sockaddr_in *cliaddr, struct connect_serv *conn) 
{
	int serv_fd;
 	serv_fd = sock_server_init(cliaddr, conn->IP, conn->Port);	
	if( !connect_retry(serv_fd, (const struct sockaddr *)cliaddr, sizeof(struct sockaddr_in)) ){
		close(serv_fd);
		printf("connect server connect fail...\n");
		return -1;
	}
	printf("连接connect server 成功...%d\n", serv_fd);
	return serv_fd;
}



int   main(int   argc,   char   *argv[]) 
{



	int sock_serv_fd;
	struct sockaddr_in   cliaddr;
	struct connect_serv   conn_serv;
	unsigned int cid = 1011;
	
	signal(SIGPIPE, sig_handler);

while(1)
{

	if(!protocol_init(cid)){
		printf("协议初始化失败...\n");
		continue;
	}
/*
	protocol_req_ready(&ConnectREQ, &present_info);
	{
		char buff[20] = {0};
		strncpy(buff, ConnectREQ.KEY, 6);
		printf("开始打印协议:\n");
		printf("key:%s\n", buff);
		printf("Length:%d\n", ConnectREQ.Length);
		printf("command:%#x\n", ConnectREQ.Command);
		printf("CID:%d%d%d%d\n",  ConnectREQ.CID[3], ConnectREQ.CID[2], ConnectREQ.CID[1], ConnectREQ.CID[0]);
		printf("IP:%d.%d.%d.%d\n", ConnectREQ.IP[0],ConnectREQ.IP[1],ConnectREQ.IP[2], ConnectREQ.IP[3]);
		printf("MAC:%02x-%02x-%02x-%02x-%02x-%02x\n", ConnectREQ.MAC[0], ConnectREQ.MAC[1], ConnectREQ.MAC[2], ConnectREQ.MAC[3], ConnectREQ.MAC[4],ConnectREQ.MAC[5]);
		printf("presentDate:20%d-days:%d\n",*(unsigned short *)ConnectREQ.PresentDate & 0x7f,  *(unsigned short *)ConnectREQ.PresentDate >> 7);
		printf("presentTime:%d-%d\n", ConnectREQ.PresentTime[0], ConnectREQ.PresentTime[1]);
		printf("model:%s\n", ConnectREQ.Model);
		strncpy(buff, ConnectREQ.Series, 10);
		printf("series:%s\n", buff);
		printf("joinVersion:%d.%d\n", ConnectREQ.JoinConnectVersion[0], ConnectREQ.JoinConnectVersion[1]);
		printf("chargerVersion:%d.%d\n", ConnectREQ.ChargerFWVersion[0], ConnectREQ.ChargerFWVersion[1]);
		printf("CRC:%d\n", *(u16 *)ConnectREQ.CRC16);

	}
*/

	sock_serv_fd = sock_server_init(&cliaddr, JOIN_SERVER1_IP, JOIN_SERVER_PORT);	

	if(  !connect_retry(sock_serv_fd, (const struct sockaddr *)&cliaddr, sizeof(cliaddr)) ){
		// 连接失败，然后连接join_server2_ip
		printf("jion server1 connect fail...\n");
		close(sock_serv_fd);
		if(!(sock_serv_fd = sock_server_init(&cliaddr, JOIN_SERVER2_IP, JOIN_SERVER_PORT))){
			close(sock_serv_fd);
		  	printf("join server2 connect fail...\n");
			continue;
		}
	}
	
	printf("客户端连接成功...\n"); 
	char sock_buff[1024] = {0};

	// 连接join server
	if( !join_conn_serv(sock_serv_fd, &conn_serv)){
		printf("与通信JOIN server 失败, 重新初始化...\n");
		sleep(5);
		continue;
	}

//	if(!join_comfirm_serv(sock_serv_fd, &conn_serv, CHARGER_READY, ONLINE)){
//		printf("发送JOIN SERVER确认连接失败...\n");
//		exit(1);
//	}
//	sleep(2);

//	break;


	printf("\n    #############################################################\n");
	printf("     #############################################################\n");
	printf("关闭JOIN SERVER连接，开始连接CONNECT SERVER...\n");	// JOIN SERVER 连接
	shutdown(sock_serv_fd, SHUT_RDWR);
	close(sock_serv_fd);
	sock_serv_fd = -1;
	memset(&cliaddr, 0, sizeof(cliaddr));
	printf("conn IP:%s port:%d\n",  conn_serv.IP, conn_serv.Port);

int g_CMD = SOCK_CMD_REQ;
while(1){
//	printf("sock_conn_fd=%d\n", sock_serv_fd);
	

	switch(g_CMD)
	{
	
	// 发送新连接
	 case  SOCK_CMD_REQ:
		if((sock_serv_fd = sock_conn_server(&cliaddr, &conn_serv))<0)	
			continue;
		protocol_CSREQ_ready();
		if( !CServer_send(sock_serv_fd, (u8 *)CServer_req, sizeof(CServer_REQ))){
			sock_close(sock_serv_fd);
			memset(&cliaddr, 0, sizeof(cliaddr));
			printf("发送新连接失败...\n");
			sleep(10);
			continue;
		}
		sleep(1);
		g_CMD = SOCK_CMD_FUP;
	continue;
	
	case 	SOCK_CMD_FUP:
	// 发送全更新
		if((sock_serv_fd = sock_conn_server(&cliaddr, &conn_serv))<0)	
			continue;
		protocol_CSFUP_ready();
		if( !CServer_send(sock_serv_fd, (u8 *)CS_FUP, sizeof(CServer_FULL_UPDATE))){
			sock_close(sock_serv_fd);
			memset(&cliaddr, 0, sizeof(cliaddr));
			printf("发送全更新失败...\n");
			sleep(10);
			continue;
		}
		sleep(1);
		g_CMD = SOCK_CMD_HB;
	continue;
	
	case SOCK_CMD_HB:

	// 发送心跳
		if((sock_serv_fd = sock_conn_server(&cliaddr, &conn_serv))<0)	
			continue;
		protocol_CSHB_ready();
		if( !CServer_send(sock_serv_fd, (u8 *)&CS_HB, sizeof(CS_HB))){
			printf("发送心跳失败...\n");
			sock_close(sock_serv_fd);
			memset(&cliaddr, 0, sizeof(cliaddr));
			sleep(2);
		}
		sleep(300);
	break;
	 } // switch
	
      } //  while(1) of conn server
  }		
}

