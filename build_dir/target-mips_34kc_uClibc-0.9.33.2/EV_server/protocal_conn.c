

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "protocal_conn.h"
#include  "net_info.h"
#include "CRC.h"
#include "AES.h"

enum  {
	CONNECT_REQ = 1,
	CONNECT_HB,
};

ConnectREQ_INFO		ConnectREQ;
ConnectComfirm		*ConnectCF = NULL;
CServer_REQ		*CServer_req = NULL;
CServer_HB		CS_HB;
CServer_FULL_UPDATE	*CS_FUP = NULL;   // connect server full update
u16			g_sequence;
CServer_PART_UPDATE	*CS_PUP = NULL;

CHARGER_INFO	get_charger_info(void)
{
	// 从数据库中取得电桩信息
	CHARGER_INFO	info;
	memset(&info, 0, sizeof(CHARGER_INFO));

	info.PresentMode = CHARGER_READY;
	*(u16*)info.SubMode = IEC32;
	info.ChargerMessage = 0;  // NULL
	info.DigitalOutputStatus[0] =(u8) ((Digital_Output_Contactor1)>>8);
	info.DigitalOutputStatus[1] =(u8) (Digital_Output_Contactor1);
	info.DigitalInputStatus[0] = (u8) ((Digital_Input_Contactor1_FB)>>8);
	info.DigitalInputStatus[1] = (u8) (Digital_Input_Contactor1_FB);
	info.PresentMode = CHARGER_READY;  //就绪
	info.SubMode[0] =  0;
	info.SubMode[1] = 0;
	info.ChargerMessage = 0x00;
	info.FirmwareVersion[0] = (u8)CHARGER_VERSION >> 8;
	info.FirmwareVersion[1] = (u8)((CHARGER_VERSION)*100);
	info.Sequence[0] = (u8)(g_sequence>>8);
	info.Sequence[1] = (u8)g_sequence;
	int power = 1000;
	info.AccumulatePower[0] = (u8)(power>>24);
	info.AccumulatePower[1] = (u8)(power>>16);
	info.AccumulatePower[2] = (u8)(power>>8);
	info.AccumulatePower[3] = (u8)power;
	short power_cnt = 10;
	info.AccumulateChargingFrequency[0] = (u8)(power_cnt>>8);
	info.AccumulateChargingFrequency[1] = (u8)power_cnt;
	info.SelectedSocket = GB20234_SIGNAL;        // 4
	info.SelectedSocketCurrent = 0x10;          
	short  min = 180;
	info.ChargingDuration[0] = (u8)(min >> 8);
	info.ChargingDuration[1] = (u8)min;
	info.DelayDuration = 0; //等待充电时间
	int i;
	for(i=0; i<=15; i++){
		info.PresentEVLink_UID[i] = i+1;
	}
	// 16  1 10 12:30
	short record = 1000;
	info.Last_Charging_Code[0] = (u8)(record>>8);
	info.Last_Charging_Code[1] = (u8)record; 
	short day = 10; //  1 mon 10days
	char  year = 16;
	*(u16*)info.Last_Charging_Date = day;
	info.Last_Charging_Date[1] = info.Last_Charging_Date[1] | (year << 1);
	info.Last_Charging_Start_Time[0] = 12;
	info.Last_Charging_Start_Time[1] = 30;
	*(u16 *)info.Last_Charging_Duration = 180;
	*(u16 *)info.Last_Charging_Power = 1000;
	info.Last_Charging_Socket = GB20234_SIGNAL;
	info.Last_Charging_Socket_Current = 0x10;
	for(i=0; i<=15; i++){
		info.Last_EV_Link_UID[i] = i+1;
	}
	*(u16*)info.DigitalOutputStatus = Digital_Output_Contactor1;
	*(u16*)info.DigitalInputStatus = Digital_Input_Contactor1_FB;
	info.SubInfo[1] = 0x02;
	info.SubInfo[0] = 0x00;
	
	
	
	
	return info;
}

