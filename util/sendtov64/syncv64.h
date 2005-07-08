/*
	Syncronisation and Sending of ROMS to V64
	Chris Field
*/


int sendbytes(unsigned short int iPort, int iLen, char szBytes[]);
int write_data(unsigned short int iPort, unsigned int iAddress,
	char szData[]);
int send_filename(unsigned short int iPort, char name[]);
int send_filelen(unsigned short int iPort, long lLength);
int sendheader(unsigned short int iPort, long lLength, char name[]);
int sync_header(unsigned short int iPort);	
void Init64Send(char sFilename[], unsigned short int iPort,
	HWND hdwnd, DWORD iPercent, DWORD iText, DWORD iSend, DWORD iCancel,
	BOOL bByteFlip);
DWORD sendfile(LPVOID param);

#define SEND_ERROR		1
#define SEND_COMPLETE	2