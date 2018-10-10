#include "wifibotClient.h"

#define udpnew

WifibotClient::WifibotClient(void)
{
	socketnotok=true;
	socketnotokudp=true;
	moyx=0;moyy=0;
	vmd=0;vmg=0;
	comg=0;comd=0;stop=0;
	ctrlg=0;ctrld=0;
	joyvirtual=false;
	joyusb=false;
	joywii=false;
	udp=false;
	//myip="";
	myport=0;
	ok=false;
	okOut=false;
	first=false;
	Thread_TCP_Run=false;
	Thread_TCP_Trooper_In_Run=false;
	Thread_TCP_Trooper_Out_Run=false;
}

WifibotClient::~WifibotClient(void)
{
}

void WifibotClient::SetControl(bool control)
{	
	if (control) ctrlg=128;
	else ctrlg=0;
	if (control) ctrld=128;
	else ctrld=0;
}

bool WifibotClient::ConnectToRobot(char* ip,int port)
{
	if(socketnotok)
	{
		char szServerAddress[30];
		sprintf_s(szServerAddress,"%s",ip);
		int iPort=port;

		WSAStartup(MAKEWORD(2,2),&wsatcp);

		memset(&sintcp,0,sizeof(sockaddr_in));
		sintcp.sin_family = AF_INET;
		sintcp.sin_addr.s_addr = inet_addr (szServerAddress);

		if (sintcp.sin_addr.s_addr == INADDR_NONE)
		{
			lphost = gethostbyname(szServerAddress);
			if (lphost != NULL)
				sintcp.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
			else
			{
				WSASetLastError(WSAEINVAL);
			}
		}
		sintcp.sin_port = htons(iPort);
		// Open the socket
		socktcp = socket(AF_INET, SOCK_STREAM, 0);
		int err = connect(socktcp, (struct sockaddr*)&sintcp,sizeof(sockaddr));
		if(err == SOCKET_ERROR)
		{ 
			if(socktcp != INVALID_SOCKET)
			{
				closesocket(socktcp);
				socktcp = INVALID_SOCKET;
			}
		}
		else 
		{
			m_hThread2=CreateThread(0,0,Thread_TCP,this,0,&m_dwThreadID2);
			Thread_TCP_Run=true;
			udp=false;
			socketnotok=0;
		}
		return 0;
	}
	else return 1;
}

void WifibotClient::DisconnectRobot()
{
	if (!socketnotok) 
	{
		Thread_TCP_Run=false;
		TerminateThread(m_hThread2,0);
		socketnotok=true;
		closesocket(socktcp);
	}
}

bool WifibotClient::ConnectToRobotUdp(char* ip,int port)
{		
	if(socketnotokudp)
	{
		sprintf_s(myipsend,"%s",ip);
		myportsend=port;

		m_hThread4=CreateThread(0,0,Thread_TCP_Trooper_In,this,0,&m_dwThreadID4);
		Thread_TCP_Trooper_In_Run=true;
		m_hThread5=CreateThread(0,0,Thread_TCP_Trooper_Out,this,0,&m_dwThreadID5);	
		Thread_TCP_Trooper_Out_Run=true;
		udp=true;		
		socketnotokudp=false;
		return 0;
	}
	else return 1;
}

void WifibotClient::DisconnectRobotUdp()
{
	if(!socketnotokudp)
	{
		Thread_TCP_Trooper_In_Run=false;
		Thread_TCP_Trooper_Out_Run=false;
		udp=false;
		ok=false;
		okOut=false;
		closesocket(sock);
		closesocket(sockOut);	
		TerminateThread(m_hThread4,0);
		TerminateThread(m_hThread5,0);
		socketnotokudp=true;
	}	
}

void WifibotClient::SendCommand(int left_speed, int right_speed, unsigned char flags)
{
	unsigned char sbuf[30];
	sbuf[0] = 255;
	sbuf[1] = 0x07;
	sbuf[2] = left_speed;
	sbuf[3] = 0;
	sbuf[4] = right_speed;
	sbuf[5] = 0;
	sbuf[6] = flags;
	short mycrcsend = Crc16((unsigned char*)sbuf+1,6);
	sbuf[7] = (unsigned )mycrcsend;
	sbuf[8] = (unsigned )(mycrcsend >> 8);	
	if (!socketnotok) send(socktcp,(char*)sbuf,9,0);
	odometry=true;
}

