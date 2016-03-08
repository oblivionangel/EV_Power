
#include "net_info.h"
#include "protocal_conn.h"
#include "AES.h"
#include "CRC.h"
#include <sys/types.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>


#define  MAXSLEEP    128
static void join_fill(struct connect_serv * addr, u8 *recv_buff, int len);
int Readable_timeout(int fd, int sec);
int CServer_recv_deal(unsigned char *recv_buff, unsigned int len);

int sock_close(int sockfd)
{
	char tmp[2];
        memset(tmp,'\0',sizeof(tmp));
        while(1) {
                if(read(sockfd,tmp,1)<=0) break;
                if(write(sockfd,tmp,1)<=0) break;
        }
        shutdown(sockfd,SHUT_RDWR);
        close(sockfd);
        return(0);
}


int CServer_send(int sockfd, unsigned char *buff, unsigned int len)
{
	u8 	input_buff[128] = {0};
	u8	send_buff[128] = {0};
	u8	recv_buff[1024] = {0};
	u8	crc[2];
	int slen = 0, res = 0;
	int i;
	memcpy(send_buff, buff, len);
	Padding(send_buff, len, input_buff, &slen);
	memset(send_buff, 0, len);
	My_AES_CBC_Encrypt(KEY.KEYB,input_buff, slen, send_buff+4);
	send_buff[0] = buff[8];
	send_buff[1] = buff[9];
	send_buff[2] = buff[10];
	send_buff[3] = buff[11];

	for(i = 0; i< slen+4; i++)
		printf("%d ", send_buff[i]);
	printf("\n发送命令:%#x\n 长度:%d\n", buff[7], slen+4);
	if((res = write(sockfd, send_buff, slen+4))!=slen+4){
		printf("发送数据不足:len=%d\n", res);
		return 0;
	}
	if(Readable_timeout(sockfd, SOCK_TIME_OUT) == 0){
		printf("接收回复超时...\n");
		return 0;
	}
	if( !(res = read(sockfd, recv_buff, sizeof(recv_buff)))){
		printf("接收失败,服务器关闭套接口...\n");
		return 0;
	}
	
	if( !CServer_recv_deal(recv_buff, res))
	{
		printf("接收数据处理失败...\n");
		return 0;
	}
	printf("接收数据成功...\n");
	return 1;
}
int CServer_recv_deal(unsigned char *recv_buff, unsigned int len)
{
	u8 	crc[2] = {0};
	u8	buff[128] = {0};
	u8	re_buff[128] = {0};
	unsigned int slen = 0;

	My_AES_CBC_Decrypt(KEY.KEYB, recv_buff, len, buff);
	memset(re_buff, 0, sizeof(re_buff));
	RePadding(buff, len, re_buff, &slen);	
	printf("接收数据长度len=%d\n", slen);
	if(re_buff[6] != slen -7){
		printf("接收数据长度不足...\n");
		return 0;
	}
	crc[0] = re_buff[slen-1];
	crc[1] = re_buff[slen-2];

	if(*(u16*)crc != getCRC(re_buff, slen-2)){
		printf("接收数据校验失败...\n");
		return 0;
	}

	switch(re_buff[7]){
		case 0x21:
			printf("接收到0x21命令...\n");
		break;
		
		case 0x43:
			printf("接收到心跳0x43...\n");
		break;
	}
	
	return 1;

}


