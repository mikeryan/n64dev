/*
    Send To V64
	Christopher Field

	History:
		25/07/98	Started
*/

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#pragma hdrstop

#include "resource.h"
#include "port.h"
#include "syncv64.h"

#define	PORT_LPT1	1
#define	PORT_LPT2	2
#define PORT_LPT3	3

#define	DEVICE_V64	1
#define DEVICE_Z64	2

#define DIALOG_MAIN		1
#define DIALOG_CONFIG	2

#define	V64_EXTENSION	".V64"
#define Z64_EXTENSION	".Z64"
#define	ROM_EXTENSION	".ROM"
#define	PAL_EXTENSION	".PAL"
#define USA_EXTENSION	".USA"
#define JAP_EXTENSION	".JAP"

// added 150898
#define GB_EXTENSION	".GB"
#define NES_EXTENSION	".NES"
#define SFC_EXTENSION	".SFC"
#define MD_EXTENSION	".MD"
#define PCE_EXTENSION	".PCE"
#define MP3_EXTENSION	".MP3"
#define GG_EXTENSION	".GG"
#define NEO_EXTENSION	".NEO"
#define EMU_EXTENSION	".EMU"

const char	*szDevices[2] = {
		"Doctor v64", "SPC64/WC/Z64"
};

const char	*szPorts[3] = { 
		"LPT1:", "LPT2:", "LPT3:"
};

DWORD	lDevice;
unsigned short int iPort;
unsigned long lSize;

HINSTANCE	hMainInstance;

HWND		hProress;

BOOL		bNeedsFlipping;

BOOL CALLBACK DialogFunc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK ConfigFunc(HWND, UINT, WPARAM, LPARAM);

#define	LPT_1	0x378
#define	LPT_2	0x278
#define LPT_3	0x3BC

BOOL	bZ64Ext, bV64Ext, bROMExt, bJAPExt, bUSAExt, bPALExt;
// added 150898
BOOL	bGBExt, bNESExt, bSFCExt, bGGExt, bMDExt, bNEOExt;
BOOL	bPCEExt, bEMUExt, bMP3Ext;

unsigned short int iBaseport;

DWORD Tid1;
HANDLE hTid1;

void LoadSettings();

typedef struct {
	unsigned char	sValidation1;	// this should be a word
	unsigned char	sValidation2;
	unsigned char	sCompressed;
	unsigned char	sCheck1;		// 0x40
	DWORD			lClockRate;
	DWORD			lPC;
	DWORD			lRelease;
	DWORD			lCRC1;
	DWORD			lCRC2;
	DWORD			lZero1;
	DWORD			lZero2;
	unsigned char	szTitle[20];
	unsigned char	szZeros[7];
	unsigned char	szManufactorer;
	unsigned char	szCartID[2];
	unsigned char	szCountryCode;
	unsigned char	szZero;
} N64HEADER;

N64HEADER	n64header;

char *szFilename;
char szSendingMsg[50];

#define	READY_MESSAGE	"Ready to send."
#define ERROR_NOTROM	"Error. This file does not appear to be a valid N64 ROM image."

char	szEXEFile[255];

BOOL	bROMLoad; 

#define	ABOUT_SCREEN	"SendToV64 1.1a by Christopher Field.\n\nemail:\tsendto@netcomuk.co.uk\nweb:\thttp://www.netcomuk.co.uk/~debug/sendtov64.html\n\nCopyright (c) 1998"


void GetFileFromPath(char szPath[], char *szFile)
{
	int		iLoop1;
	int		iMax;
	
	if (szPath[0] != '\"') return;
	iMax = strlen(szPath) - 1;

	for (iLoop1 = 1; iLoop1 < iMax; iLoop1++) 
	{
		if (szPath[iLoop1] != '\"')
			szFile[iLoop1-1] = szPath[iLoop1];
		else
			break;
	}

	szFile[iLoop1-1] = 0;
}


