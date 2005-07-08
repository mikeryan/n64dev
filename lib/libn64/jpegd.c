/* jpegd.c - compile with gcc -ljpeg -o jpegd jpegd.c -I/usr/mips-linux/include */

#include <mips.h>
#include "jpegd.h"
#include <stdio.h>
#include <jpeglib.h>
#include <jinclude.h>
#include "libn64.h"

typedef struct {
  struct jpeg_source_mgr pub;	/* public fields */
  void* jpegfile;		/* source address (cart) */
  int length;
  boolean start_of_file;        /* have we gotten any data yet? */
} n64_source_mgr;

typedef n64_source_mgr * n64_src_ptr;

METHODDEF(void)
init_source (j_decompress_ptr cinfo)
{
n64_src_ptr src = (n64_src_ptr) cinfo->src;

src->start_of_file = TRUE;
}

METHODDEF(void)
term_source (j_decompress_ptr cinfo)
{
}

u8 eoi_buf[2]={0xff,JPEG_EOI};
METHODDEF(boolean)
fill_input_buffer (j_decompress_ptr cinfo)
{
  //n64_src_ptr src = (n64_src_ptr) cinfo->src;

  // printf("fill input buffer\n");
  /*
  if (src->start_of_file==TRUE)
    {
      src->pub.next_input_byte=src->jpegfile;
      src->pub.bytes_in_buffer=src->length;
      src->start_of_file=FALSE;
      }*/
  return TRUE;
}

METHODDEF(void)
skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
  n64_src_ptr src = (n64_src_ptr) cinfo->src;
  
  // printf("skip input data (%d bytes)\n",num_bytes);
  src->pub.next_input_byte += (size_t) num_bytes;
  src->pub.bytes_in_buffer -= (size_t) num_bytes;
}

GLOBAL(void)
jpeg_mem_src(j_decompress_ptr cinfo, void *jpegfile,int length)
{
  n64_src_ptr src;

  cinfo->src = (struct jpeg_source_mgr *)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
				SIZEOF(n64_source_mgr));
  src = (n64_src_ptr) cinfo->src;

  src->pub.init_source = init_source;
  src->pub.fill_input_buffer = fill_input_buffer;
  src->pub.skip_input_data = skip_input_data;
  src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
  src->pub.term_source = term_source;

  src->jpegfile=jpegfile;
  src->length=length;
  src->start_of_file=TRUE;
  src->pub.bytes_in_buffer = length; /* forces fill_input_buffer on first read */
  src->pub.next_input_byte = jpegfile ; /* until buffer loaded */
}

