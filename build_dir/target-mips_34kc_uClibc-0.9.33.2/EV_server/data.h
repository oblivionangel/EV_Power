

#ifndef  __DATA__H
#define  __DATA__H


typedef  unsigned char    u8;
typedef  unsigned int     u32;
typedef  unsigned short   u16;

typedef  struct key {
	u8  KEYA[16];
	u8  KEYB[16];
}AES_Key;


#define	   	SOCK_CMD_REQ		0x01
#define		SOCK_CMD_FUP		0x02
#define		SOCK_CMD_HB		0x03



#define    Join_Conn_Ver(m, n)    	 ( (m) | ((n)<<8))
#define    Charg_Ver(m,n)		 ( (m) | ((n)<<8))



#define    JOIN_CONN_VERSION		Join_Conn_Ver(2,0)
#define    CHARGER_VERSION		Charg_Ver(1,2)

// select  socket
/* 
enum	select_socket {
	BS1363 = 0x01,
	SAEJ1772,
	IEC62196_signal,
	GB

};
*/

// select socket
#define		BS1363				0x01
#define		SAEJ1772			0x02
#define		IEC62196_SIGNAL			0x03
#define		GB20234_SIGNAL			0x04
#define		IEC62196_THRD			0X05
#define		GB20234_THRD			0x06
#define		CHADEMO				0X08
#define		IEC_COMBO			0x09
#define		GB_QUICK_CHARGER		0x0A

// Charger  Mode(PresentMode)
#define     	CHARGER_READY			0x01
#define 	CHARGER_RESERVERD		0x04
#define 	CHARGER_PRE_CHARGING		0x09
#define		CHARGER_CHARGING_IN_QUEUE	0x0A
#define		CHARGER_CHARGING		0x0B
#define		CHARGER_CHARGING_COMPLETE	0x0D
#define		CHARGER_OUT_OF_SERVICE		0x0E
#define		CHARGER_CHARGING_COMPLETE_LOCK	0x1E
#define		CHARGER_DEBUGGING		0x65
#define		CHARGER_ESTOP			0x66

// SUB Mode
#define 	IEC_EV_PLUG			0x0001  //已经插线
#define		IEC_EV_READY			0x0002	// 正在充电
#define		IEC_EV_COMPLETE			0x0004	// 充电完成
#define		SUB_RESERVED1			0x0008  // 没用
#define		SUB_RESERVED2			0x0010  // 没用
#define		SUB_RESERVED3			0x0020  // 没用
#define		PARKED				0x0040  // 有泊车
#define		ONLINE				0x0080  // 已经连接伺服 
#define		SAE_PLUG			0x0100  // 没用
#define		SAE_READY			0x0200	// 没用
#define		SAE_COMPLETE			0x0400	// 没用
#define		IEC13				0x0800	// 13A线已插
#define		IEC20				0x1000	// 20A线已插
#define		IEC32				0X2000	// 32A线已插
#define		IEC63				0x4000	// 63A线已插
#define		IEC16				0x8000	// 16A线已插
#define		SUB_MODE_MASK			0xFFFF


// Digital Input Signal
#define 	Digital_Input_Contactor1_FB	0x0001  // 单相继电器
#define		Digital_Input_Contactor2_FB	0x0002	// 三相继电器
#define		Digital_Input_RCB0_FB		0x0004
#define		Digital_Input_RFID_FB		0x0008	// RFID 卡
#define		Digital_Input_Parking_Sensor    0x0010	// 停车
#define		Digital_Input_Solenoid_FB	0x0020
#define		Digital_Input_Manual_On		0x0040	// 紧急按钮
#define		Digital_Input_Manual_Off	0x0080
#define		Digital_Input_CAP_Touch1	0X0100
#define		Digital_Input_CAP_Touch2	0x0200
#define		Digital_Input_CAP_Touch3	0x0400
#define		Digital_Input_CAP_Touch4	0x0800
#define		Digital_Input_ORR(dat)		((dat) | (0x0FAE))
#define		Digital_Input_MASK		0xffff

