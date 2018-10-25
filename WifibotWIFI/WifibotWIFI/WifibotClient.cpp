#include "WifibotClient.h"

/*
 * @brief Initialisation des resources necessaires pour le reseau
 * Prepare la creation du client mais n'initialise pas la connection
 */
WifibotClient::WifibotClient()
{
	init();
}

/*
 * @brief Ferme la connection reseau et libere les resources
 */
WifibotClient::~WifibotClient()
{
	end_connection();
	end();
}

/*
 * @brief Initialise la connection au WifiBot
 * Retro compatibilite avec la version de la bibliotheque donnee
 * par les professeurs
 */
bool WifibotClient::ConnectToRobot(char* ip,int port)
{
	m_ip = ip;
	m_port = port;
	init_connection();
}

/*
 * @brief Reception des donnees des capteurs
 * Retro compatibilite avec la version de la bibliotheque donnee
 * par les professeurs
 */
void WifibotClient::GetSensorData(SensorData* RobotSensors)
{
	int n = read_server(rcvbuf);
	unsigned char rcvbuftemp[BUF_SIZE];

	// Utilisation d'un buffer intermediaire
	std::memcpy(rcvbuftemp,rcvbuf, BUF_SIZE-2);

	// Verification d'integrite des paquets
	short mycrcrcv = (short)((rcvbuftemp[20] << 8) + rcvbuftemp[19]);
	short mycrcsend = Crc16((unsigned char*)rcvbuftemp,18);
	if (mycrcrcv != mycrcsend) {
		std::cout<<"CRC Error"<<std::endl;
		return;
	}

	// Reagencement des donnes recues
	unsigned char buffso_send[17];
	buffso_send[0]=(unsigned char)(rcvbuftemp[2]);
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
	float tmpx = (((float)((unsigned char)buffso_send[2])));		

	// Retour des valeurs
	RobotSensors->BatVoltage=(unsigned char)buffso_send[0];
	RobotSensors->SpeedFrontLeft=myspeedL;
	RobotSensors->SpeedFrontRight=myspeedR;
	RobotSensors->IRLeft=(unsigned char)buffso_send[5];
	RobotSensors->IRRight=(unsigned char)buffso_send[6];
	RobotSensors->IRLeft2=(unsigned char)buffso_send[15];
	RobotSensors->IRRight2=(unsigned char)buffso_send[16];
	RobotSensors->OdometryLeft = *(long *)(buffso_send+7);
	RobotSensors->OdometryRight = *(long *)(buffso_send+11);	
}

/*
 * @brief envoie d'une commande de vitesse au moteurs du WifiBot
 * Retro compatibilite avec la version de la bibliotheque donnee
 * par les professeurs
 */
void WifibotClient::SendCommand(int left_speed, int right_speed, unsigned char
flags)
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
	write_server(sbuf);
}

/*
 * @brief initialisation de la connection necessaire pour la
 * compatibilite windows
 */
void WifibotClient::init(void)
{
#ifdef WIN32
   WSADATA wsa;
   int err = WSAStartup(MAKEWORD(2, 2), &wsa);
   if(err < 0)
   {
      puts("WSAStartup failed !");
      exit(EXIT_FAILURE);
   }
#endif
}

/*
 * @brief liberation des ressources, necessaire pour la compatibilite avec
 * windows
 */
void WifibotClient::end(void)
{
#ifdef WIN32
   WSACleanup();
#endif
}

/*
 * @brief Initialisation de la connection tcp avec un minimum de dependances
 * les seuls dependances sont les biblitheques standards C
 * l'adresse IP et le port doit etre renseigne dans les attributs de la classe
 */
int WifibotClient::init_connection()
{
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   SOCKADDR_IN sin = { 0 };
   struct hostent *hostinfo;

   if(sock == INVALID_SOCKET)
   {
      std::cout<<"Impossible de creer un socket, ceci peut etre du a un probleme de permissions"<<std::endl;
      perror("socket()");
      exit(errno);
   }

   hostinfo = gethostbyname(m_ip);
   if (hostinfo == NULL)
   {
      std::cout<<"Host "<<m_ip<<" introuvable - probleme wifi ou reboot"<<std::endl;
      fprintf (stderr, "Unknown host %s.\n", m_ip);
      exit(EXIT_FAILURE);
   }

   sin.sin_addr = *(IN_ADDR *) hostinfo->h_addr;
   sin.sin_port = htons(m_port);
   sin.sin_family = AF_INET;

   if(connect(sock,(SOCKADDR *) &sin, sizeof(SOCKADDR)) == SOCKET_ERROR)
   {
      std::cout<<"Erreur de connection inconnue"<<std::endl;
      perror("connect()");
      exit(errno);
   }

   std::cout<<"Connection etablie avec success - "<<m_ip<<":"<<m_port<<std::endl;

   m_sock = sock;
   return sock;
}

/*
 * @brief Fermeture de la connection TCP
 */
void WifibotClient::end_connection()
{
	closesocket(m_sock);
}

/*
 * @brief calcul du CRC (detection d'erreur)
 * Fonction tel que definie dans la documentation du wifibot et implementation
 * retro-compatible avec la version distribue par les professeurs
 */
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

/*
 * @brief redefinition de strlen pour les caracteres non signes
 */
size_t ustrlen(const unsigned char *buffer)
{
	int i=0;
	while(buffer[i] != 0) i++;
	return i;
}

/*
 * @brief envoie de donnees au serveur
 * la connection doit etre prealablement initialisee
 * dans la classe (attribut m_sock)
 */
void WifibotClient::write_server(const unsigned char *buffer)
{
   if(send(m_sock, buffer, BUF_SIZE-1, 0) < 0)
   {
      std::cout<<"Send Error"<<std::endl;
      perror("send()");
      exit(errno);
   }
}

/*
 * @brief reception de donnees du serveur
 * la connection doit etre prealablement initialisee
 * dans la classe (attribut m_sock)
 */
int WifibotClient::read_server(unsigned char *buffer)
{
	int n = 0;

	if((n = recv(m_sock, buffer, BUF_SIZE - 1, 0)) < 0)
	{
		std::cout<<"Receive Error"<<std::endl;
		perror("recv()");
		exit(errno);
	}

	buffer[n] = 0;

	return n;
}