void WifibotClient::SendConsigne(CPoint point)
{
	/*sendbuf[0]=(char)(point.x);
	sendbuf[1]=(char)(point.y);
	if (!socketnotok) send(socktcp,sendbuf,2,0);
	odometry=true;*/

	unsigned char sbuf[30];
	BYTE tt=0;
	sbuf[0] = 255;
	sbuf[1] = 0x07;

	unsigned char speed1 = (unsigned char)(point.x);
	unsigned char speed2 = (unsigned char)(point.y);

	int tmp1 = 8*(speed1&0x3F);
	int tmp2 = 8*(speed2&0x3F);
	if (speed2&0x80) tt=tt+32;
	if (speed2&0x40) tt=tt+16;
	sbuf[2] = (unsigned )tmp1;
	sbuf[3] = (unsigned )(tmp1 >> 8);

	sbuf[4] = (unsigned )tmp2;
	sbuf[5] = (unsigned )(tmp2 >> 8);

	sbuf[6] = (speed1&0x80) + (speed1&0x40) + tt +0+2+8+1;//+1 Relay ON +8 10ms pid mode ;

	short mycrcsend = Crc16((unsigned char*)sbuf+1,6);

	sbuf[7] = (unsigned )mycrcsend;
	sbuf[8] = (unsigned )(mycrcsend >> 8);

	//bool res = WriteFile(hUSB, &sbuf, 9,&n, NULL);
	if (!socketnotok) send(socktcp,(char*)sbuf,9,0);
	odometry=true;
}

void WifibotClient::SendConsigneNoOdometry(CPoint point)
{
	sendbuf[0]=(char)(point.x);
	sendbuf[1]=(char)(point.y);
	if (!socketnotok) send(socktcp,sendbuf,2,0);
	odometry=false;
}

void WifibotClient::SendConsigneRC(CPoint speed,CPoint steer)
{
	char sendbufrc[4];
	sendbufrc[0]=(char)(speed.x);
	sendbufrc[1]=(char)(speed.y);
	sendbufrc[2]=(char)(steer.x);
	sendbufrc[3]=(char)(steer.y);
	//printf("speed.x=%d speed.y=%d steer.x=%d steer.y=%d\n",speed.x,speed.y,steer.x,steer.y);
	if (!socketnotok) send(socktcp,sendbufrc,4,0);
}

void WifibotClient::SendConsigneRC_UDP(CPoint speed,CPoint steer)
{
	unsigned char sendbufrc[10];
	mutexSend.acquire();
	unsigned short servo0=(unsigned short)(1500-(speed.x/2))-150;
	unsigned short servo1=(unsigned short)(1500-(speed.y/2));
	unsigned short servo2=1000;//(unsigned short)(1500+abs(speed.y));
	unsigned short servo3=1000;//(unsigned short)(1500+abs(speed.y));
	sendbufrc[0]=(unsigned char)servo0;
	sendbufrc[1]=(unsigned char)(servo0 >> 8);
	unsigned short tmpp;

	tmpp = (( (unsigned short)sendbufrc[1] << 8) + (unsigned short)sendbufrc[0]);

	sendbufrc[2]=(unsigned char)servo1;
	sendbufrc[3]=(unsigned char)(servo1 >> 8);
	sendbufrc[4]=(unsigned char)servo2;
	sendbufrc[5]=(unsigned char)(servo2 >> 8);
	sendbufrc[6]=(unsigned char)servo3;
	sendbufrc[7]=(unsigned char)(servo3 >> 8);
	short mycrcsend = Crc16((unsigned char*)(sendbufrc),8);
	sendbufrc[8]=(unsigned char)mycrcsend;
	sendbufrc[9]=(unsigned char)(mycrcsend >> 8);
	if(ok)
	{
		if (sendto(sock,(char*)sendbufrc,10,0,(SOCKADDR*)&sin,sizeof(sin))==10) ok=true;
		else {
			ok=false;
			printf("ok send false\n");
		}
	}
	mutexSend.release();
}