int CServer_conn_HB(int sockfd)
{
	u8 	input_buff[128] = {0};
	u8	send_buff[128] = {0};
	u8	recv_buff[128] = {0};
	u8	crc[2];
	int 	len =0;
	int 	slen = 0;
	int 	res = 0;
	memcpy(send_buff, (u8 *)&CS_HB, 24);
	Padding(send_buff, 24, input_buff, &len);
	memset(send_buff, 0, 24);
	My_AES_CBC_Encrypt(KEY.KEYB, input_buff, len, recv_buff); 
	
	send_buff[0] = CS_HB.CID[0];
	send_buff[1] = CS_HB.CID[1];
	send_buff[2] = CS_HB.CID[2];
	send_buff[3] = CS_HB.CID[3];
	
	printf("发送命令:%#x\n", 0x34);
	
	if((res = write(sockfd, send_buff, len+4)) != len+4){
		printf("发送心跳长度不足len=%d...\n", res);
		return 0;
	}
	printf("发送心跳命令成功...\n");

	if(Readable_timeout(sockfd, SOCK_TIME_OUT) == 0){
		printf("接收心跳超时...\n");
		return 0;
	}
	
	if((len = read(sockfd, recv_buff, 15)) != 15){
		printf("接收数据长度不足...\n");
		return 0;
	}
	printf("心跳接收长度为:%d\n", len);
	memset(input_buff, 0, sizeof(input_buff));
	My_AES_CBC_Decrypt(KEY.KEYB, recv_buff, len, input_buff);
	RePadding(input_buff, len, recv_buff, &slen);
	
	crc[0] = recv_buff[slen-1];
	crc[1] = recv_buff[slen-2];
	if(*(u16*)crc != getCRC(recv_buff, slen-2)){
		printf("心跳CRC校验失败...\n");
		return 0;
	}
	if(recv_buff[6] != 0x43){
		printf("接收心跳命令0x43不正确...\n");
		return 0;
	}
	printf("接收到心跳0x43命令...\n");
	return 1;
}

static void CServ_fill(u8 * recv_buff, int len);
//int join_comfirm_conn();
int Readable_timeout(int fd, int sec)
{
	fd_set	rset;
	struct timeval	tv;
	FD_ZERO(&rset);
	FD_SET(fd, &rset);
	tv.tv_sec = sec;
	tv.tv_usec = 0;
	return (select(fd+1, &rset, NULL, NULL, &tv));

}

void commu_alarm(int sig)
{
	printf("ALARM...\n");
	return ;
}

int CServ_conn_req(int sockfd,  u32 cid)
{
	u8 	input_buff[128] = {0};
	u8	send_buff[128] = {0};
	u8	recv_buff[128] = {0};
	u8	aes_buff[128] = {0};
	int 	len = 0;
	int slen = 0;
	int i;
	unsigned char *p = (u8*)CServer_req;

//	signal(SIGALRM, commu_alarm);

	protocol_CSREQ_ready();
//	for(i = 0; i<=27;i++)
//		printf("%d ", p[i]);
//	printf("\n");
	
	memcpy(send_buff, (u8*)CServer_req, 28);
	Padding(send_buff, 28, input_buff, &len);
	printf("padding....%d\n", input_buff[len-1]);
	memset(send_buff, 0, 128);
// 报错。。。。
	My_AES_CBC_Encrypt(KEY.KEYB, input_buff, len, aes_buff); 
	My_AES_CBC_Decrypt(KEY.KEYB, aes_buff, len, input_buff);
	memset(aes_buff, 0, sizeof(aes_buff));
	RePadding(input_buff, len, aes_buff, &slen);
//	for(i = 0; i<=slen-1;i++)
//		printf("%d ", aes_buff[i]);
//	printf("\n");
	
	memset(input_buff, 0, sizeof(input_buff));
	Padding(aes_buff, 28, input_buff, &len);
	memset(send_buff, 0, 128);
// 报错。。。。
	memset(aes_buff, 0, sizeof(aes_buff));
	My_AES_CBC_Encrypt(KEY.KEYB, input_buff, len, aes_buff);
	 

	memcpy(send_buff+4, aes_buff, len);
	send_buff[0] = ConnectREQ.CID[0];
	send_buff[1] = ConnectREQ.CID[1];
	send_buff[2] = ConnectREQ.CID[2];
	send_buff[3] = ConnectREQ.CID[3];
//	for(i = 0; i<36;i++)
//		printf("%d ", send_buff[i]);
//	printf("\n");
	printf("确认连接长度...\n");

	int res = -1;
	if(write(sockfd, send_buff, len+4) != (len + 4)){
		return 0;
	}
	printf("data len:%d\n", len+4);
	printf("send data to connect success...\n");

	// 读取回复
	sleep(1);
//	alarm(SOCK_TIME_OUT);
	if(Readable_timeout(sockfd, SOCK_TIME_OUT)==0)
	{
		printf("connect server receive timeout...\n");
		return 0;
	}
	if(!(res = read(sockfd, recv_buff, sizeof(recv_buff))) ){
//		alarm(0);
//		write(sockfd, "helloworld", 10);
		printf("接收len = %d connect server失败...\n", res);
		return 0;
	}
//	alarm(0);
	memset(input_buff, 0, sizeof(input_buff));
	My_AES_CBC_Decrypt(KEY.KEYB, recv_buff, res, input_buff);
	memset(send_buff, 0, sizeof(send_buff));
	RePadding(input_buff, res, recv_buff, &slen);
	
	printf("decode len:%d\n", slen);
	u8	crc[2];
	crc[0] = recv_buff[27];
	crc[1] = recv_buff[26];
	if( *(u16 *)crc !=getCRC(recv_buff, 26)){
		printf("recv connect server crc  error...\n");
		return 0;
	}

	if(recv_buff[7] != 0x21)
	{
		printf("CServer command x021 error...\n");
		return 0;
	}
	printf("recv command:%#x\n", 0x21);
	free(CServer_req);

	return 1;
}
static void CServ_fill(u8 * recv_buff, int len)
{
	

}