/*
	Checks HKCU\Chris Field\SendToV64\Configured to see if 
	the program has already been configured, or not
*/
BOOL HasBeenConfiged()
{

	HKEY			hRegUserKey;
	unsigned long	result;
	DWORD	iConfig;
	unsigned long nLen = sizeof(DWORD);

	RegCreateKeyEx(HKEY_CURRENT_USER,
		"SOFTWARE\\Chris Field\\Send To V64",
		0, " ", 0, KEY_ALL_ACCESS,
		NULL, &hRegUserKey, &result);	

	RegQueryValueEx(hRegUserKey, "Configured", 0,
		&result, (LPBYTE)&iConfig, &nLen);

	RegCloseKey(hRegUserKey);			

	if (iConfig == 1) return TRUE;

	return FALSE;

}


BOOL CheckAssociation(char ext[])
{
	HKEY	hRegUserKey;
	DWORD	result;
	char	szAssoc[255];
	DWORD	nBuflen = 255;
	DWORD	lReturn;

	RegCreateKeyEx(HKEY_CLASSES_ROOT,
		ext,
		0, " ", 0, KEY_ALL_ACCESS,
		NULL, &hRegUserKey, &result);	

	lReturn = RegQueryValueEx(hRegUserKey, "", NULL,
		&result, (LPBYTE)&szAssoc, &nBuflen);
	
	RegCloseKey(hRegUserKey);

	if (!strcmpi("N64ROMImage", szAssoc)) return TRUE;

	return FALSE;
}

	
void UnAssociateExtension(char ext[])
{
	HKEY	hRegUserKey;
	DWORD	result;

	RegCreateKeyEx(HKEY_CLASSES_ROOT,
		"",
		0, " ", 0, KEY_ALL_ACCESS,
		NULL, &hRegUserKey, &result);	
	
	RegDeleteKey(hRegUserKey, ext);

	RegCloseKey(hRegUserKey);
}

void AssociateExtension(char ext[])
{

	HKEY			hRegUserKey;

	const char szV64[] = "N64ROMImage";
	unsigned long	result;

	// .v64
	RegCreateKeyEx(HKEY_CLASSES_ROOT,
		ext,
		0, " ", 0, KEY_ALL_ACCESS,
		NULL, &hRegUserKey, &result);	

	RegSetValueEx(hRegUserKey, "", 0, 
		REG_SZ, (LPBYTE)&szV64, sizeof(szV64));

	RegCloseKey(hRegUserKey);	

}

