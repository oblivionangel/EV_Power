


#ifndef  __PROTOCAL_CONN__H
#define  __PROTOCAL_CONN__H0

#include "data.h"


extern ConnectREQ_INFO 	ConnectREQ;
extern ConnectComfirm	*ConnectCF;
extern CServer_REQ	*CServer_req;
extern CServer_HB	CS_HB;
extern CServer_FULL_UPDATE	*CS_FUP;

int protocol_init(unsigned  int cid);
void protocol_comfirm_ready(struct connect_serv *conn_serv, u8 PresentMode, u16 SubMode);
void protocol_req_ready(ConnectREQ_INFO *req);
void protocol_CSREQ_ready(void);
void protocol_CSHB_ready(void);

Present_INFO  DayofYear(void);	



#endif  // protocal_conn.h



