/*
 * Version de la bibliotheque WifiBot distribuee par les professeurs
 * modifiee afin d'etre compatible Unix avec pour seuls dependances
 * les bibliotheques C standard
 * 
 * Cette outil ce veut retro-compatible avec la version qui nous a ete distribue
 */
#ifndef WIFIBOTCLIENT_H
#define WIFIBOTCLIENT_H

#ifdef WIN32

#include <winsock2.h>

#elif defined (linux)

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#else

#error not defined for this platform

#endif

#define CRLF	 "\r\n"

#define BUF_SIZE 22

struct CPoint
{
	int x;
	int y;
};

struct SensorData
{
	int BatVoltage;
	int SpeedFrontLeft;
	int SpeedRearLeft;
	int SpeedFrontRight;
	int SpeedRearRight;
	int IRLeft;
	int IRRight;
	int IRLeft2;
	int IRRight2;
	long OdometryLeft;
	long OdometryRight;
};

class WifibotClient
{
public:
	WifibotClient();
	~WifibotClient();
	bool ConnectToRobot(char* ip,int port);
	void GetSensorData(SensorData*);
	void SendCommand(int left_speed, int right_speed, unsigned char FLAGS);
private:
	void init(void);
	void end(void);
	int init_connection();
	void end_connection();
	int read_server(char *buffer);
	void write_server(const char *buffer);

	short Crc16(unsigned char *Adresse_tab , unsigned char Taille_max);

	SOCKET m_sock; // initialise par init_connection
	char* m_ip; // initialise par ConnectToRobot
	int m_port; // initialise par ConnectToRobot
	unsigned char rcvbuf[BUF_SIZE];
};

#endif // WIFIBOTCLIENT_H