void WifibotClient::SendConsigneUdp(CPoint point)
{	
	mutexSend.acquire();	
	unsigned char sbuf[30];
	unsigned char tt=0;
	sbuf[0] = 255;
	sbuf[1] = 0x07;

	unsigned char speed1 = (char)(point.x);
	unsigned char speed2 = (char)(point.y);

	short tmp1 = 8*(speed1&0x3F);
	short tmp2 = 8*(speed2&0x3F);
	if (speed2&0x80) tt=tt+32;
	if (speed2&0x40) tt=tt+16;
	sbuf[2] = (unsigned char)tmp1;
	sbuf[3] = (unsigned char)(tmp1 >> 8);

	sbuf[4] = (unsigned char)tmp2;
	sbuf[5] = (unsigned char)(tmp2 >> 8);

	sbuf[6] = (speed1&0x80) + (speed1&0x40) + tt +0+0+8;//+1 Relay ON +8 10ms pid mode ;

	short mycrcsend = Crc16((unsigned char*)(sbuf+1),6);

	sbuf[7] = (unsigned char)mycrcsend;
	sbuf[8] = (unsigned char)(mycrcsend >> 8);

	if(ok)
	{
		if (sendto(sock,(char*)sbuf,9,0,(SOCKADDR*)&sin,sizeof(sin))==9) ok=true;
		else {
			ok=false;
			printf("ok send false\n");
		}
	}
	mutexSend.release();
}