void MakeAssociations()
{

	char			szTemp[500];

	HKEY			hRegUserKey;
	unsigned long	result;

	const char szV64[] = "N64ROMImage";

	const char szV64Desc[] = "N64 Rom File";

	const char szV64Shell[] = "N64ROMImage\\shell\\Open\\Command";

	const char szV64ShellConfig[] = "N64ROMImage\\shell\\Config\\Command";

	const char szV64Icon[] = "N64ROMImage\\DefaultIcon";
	
	// N64ROMImage
	RegCreateKeyEx(HKEY_CLASSES_ROOT,
		szV64,
		0, " ", 0, KEY_ALL_ACCESS,
		NULL, &hRegUserKey, &result);	

	RegSetValueEx(hRegUserKey, "", 0, 
		REG_SZ, (LPBYTE)&szV64Desc, sizeof(szV64Desc));

	RegCloseKey(hRegUserKey);	

	// The icon
	RegCreateKeyEx(HKEY_CLASSES_ROOT,
		szV64Icon,
		0, " ", 0, KEY_ALL_ACCESS,
		NULL, &hRegUserKey, &result);	

	wsprintf(szTemp, "%s,0", szEXEFile);

	RegSetValueEx(hRegUserKey, "", 0, 
		REG_SZ, (LPBYTE)&szTemp, sizeof(szTemp));

	RegCloseKey(hRegUserKey);	

	// N64ROMImage\\Shell
	RegCreateKeyEx(HKEY_CLASSES_ROOT,
		szV64Shell,
		0, " ", 0, KEY_ALL_ACCESS,
		NULL, &hRegUserKey, &result);	

	wsprintf(szTemp, "\"%s\" %%1", szEXEFile);

	RegSetValueEx(hRegUserKey, "", 0, 
		REG_SZ, (LPBYTE)&szTemp, sizeof(szTemp));

	RegCloseKey(hRegUserKey);	


	// N64ROMImage\\Shell\\Config
	RegCreateKeyEx(HKEY_CLASSES_ROOT,
		szV64ShellConfig,
		0, " ", 0, KEY_ALL_ACCESS,
		NULL, &hRegUserKey, &result);	

	wsprintf(szTemp, "\"%s\" /config", szEXEFile);
	RegSetValueEx(hRegUserKey, "", 0, 
		REG_SZ, (LPBYTE)&szTemp, sizeof(szTemp));

	RegCloseKey(hRegUserKey);	

}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst,
				   LPSTR lpszCmds, INT nCmdShow)
{
	
	BOOL bConfig;
	char	sExt[5];

	hMainInstance = hInstance;

	InitCommonControls();

	GetFileFromPath(GetCommandLine(), szEXEFile);

	bConfig = HasBeenConfiged();

	if (bConfig == TRUE) {
		if (lpszCmds[0] == 0) {
			MessageBox (0, "You need to supply a rom file to load", "Error", 0);
			return FALSE;
		};
	} else {
		MessageBox(0, "This is the first time you have run this program.\n\nPlease configure it on the next screen by selecting your options and pressing OK.", "SendToV64", MB_OK | MB_ICONINFORMATION);
	}

	LoadSettings();

	if ((strcmpi(lpszCmds, "/config") == 0) | (bConfig == FALSE)) {
										
		DialogBox(hMainInstance, MAKEINTRESOURCE(IDD_CONFIG), NULL, (DLGPROC)ConfigFunc);	

	} else {
				
		szFilename = (char *)malloc(strlen(lpszCmds)+1);
		strcpy(szFilename, lpszCmds);

		_splitpath(szFilename, NULL, NULL, NULL, sExt);

		strupr(szFilename);

		if (!strcmpi(sExt, V64_EXTENSION) | 
			!strcmpi(sExt, Z64_EXTENSION) | 
			!strcmpi(sExt, ROM_EXTENSION) | 
			!strcmpi(sExt, PAL_EXTENSION) | 
			!strcmpi(sExt, USA_EXTENSION) | 
			!strcmpi(sExt, JAP_EXTENSION)) {

			bROMLoad = TRUE;

			DialogBox(hMainInstance, MAKEINTRESOURCE(IDD_MAINDLG), NULL, (DLGPROC)DialogFunc);

		} else {

			bROMLoad = FALSE;
			
			DialogBox(hMainInstance, MAKEINTRESOURCE(IDD_EXTDLG), NULL, (DLGPROC)DialogFunc);

		}

	};

	return FALSE;
};

void LoadSettings()
{
	HKEY			hRegUserKey;
	unsigned long	result;
	unsigned long	nBuflen;
	unsigned long	lReturn;

	RegCreateKeyEx(HKEY_CURRENT_USER,
		"SOFTWARE\\Chris Field\\Send To V64",
		0, " ", 0, KEY_ALL_ACCESS,
		NULL, &hRegUserKey, &result);	

	nBuflen = sizeof(unsigned short int);

	lReturn = RegQueryValueEx(hRegUserKey, "Port", NULL,
		&result, (LPBYTE)&iPort, &nBuflen);
	if (lReturn != ERROR_SUCCESS) {		
		iPort = PORT_LPT1;
		RegSetValueEx(hRegUserKey, "Port", NULL, 
			REG_BINARY, (LPBYTE)&iPort, sizeof(unsigned short int));
	}

	nBuflen = sizeof(DWORD);

	lReturn = RegQueryValueEx(hRegUserKey, "Device", NULL,
		&result, (LPBYTE)&lDevice, &nBuflen);
	if (lReturn != ERROR_SUCCESS) {
		lDevice = DEVICE_V64;
		RegSetValueEx(hRegUserKey, "Device", NULL, 
			REG_DWORD, (LPBYTE)&lDevice, sizeof(DWORD));
	}

	RegCloseKey(hRegUserKey);	

	bV64Ext = CheckAssociation(V64_EXTENSION);
	bZ64Ext = CheckAssociation(Z64_EXTENSION);
	bROMExt = CheckAssociation(ROM_EXTENSION);

	bPALExt = CheckAssociation(PAL_EXTENSION);
	bJAPExt = CheckAssociation(JAP_EXTENSION);
	bUSAExt = CheckAssociation(USA_EXTENSION);

	bGBExt = CheckAssociation(GB_EXTENSION);
	bNESExt = CheckAssociation(NES_EXTENSION);
	bSFCExt = CheckAssociation(SFC_EXTENSION);
	bGGExt = CheckAssociation(GG_EXTENSION);
	bMDExt = CheckAssociation(MD_EXTENSION);
	bPCEExt = CheckAssociation(PCE_EXTENSION);
	bNEOExt = CheckAssociation(NEO_EXTENSION);
	bEMUExt = CheckAssociation(EMU_EXTENSION);
	bMP3Ext = CheckAssociation(MP3_EXTENSION);

};