int join_comfirm_serv(int sockfd, struct connect_serv * conn_serv, u8 PreMod, u16 SubMod)
{
	int cnt = 0;
	u8  	input_buff[128] = {0};
	u8	send_buff[128] = {0};
	int	len = 0;
	protocol_comfirm_ready(conn_serv, PreMod, SubMod);
	memcpy(send_buff, (u8 *)ConnectCF,37 );
	Padding(send_buff, 37, input_buff, &len);
	memset(send_buff, 0, 128);
	My_AES_CBC_Encrypt(conn_serv->KEYB, input_buff, len,send_buff+4 );
	send_buff[0] = ConnectREQ.CID[0];
	send_buff[1] = ConnectREQ.CID[1];
	send_buff[2] = ConnectREQ.CID[2];
	send_buff[3] = ConnectREQ.CID[3]; 
	printf("确认连接长度:%d\n", len);
	if(write(sockfd, send_buff, len+4) != (len+4)){
		printf("发送确认请求字数不足..\n");
		return 0;
	}
	printf("发送请求结束...\n");
	sleep(2);
	free(ConnectCF);
	return 1;
}
int  join_conn_serv(int sockfd, struct connect_serv *addr)
{
	u8 	crc[2];
	u8 	recv_buff[1024] = {0};
	u8      outputMessage[128] = {0};
	int 	cnt = 0;
	int len = 0,  slen = 0;

	signal(SIGALRM, commu_alarm);	
	printf("正在想JOIN Serv发送连接请求...\n");
	protocol_req_ready(&ConnectREQ);
	if(write(sockfd, (u8 *)&ConnectREQ, 52) != 52){
		printf("发送数据不足...\n");
		goto err;	
	}
	
	printf("发送数据0x10完毕...\n");
	if( Readable_timeout(sockfd, SOCK_TIME_OUT) == 0)
	{
		printf("join server timeout...\n");
		return 0;
	}
	if( (len = read(sockfd, recv_buff, sizeof(recv_buff))) <=0){
		printf("接收数据长度不足:%d\n", len);
		goto err;
	}
	printf("接收到数据len=%d...\n", len);
	// 接收信息进行解密
	My_AES_CBC_Decrypt(KEY.KEYA, recv_buff, len, outputMessage);
	memset(recv_buff, 0, 128);
	RePadding(outputMessage, len, recv_buff, &slen);
		
	printf("解密长度为:%d\n", slen);
	crc[1] = recv_buff[slen-2];
	crc[0] = recv_buff[slen-1];
	int i;
	printf("JOIN 接收到的数据：");
	for(i = 0; i<slen; i++)
		printf("%d ", recv_buff[i]);
	printf("\n");
	u16 Crc = 0;

	Crc = getCRC(recv_buff, slen-2);
	printf("Crc:%d  crc:%d\n", Crc, *(u16 *)crc);
	printf("CRC:%d %d crc:%d %d", ((u8*)&Crc)[0], ((u8*)&Crc)[1], crc[0], crc[1]);
//	return 0;	
	if(Crc != *(u16 *)crc){
		printf("CRC校验出错...\n");
		goto err;	
	}
	if(recv_buff[7] ==  0x11){
		printf("接收到0x11命令...\n");
		// 连接命令
		//	memcpy(addr->KEYB, recv_buff+26, 16);
	}else if(recv_buff[7] == 0x98)
	{
		printf("接收到更新系统命令...\n");
		// 更新的命令,调用update_firmware函数，最后确认充电桩已连接然后回到join_conn_serv函数
		// update_firmware(sockfd);
	}
	printf("解密之后的数据:");
	for(i=0;i<slen; i++){
		printf("%d ", recv_buff[i]);
	}
	printf("\n");

	join_fill(addr, recv_buff, slen);
	printf("port:%d\n", addr->Port);
	printf("copnnect IP:%s\n", addr->IP);
	
	return 1;
//	memcpy(addr->KEYB, recv_buff+26, 16);
err:
	return 0;	
}

