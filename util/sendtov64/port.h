/*
	Accessing ports from Win9x. NOT WINNT.
	Chris Field
*/

unsigned char InportByte(unsigned short int iPort);
unsigned short int InportWord(unsigned short int iPort);
void OutportByte(unsigned short int iPort, unsigned char sByte);
void OutportWord(unsigned short int iPort, unsigned short int iValue);