void SaveSettings(HWND hdwnd)
{

	HKEY			hRegUserKey;
	unsigned long	result;
	unsigned long	nBuflen;
	DWORD			lTemp;
	DWORD			lConfigured = 1;

	int				iV64Status, iZ64Status, iROMStatus;
	int				iPALStatus, iUSAStatus, iJAPStatus;

	int	iGBStatus, iNESStatus, iSFCStatus, iMDStatus, iGGStatus;
	int iPCEStatus, iMP3Status, iNEOStatus, iEMUStatus;

	/* Get the settins from the form */
	
	// V64 supported only atm.
	lDevice = DEVICE_V64;

	lTemp = SendDlgItemMessage(hdwnd, IDC_LPT1, BM_GETCHECK, 0, 0);
	if (lTemp == 1) {
		iPort = PORT_LPT1;
		iBaseport = LPT_1;
	}
	lTemp = SendDlgItemMessage(hdwnd, IDC_LPT2, BM_GETCHECK, 0, 0);
	if (lTemp == 1) {
		iPort = PORT_LPT2;
		iBaseport = LPT_2;
	}
	lTemp = SendDlgItemMessage(hdwnd, IDC_LPT3, BM_GETCHECK, 0, 0);
	if (lTemp == 1) {
		iPort = PORT_LPT3;
		iBaseport = LPT_3;
	}

	RegCreateKeyEx(HKEY_CURRENT_USER,
		"SOFTWARE\\Chris Field\\Send To V64",
		0, " ", 0, KEY_ALL_ACCESS,
		NULL, &hRegUserKey, &result);	

	nBuflen = sizeof(DWORD);

	RegSetValueEx(hRegUserKey, "Port", 0, 
		REG_BINARY, (LPBYTE)&iPort, sizeof(unsigned short int));
	
	RegSetValueEx(hRegUserKey, "Device", 0, 
		REG_DWORD, (LPBYTE)&lDevice, sizeof(DWORD));

	RegSetValueEx(hRegUserKey, "Configured", 0, 
		REG_DWORD, (LPBYTE)&lConfigured, sizeof(DWORD));

	// File associations

	// Naughty, but it'll do...
	MakeAssociations();

	iV64Status = SendDlgItemMessage(hdwnd, IDC_V64CHECK, BM_GETCHECK, 0, 0);
	iZ64Status = SendDlgItemMessage(hdwnd, IDC_Z64CHECK, BM_GETCHECK, 0, 0);
	iROMStatus = SendDlgItemMessage(hdwnd, IDC_ROMCHECK, BM_GETCHECK, 0, 0);
	iPALStatus = SendDlgItemMessage(hdwnd, IDC_PALCHECK, BM_GETCHECK, 0, 0);
	iJAPStatus = SendDlgItemMessage(hdwnd, IDC_JAPCHECK, BM_GETCHECK, 0, 0);
	iUSAStatus = SendDlgItemMessage(hdwnd, IDC_USACHECK, BM_GETCHECK, 0, 0);

	if (iV64Status) AssociateExtension(V64_EXTENSION);
	else UnAssociateExtension(V64_EXTENSION);

	if (iZ64Status) AssociateExtension(Z64_EXTENSION);
	else UnAssociateExtension(Z64_EXTENSION);

	if (iROMStatus) AssociateExtension(ROM_EXTENSION);
	else UnAssociateExtension(ROM_EXTENSION);

	if (iPALStatus) AssociateExtension(PAL_EXTENSION);
	else UnAssociateExtension(PAL_EXTENSION);

	if (iJAPStatus) AssociateExtension(JAP_EXTENSION);
	else UnAssociateExtension(JAP_EXTENSION);

	if (iUSAStatus) AssociateExtension(USA_EXTENSION);
	else UnAssociateExtension(USA_EXTENSION);


	// Added 140898
	iGBStatus = SendDlgItemMessage(hdwnd, IDC_GB, BM_GETCHECK, 0, 0);
	iNESStatus = SendDlgItemMessage(hdwnd, IDC_NES, BM_GETCHECK, 0, 0);
	iSFCStatus = SendDlgItemMessage(hdwnd, IDC_SFC, BM_GETCHECK, 0, 0);
	iMDStatus = SendDlgItemMessage(hdwnd, IDC_MD, BM_GETCHECK, 0, 0);
	iGGStatus = SendDlgItemMessage(hdwnd, IDC_GG, BM_GETCHECK, 0, 0);
	iPCEStatus = SendDlgItemMessage(hdwnd, IDC_PCE, BM_GETCHECK, 0, 0);
	iNEOStatus = SendDlgItemMessage(hdwnd, IDC_NEO, BM_GETCHECK, 0, 0);
	iMP3Status = SendDlgItemMessage(hdwnd, IDC_MP3, BM_GETCHECK, 0, 0);
	iEMUStatus = SendDlgItemMessage(hdwnd, IDC_EMU, BM_GETCHECK, 0, 0);

	if (iGBStatus) AssociateExtension(GB_EXTENSION);
	else UnAssociateExtension(GB_EXTENSION);

	if (iNESStatus) AssociateExtension(NES_EXTENSION);
	else UnAssociateExtension(NES_EXTENSION);

	if (iSFCStatus) AssociateExtension(SFC_EXTENSION);
	else UnAssociateExtension(SFC_EXTENSION);

	if (iMDStatus) AssociateExtension(MD_EXTENSION);
	else UnAssociateExtension(MD_EXTENSION);

	if (iPCEStatus) AssociateExtension(PCE_EXTENSION);
	else UnAssociateExtension(PCE_EXTENSION);

	if (iGGStatus) AssociateExtension(GG_EXTENSION);
	else UnAssociateExtension(GG_EXTENSION);

	if (iNEOStatus) AssociateExtension(NEO_EXTENSION);
	else UnAssociateExtension(NEO_EXTENSION);

	if (iMP3Status) AssociateExtension(MP3_EXTENSION);
	else UnAssociateExtension(MP3_EXTENSION);

	if (iEMUStatus) AssociateExtension(EMU_EXTENSION);
	else UnAssociateExtension(EMU_EXTENSION);

}

