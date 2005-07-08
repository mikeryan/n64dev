int dexdelete(int dexuxfd, int blocknum, int undeleteflag);
int dexlist(int dexuxfd);
int dexreadfull(int dexuxfd, int fullsaveformat, char *fname);
int dexreadsingle(int dexuxfd, int singlesaveformat, 
                  int blocknum, char *fname);
int dexwrite(int dexuxfd, char *fname);
int dexreadfile(char *fname, char *inputbuffer, char *fileformat, int *imagesize, int *offset);
int dexwritefull(int dexuxfd, char *fname, char *fileformat, unsigned char *pblock);
int dexwritesingle(int dexuxfd, char *fname, char *fileformat, unsigned char *pblock, int imagesize);