// Digital  Output Signal	
#define		Digital_Output_Contactor1	0x0001	// 
#define		Digital_Output_Contactor2	0x0002
#define		Digital_Output_Solenoid_ative	0x0004
#define		Digital_Output_Solenoid_neative	0x0008
#define		Digital_Output_RED_LED		0x0010
#define		Digital_Output_BLUE_LED		0x0020
#define		Digital_Output_GREEN_LED	0x0040
#define		Digital_Output_BUZZER		0x0080
#define		Digital_OLED_VCC_EN		0x0100
#define		Digital_EEPROM_VCC		0x0200

// Charger  Message(charger to server for 5s)
#define		Charger_MSG_NULL		0x00
#define		Charger_MSG_CMD_IN_Progress	0x01
#define		Charger_MSG_CMD_SUCCESS		0x02
#define		Charger_MSG_CMD_FAIL		0x03
#define		Charger_MSG_CMD_Param_Incorr	0x04
#define		Charger_MSG_Other_MODE_FAIL	0x05 
#define		Charger_MSG_Charger_Not_Active	0x10
#define		Charger_MSG_EV_Not_Ready	0x11
#define		Charger_MSG_Cable_Unplug	0x12  //充电抢被拔出
#define		RESERVED			0x13  //保留
#define		Charger_MSG_NO_Power_In_2Min	0x20
#define		Charger_MSG_Charger_Stop_TimeUp 0x21
#define		Charger_MSG_Charger_Stop_Unplug 0x22
#define		Charger_MSG_Charger_Stop_FullCharg	0x23
#define		Charger_MSG_Ev_Self_Complete	0x24
#define		Charger_MSG_Will_Stop_In_1min	0x25
#define		Charger_MSG_Will_Stop_In_10min  0x26
#define		Charger_MSG_Parking		0x30
#define		Charger_MSG_Parking_Overtime	0x31
#define		Charger_MSG_Reservation_Will_1min	0x32 // 预订
#define		Charger_MSG_Reservation_Will_10sec	0x33
#define		Charger_MSG_In_Maintenance	0xF0
#define		Charger_MSG_Out_Serv_RCCD	0xF1
#define		Charger_MSG_Out_Serv_Power_Err	0xF2
#define		Charger_MSG_Out_Serv_RFID_Err	0xF3
#define		Charger_MSG_Out_Serv_NFC_Err	0XF4
#define		Charger_MSG_Contactor1_FB_Err  	0xF5
#define		Charger_MSG_Contactor2_FB_Err	0xF6



struct connect_serv {
	u8  	ip[4];   
	u8	KEYB[16];
	u16 	Port;
	u8	IP[20];
};

typedef  struct  conn_info {
	u8    	KEY[6];
	u8 	Length;
	u8    	Command;
	u8    	CID[4];
	u8    	IP[4];
	u8    	MAC[6];
	u8 	PresentDate[2];
	u8    	PresentTime[2];	
	u8	Model[10];
	u8 	Series[10];
	u8    	JoinConnectVersion[2];
	u8    	ChargerFWVersion[2];
	u8 	CRC16[2];

}ConnectREQ_INFO;


typedef  struct conn_comfirm {
//	u8 AddCID[4];
	u8 KEY[6];
	u8 Length;
 	u8 Command;
	u8 CID[4];
	u8 IP[4];
	u8 MAC[6];
	u8 PresentDate[2];
	u8 PresentTime[2];
	u8 PresentMode;
	u8 SubMode[2];
	u8 ConnectServerIP[4];
	u8 ConnectServerPort[2];
	u8 CRC16[2]; 
}ConnectComfirm;



typedef  struct present {
	u8	 PresentDate[2];
	u8	 PresentTime[2];
}Present_INFO;

typedef struct  new_conn {
	u8 	KEY[6];
	u8	Length;
	u8	Command;
	u8	CID[4];
	u8	IP[4];
	u8	MAC[6];
	u8	PresentDate[2];
	u8	PresentTime[2];
	u8	CRC16[2];
}CServer_REQ;