/*
Function: PlaceSettings
Desc:     Transfers the registry settings onto the radio buttons
*/
void PlaceSettings(int iDialog, HWND hdwnd)
{
	switch (iDialog) {
		
		// Main dialog box
		case DIALOG_MAIN:
			switch (iPort) {
			case PORT_LPT1:
				iBaseport = LPT_1;
				break;
			case PORT_LPT2:
				iBaseport = LPT_2;
				break;
			case PORT_LPT3:
				iBaseport = LPT_3;
				break;
			}


			break;
		
		// Configuration screen
		case DIALOG_CONFIG:
//			switch (lDevice) {
			//case DEVICE_V64:
				//SendDlgItemMessage(hdwnd, IDC_DOCTOR, BM_SETCHECK, 1, 0);
				//break;
			//case DEVICE_Z64:
				//SendDlgItemMessage(hdwnd, IDC_Z64, BM_SETCHECK, 1, 0);
				//break;
			//}

			switch (iPort) {
			case PORT_LPT1:
				SendDlgItemMessage(hdwnd, IDC_LPT1, BM_SETCHECK, 1, 0);
				break;
			case PORT_LPT2:
				SendDlgItemMessage(hdwnd, IDC_LPT2, BM_SETCHECK, 1, 0);
				break;
			case PORT_LPT3:
				SendDlgItemMessage(hdwnd, IDC_LPT3, BM_SETCHECK, 1, 0);
				break;
			}

			if (bV64Ext == TRUE) SendDlgItemMessage(hdwnd, IDC_V64CHECK, BM_SETCHECK, 1, 0);
			else SendDlgItemMessage(hdwnd, IDC_V64CHECK, BM_SETCHECK, 0, 0);

			if (bZ64Ext == TRUE) SendDlgItemMessage(hdwnd, IDC_Z64CHECK, BM_SETCHECK, 1, 0);
			else SendDlgItemMessage(hdwnd, IDC_Z64CHECK, BM_SETCHECK, 0, 0);

			if (bROMExt == TRUE) SendDlgItemMessage(hdwnd, IDC_ROMCHECK, BM_SETCHECK, 1, 0);
			else SendDlgItemMessage(hdwnd, IDC_ROMCHECK, BM_SETCHECK, 0, 0);

			if (bPALExt == TRUE) SendDlgItemMessage(hdwnd, IDC_PALCHECK, BM_SETCHECK, 1, 0);
			else SendDlgItemMessage(hdwnd, IDC_PALCHECK, BM_SETCHECK, 0, 0);

			if (bJAPExt == TRUE) SendDlgItemMessage(hdwnd, IDC_JAPCHECK, BM_SETCHECK, 1, 0);
			else SendDlgItemMessage(hdwnd, IDC_JAPCHECK, BM_SETCHECK, 0, 0);

			if (bUSAExt == TRUE) SendDlgItemMessage(hdwnd, IDC_USACHECK, BM_SETCHECK, 1, 0);
			else SendDlgItemMessage(hdwnd, IDC_USACHECK, BM_SETCHECK, 0, 0);

			// Added 150898

			if (bGBExt == TRUE) SendDlgItemMessage(hdwnd, IDC_GB, BM_SETCHECK, 1, 0);
			else SendDlgItemMessage(hdwnd, IDC_GB, BM_SETCHECK, 0, 0);

			if (bNESExt == TRUE) SendDlgItemMessage(hdwnd, IDC_NES, BM_SETCHECK, 1, 0);
			else SendDlgItemMessage(hdwnd, IDC_NES, BM_SETCHECK, 0, 0);

			if (bSFCExt == TRUE) SendDlgItemMessage(hdwnd, IDC_SFC, BM_SETCHECK, 1, 0);
			else SendDlgItemMessage(hdwnd, IDC_SFC, BM_SETCHECK, 0, 0);

			if (bGGExt == TRUE) SendDlgItemMessage(hdwnd, IDC_GG, BM_SETCHECK, 1, 0);
			else SendDlgItemMessage(hdwnd, IDC_GG, BM_SETCHECK, 0, 0);

			if (bMDExt == TRUE) SendDlgItemMessage(hdwnd, IDC_MD, BM_SETCHECK, 1, 0);
			else SendDlgItemMessage(hdwnd, IDC_SFC, BM_SETCHECK, 0, 0);

			if (bNEOExt == TRUE) SendDlgItemMessage(hdwnd, IDC_NEO, BM_SETCHECK, 1, 0);
			else SendDlgItemMessage(hdwnd, IDC_NEO, BM_SETCHECK, 0, 0);

			if (bPCEExt == TRUE) SendDlgItemMessage(hdwnd, IDC_PCE, BM_SETCHECK, 1, 0);
			else SendDlgItemMessage(hdwnd, IDC_PCE, BM_SETCHECK, 0, 0);

			if (bEMUExt == TRUE) SendDlgItemMessage(hdwnd, IDC_EMU, BM_SETCHECK, 1, 0);
			else SendDlgItemMessage(hdwnd, IDC_EMU, BM_SETCHECK, 0, 0);

			if (bMP3Ext == TRUE) SendDlgItemMessage(hdwnd, IDC_MP3, BM_SETCHECK, 1, 0);
			else SendDlgItemMessage(hdwnd, IDC_MP3, BM_SETCHECK, 0, 0);

			break;
	}
}			