void read_JPEG_file (jpegfile *jpgstruct)
{
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;

  JSAMPARRAY buffer;		/* Output row buffer */
  int row_stride;		/* physical row width in output buffer */
  JSAMPROW inptr;
  JDIMENSION col;
  u16 rgba_pixel16;
  u16 *output,*outptr16;
  u8 r,g,b,a;
  u32 rgba_pixel32;
  u32 *outptr32;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);

  jpeg_mem_src(&cinfo,(char*)jpgstruct->jpeg_image,jpgstruct->jpeg_length);

  // printf("reading header\n");
  (void) jpeg_read_header(&cinfo, TRUE);

  //printf("setting dec. parameters\n");
  // set parameters for decompression here
  // scale down image if larger than screen
  if (jpgstruct->autoscale==640 && (cinfo.image_height>480 
				    || cinfo.image_width>640))
    {
      while (cinfo.image_height/cinfo.scale_denom>480 
	     || cinfo.image_width/cinfo.scale_denom>640)
	cinfo.scale_denom*=2;
    }
  
  (void) jpeg_start_decompress(&cinfo);
  // printf("screenbuffer at %x\n",jpgstruct->rgb_image);
  if (jpgstruct->autoscale==640 && jpgstruct->rgb_image!=NULL)
    jpgstruct->rgb_image=(u32*)
      ( (u32)jpgstruct->rgb_image 
	+( ((640-cinfo.output_width)/2+640*((480-(u32)cinfo.output_height)/2)) )
	*(jpgstruct->rgb_bpp/8));
  //printf("rgb image at %x(%x)\n",jpgstruct->rgb_image,(u32)jpgstruct->rgb_image);
  //printf("den %d h %d w %d\n",cinfo.scale_denom,cinfo.output_height,cinfo.output_width);
  //printf("%d %d %d\n",(640-cinfo.output_width)/2,640*((480-(u32)cinfo.output_height)/2),(jpgstruct->rgb_bpp/8));

  // JSAMPLEs per row in output buffer
  row_stride = cinfo.output_width * cinfo.output_components;
  // Make a one-row-high sample array that will go away when done with image
  buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

  // allocate space for decompressed image
  if (jpgstruct->rgb_image==NULL)
    outptr16=(u16*)outptr32=(u32*)output=(u16*)
      malloc((jpgstruct->rgb_bpp/8)*cinfo.output_height*cinfo.output_width);
  else
    outptr16=(u16*)outptr32=(u32*)output=(u16*)jpgstruct->rgb_image;

  while (cinfo.output_scanline < cinfo.output_height) {
    if (jpgstruct->autoscale==640 && jpgstruct->rgb_image!=NULL)
      {
	outptr32=(u32*)((u32)jpgstruct->rgb_image+(640*(u32)cinfo.output_scanline*(jpgstruct->rgb_bpp/8)));
	//printf("scanline %d ptr %x rgbim %x\n",cinfo.output_scanline,outptr32,jpgstruct->rgb_image);
      }

    (void) jpeg_read_scanlines(&cinfo, buffer, 1);

    inptr = buffer[0];

    for (col = cinfo.output_width; col > 0; col--) 
      {
	if (jpgstruct->rgb_bpp==16 && cinfo.output_components==3)
	  {
	    r = *inptr++;
	    g = *inptr++;
	    b = *inptr++;
	    a=0xF;
	    rgba_pixel16=PACK_RGBA5551(r,g,b,a);
	    *outptr16=rgba_pixel16;
	    outptr16++;
	  }
	else if (jpgstruct->rgb_bpp==16 && cinfo.output_components==1)
	  {
	    r=g=b= *inptr++;
	    a=0xF;
	    rgba_pixel16=PACK_RGBA5551(r,g,b,a);
	    *outptr16=rgba_pixel16;
	    outptr16++;
	  }
	else if (jpgstruct->rgb_bpp==32 && cinfo.output_components==3)
	  {
	    r = *inptr++;
	    g = *inptr++;
	    b = *inptr++;
	    a=0xFF;
	    rgba_pixel32=PACK_RGBA8888(r,g,b,a);
	    *outptr32++=rgba_pixel32;
	  }
	else if (jpgstruct->rgb_bpp==32 && cinfo.output_components==1)
	  {
	    r=g=b= *inptr++;
	    a=0xFF;
	    rgba_pixel32=PACK_RGBA8888(r,g,b,a);
	    *outptr32=rgba_pixel32;
	    outptr32++;
	  }
      }

  }

 (void) jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  jpgstruct->rgb_image=output;
  jpgstruct->w=cinfo.output_width;
  jpgstruct->h=cinfo.output_height;

}

/* This is for testing on a PC.. */

#ifdef PC
main(int argc,char *argv[])
{
  FILE *fin,*fout;
  int i,j,length;
  char *jpegptr,*ptr;
  UINT16 rgba_pixel,*outptr;
  jpegfile jpgstruct;
  u8 *jsamp;

  if (argc<2) 
    {
      printf("jpeg file.jpg\n");
      exit(0);
    }
  
  if ((fin=fopen(argv[1],"rb"))==NULL) {
    printf("file not found\n");
    exit(1);
  }
  fseek(fin,0,SEEK_END);
  length=ftell(fin);
  fseek(fin,0,SEEK_SET);
  
  jpegptr=malloc(length);
  ptr=jpegptr;
  while(length-((int)ptr-(int)jpegptr)) 
    ptr+=fread(ptr,1,length-((int)ptr-(int)jpegptr),fin);
  fclose(fin);

  jpgstruct.jpeg_image=(UINT16*)jpegptr;
  jpgstruct.jpeg_length=length;
  jpgstruct.rgb_bpp=32;
  jpgstruct.rgb_image=NULL;
  jpgstruct.autoscale=640;

  read_JPEG_file(&jpgstruct);

  fout=fopen("jpeg.rgb","wb");
  jsamp=(u8*)jpgstruct.rgb_image;
  for (i=0;i<jpgstruct.w*jpgstruct.h;i++)
    for (j=0;j<jpgstruct.rgb_bpp/8;j++)
      fputc(*jsamp++,fout);
  fclose(fout);
}


void print(char *a)
{
  printf(a);
}

putnum(unsigned int a)
{
  printf("%x",a);
}

#endif