void protocol_CSPUP_ready(void) // 部分更新，命令0x56
{
	static u16	g_Res = 0x1d0f;
	Present_INFO	present_info;
	CHARGER_INFO	charger_info;

	charger_info = get_charger_info();
	present_info = DayofYear();
	*(u16 *)CS_PUP->PresentDate = *(u16 *)present_info.PresentDate;
	*(u16 *)CS_PUP->PresentTime = *(u16 *)present_info.PresentTime;
	CS_PUP->PresentMode = charger_info.PresentMode;
	CS_PUP->ChargerMessage = charger_info.ChargerMessage;
	memcpy(CS_PUP->SubMode, charger_info.SubMode, 2);
	memcpy(CS_PUP->Sequence, &g_sequence, 2);
	CS_PUP->CurrentOutputCurrent = charger_info.CurrentOutputCurrent;
	memcpy(CS_PUP->Last_Charging_Code, charger_info.Last_Charging_Code, 2);
	memcpy(CS_PUP->Last_Charging_Date, charger_info.Last_Charging_Date, 2);
	memcpy(CS_PUP->Last_Charging_Start_Time, charger_info.Last_Charging_Start_Time, 2);
	memcpy(CS_PUP->Last_Charging_Duration, charger_info.Last_Charging_Duration, 2);
	memcpy(CS_PUP->Last_Charging_Power, charger_info.Last_Charging_Power, 2);
	CS_PUP->Last_Charging_Socket = charger_info.Last_Charging_Socket;
	CS_PUP->Last_Charging_Socket_Current = charger_info.Last_Charging_Socket_Current;
	memcpy(CS_PUP->DigitalOutputStatus, charger_info.DigitalOutputStatus, 2);
	memcpy(CS_PUP->DigitalInputStatus, charger_info.DigitalInputStatus, 2);
	memcpy(CS_PUP->SubInfo, charger_info.SubInfo, 2);

	g_Res = getCRC((u8 *)CS_PUP, 51);
	CS_PUP->CRC16[0] = (u8)(g_Res >> 8);
	CS_PUP->CRC16[1] = (u8)g_Res ;

	g_sequence++;

}


void protocol_CSFUP_ready(void)
{
	static u16	g_Res = 0x1d0f;
	Present_INFO	present_info;
	CHARGER_INFO	charger_info;

	charger_info = get_charger_info();
	present_info = DayofYear();

	*(u16 *)CS_FUP->PresentDate = *(u16*)present_info.PresentDate;
	*(u16 *)CS_FUP->PresentTime = *(u16*)present_info.PresentTime;
	CS_FUP->PresentMode = charger_info.PresentMode;
	memcpy(CS_FUP->SubMode, charger_info.SubMode, 2);
	CS_FUP->ChargerMessage = charger_info.ChargerMessage;
//	CS_FUP->Sequence = 
	memcpy(CS_FUP->AccumulateChargingFrequency, charger_info.AccumulateChargingFrequency, 2);
	CS_FUP->SelectedSocket = charger_info.SelectedSocket;
	CS_FUP->SelectedSocketCurrent = charger_info.SelectedSocketCurrent;
	memcpy(CS_FUP->ChargingDuration, charger_info.ChargingDuration, 2);
	CS_FUP->DelayDuration =  charger_info.DelayDuration;
	memcpy(CS_FUP->PresentEVLink_UID, charger_info.PresentEVLink_UID, 16);
	memcpy(CS_FUP->Last_Charging_Code, charger_info.Last_Charging_Code, 2);
	memcpy(CS_FUP->Last_Charging_Date, charger_info.Last_Charging_Date, 2);
	memcpy(CS_FUP->Last_Charging_Start_Time, charger_info.Last_Charging_Start_Time, 2);
	memcpy(CS_FUP->Last_Charging_Duration, charger_info.Last_Charging_Duration, 2);
	memcpy(CS_FUP->Last_Charging_Power, charger_info.Last_Charging_Power, 2);
	CS_FUP->Last_Charging_Socket = charger_info.Last_Charging_Socket;
	CS_FUP->Last_Charging_Socket_Current = charger_info.Last_Charging_Socket_Current;
	memcpy(CS_FUP->Last_EV_Link_UID, charger_info.Last_EV_Link_UID, 16);
	memcpy(CS_FUP->DigitalOutputStatus, charger_info.DigitalOutputStatus, 2);
	memcpy(CS_FUP->SubInfo, charger_info.SubInfo, 2);

	g_Res = getCRC((u8 *)CS_FUP, 96);	
	CS_FUP->CRC16[0] = (u8)(g_Res >> 8);
	CS_FUP->CRC16[1] = (u8)g_Res;


}
void protocol_CSHB_ready(void)
{
	static u16	g_Res = 0x1d0f;
	CHARGER_INFO	charger_info;

	charger_info =get_charger_info();
	CS_HB.PresentMode = charger_info.PresentMode;
	*(u16*)CS_HB.SubMode = *(u16*)charger_info.SubMode;
	CS_HB.ChargerMessage = charger_info.ChargerMessage;
	*(u16*)CS_HB.DigitalOutputStatus = *(u16*)charger_info.DigitalOutputStatus;
	*(u16*)CS_HB.DigitalInputStatus = *(u16*)charger_info.DigitalInputStatus;
	*(u16*)CS_HB.Sequence = g_sequence;

	g_Res = getCRC((u8 *)&CS_HB, 22);
	CS_HB.CRC16[0] = (u8)(g_Res >> 8);
	CS_HB.CRC16[1] = (u8)g_Res;
	g_sequence++;

}

