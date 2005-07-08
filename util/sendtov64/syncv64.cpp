/*
	Syncronisation and Sending of ROMS to V64
	Chris Field
*/

#include <stdio.h>
#include <windows.h>
#include <time.h>
#include "port.h"
#include "commctrl.h"

#include "syncv64.h"

#define MAX(a, b)	((a > b) ? a : b)
#define MIN(a, b)	((a > b) ? b : a)

#define SEND_MAX_WAIT 0x10000
#define SYNC_MAX_CNT	8192
#define	BUFFER_SIZE		32768

void flipbuffer(char buffer[])
{ 
	// flips the buffer of size BUFFER_SIZE

	char	byte1;

	int		iLoop1;

	for (iLoop1 = 0; iLoop1 < BUFFER_SIZE; iLoop1+=2) {
		byte1 = buffer[iLoop1];
		buffer[iLoop1] = buffer[iLoop1+1];
		buffer[iLoop1+1] = byte1;
	}

}

int sendbytes(unsigned short int iPort, int iLen, char szBytes[])
{
	int iLoop1;
	long lMaxWait;

	for (iLoop1 = 0; iLoop1 < iLen; iLoop1++) {
		lMaxWait = SEND_MAX_WAIT;
		if ((InportByte(iPort+2) & 1) == 0) {
			while (((InportByte(iPort+2) & 2) != 0) && lMaxWait--);
			if (lMaxWait <= 0) return 0;
			OutportByte(iPort, szBytes[iLoop1]);
			OutportByte(iPort+2, 5);
		} else {
			while (((InportByte(iPort+2) & 2) == 0) && lMaxWait--);
			if (lMaxWait <= 0) return 0;
			OutportByte(iPort, szBytes[iLoop1]);
			OutportByte(iPort+2, 4);
		};
	}

	return 1;
};

int write_data(unsigned short int iPort, unsigned int iAddress,
	char szData[])
{
	long iLen;
	char *szTemp;
	long lMaxWait;

	static char protocolId[] = "GD6EW\1";
	
	if (!sendbytes(iPort, strlen(protocolId), protocolId)) { 
		MessageBox(0, "Unable to send header.", protocolId, 0);
		return 0;
	}

	iLen = strlen(szData);

	szTemp = (char *)malloc(iLen+sizeof(iAddress)+sizeof(iLen));

	szTemp[0] = (iAddress>>24) & 0x0ff;
	szTemp[1] = (iAddress>>16) & 0x0ff;
	szTemp[2] = (iAddress>>8) & 0x0ff;
	szTemp[3] = (iAddress) & 0x0ff;
	szTemp[4] = (iLen >> 24) & 0xff;
	szTemp[5] = (iLen >> 16) & 0xff;
	szTemp[6] = (iLen >> 8) & 0xff;
	szTemp[7] = (iLen) & 0xff;

	iLen = strlen(szTemp);
	if (!sendbytes(iPort, 8, szTemp)) {
		free(szTemp);
		return 0;
	}

	free(szTemp);

	iLen = strlen(szData);
	if (!sendbytes(iPort, iLen, szData)) {
		return 0;
	}

	return 1;
}

int send_filename(unsigned short int iPort, char name[])
{
	int	i;
	char	*c;
	char	mname[12];

	memset(mname, ' ', 11);
	c = MAX(strrchr(name, '/'), strrchr(name, '\\'));
	if (c == NULL) {
		c = name;
	} else {
		c++;
	}

	for (i = 0; i < 8 && *c != '.' && *c != '\0';i++, c++) mname[i] = *c;

	c = strrchr(c, '.');

	if (c != NULL) {
		c++;
		for (i = 8; i < 11 && *c != '\0'; i++, c++) mname[i] = *c;
	}

	return sendbytes(iPort, 11, mname);
};

int send_filelen(unsigned short int iPort, long lLength)
{
	
	char	lenbuffer[4];

	lenbuffer[0] = (lLength);
	lenbuffer[1] = (lLength>>8);
	lenbuffer[2] = (lLength>>16);
	lenbuffer[3] = (lLength>>24);
	
	return sendbytes(iPort, 4, lenbuffer);
		
}

int sendheader(unsigned short int iPort, long lLength, char name[])
{
	
	static char protocolId[] = "GD6R\1";

	//char	szTmp[100];
	//wsprintf(szTmp, "%.4d %.4d %s", iPort, lLength, name);
	//MessageBox(0, "", szTmp, 0);

	if (!sendbytes(iPort, strlen(protocolId), protocolId)) return 0;

	if (!send_filelen(iPort, lLength)) return 0;

	if (!send_filename(iPort, name)) return 0;

	return 1;
}