void FlipHeader()
{
	int	loop1;
	unsigned char byte1, byte2, byte3;

	 char *pointer;
	
	pointer = (char *)(&n64header);

	for (loop1 = 0; loop1 < sizeof(n64header); loop1+=2) {
		byte1 = *(pointer);
		byte2 = *(pointer+1);
		
		byte3 = byte1;
		byte1 = byte2;
		byte2 = byte3;

		*(pointer) = byte1;
		*(pointer+1) = byte2;

		pointer+=2;
	}

}

int LoadInformation(HWND hdwnd)
{

	FILE	*fp;

	BOOLEAN	bFlipped;
	char	*szErrMsg;	
	char	szTmp[50];

	unsigned long	lFileLen;

	fp = fopen(szFilename, "rb");	
	if (fp == NULL) {
		szErrMsg = (char *)malloc(500);
		wsprintf(szErrMsg, "Unable to locate file %s", szFilename);
		MessageBox(hdwnd, szErrMsg, "Error", 0);
		return 0;
	}

	if (bROMLoad == TRUE)
		fread(&n64header, 1, sizeof(n64header), fp);

	fseek(fp, 0, SEEK_END);

	lFileLen = ftell(fp);

	lSize = lFileLen;

	lFileLen = lSize / 1024 / 1024;

	fclose(fp);

	bNeedsFlipping = FALSE;

	if (bROMLoad == TRUE) {
		if (n64header.sValidation1 == 0x37) {
			// Its a V64, but we want normal so we can read the info
			FlipHeader();
			bFlipped = FALSE;
			bNeedsFlipping = FALSE;
		} else {
			bFlipped = TRUE;
			bNeedsFlipping = TRUE;
		};

		if (n64header.sCheck1 != 0x40) {
			MessageBox(0, ERROR_NOTROM, "SendToV64", 0);
			return 0;
		}

		if (bFlipped == TRUE) {
			SendDlgItemMessage(hdwnd, IDC_FORMAT, WM_SETTEXT, 0, (LPARAM)"SP64/WC/Z64 [Flipping]");
		} else {
			SendDlgItemMessage(hdwnd, IDC_FORMAT, WM_SETTEXT, 0, (LPARAM)"Doctor v64");
		}

		SendDlgItemMessage(hdwnd, IDC_NAME, WM_SETTEXT, 0, (LPARAM)n64header.szTitle);

		switch (n64header.szCountryCode) { 
			case 0x44:
				strcpy(szTmp, "44h - German");
				break;
			case 0x45:
				strcpy(szTmp, "45h - USA");
				break;
			case 0x4A:
				strcpy(szTmp, "4Ah - Japan");
				break;
			case 0x50:
				strcpy(szTmp, "50h - Europe");
				break;
			case 0x55:
				strcpy(szTmp, "55h - Australia");
				break;
			default:
				strcpy(szTmp, "Unknown");
				break;
		};
		
		SendDlgItemMessage(hdwnd, IDC_COUNTRY, WM_SETTEXT, 0, (LPARAM)szTmp);

		wsprintf(szTmp, "%d", lFileLen);

		SendDlgItemMessage(hdwnd, IDC_SIZEMB, WM_SETTEXT, 0, (LPARAM)szTmp);

		wsprintf(szTmp, "%d", lSize >> 17);

		SendDlgItemMessage(hdwnd, IDC_ROMSIZE, WM_SETTEXT, 0, (LPARAM)szTmp);

	};

	// Set up the progress bar
	SendDlgItemMessage(hdwnd, IDC_PROGRESS, PBM_SETRANGE, 0, MAKELONG(0, 100));
	SendDlgItemMessage(hdwnd, IDC_PROGRESS, PBM_SETSTEP, 5, 0);

	SendDlgItemMessage(hdwnd, IDC_PROGRESS, PBM_SETPOS, 0, 0);

	SendDlgItemMessage(hdwnd, IDC_PERCENTDONE, WM_SETTEXT, 0, (LPARAM)"0%");

	return 1;
};