static void join_fill(struct connect_serv * addr, u8 *recv_buff, int len)
{
	int i;
	char port[2] = {0};
	if(!addr || !recv_buff)
		return ;
	for(i = 0; i<=15; i++){
		addr->KEYB[i] = recv_buff[26+i];
		KEY.KEYB[i] = recv_buff[26+i];
		IV[i] = recv_buff[26+i];
		printf("%d   ", KEY.KEYB[i]);
	}
	printf("\n");
	sprintf(addr->IP, "%d%c%d%c%d%c%d", recv_buff[42], '.', recv_buff[43],'.',  recv_buff[44], '.', recv_buff[45]);
	for(i = 0; i <=3; i++){
		addr->ip[i] = recv_buff[42+i];
	}
	port[0] = recv_buff[47];
	port[1] = recv_buff[46];
	addr->Port = *(unsigned short *)port;
	printf("PORT:%d\n", addr->Port);


}

int  sock_server_init(struct sockaddr_in *cliaddr, char *ip_addr, short port)
{
	int sock_fd;

	printf("--ip:%s  port:%d\n", ip_addr, port);
	if( (sock_fd = socket(AF_INET, SOCK_STREAM, 0)) <0 ){
		err_quit("client sock");
	}
	printf("创建套接字成功...\n");

	bzero(cliaddr, sizeof(struct sockaddr));
	cliaddr->sin_family = AF_INET;
	cliaddr->sin_port = htons(port);
	if(!inet_pton(AF_INET, ip_addr, &cliaddr->sin_addr)){
		err_quit("inet_pton ip addr");
	}

	printf("地址初始化成功...\n");
	return sock_fd;
}


// 指数补偿算法，套接口连接
int connect_retry(int sockfd, const struct sockaddr *addr, socklen_t alen)
{

	int nsec;
	for(nsec = 1; nsec <= MAXSLEEP; nsec<<=1){
		if(connect(sockfd, addr, alen) == 0){
			return 1;
		}
		if(nsec <= MAXSLEEP/2)
		{
			printf("连接重试，休眠了...\n");
			sleep(nsec);
		}
	}
	return 0;
}


void err_quit(char *msg)
{
	perror(msg);
	exit(1);
}