void protocol_CSREQ_ready(void)
{
	static u16 g_Res = 0x1d0f;
	if( !CServer_req)
		return ;
	Present_INFO	info;
	info = DayofYear();
		
	*(u16 *)CServer_req->PresentDate = *(u16 *)info.PresentDate;
	*(u16 *)CServer_req->PresentTime = *(u16 *)info.PresentTime;

	g_Res = getCRC((u8 *)CServer_req, 26);
	CServer_req->CRC16[0] = (u8)(g_Res >> 8);
	CServer_req->CRC16[1] = (u8)g_Res;
	printf("CRC[0]=%d CRC[1]=%d\n", CServer_req->CRC16[0], CServer_req->CRC16[1]);

}

void protocol_comfirm_ready(struct connect_serv *conn_serv, u8 PresentMode, u16 SubMode)
{
	Present_INFO 	info;
	static u16 	g_Res = 0x1d0f;
	if(!conn_serv || !ConnectCF)
		return ;
	info = DayofYear();
	*(u16 *)ConnectCF->PresentDate = *(u16 *)info.PresentDate;		
	*(u16 *)ConnectCF->PresentTime = *(u16 *)info.PresentTime;		
//	info.AccumulatePower[0] = (u8)(AccuPower>>24);
	
	ConnectCF->ConnectServerIP[0] = conn_serv->ip[0];
	ConnectCF->ConnectServerIP[1] = conn_serv->ip[1];
	ConnectCF->ConnectServerIP[2] = conn_serv->ip[2];
	ConnectCF->ConnectServerIP[3] = conn_serv->ip[3];
	ConnectCF->ConnectServerPort[0] = (u8)(conn_serv->Port >>8);
	ConnectCF->ConnectServerPort[1] = (u8)conn_serv->Port;
	ConnectCF->PresentMode = PresentMode;
	ConnectCF->SubMode[0] = (u8)(SubMode >> 8);
	ConnectCF->SubMode[1] = (u8)SubMode;
	
	g_Res = getCRC((u8 *)ConnectCF, 35);
	ConnectCF->CRC16[0] = (u8)(g_Res >> 8);
	ConnectCF->CRC16[1] = (u8)g_Res;
	printf("CRC[0]=%d CRC[1]=%d\n", ConnectCF->CRC16[0], ConnectCF->CRC16[1]);
}

void  protocol_req_ready(ConnectREQ_INFO * req)
{
	u16	g_Res = 0x1d0f;	
	Present_INFO  info;
	
	if(req == NULL)
		return ;

	info = DayofYear();
	*(u16 *)req->PresentDate = *(u16 *)info.PresentDate;
	*(u16 *)req->PresentTime = *(u16 *)info.PresentTime;	
	
	g_Res = getCRC((u8*)req, 50);
	req->CRC16[0] = (unsigned char)(g_Res >> 8);
	req->CRC16[1] = (unsigned char)g_Res;
	printf("发送请求,命令:0x10\n");
}


