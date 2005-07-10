#include "N64sys.h"

unsigned long read_count(void) {
    unsigned long count;
    // reg $9 on COP0 is count
    asm("\tmfc0 %0,$9\n\tnop":"=r"(count));

    return count;
}