int send(HWND hdwnd)
{
	if (!sync_header(iBaseport)) {
		MessageBox(hdwnd, "Unable to sync with device.\n\nMake sure machine is switched on.", "Error!", MB_ICONERROR);
		return 0;
	} else {			
		if (!sendheader(iBaseport, lSize, szFilename)) {
			MessageBox(hdwnd, "Unable to send header.\n\nMake sure machine is switched on.", "Error!", MB_ICONERROR);	
			return 0;
		} else {
			wsprintf(szSendingMsg, "Sending to %s on %s", szDevices[lDevice-1], szPorts[iPort-1]);
			SendDlgItemMessage(hdwnd, IDC_TITLE, WM_SETTEXT, 0, (LPARAM)szSendingMsg);

			Init64Send(szFilename, iBaseport, hdwnd, IDC_PROGRESS, IDC_PERCENTDONE, IDC_SEND, IDCANCEL, bNeedsFlipping);
			hTid1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)sendfile, 
				(LPVOID) 0, 0, &Tid1);
			SetPriorityClass(hTid1, THREAD_PRIORITY_ABOVE_NORMAL);
		}
	}

	return 1;
}

BOOL CALLBACK DialogFunc(HWND hdwnd, UINT message,
                         WPARAM wParam, LPARAM lParam)
{		
	static int bSending;
	char	szMessage[100];

	switch (message) {
	case WM_INITDIALOG:
		wsprintf(szSendingMsg, READY_MESSAGE);
		SendDlgItemMessage(hdwnd, IDC_TITLE, WM_SETTEXT, 0, (LPARAM)szSendingMsg);
		
		strcpy(szMessage, szFilename);
		SendDlgItemMessage(hdwnd, IDC_EXTRATEXT, WM_SETTEXT, 0, (LPARAM)szMessage);
		
		PlaceSettings(DIALOG_MAIN, hdwnd);

		if (!LoadInformation(hdwnd)) {
			EndDialog(hdwnd, 0);
		}
		break;

	case WM_USER:
		switch (wParam) {
			TerminateThread(hTid1, 0);
			wsprintf(szSendingMsg, READY_MESSAGE);
			SendDlgItemMessage(hdwnd, IDC_TITLE, WM_SETTEXT, 0, (LPARAM)szSendingMsg);
			case SEND_ERROR:				
				MessageBox(hdwnd, "Unable to send to device.", "Error", MB_ICONERROR);
				wsprintf(szSendingMsg, READY_MESSAGE);
				SendDlgItemMessage(hdwnd, IDC_TITLE, WM_SETTEXT, 0, (LPARAM)szSendingMsg);
				break;
			case SEND_COMPLETE:
				MessageBox(hdwnd, "Send Completed.", "SendToV64", MB_OK | MB_ICONINFORMATION);
				SendDlgItemMessage(hdwnd, IDC_PROGRESS, PBM_SETPOS, 0, 0);
				SendDlgItemMessage(hdwnd, IDC_PERCENTDONE, WM_SETTEXT, 0, (LPARAM)"0%");
				break;
			default:
				break;
		};
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam)) {

			case IDC_TEST:
				if (!sync_header(iBaseport)) {
					MessageBox(0, "No Sync", "", 0);
					break;
				};
				write_data(iBaseport, 0x80100400, "hello");
				break;

			case IDC_SEND:
				send(hdwnd);
				break;

			case IDCANCEL:
				EndDialog(hdwnd, 0);
				break;
		}
		break;

	case WM_CLOSE:
		EndDialog(hdwnd, 0);
		break;
	};

	return FALSE;
};


BOOL CALLBACK ConfigFunc(HWND hdwnd, UINT message,
                         WPARAM wParam, LPARAM lParam)
{

	switch (message) {
	case WM_INITDIALOG:
		PlaceSettings(DIALOG_CONFIG, hdwnd);
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam)) {
			case IDOK:
				SaveSettings(hdwnd);
			case IDCANCEL:
				EndDialog(hdwnd, 0);
				break;
			case IDABOUT:
				MessageBox(hdwnd, ABOUT_SCREEN, "About", MB_OK | MB_ICONINFORMATION);
				break;
		}
		break;

	case WM_CLOSE:		
		EndDialog(hdwnd, 0);
		break;
	};

	return FALSE;
};
