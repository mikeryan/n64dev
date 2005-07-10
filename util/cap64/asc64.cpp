#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <dir.h>
#include <fstream.h>

#define set_ai_write outportb(port_a,5);		// ninit=1, nwrite=0
#define set_data_write outportb(port_a,1);	// ninit=0, nwrite=0
#define set_data_read outportb(port_a,0);	// ninit=0, nwrite=1
#define set_normal outportb(port_a,4);		// ninit=1, nwrite=1

char *file_name=NULL;
unsigned int port[2];
unsigned char port_no;
unsigned int port_8,port_9,port_a,port_b,port_c;
int i,j,page,sel,err,wv_mode;
char ch=' ';

void set_ai(unsigned char _ai)
{
   set_ai_write			// ninit=1, nwrite=0
   outportb(port_b,_ai);
}

void set_ai_data(unsigned char _ai,unsigned char _data)
{
   set_ai(_ai);
   set_data_write		// ninit=0, nwrite=0
   outportb(port_c,_data);
}

void init_port(void)
{
   outportb(port_9,1);		// clear EPP time flag
   set_ai_data(6,0x0a);
   set_ai_data(7,0x05);		// 6==0x0a, 7==0x05 is pc_control mode
//   set_ai(5);
//   set_data_read
//   write_en=inportb(port_c);
   set_ai_data(5,1);      // d0=0 is write protect mode
}

void end_port(void)
{
   set_ai_data(5,1);      // d0=0 is write protect mode
   set_ai_data(7,0);		// release pc mode
   set_ai_data(6,0);		// 6==0x0a, 7==0x05 is pc_control mode
   set_normal        // ninit=1, nWrite=1
}

// set up to read from a given addres
void setadr(unsigned long adr) {
   init_port();
   set_ai_data(3,((adr&0x1f000000)>>24)|0x10);
   set_ai_data(2,(adr&0x00ff0000)>>16);
   set_ai_data(1,(adr&0x0000ff00)>>8);
   set_ai_data(0,(adr&0xff));
   set_ai(4);
   set_data_read
   return;
}

unsigned char check_card(void)
{
   init_port();
   set_ai_data(3,0x12);
   set_ai_data(2,0x34);
   set_ai_data(1,0x56);
   set_ai_data(0,0x78);
   set_ai(3);
   set_data_read		// ninit=0, nwrite=1
   if ((inportb(port_c)&0x1f)!=0x12) return(1);
   set_ai(2);
   set_data_read		// ninit=0, nwrite=1
   if (inportb(port_c)!=0x34) return(1);
   set_ai(1);
   set_data_read		// ninit=0, nwrite=1
   if (inportb(port_c)!=0x56) return(1);
   set_ai(0);
   set_data_read		// ninit=0, nwrite=1
   if (inportb(port_c)!=0x78) return(1);
   end_port();
   return(0);
}

void initdrjr(void) {
   char card_present;
   port[0]=peek(0x40,8); 		// lpt1 base address
   port[1]=peek(0x40,10);		// lpt2 base address
   if (port[0]==0){
      printf("No Printer Port Avialable!\07\n");
      exit(-1);
   }

   if (port[1]==0)
      port_no=1;		// only one printer port
   else
      port_no=2;		// two printer port
   card_present=0;
   for (i=0;i<port_no;i++){
      port_8=port[i];
      port_9=port_8+1;
      port_a=port_9+1;
      port_b=port_a+1;
      port_c=port_b+1;
      if (check_card()==0){
	 card_present=1;
	 break;
      }
   }

   if (card_present==0){
      printf("\nNo V64jr card present!!!\07\n\n");
      exit(-1);
   }
   else
      printf("V64jr card found at port%d\nWrite protect mode off: Now a screen shot can be taken\n\n",port_no);

   init_port();
   set_ai(3);
   set_data_read
   inportb(port_c);
}

int main(int argc, char *argv[])
{
   // 0x12-0x13: width (little endian)
   // 0x16-0x17: height (little endian)

   unsigned char bmphead[54] = {
      0x42, 0x4D, 0xB6, 0x13, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00,
      0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x18, 0x01, 0x00, 0x00, 0xF0, 0x00,
      0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x13,
      0x03, 0x00, 0xCE, 0x0E, 0x00, 0x00, 0xD8, 0x0E, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00
   };

   cout << "ASC64 (Accessory to Screen Capture)\n";

   initdrjr();

   unsigned long height,width,depth,t;
   if (argc != 2) {
      cout << "A screen capture utility for the N64 (with Dr. V64 Jr.)\n";
      cout << "Parameter is output file.\n\n";
      cout << "(c) 2003 Halley's Comet Software (http://here.is/halleyscomet)\n";
      cout << "Transfer based on Bung's drjr source.\n";
      end_port();
      return 1;
   }

   setadr(0x400000);
   t=inport(port_c);
   if (t != 0x4147) {
      cout << "Screen shot not found!\n";
      return 1;
   }

   ofstream output(argv[1],ios::binary);

   if (!output) {
      cout << "Failed opening output file.\n";
      return 1;
   }

   cout << "N64 Framebuffer:\n";

   height=inport(port_c);
   width=inport(port_c);
   depth=inport(port_c);

   cout << width << 'x' << height << ", " << depth << " bits per pixel\n";
   bmphead[0x12]=width&0xff;  // width
   bmphead[0x13]=width>>8;
   bmphead[0x16]=height&0xff; // height
   bmphead[0x17]=height>>8;

   output.write(bmphead,54);
   cout << "Writing to BMP..." << flush;
   if (depth==16) {  // 16 bpp version
      unsigned char red, green, blue;

      for (;height>0;height--) {
         for (unsigned int c=0;c<width;c++) {
            setadr(0x400008+(width*(height-1)+c)*2);
            t=inport(port_c);
            blue=(t&0x003e)<<2;
            green=(t&0x07c0)>>3;
            red=(t&0xf800)>>8;
            output.put(blue);
            output.put(green);
            output.put(red);
         }
      }
   } else {
      unsigned char red, green, blue;

      for (;height>0;height--) {
         for (unsigned int c=0;c<width;c++) {
            setadr(0x400008+(width*(height-1)+c)*4);
            t=inport(port_c);
            red=t&0xff;
            green=(t&0xff00)>>8;
            t=inport(port_c);
            blue=t&0xff;
            output.put(blue);
            output.put(green);
            output.put(red);
         }
      }
   }
   cout << "Done.\n";

   output.close();
   end_port();

   return 0;
}