void WifibotClient::GetSensorData(SensorData *RobotSensors)
{
	mutexRcv.acquire();
	if(!udp)
	{

		short mycrcrcv = (short)((rcvbuftemp[20] << 8) + rcvbuftemp[19]);
		short mycrcsend = Crc16((unsigned char*)rcvbuftemp,18);
		if (mycrcrcv = mycrcsend) {

			unsigned char buffso_send[17];
			buffso_send[0]=(unsigned char)(rcvbuftemp[2]);//GetADC(hUSB,0x48); not speed but batery level
			
			int myspeedL=(int)((rcvbuftemp[1] << 8) + rcvbuftemp[0]);
			if (myspeedL > 32767) myspeedL=myspeedL-65536;
			myspeedL=myspeedL/5;
			buffso_send[1]=myspeedL;
			
			buffso_send[1]=(unsigned char)(((int)((rcvbuftemp[1] << 8) + rcvbuftemp[0]))/5);
			if (buffso_send[1] > 32767) buffso_send[1]=buffso_send[1]-65536;
			
			long  odoL = ((((long)rcvbuftemp[8] << 24))+(((long)rcvbuftemp[7] << 16))+(((long)rcvbuftemp[6] << 8))+((long)rcvbuftemp[5]));
			long odoR = ((((long)rcvbuftemp[16] << 24))+(((long)rcvbuftemp[15] << 16))+(((long)rcvbuftemp[14] << 8))+((long)rcvbuftemp[13]));
			
			buffso_send[2]=(unsigned char)(rcvbuftemp[17]);
			
			int myspeedR=(int)((rcvbuftemp[10] << 8) + rcvbuftemp[9]);
			if (myspeedR > 32767) myspeedR=myspeedR-65536;
			myspeedR=myspeedR/5;
			buffso_send[3]=myspeedR;
					
			buffso_send[4]=(unsigned char)(rcvbuftemp[17]);
			buffso_send[5]=(unsigned char)rcvbuftemp[3];
			buffso_send[6]=(unsigned char)rcvbuftemp[11];
			buffso_send[7]=(unsigned char)odoL;
			buffso_send[8]=(unsigned char)(odoL >> 8);
			buffso_send[9]=(unsigned char)(odoL >> 16);
			buffso_send[10]=(unsigned char)(odoL >> 24);
			buffso_send[11]=(unsigned char)odoR;
			buffso_send[12]=(unsigned char)(odoR >> 8);
			buffso_send[13]=(unsigned char)(odoR >> 16);
			buffso_send[14]=(unsigned char)(odoR >> 24);
			buffso_send[15]=rcvbuftemp[4];
			buffso_send[16]=rcvbuftemp[12];
			//float tmpx = (((float)((unsigned char)buffso_send[2])*0.194-37.5));//data to filter	30A			
			float tmpx = (((float)((unsigned char)buffso_send[2])));
			//float tmpx = (((float)((unsigned char)buffso_send[2])*0.129-25.0));//data to filter 20A			
			RobotSensors->BatVoltage=(unsigned char)buffso_send[0];
			//RobotSensors->SpeedFrontLeft=(int)buffso_send[1];
			RobotSensors->SpeedFrontLeft=myspeedL;
		//	RobotSensors->SpeedRearLeft=(int)(wiix);
			//RobotSensors->SpeedFrontRight=(int)buffso_send[3];
			RobotSensors->SpeedFrontRight=myspeedR;
		//	RobotSensors->SpeedRearRight=(int)(wiix);
			RobotSensors->IRLeft=(unsigned char)buffso_send[5];
			RobotSensors->IRRight=(unsigned char)buffso_send[6];
			RobotSensors->IRLeft2=(unsigned char)buffso_send[15];
			RobotSensors->IRRight2=(unsigned char)buffso_send[16];
			RobotSensors->OdometryLeft = *(long *)(buffso_send+7);
			RobotSensors->OdometryRight = *(long *)(buffso_send+11);	
			//printf(" IR Rear %d \n",RobotSensors->IRLeft2);
		}
	}
	else
	{
#ifndef udpnew
		/*//float tmpx = (((float)((unsigned char)rcvbuftemp[2])*0.194-37.5));//data to filter	30A			
		float tmpx = (((float)((unsigned char)buffso_rcvOUT[87])*0.129-25.0));//data to filter 20A			
		float wiix = ((float)mywiifilter.iir_filter(tmpx*10,&mywiifilter.iir1));
		RobotSensors->BatVoltage=(unsigned char)buffso_rcvOUT[85];
		RobotSensors->SpeedFrontLeft=(int)buffso_rcvOUT[86];
		RobotSensors->SpeedRearLeft=(int)(-wiix);
		RobotSensors->SpeedFrontRight=(int)buffso_rcvOUT[88];
		RobotSensors->SpeedRearRight=(int)(-wiix);
		RobotSensors->IRLeft=(unsigned char)buffso_rcvOUT[90];
		RobotSensors->IRRight=(unsigned char)buffso_rcvOUT[91];
		RobotSensors->OdometryLeft = *(long *)(buffso_rcvOUT+92);
		RobotSensors->OdometryRight = *(long *)(buffso_rcvOUT+96);	
		*/
		RobotSensors->BatVoltage=(unsigned char)buffso_rcvOUT[93]-20;
		RobotSensors->SpeedFrontLeft=(int)buffso_rcvOUT[94];
		RobotSensors->SpeedRearLeft=(int)buffso_rcvOUT[95];
		RobotSensors->SpeedFrontRight=(int)buffso_rcvOUT[96];
		RobotSensors->SpeedRearRight=(int)buffso_rcvOUT[97];
		RobotSensors->IRLeft=(int)buffso_rcvOUT[98];
		RobotSensors->IRRight=(int)buffso_rcvOUT[99];
#endif
#ifdef udpnew

		int mycrcrcv = (short)((((unsigned char)rcvbuftemp[20] << 8) + (unsigned char)rcvbuftemp[19]));;//(short)(((unsigned char)(rcvbuftemp[20] << 8)) + (unsigned char)rcvbuftemp[19]);
		int mycrcsend = Crc16((unsigned char*)rcvbuftemp,18);
		if (mycrcrcv = mycrcsend) {
			char buffso_send[17];
			buffso_send[0]=(char)(rcvbuftemp[2]);//GetADC(hUSB,0x48); not speed but batery level
			
			//buffso_send[1]=(char)(((int)((rcvbuftemp[1] << 8) + rcvbuftemp[0]))/5);
			//if (buffso_send[1] > 32767) buffso_send[1]=buffso_send[1]-65536;
			
			int myspeedL=(int)((rcvbuftemp[1] << 8) + rcvbuftemp[0]);
			if (myspeedL > 32767) myspeedL=myspeedL-65536;
			myspeedL=myspeedL/5;
			buffso_send[1]=myspeedL;

			long  odoL = ((((long)rcvbuftemp[8] << 24))+(((long)rcvbuftemp[7] << 16))+(((long)rcvbuftemp[6] << 8))+((long)rcvbuftemp[5]));
			long odoR = ((((long)rcvbuftemp[16] << 24))+(((long)rcvbuftemp[15] << 16))+(((long)rcvbuftemp[14] << 8))+((long)rcvbuftemp[13]));
			buffso_send[2]=(char)(rcvbuftemp[17]);
			//buffso_send[3]=(char)(((int)(rcvbuftemp[10] << 8) + rcvbuftemp[9])/5);
			//if (buffso_send[3] > 32767) buffso_send[3]=buffso_send[3]-65536;
			
			int myspeedR=(int)((rcvbuftemp[10] << 8) + rcvbuftemp[9]);
			if (myspeedR > 32767) myspeedR=myspeedR-65536;
			myspeedR=myspeedR/5;
			buffso_send[3]=myspeedR;

			buffso_send[4]=(char)(rcvbuftemp[17]);
			buffso_send[5]=(char)rcvbuftemp[3];
			buffso_send[6]=(char)rcvbuftemp[11];
			buffso_send[7]=(char)odoL;
			buffso_send[8]=(char)(odoL >> 8);
			buffso_send[9]=(char)(odoL >> 16);
			buffso_send[10]=(char)(odoL >> 24);
			buffso_send[11]=(char)odoR;
			buffso_send[12]=(char)(odoR >> 8);
			buffso_send[13]=(char)(odoR >> 16);
			buffso_send[14]=(char)(odoR >> 24);
			buffso_send[15]=rcvbuftemp[4];
			buffso_send[16]=rcvbuftemp[12];
			//float tmpx = (((float)((unsigned char)buffso_send[2])*0.194-37.5));//data to filter	30A			
			//float tmpx = (((float)((unsigned char)buffso_send[2])*0.129-25.0));//data to filter 20A			
			float tmpx = (((float)((unsigned char)buffso_send[2])));
//			float wiix = ((float)mywiifilter.iir_filter(tmpx,&mywiifilter.iir1));
			RobotSensors->BatVoltage=(unsigned char)buffso_send[0];
			//RobotSensors->SpeedFrontLeft=(int)buffso_send[1];
			RobotSensors->SpeedFrontLeft=myspeedL;
//			RobotSensors->SpeedRearLeft=(int)(wiix);
			//RobotSensors->SpeedFrontRight=(int)buffso_send[3];
			RobotSensors->SpeedFrontRight=myspeedR;
		//	RobotSensors->SpeedRearRight=(int)(wiix);
			RobotSensors->IRLeft=(unsigned char)buffso_send[5];
			RobotSensors->IRRight=(unsigned char)buffso_send[6];
			RobotSensors->IRLeft2=(unsigned char)buffso_send[15];
			RobotSensors->IRRight2=(unsigned char)buffso_send[16];
			RobotSensors->OdometryLeft = *(long *)(buffso_send+7);
			RobotSensors->OdometryRight = *(long *)(buffso_send+11);
			printf(" IR Rear %d \n",RobotSensors->IRLeft2);
		}
	}
#endif
	//}
	mutexRcv.release();	
}

