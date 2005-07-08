/* Cache commands */

// write back and invalidate the data cache
void writeBackInvalDCache()
{
  asm (
       "li $8,0x80000000\n"
       "li $9,0x80000000\n"
       "addu $9,$9,0x1FF0\n"
       "cacheloop:\n"
       "cache 1,0($8)\n"
       "cache 1,16($8)\n"
       "cache 1,32($8)\n"
       "cache 1,48($8)\n"
       "cache 1,64($8)\n"
       "cache 1,80($8)\n"
       "cache 1,96($8)\n"
       "addu $8,$8,112\n"
       "bne $8,$9,cacheloop\n"
       "cache 1,0($8)\n"
       : // no outputs
       : // no inputs
       : "$8","$9" // trashed registers
       );
}
