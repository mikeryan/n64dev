/*
	Accessing ports from Win9x. NOT WINNT.
	Chris Field
*/

#include "port.h"

unsigned char InportByte(unsigned short int iPort)
{
	unsigned char	bResult;

	_asm {
		mov	dx, iPort
		in	al, dx
		mov	bResult, al
	};

	return bResult;
};

unsigned short int InportWord(unsigned short int iPort)
{

	unsigned short int iResult;

	_asm {
		mov	dx, iPort
		in	ax, dx
		mov	iResult, ax
	};

	return iResult;
};

void OutportByte(unsigned short int iPort, unsigned char sByte)
{

	_asm {
		mov	dx, iPort
		mov	al, sByte
		out	dx, al
	};

};

void OutportWord(unsigned short int iPort, unsigned short int iValue)
{

	_asm {
		mov	dx, iPort
		mov	ax, iValue
		out	dx, ax
	};

};

/*
void OutPort32(int *unbuffer, int read_length,
			   int port_adr, int com_mode)
{


#define	STB_LO	0x05
#define	STB_HI	0x05
#define	BUSY	0x80
#define	ERROR	0x08
#define	XTIMER	20
#define	TIMER	0
#define	SLCT	0x10

	_asm { 
			push ebp
			mov	ebx, unbuffer
			mov	ecx, read_length
			mov	edx, port_adr
			mov	eax, com_mode
			push	esi
			mov	si,cx			;read length
		;port_adr2=port_a(0x37a)
		;*********************************************************
		;  d             3           2           1           0
		;             slct_in       init   auto.fd.xt.     strobe
		;*********************************************************
		;port_adr1=port_9(0x379)
		;*********************************************************
		;  d     7        6       5      4         3
		;       busy     ack     pe     slct     error
		;*********************************************************

			cmp 	ax,0
			jne	new_mode
			in	al,dx
			test	al,1			;check strobe status(D0)
			jnz	out_32k_odd	
		out_32k_even:
			mov	ah,XTIMER		;how many timer value
			mov	cx,TIMER		;timer value
		out_32k_even_1:
			in	al,dx			;port_adr2
			test	al,2			;check auto.fd.xt status(D1)
			jz	out_32k_even_2
			loop	out_32k_even_1
			dec	ah
			jnz	out_32k_even_1
			jmp	out_32k_err
		out_32k_even_2:
			dec	dx
			dec	dx
			mov	al, [ebx]
			out	dx,al				;port_adr : data
			inc	dx
			inc	dx
			mov	al,5		;stb=0
			out	dx,al				;port_adr2
			inc	bx
			dec	si
			jz	out_32k_exit
		out_32k_odd:
			mov	ah,XTIMER		;how many timer value
			mov	cx,TIMER		;timer value
		out_32k_odd_1:
			in	al,dx			;port_adr2
			test	al,2			;check auto.fd.xt status(D1)
			jnz	out_32k_odd_2
			loop	out_32k_odd_1
			dec	ah
			jnz	out_32k_odd_1
			jmp	out_32k_err
		out_32k_odd_2:
			dec	dx
			dec	dx
			mov	al,[ebx]
			out	dx,al				;port_adr : data
			inc	dx
			inc	dx
			mov	al,4		;stb=1
			out	dx,al				;port_adr2
			inc	bx
			dec	si
			jnz	out_32k_even
		out_32k_exit:
			mov	ax,0
			pop	esi
			pop ebp
			ret

		new_mode:				;port_adr2(0x37a)
			in	al,dx
			test	al,1			;check strobe status(D0)
			jnz	new_out_32k_odd	
		new_out_32k_even:
			dec	dx			;point to port_adr1(0x379)
			mov	ah,XTIMER		;how many timer value
			mov	cx,TIMER		;timer value
		new_out_32k_even_1:
			in	al,dx			;port_adr1(0x379)
			test	al,10h 			;check SLCT status(D4)
			jnz	new_out_32k_even_2
			loop	new_out_32k_even_1
			dec	ah
			jnz	new_out_32k_even_1
		out_32k_err:
			mov	ax,si
			pop	si
			ret

		new_out_32k_even_2:
			dec	dx			;point to port_adr0(0x378)
			mov	al,[ebx]
			out	dx,al			;port_adr : data
			inc	dx
			inc	dx
			mov	al,5			;stb=0
			out	dx,al			;port_adr2
			inc	bx
			dec	si
			jz	out_32k_exit
		new_out_32k_odd:
			dec	dx			;point to port_adr1(0x379)
			mov	ah,XTIMER		;how many timer value
			mov	cx,TIMER		;timer value
		new_out_32k_odd_1:
			in	al,dx			;port_adr1(0x379)
			test	al,10h			;check SLCT status(D4)
			jz	new_out_32k_odd_2
			loop	new_out_32k_odd_1
			dec	ah
			jnz	new_out_32k_odd_1
			jmp	out_32k_err
		new_out_32k_odd_2:
			dec	dx			;point to port_adr0(0x378)
			mov	al,[ebx]
			out	dx,al			;port_adr : data
			inc	dx
			inc	dx			;point to port_adr2(0x37a)
			mov	al,4			;stb=1
			out	dx,al			;port_adr2
			inc	bx
			dec	si
			jnz	new_out_32k_even
			jmp	out_32k_exit

	}

}

void OutportWord(unsigned short int iPort, unsigned short int iValue)
{
	OutPort32((int *)iValue, sizeof(iValue), iPort, 1);
}

void OutportByte(unsigned short int iPort, unsigned char sByte)
{
	OutPort32((int *)sByte, sizeof(sByte), iPort, 0);
}
*/