CPoint WifibotClient::CalculConsigne(CPoint joypoint,int speedl,int speedr)
{
	CPoint calcpoint;

	////debut filtrage	
	moyx=(int)joypoint.x;
	moyy=(int)-joypoint.y;

	if(moyy>=0)
	{
		vmg=(abs(moyx+moyy));
		vmd=(abs(moyx-moyy));
	}
	else if (moyy<0)
	{
		vmg=(abs(moyx-moyy));
		vmd=(abs(moyx+moyy));
	}

	if (ctrlg==128) minmax=60;//
	else minmax=60;

	if (moyy>0) 
	{	
		if (moyx>=0) 
		{
			if(vmg>minmax) vmg=minmax;
			if(moyx>moyy) vmd=0;
		}

		if (moyx<=0) // 
		{
			if(abs(moyx)>moyy) vmg=0;
			if(vmd>minmax) vmd=minmax;
		}
		if (ctrlg==128) if(vmg>minmax)vmg=minmax;
		if (ctrlg==128) if(vmd>minmax)vmd=minmax;
		comg=ctrlg+64+vmg;comd=ctrld+64+vmd;							   //Av
	}	
	else if (moyy<0) 
	{
		if (moyx>=0) 
		{
			if(vmg>minmax) vmg=minmax;
			if(moyx>abs(moyy)) vmd=0;	
		}

		if (moyx<=0)  //
		{
			if(vmd>minmax) vmd=minmax;
			if(moyx<moyy) vmg=0;		
		}
		if (ctrlg==128) if(vmg>minmax)vmg=minmax;
		if (ctrlg==128) if(vmd>minmax)vmd=minmax;
		comg=ctrlg+vmg;comd=ctrld+vmd; 							//Ar
	}

	else 
	{
		if ((moyy==0)&&(moyx<0)) {comg=ctrlg+vmg;comd=ctrld+64+vmd;} 			//Gpivot
		if ((moyy==0)&&(moyx>0)) {comg=ctrlg+64+vmg;comd=ctrld+vmd;}			//Dpivot

		if ((moyx==0)&&(moyy==0)) {stop=1;comg=0+ctrlg;comd=0+ctrld;vmg=0;vmd=0;} //+ctrlg pb !!!!!!!
		//Fin filtrage
	}
	calcpoint.x=comg;
	calcpoint.y=comd;

	return calcpoint;
}

