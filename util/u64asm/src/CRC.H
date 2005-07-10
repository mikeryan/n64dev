// CRC calculation routine.

// All important parts by
//  Andreas Sterbenz (stan@sbox.tu-graz.ac.at)
// Who RE'd Nagra's CRC calculator.
// This particular incarnation (as a self-contained function to use on an
//  already open file) by themind.

#define max2(a, b) ( (a)>(b) ? (a) : (b) )
#define min2(a, b) ( (a)<(b) ? (a) : (b) )

#define BUFSIZE 32768

#define CHECKSUM_START 0x1000
#define CHECKSUM_LENGTH 0x100000L
#define CHECKSUM_HEADERPOS 0x10
#define CHECKSUM_END (CHECKSUM_START + CHECKSUM_LENGTH)

#define CHECKSUM_STARTVALUE 0xf8ca4ddc

#define ROL(i, b) (((i)<<(b)) | ((i)>>(32-(b))))

#define BYTES2LONG(b)    ( (((b)[0] & 0xffL) << 24) | \
                           (((b)[1] & 0xffL) << 16) | \
                           (((b)[2] & 0xffL) <<  8) | \
                           (((b)[3] & 0xffL)) )

#define HEADER_MAGIC 0x80371240

/*
  Function crc: calculates the CRC for a file, given its handle and whether
   it's swapped or not.
  
  file1 = handle of the file to be checksummed
  swapped = an int, 0 if original format, 1 if byte-swapped

  returns an error level
*/

int crc(int file1) {
    long flen1=lseek(file1,0,SEEK_END);
    unsigned char * buffer1 = new unsigned char [BUFSIZE];
    if (!buffer1) {cout << "Out of memory.\n"; return 1;}
    
    unsigned long sum1,sum2;
    unsigned long i;
    unsigned long c1, k1, k2;
    unsigned long t1, t2, t3, t4;
    unsigned long t5, t6;
    unsigned int n;
    long clen = CHECKSUM_LENGTH;
    long rlen = flen1 - CHECKSUM_START;

    lseek(file1, CHECKSUM_START, SEEK_SET);

    /* Below is the actual checksum calculation algorithm, which was
       reverse engineered out of Nagra's program.

       As you can see, the algorithm is total crap. Obviously it was
       designed to be difficult to guess and reverse engineer, and not
       to give a good checksum. A simple XOR and ROL 13 would give a
       just as good checksum. The ifs and the data dependent ROL are really
       extreme nonsense.
    */

    t1 = CHECKSUM_STARTVALUE;
    t2 = CHECKSUM_STARTVALUE;
    t3 = CHECKSUM_STARTVALUE;
    t4 = CHECKSUM_STARTVALUE;
    t5 = CHECKSUM_STARTVALUE;
    t6 = CHECKSUM_STARTVALUE;

    for( ;; ) {
      if( rlen > 0 ) {
        n = read(file1,buffer1,min2(BUFSIZE,clen));
        if( (n & 0x03) != 0 ) {
          n += read(file1,buffer1+n,4-(n&3));
        }
      } else {
        n = min2(BUFSIZE, clen);
      }
      if( (n == 0) || ((n&3) != 0) ) {
        if( (clen != 0) || (n != 0) ) {
          cout << "WARNING: Short read, checksum may be incorrect.\n";
        }
        break;
      }
      for( i=0; i<n; i+=4 ) {
        c1 = BYTES2LONG(&buffer1[i]);
        k1 = t6 + c1;
        if( k1 < t6 ) t4++;
        t6 = k1;
        t3 ^= c1;
        k2 = c1 & 0x1f;
        k1 = ROL(c1, k2);
        t5 += k1;
        if( c1 < t2 ) {
          t2 ^= k1;
        } else {
          t2 ^= t6 ^ c1;
        }
        t1 += c1 ^ t5;
      }
      if( rlen > 0 ) {
        rlen -= n;
        if( rlen <= 0 ) memset(buffer1, 0, BUFSIZE);
      }
      clen -= n;
    }
    
    delete [] buffer1;
    
    sum1 = t6 ^ t4 ^ t3;
    sum2 = t5 ^ t2 ^ t1;

    // Write it
    lseek(file1,CHECKSUM_HEADERPOS,SEEK_SET);
    write(file1,(unsigned char *)(&sum1)+3,1);
    write(file1,(unsigned char *)(&sum1)+2,1);
    write(file1,(unsigned char *)(&sum1)+1,1);
    write(file1,(unsigned char *)(&sum1)+0,1);
    write(file1,(unsigned char *)(&sum2)+3,1);
    write(file1,(unsigned char *)(&sum2)+2,1);
    write(file1,(unsigned char *)(&sum2)+1,1);
    write(file1,(unsigned char *)(&sum2)+0,1);
    
    return 0;
}