typedef  struct new_conn_hb {
	u8	KEY[6];
	u8	Length;
	u8	Command;
	u8	CID[4];
	u8	PresentMode;
	u8	SubMode[2];
	u8	ChargerMessage;
	u8	Sequence[2];
	u8	DigitalOutputStatus[2];
	u8	DigitalInputStatus[2];
	u8	CRC16[2];
}CServer_HB;

typedef  struct {
	u8	KEY[6];
	u8	Length;
	u8	Command;
	u8	CID[4];
	u8	IP[4];
	u8	MAC[6];
	u8	PresentDate[2];
	u8	PresentTime[2];
	u8	PresentMode;
	u8	SubMode[2];
	u8	ChargerMessage;
	u8	FirmwareVersion[2];
	u8	Sequence[2];
	u8	AccumulatePower[4];
	u8	AccumulateChargingFrequency[2]; //累计以冲次数
	u8	SelectedSocket;			// 选择的头
	u8	SelectedSocketCurrent;		// 选择的电流
	u8	CurrentOutputCurrent;		// 当前输出的电流
	u8	ChargingDuration[2];		
	u8	DelayDuration;
	u8	PresentEVLink_UID[16];		
	u8	Last_Charging_Code[2];
	u8	Last_Charging_Date[2];		// YYDDD
	u8	Last_Charging_Start_Time[2];	//HHMM
	u8	Last_Charging_Duration[2];	//min
	u8	Last_Charging_Power[2];
	u8	Last_Charging_Socket;
	u8	Last_Charging_Socket_Current;
	u8	Last_EV_Link_UID[16];
	u8	DigitalOutputStatus[2];
	u8	DigitalInputStatus[2];
	u8	SubInfo[2];
	u8	CRC16[2];
}CServer_FULL_UPDATE;

typedef struct part_update {
	u8	KEY[6];
	u8	Length;
	u8	Command;
	u8	CID[4];
	u8	IP[4];
	u8	MAC[6];
	u8	PresentDate[2];
	u8	PresentTime[2];
	u8	PresentMode;
	u8	ChargerMessage;
	u8	SubMode[2];
	u8	Sequence[2];
	u8	CurrentOutputCurrent;
	u8	Last_Charging_Code[2];
	u8	Last_Charging_Date[2];
	u8	Last_Charging_Start_Time[2];
	u8	Last_Charging_Duration[2];
	u8	Last_Charging_Power[2];
	u8	Last_Charging_Socket;
	u8	Last_Charging_Socket_Current;
	u8	DigitalOutputStatus[2];
	u8	DigitalInputStatus[2];
	u8	SubInfo[2];
	u8	CRC16[2];
}CServer_PART_UPDATE;


typedef struct charger_info {
	u8	CID[4];
	u8	IP[4];
	u8	MAC[6];
// 全更新
	u8	PresentDate[2];
	u8	PresentTime[2];
	u8	PresentMode;
	u8	SubMode[2];
	u8	ChargerMessage;
	u8	FirmwareVersion[2];
	u8	Sequence[2];
	u8	AccumulatePower[4];
	u8	AccumulateChargingFrequency[2]; //累计以冲次数
	u8	SelectedSocket;			// 选择的头
	u8	SelectedSocketCurrent;		// 选择的电流
	u8	CurrentOutputCurrent;		// 当前输出的电流
	u8	ChargingDuration[2];		
	u8	DelayDuration;
	u8	PresentEVLink_UID[16];		
	u8	Last_Charging_Code[2];
	u8	Last_Charging_Date[2];		// YYDDD
	u8	Last_Charging_Start_Time[2];	//HHMM
	u8	Last_Charging_Duration[2];	//min
	u8	Last_Charging_Power[2];
	u8	Last_Charging_Socket;
	u8	Last_Charging_Socket_Current;
	u8	Last_EV_Link_UID[16];
	u8	DigitalOutputStatus[2];
	u8	SubInfo[2];
//心跳
	u8	DigitalInputStatus[2]; 
}CHARGER_INFO;


#endif  //data.h