int sync_header(unsigned short int iPort)
{

  int i=0;

  OutportByte(iPort, 0);                        /* data = 00000000    */
  OutportByte(iPort+2, 4);                      /* ~strobe=0          */
  while( i<SYNC_MAX_CNT ) {
    if( (InportByte(iPort+2) & 8) == 0 ) {      /* wait for select=0  */
      OutportByte(iPort, 0xaa);                 /* data = 10101010    */
      OutportByte(iPort+2, 0);                  /* ~strobe=0, ~init=0 */
      while( i<SYNC_MAX_CNT ) {
        if( (InportByte(iPort+2) & 8) != 0 ) {  /* wait for select=1  */
          OutportByte(iPort+2, 4);              /* ~strobe=0          */
          while( i<SYNC_MAX_CNT ) {
            if( (InportByte(iPort+2) & 8) == 0 ) { /* w for select=0  */
              OutportByte(iPort, 0x55);         /* data = 01010101    */
              OutportByte(iPort+2, 0);          /* ~strobe=0, ~init=0 */
              while( i<SYNC_MAX_CNT ) {
                if( (InportByte(iPort+2) & 8) != 0 ) {  /* w select=1 */
                  OutportByte(iPort+2, 4);      /* ~strobe=0          */
                  while( i<SYNC_MAX_CNT ) {
                    if( ((iPort+2) & 8) == 0) { /* select=0 */
                      return 0;
                    }
                    i++;
                  }
                }
                i++;
              }
            }
            i++;
          }
        }
        i++;
      }
      i++;
    }
    i++;
  }
  OutportByte(iPort+2, 4);
  return 1;

}

char	sV64Filename[255];
unsigned short i64Port;
HWND	hDialogBox;
DWORD	iPercentControl;
DWORD	iTextControl;
DWORD	iSendButton;
BOOL	bFlip;
DWORD	iCancelButton;

void Init64Send(char sFilename[], unsigned short int iPort,
	HWND hdwnd, DWORD iPercent, DWORD iText, DWORD iSend, DWORD iCancel,
	BOOL bByteFlip)
{
	strcpy(sV64Filename, sFilename);
	i64Port = iPort;
	hDialogBox = hdwnd;
	iPercentControl = iPercent;
	iTextControl = iText;	
	iSendButton = iSend;
	iCancelButton = iCancel;
	bFlip = bByteFlip;
};

DWORD sendfile(LPVOID param)
{
	char			buffer[BUFFER_SIZE];
	FILE	*fp;
	unsigned long	lFileLen;
	unsigned long	lRead, lToRead, lTotalRead;

	UINT	iPerDone;
	char	szTmp[100];

	fp = fopen(sV64Filename, "rb");
	if (fp == NULL) return 0;

	fseek(fp, 0, SEEK_END);	
	lFileLen = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	lToRead = lFileLen;
	lTotalRead = 0;

	//SendDlgItemMessage(hDialogBox, iSendButton, WS_DISABLED, 0, 0);
	EnableWindow(GetDlgItem(hDialogBox, iSendButton), FALSE);
	SetFocus(GetDlgItem(hDialogBox, iCancelButton));
	

	for (;;) {
		lRead = fread(buffer, 1, MIN(BUFFER_SIZE, lToRead), fp);
		if (lRead < 0) {
			MessageBox(hDialogBox, "File finished early. Possible error. Retry.", "Error", 0);
			break;
		};

		if (lToRead <= 0) break;		

		lToRead -= lRead;
		lTotalRead += lRead;

		if (bFlip == TRUE) {
			flipbuffer(buffer);
		};

		if (!sendbytes(i64Port, lRead, buffer)) {
			fclose(fp);
			EnableWindow(GetDlgItem(hDialogBox, iSendButton), TRUE);
			SendMessage(hDialogBox, WM_USER, SEND_ERROR, 0);
			return 0;
		};		

		iPerDone = (100 * lTotalRead) / lFileLen;

		wsprintf(szTmp, "%ld%%", iPerDone);
		SendDlgItemMessage(hDialogBox, iTextControl, WM_SETTEXT, 0, (LPARAM)szTmp);
		SendDlgItemMessage(hDialogBox, iPercentControl, PBM_SETPOS, iPerDone, 0);

	};

	fclose(fp);

	EnableWindow(GetDlgItem(hDialogBox, iSendButton), TRUE);

	SendMessage(hDialogBox, WM_USER, SEND_COMPLETE, 0);

	return 0;
}