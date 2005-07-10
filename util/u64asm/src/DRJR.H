// Bung's drjr transfer program, modified somewhat to send unswapped
//  files.

#define trans_size 32768
#define set_ai_write outportb(port_a,5);                // ninit=1, nwrite=0
#define set_data_write outportb(port_a,1);      // ninit=0, nwrite=0
#define set_data_read outportb(port_a,0);       // ninit=0, nwrite=1
#define set_normal outportb(port_a,4);          // ninit=1, nwrite=1
char write_en=0;
char test_en=0;
char verify_en=0;
char disp_on=1;
FILE *fptr;
struct mix_buffer {
unsigned char * buffer;
unsigned int * bufferx;
} mix;
unsigned int port[2];
unsigned char port_no;
unsigned int port_8,port_9,port_a,port_b,port_c;
unsigned char disp_buf[16];
int i,j,page,sel,err,wv_mode;
char ch=' ';

void set_ai(unsigned char _ai)
{
   set_ai_write                 // ninit=1, nwrite=0
   outportb(port_b,_ai);
}

void set_ai_data(unsigned char _ai,unsigned char _data)
{
   set_ai(_ai);
   set_data_write               // ninit=0, nwrite=0
   outportb(port_c,_data);
}

void init_port(void)
{
   outportb(port_9,1);          // clear EPP time flag
   set_ai_data(6,0x0a);
   set_ai_data(7,0x05);         // 6==0x0a, 7==0x05 is pc_control mode
   set_ai_data(5,write_en);             // d0=0 is write protect mode
}

void end_port(void)
{
   set_ai_data(5,write_en);             // d0=0 is write protect mode
   set_ai_data(7,0);            // release pc mode
   set_ai_data(6,0);            // 6==0x0a, 7==0x05 is pc_control mode
   set_normal                   // ninit=1, nWrite=1
}

char write_32k(unsigned int hi_word, unsigned int lo_word)
{
   unsigned char unpass,pass1;
   unsigned int i,j;
   unsigned int fix,temp;
   init_port();
   set_ai_data(3,0x10|(hi_word>>8));
   set_ai_data(2,(hi_word & 0xff));
   for (i=0;i<0x40;i++){
      unpass=32;
      while(unpass){
         set_ai_data(1,((i<<1)|lo_word));
         set_ai_data(0,0);
         set_ai(4);             // set address index=4
         set_data_write         // ninit=0, nWrite=0
         fix=i<<8;
         // assume that the file needs to be swapped
         for (j=0;j<256;j++){
            temp=((mix.bufferx[j+fix]&0xff)<<8)|((mix.bufferx[j+fix]&0xff00)>>8);
            outport(port_c,temp);
         }
            set_data_read                  // ninit=0, nwrite=1

            pass1=1;
            for (j=0;j<4;j++){
               temp=inport(port_c);
               temp=((temp&0xff)<<8)|((temp&0xff00)>>8);
               if(mix.bufferx[j+fix]!=temp){
                  pass1=0;
                  break;
               }
            }
            if (pass1) {
               set_ai_data(1,((i<<1)|lo_word|1));
               set_ai_data(0,0xf8);
               set_ai(4);
               set_data_read            // ninit=0, nWrite=1
               for (j=252;j<256;j++){
                  temp=inport(port_c);
                  temp=((temp&0xff)<<8)|((temp&0xff00)>>8);
                  if(mix.bufferx[j+fix]!=temp){
                        break;
                  }
               }
            }

         set_ai(0);
         set_data_read                  // ninit=0, nwrite=1
         if (inportb(port_c)!=0x00){
            unpass--;
            outportb(port_a,0x0b);      // set all pin=0 for debug
            if (disp_on) printf("*");
            init_port();
            set_ai_data(3,0x10|(hi_word>>8));
            set_ai_data(2,(hi_word & 0xff));
            if (unpass==0)
               return(1);
         }
         else
            unpass=0;
      }
   }
   return(0);
}

unsigned char check_card(void)
{
   init_port();
   set_ai_data(3,0x12);
   set_ai_data(2,0x34);
   set_ai_data(1,0x56);
   set_ai_data(0,0x78);
   set_ai(3);
   set_data_read                // ninit=0, nwrite=1
   if ((inportb(port_c)&0x1f)!=0x12) return(1);
   set_ai(2);
   set_data_read                // ninit=0, nwrite=1
   if (inportb(port_c)!=0x34) return(1);
   set_ai(1);
   set_data_read                // ninit=0, nwrite=1
   if (inportb(port_c)!=0x56) return(1);
   set_ai(0);
   set_data_read                // ninit=0, nwrite=1
   if (inportb(port_c)!=0x78) return(1);
   end_port();
   return(0);
}

int read_file(void)
{
   if (fread((char *)mix.buffer,sizeof(char),trans_size,fptr)!=trans_size)
      {
      fclose(fptr);     /* read data error */
      return(-1);
      }
   printf(".");
   return(0);
}

int download_n64(char * outfile)
{
   fptr=fopen(outfile,"rb");
   
   for (page=0;page<0x200;page++) {
      if (read_file()!=0){
         fclose(fptr);
         printf("\n");
         return (0);
      }
      if (write_32k(page,0)) {
         fclose(fptr);
         return(-1);
      }
      if (read_file()!=0){
         fclose(fptr);
         return (-1);
      }
      if (write_32k(page,0x80)) {
         fclose(fptr);
         return(-1);
      }

   }
   printf("\n");
   fclose(fptr);
   end_port();
   return(0);
}

/*************************************************
*                  MAIN ENTRY                    *
*************************************************/
int drjrsend(char * outfile)
{
   char card_present;
   unsigned int dram_size;
   
   port[0]=peek(0x40,8);                // lpt1 base address
   port[1]=peek(0x40,10);               // lpt2 base address
   if (port[0]==0){
      printf("No Printer Port Avialable!\07\n");
      return(-1);
   }

   if (port[1]==0)
      port_no=1;                // only one printer port
   else
      port_no=2;                // two printer port
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
      printf("\nNo V64jr present!!!\07\n\n");
      return(-1);
   }
   init_port();
   set_ai(3);
   set_data_read
   
   mix.buffer = new unsigned char [trans_size];
   mix.bufferx = (unsigned int *)mix.buffer;
   if (!mix.buffer) {printf("Error: Out of memory."); exit(1);}

   disp_on=1;           // display #/*
   if (download_n64(outfile)!=0) {printf("download error !!!\n"); return -1;}
   
   end_port();
   delete [] mix.buffer;
   return(0);
}