// 获取IP地址程序
int Get_ip(unsigned char * ipaddr)
{

    int sock_get_ip;  
    unsigned char buff[20] = {0};
    struct   sockaddr_in *sin;  
    struct   ifreq ifr_ip;
    int ip = 0;     
    unsigned  char *p_addr = ipaddr;
    if ((sock_get_ip=socket(AF_INET, SOCK_STREAM, 0)) == -1)  
    {  
         printf("socket create failse...GetLocalIp!\n");  
         return 0;  
    }  
     
    memset(&ifr_ip, 0, sizeof(ifr_ip));     
    strncpy(ifr_ip.ifr_name, "eth0", sizeof(ifr_ip.ifr_name) - 1);     
   
    if( ioctl( sock_get_ip, SIOCGIFADDR, &ifr_ip) < 0 )     
    {     
	printf("ioctl get ip failse...\n");
         return 0;     
    }       
    sin = (struct sockaddr_in *)&ifr_ip.ifr_addr;     
    strcpy(buff,inet_ntoa(sin->sin_addr));         
    unsigned char arr[4] = {0};
    unsigned char *p = buff;
    unsigned char *p_1 = arr;
    printf("local ip:%s\n", buff);

    while(p != buff+strlen(buff))
    {
	if(*p== '.'){
		*ipaddr++ = atoi(arr);
//		printf("%s\n", arr);
                memset(arr, 0,  4);
		p++;
		p_1=arr;
		continue;
	}
       *p_1++ = *p++;
    }
   *ipaddr = atoi(arr);	
//    printf("local ip:%d %d %d %d \n",p_addr[0], p_addr[1], p_addr[2],  p_addr[3]);    

    close( sock_get_ip ); 
    return 1;
}

// 获取IP地址程序
int Get_ip_str(unsigned char * ipaddr)
{

    int sock_get_ip;  
    struct   sockaddr_in *sin;  
    struct   ifreq ifr_ip;
    int ip = 0;     
    unsigned  char *p_addr = ipaddr;
    if ((sock_get_ip=socket(AF_INET, SOCK_STREAM, 0)) == -1)  
    {  
         printf("socket create failse...GetLocalIp!\n");  
         return 0;  
    }  
     
    memset(&ifr_ip, 0, sizeof(ifr_ip));     
    strncpy(ifr_ip.ifr_name, "eth0", sizeof(ifr_ip.ifr_name) - 1);     
   
    if( ioctl( sock_get_ip, SIOCGIFADDR, &ifr_ip) < 0 )     
    {     
	printf("ioctl get ip failse...\n");
         return 0;     
    }       
    sin = (struct sockaddr_in *)&ifr_ip.ifr_addr;     
    strcpy(ipaddr,inet_ntoa(sin->sin_addr));         
    printf("local ip:%s\n", ipaddr);
    return 1;
}


// 获取MAC地址程序
int Get_mac(unsigned  char *mac_addr)
{
    struct   ifreq   ifreq; 
    int   sock; 

    if((sock=socket(AF_INET,SOCK_STREAM,0)) <0) 
    { 
  //      perror( "socket "); 
        return   0; 
    } 
    strcpy(ifreq.ifr_name,"eth0"); 
    if(ioctl(sock,SIOCGIFHWADDR,&ifreq) <0) 
    { 
//        perror( "ioctl "); 
        return   0; 
    } 
    printf( "%02x:%02x:%02x:%02x:%02x:%02x\n ", 
            (unsigned   char)ifreq.ifr_hwaddr.sa_data[0], 
            (unsigned   char)ifreq.ifr_hwaddr.sa_data[1], 
            (unsigned   char)ifreq.ifr_hwaddr.sa_data[2], 
            (unsigned   char)ifreq.ifr_hwaddr.sa_data[3], 
            (unsigned   char)ifreq.ifr_hwaddr.sa_data[4], 
            (unsigned   char)ifreq.ifr_hwaddr.sa_data[5]); 

     mac_addr[0] = (unsigned   char)ifreq.ifr_hwaddr.sa_data[0]; 
     mac_addr[1] = (unsigned   char)ifreq.ifr_hwaddr.sa_data[1]; 
     mac_addr[2] = (unsigned   char)ifreq.ifr_hwaddr.sa_data[2]; 
     mac_addr[3] = (unsigned   char)ifreq.ifr_hwaddr.sa_data[3]; 
     mac_addr[4] = (unsigned   char)ifreq.ifr_hwaddr.sa_data[4]; 
     mac_addr[5] = (unsigned   char)ifreq.ifr_hwaddr.sa_data[5]; 
     close(sock);
     return 1;	
 //   return   0; 
}