CPoint WifibotClient::CalculConsigneRC(CPoint joypoint,CPoint* speed,CPoint* steer)
{
	CPoint calcpoint;
	int upper=0,lower=0;

	calcpoint.x=3000-joypoint.x;
	calcpoint.y=3000-joypoint.y;

	Servo_Spliter(calcpoint.y,&upper,&lower);
	speed->x=upper;
	speed->y=lower;
	Servo_Spliter(calcpoint.x,&upper,&lower);
	steer->x=upper;
	steer->y=lower;
	return calcpoint;
}

int  WifibotClient::Servo_Spliter(int ServoPos,int *upper,int *lower)
{
	//code to convert integer servo position (500-5500) into 2 7 bit bytes.
	int binaryArray [13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int upperBits[6];
	int upperBitsInt = 0;
	int lowerBits[7];
	int lowerBitsInt = 0;
	char binarystring[13];
	//2^12 == 4096
	int pow = 4096;
	int i;
	//MSB to LSB
	for (i = 0; i < 13; i++)
	{
		if (ServoPos >= pow)
		{
			binaryArray[i] = 1;
			ServoPos = ServoPos - pow;
			binarystring[i] = '1';
		}
		//reduce the power of 2
		pow = pow / 2;
	}
	//LOWER BITS
	//COPY 
	for (i = 6; i < 12; i++)
	{
		lowerBits[i - 6] = binaryArray[i];
	}
	//convert lowerbits to decimal
	pow = 1;
	for (i = 6; i >= 0; i--)  //start at 6 and go down so that we start with the LSB (pow = 1) and go up to the MSB
	{
		if (lowerBits[i] == 1)
		{
			lowerBitsInt = lowerBitsInt + pow;
		}
		pow = pow * 2;
	}
	//UPPER BITS
	//COPY
	for (i = 0; i < 6; i++)
	{
		upperBits[i] = binaryArray[i];
	}
	//convert upper bits to decimal
	pow = 1;
	for (i = 5; i >= 0; i--) //start at 6 and go down so that we start with the LSB (pow = 1) and go up to the MSB
	{
		if (upperBits[i] == 1)
		{
			upperBitsInt = upperBitsInt + pow;
		}
		pow = pow * 2;
	}

	//declare and initalize our first data byte at 0
	int data1 = 0;
	if (ServoPos > 127)
	{
		//set our first data byte (composed of 1 bit) to 1
		data1 = 1;
		ServoPos = ServoPos - 128;
	}
	*upper=upperBitsInt;
	*lower=lowerBitsInt;            
	return 0;
}

CPoint WifibotClient::CalculConstConsigne(CPoint point)
{
	CPoint calcpoint;

	////debut filtrage	
	vmg=point.x;
	vmd=point.y;

	if (ctrlg==128) minmax=40;
	else minmax=60;

	if (vmg>0) 
	{
		comg=ctrlg+64+abs(vmg);							   //Av
	}
	if (vmd>0) 
	{
		comd=ctrld+64+abs(vmd);							   //Av
	}
	if (vmg<0) 
	{
		comg=ctrlg+abs(vmg);							//Ar
	}

	if (vmd<0) 
	{
		comd=ctrld+abs(vmd); 							//Ar
	}

	calcpoint.x=comg;
	calcpoint.y=comd;

	return calcpoint;
}

void WifibotClient::Thread_TCP(void) 
{
	int rcvbites=21;
	while(Thread_TCP_Run)
	{	
		int rcvnbr = recv(socktcp,(char*)rcvbuf,rcvbites,0);

		if (rcvnbr==rcvbites)
		{
			//mutexRcv.acquire();
			memcpy(rcvbuftemp,rcvbuf,rcvbites);
			//mutexRcv.release();
		}		
		//Sleep(1);
	}		
}

DWORD WINAPI WifibotClient::Thread_TCP(LPVOID p)
{
	WifibotClient *me = (WifibotClient *)p;
	me->Thread_TCP();
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef udpnew
void WifibotClient::Thread_TCP_Trooper_In(void) 
{

	// Source address of echo
	SOCKADDR_IN fromAddr;   
	WSAStartup(MAKEWORD(2,2),&wsaOut);
	if (sockOut) closesocket(sockOut);
	sockOut=socket(AF_INET,SOCK_DGRAM,0);//On initialise le socket avec SOCK_DGRAM pour dire qu'on est en UDP

	sinOut.sin_family=AF_INET;

	sinOut.sin_addr.s_addr=inet_addr(myipsend);
	sinOut.sin_port=htons(myportsend+10);
	sinsizeOut=sizeof(sinOut);

	while(Thread_TCP_Trooper_In_Run)
	{		
		// In-out address size for RecvFrom()
		int fromSize = sizeof(fromAddr);

		while (!okOut)
		{
			if (sendto(sockOut,"init",21,0,(SOCKADDR*)&sinOut,sizeof(sinOut)) == 21)
			{
				Sleep(300);
				memset(buffso_rcvOUT,0,3);
				int pp=recvfrom(sockOut,buffso_rcvOUT, 3,0,(SOCKADDR*)&sinOut, &sinsizeOut);
				if(!(strstr(buffso_rcvOUT,"ok")==NULL))
				{
					okOut=true;
					printf("ok rcv \n");
				}
			}
			Sleep(100);
		}

		memset(buffso_sendIN,0,21);
		sprintf_s(buffso_sendIN,"data");

		if (sendto(sockOut,buffso_sendIN,21,0,(SOCKADDR*)&sinOut,sizeof(sinOut))==21)
		{
			//mutexRcv.acquire();
			Sleep(100);
			int pp=recvfrom(sockOut,buffso_rcvOUT, 21,0,(SOCKADDR*)&sinOut, &sinsizeOut);//
			if (pp==21) memcpy(rcvbuftemp,buffso_rcvOUT,21);
		}

	}
}

DWORD WINAPI WifibotClient::Thread_TCP_Trooper_In(LPVOID p)
{
	WifibotClient *me = (WifibotClient *)p;
	me->Thread_TCP_Trooper_In();
	return 0;
}

void WifibotClient::Thread_TCP_Trooper_Out(void) 
{
	WSAStartup(MAKEWORD(2,2),&wsa);
	closesocket(sock);
	sock=socket(AF_INET,SOCK_DGRAM,0);//On initialise le socket avec SOCK_DGRAM pour dire qu'on est en UDP

	sin.sin_family=AF_INET;
	sin.sin_addr.s_addr=inet_addr(myipsend);
	//int res= bind(sock,(SOCKADDR*)&sin,sizeof(sin));
	sin.sin_port=htons(myportsend);
	sinsize=sizeof(sin);
	//int res= bind(sock,(SOCKADDR*)&sin,sizeof(sin));//bug to be tested on SBC

	while(Thread_TCP_Trooper_Out_Run)
	{	
		ok=true;
		Sleep(1000);
	}	
}

DWORD WINAPI WifibotClient::Thread_TCP_Trooper_Out(LPVOID p)
{
	WifibotClient *me = (WifibotClient *)p;
	me->Thread_TCP_Trooper_Out();
	return 0;
}
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef udpnew

void WifibotClient::Thread_TCP_Trooper_In(void) 
{

	// Source address of echo
	SOCKADDR_IN fromAddr;   
	WSAStartup(MAKEWORD(2,2),&wsaOut);
	if (sockOut) closesocket(sockOut);
	sockOut=socket(AF_INET,SOCK_DGRAM,0);//On initialise le socket avec SOCK_DGRAM pour dire qu'on est en UDP

	sinOut.sin_family=AF_INET;

	sinOut.sin_addr.s_addr=inet_addr(myipsend);
	//int res2=bind(sockOut,(SOCKADDR*)&sinOut,sizeof(sinOut));
	sinOut.sin_port=htons(myportsend+10);
	sinsizeOut=sizeof(sinOut);
	//int res2=bind(sockOut,(SOCKADDR*)&sinOut,sizeof(sinOut));

	while(Thread_TCP_Trooper_In_Run)
	{		
		// In-out address size for RecvFrom()
		int fromSize = sizeof(fromAddr); 

		while (!okOut)
		{
			if (sendto(sockOut,"init",20,0,(SOCKADDR*)&sinOut,sizeof(sinOut)) ==20)
			{
				Sleep(300);
				//sendto(sockOut,"init",20,0,(SOCKADDR*)&sinOut,sizeof(sinOut));
				memset(buffso_rcvOUT,0,3);
				//Sleep(1);
				//printf("befor ");
				int pp=recvfrom(sockOut,buffso_rcvOUT, 3,0,(SOCKADDR*)&sinOut, &sinsizeOut);
				//printf("after \n") ;
				if(!(strstr(buffso_rcvOUT,"ok")==NULL))
				{
					okOut=true;
					printf("ok \n");
				}
			}
			//Sleep(10);
		}
		//else
		{/////////////////////////
			memset(buffso_sendIN,0,20);
			sprintf_s(buffso_sendIN,"data");

			if (sendto(sockOut,buffso_sendIN,20,0,(SOCKADDR*)&sinOut,sizeof(sinOut))==20)
			{
				//	mutexRcv.acquire();
				Sleep(5);
				sendto(sockOut,buffso_sendIN,20,0,(SOCKADDR*)&sinOut,sizeof(sinOut));
				//printf("recv befor ");
				int pp=recvfrom(sockOut,buffso_rcvOUT, 100,0,(SOCKADDR*)&sinOut, &sinsizeOut);//
				//printf("recv after %d\n",pp);
				//	mutexRcv.release();	
			}
			else {
				ok=false;
				printf("ok=false \n");
			}
		}////////////////////////
	}
}

DWORD WINAPI WifibotClient::Thread_TCP_Trooper_In(LPVOID p)
{
	WifibotClient *me = (WifibotClient *)p;
	me->Thread_TCP_Trooper_In();
	return 0;
}

void WifibotClient::Thread_TCP_Trooper_Out(void) 
{
	WSAStartup(MAKEWORD(2,2),&wsa);
	closesocket(sock);
	sock=socket(AF_INET,SOCK_DGRAM,0);//On initialise le socket avec SOCK_DGRAM pour dire qu'on est en UDP

	sin.sin_family=AF_INET;
	sin.sin_addr.s_addr=inet_addr(myipsend);
	//int res= bind(sock,(SOCKADDR*)&sin,sizeof(sin));
	sin.sin_port=htons(myportsend);
	sinsize=sizeof(sin);
	//int res= bind(sock,(SOCKADDR*)&sin,sizeof(sin));//bug to be tested on SBC

	while(Thread_TCP_Trooper_Out_Run)
	{	
		if (!ok)
		{
			//	first=true;
			while (!ok)
			{		
				Sleep(600);
				if (sendto(sock,"init",20,0,(SOCKADDR*)&sin,sizeof(sin)) == 20)
				{
					//sendto(sock,"init",20,0,(SOCKADDR*)&sin,sizeof(sin));
					memset(buffso_rcvIN,0,20);
					//Sleep(100);
					int pp=recvfrom(sock,buffso_rcvIN, 3,0,(SOCKADDR*)&sin, &sinsize);
					if(!(strstr(buffso_rcvIN,"ok")==NULL))
					{
						ok=true;
					}
				}
			}
		}
		//else
		{
			memset(buffso_sendOUT,0,20);
			//mutexSend.acquire();
			sprintf_s(buffso_sendOUT,"speed %c %c",pointy,pointx);
			//mutexSend.release();		
		}
		Sleep(1);
	}	

}

DWORD WINAPI WifibotClient::Thread_TCP_Trooper_Out(LPVOID p)
{
	WifibotClient *me = (WifibotClient *)p;
	me->Thread_TCP_Trooper_Out();
	return 0;
}
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef api
Mutex::Mutex ()
{
	mutex = (void*)CreateMutex (0, false, 0);
}

Mutex::~Mutex ()
{
	CloseHandle (mutex);
}

void Mutex::acquire ()
{
	WaitForSingleObject (mutex, INFINITE);
}

int Mutex::acquired ()
{
	return (WaitForSingleObject (mutex, 0) != WAIT_TIMEOUT);
}

void Mutex::release ()
{
	ReleaseMutex (mutex);
}
#endif

short WifibotClient::Crc16(unsigned char *Adresse_tab , unsigned char Taille_max)
{
	unsigned int Crc = 0xFFFF;
	unsigned int Polynome = 0xA001;
	unsigned int CptOctet = 0;
	unsigned int CptBit = 0;
	unsigned int Parity= 0;

	Crc = 0xFFFF;
	Polynome = 0xA001;
	for ( CptOctet= 0 ; CptOctet < Taille_max ; CptOctet++)
	{
		Crc ^= *( Adresse_tab + CptOctet);

		for ( CptBit = 0; CptBit <= 7 ; CptBit++)
		{
			Parity= Crc;
			Crc >>= 1;
			if (Parity%2 == true) Crc ^= Polynome;
		}
	}
	return(Crc);
}