int  protocol_init(unsigned int cid)
{
	unsigned char mac_addr[6] = {0};
	unsigned char ip_addr[5] = {0};
	int i;

	// 清零
	memset(&ConnectREQ, 0, sizeof(ConnectREQ));

	// 获取ip地址和mac地址
	if(  !Get_ip(ip_addr) ){
		printf("获取IP地址失败...\n");
		return 0;
	}

	if( !Get_mac(mac_addr) ){
		printf("获取MAC地址失败...\n");
		return 0;
	}

	strncpy(ConnectREQ.KEY, "EVNet>", 6);
	ConnectREQ.Length = 45;
	ConnectREQ.Command = 0x10;
	ConnectREQ.CID[0] = (unsigned char )(cid >> 24);
	ConnectREQ.CID[1] = (unsigned char )(cid >> 16);
	ConnectREQ.CID[2] = (unsigned char )(cid >> 8);
	ConnectREQ.CID[3] = (unsigned char )cid;

	printf("CID:%d\n", *(int *)ConnectREQ.CID);
//	printf("protocol IP:");
	for(i=0; i <=3; i++){
		ConnectREQ.IP[i] = ip_addr[i];
//		printf("%d  ", ConnectREQ.IP[i]);
	}
//	printf("\nprotocol MAC:");
	for(i=0; i<=5; i++){
		ConnectREQ.MAC[i] = mac_addr[i];
//		printf("%02x  ", ConnectREQ.MAC[i]);
	}
//	printf("\n");
	strcpy(( char *)ConnectREQ.Model, "EVG-32N");
	strncpy((char *)ConnectREQ.Series, "EM3C505417", 10);

	ConnectREQ.JoinConnectVersion[0] = (u8)JOIN_CONN_VERSION >> 8;
	ConnectREQ.JoinConnectVersion[1] = (u8)((JOIN_CONN_VERSION)*100);
	printf("jon:%d\n", (u8)((JOIN_CONN_VERSION)*100));
	printf("conn  version:%d.%d\n", ConnectREQ.JoinConnectVersion[0], ConnectREQ.JoinConnectVersion[1]);
	*(short *)ConnectREQ.ChargerFWVersion   = (short)CHARGER_VERSION; 
	printf("charger version:%d.%d\n", ConnectREQ.ChargerFWVersion[0], ConnectREQ.ChargerFWVersion[1]);



	// 填充确认连接的结构体，malloc分配，方便释放
	ConnectCF = (ConnectComfirm *)malloc(sizeof(ConnectComfirm));
	memset(ConnectCF, 0, sizeof(ConnectComfirm));

//	ConnectCF->AddCID[0] = (u8)(cid >>24);
//	ConnectCF->AddCID[1] = (u8)(cid >>16);
//	ConnectCF->AddCID[2] = (u8)(cid >>8);
//	ConnectCF->AddCID[3] = (u8)cid;
	strncpy(ConnectCF->KEY, "EVNet>", 6);
	ConnectCF->Length = 30;
	ConnectCF->Command = 0x12;
	ConnectCF->CID[0] = (u8)(cid >> 24);
	ConnectCF->CID[1] = (u8)(cid >> 16);
	ConnectCF->CID[2] = (u8)(cid >> 8);
	ConnectCF->CID[3] = (u8)cid;
	for(i = 0; i<=3; i++){
		ConnectCF->IP[i] = ip_addr[i];
	}
	for(i = 0; i<=5; i++){
		ConnectCF->MAC[i] = mac_addr[i]; 
	}


	// 连接connect server请求数据结构
	CServer_req = (CServer_REQ *)malloc(sizeof(CServer_REQ));
	memset(CServer_req, 0, sizeof(CServer_REQ));
	strncpy(CServer_req->KEY, "EVNet>", 6);
	CServer_req->Length = 21;
	CServer_req->Command= 0x20;
	CServer_req->CID[0] = (u8)(cid >> 24);
	CServer_req->CID[1] = (u8)(cid >> 16);
	CServer_req->CID[2] = (u8)(cid >> 8);
	CServer_req->CID[3] = (u8)cid;
	for(i = 0; i<=3; i++){
		CServer_req->IP[i] = ip_addr[i];
	}
	for(i= 0; i<=5; i++){
		CServer_req->MAC[i] = mac_addr[i];
	}

	// 初始化心跳
	memset(&CS_HB, 0, sizeof(CS_HB));
	strncpy(CS_HB.KEY, "EVNet>", 6);
	CS_HB.Length = 17;
	CS_HB.Command = 0x34;
	CS_HB.CID[0] = (u8)(cid >> 24);
	CS_HB.CID[1] = (u8)(cid >> 16);
	CS_HB.CID[2] = (u8)(cid >> 8);
	CS_HB.CID[3] = (u8)cid;


	// 初始化全连接

	CS_FUP = (CServer_FULL_UPDATE *)malloc(sizeof(CServer_FULL_UPDATE));
	printf("全更新长度为:%ld\n", sizeof(CServer_FULL_UPDATE));
	strncpy(CS_FUP->KEY, "EVNet>", 6);
	CS_FUP->Length = 91;
	CS_FUP->Command=0x54;
	CS_FUP->CID[0] = (u8)(cid >> 24);		
	CS_FUP->CID[1] = (u8)(cid >> 16);		
	CS_FUP->CID[2] = (u8)(cid >> 8);		
	CS_FUP->CID[3] = (u8)cid;
	for(i=0; i<=3; i++){
		CS_FUP->IP[i] = ip_addr[i]; 
	}	
	for(i=0; i<=5; i++){
		CS_FUP->MAC[i] =mac_addr[i];
	}
	*(short *)CS_FUP->FirmwareVersion  = (short)CHARGER_VERSION; 

	// 初始化部分更新
	CS_PUP	= (CServer_PART_UPDATE *)malloc(sizeof(CServer_PART_UPDATE));
	printf("部分更新初始化...\n");
	strncpy(CS_PUP->KEY, "EVNet>", 6);
	CS_PUP->Length = 53;
	CS_PUP->Command = 0x56;
	CS_PUP->CID[0] = (u8)(cid >> 24);
	CS_PUP->CID[1] = (u8)(cid >> 16);
	CS_PUP->CID[2] = (u8)(cid >> 8);
	CS_PUP->CID[3] = (u8)(cid );
	for(i=0; i<=3; i++){
		CS_PUP->IP[i] = ip_addr[i]; 
	}	
	for(i=0; i<=5; i++){
		CS_PUP->MAC[i] =mac_addr[i];
	}

	return 1;
}

