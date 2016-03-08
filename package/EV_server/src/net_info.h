
#ifndef  __NET_INFO__H
#define  __NET_INFO__H
#include <stdio.h>      
#include <sys/types.h>
#include <stdlib.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <net/if.h>
#include "data.h"

#define   JOIN_SERVER1_IP	"192.168.0.253"
#define   JOIN_SERVER2_IP	"192.168.0.2"
#define   JOIN_SERVER_PORT	 8500
#define   SOCK_TIME_OUT		 4

void err_quit(char *msg);
// 获取IP地址程序
int Get_ip(unsigned char * ipaddr);
int Get_ip_str(unsigned char * ipaddr);

// 获取MAC地址程序
int Get_mac(unsigned  char *mac_addr);


int sock_server_init(struct sockaddr_in *cliaddr, char *ip_addr, short port);
int connect_retry(int sockfd, const struct sockaddr *addr, socklen_t alen);
int join_conn_serv(int sockfd, struct connect_serv *addr);
int join_comfirm_serv(int sockfd, struct connect_serv *conn_serv, u8 PreMod, u16 SubMod);
int CServer_conn_HB(int sockfd);


int CServer_send(int sockfd, unsigned char *buff, unsigned int len);
int sock_close(int sockfd);

#endif   // net_info.h