Present_INFO   DayofYear(void)
{
	time_t  tim;
	struct  tm  *stm; 
	int leap = 0;
	int  year;
	int month;
	unsigned short Days = 0;
	unsigned char arr[2] = {0};
	Present_INFO 	present_info;
	memset(&present_info, 0, sizeof(present_info));
	tim = time(0);

	stm = localtime(&tim);

	year = stm->tm_year+1900;
	month = stm->tm_mon+1;
	if((year%4==0 && year%100!=0) || (year%400)== 0){
		leap = 1;
	}
	
	while(--month){
		switch(month){
			case 1: case 3: case 5: case 7: case 8: case 10: case 12:
				Days += 31;
			break;
			case 4: case 6: case 9: case 11:
				Days += 30;
			break;
			case 2:
				Days += 28 + leap;
			break;
		}
	}
	Days += stm->tm_mday;	
	

//	*(u16 *)present_info.PresentDate =  Days & 0x1FF;
//	present_info.PresentDate[1] =  present_info.PresentDate[1] | ((year %100) << 1);
	*(u16 *)arr = Days & 0x1ff;
	arr[1] = arr[1] | ((year%100) << 1);
	present_info.PresentDate[0] = arr[1];
	present_info.PresentDate[1] = arr[0];
		
	printf("20%02d 是今年的第%d天\n", *(u16 *)present_info.PresentDate &0x7f,  *(u16 *)present_info.PresentDate >> 7);

	printf("%#x %#x\n", present_info.PresentDate[0], present_info.PresentDate[1]);
	present_info.PresentTime[0] = stm->tm_hour;
	present_info.PresentTime[1] = stm->tm_min;

	return present_info;
}




