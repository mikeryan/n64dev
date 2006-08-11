#ifndef _SI_h
#define _SI_h

typedef struct SI_regs_s {
    volatile void * DRAM_addr;
    volatile void * PIF_addr_read; // for a read from PIF RAM
    unsigned long reserved1,reserved2;
    volatile void * PIF_addr_write; // for a write to PIF RAM
    unsigned long reserved3;
    unsigned long status;
} _SI_regs_s;

#define SI_status_DMA_busy	(1<<0)
#define SI_status_IO_busy	(1<<1)
#define SI_status_DMA_error	(1<<3)
#define SI_status_interrupt	(1<<12)

typedef struct SI_condat {
    unsigned : 16;
    unsigned err : 2; // error status
    unsigned : 14;

    union {
	struct {
	    unsigned buttons : 16;
	    unsigned : 16;
	};
	struct {
	    unsigned A : 1;
	    unsigned B : 1;
	    unsigned Z : 1;
	    unsigned start : 1;
	    unsigned up : 1;
	    unsigned down : 1;
	    unsigned left : 1;
	    unsigned right : 1;
	    unsigned : 2;
	    unsigned L : 1;
	    unsigned R : 1;
	    unsigned C_up : 1;
	    unsigned C_down : 1;
	    unsigned C_left : 1;
	    unsigned C_right : 1;
	    signed x : 8;
	    signed y : 8;
	};
    };
} _SI_condat;

typedef struct controller_data {
    struct {
	struct SI_condat c[4];
	unsigned long unused[4*8]; // to map directly to PIF block
    };
} _controller_data;

void controller_Read(struct controller_data *);

void controller_exec_PIF(unsigned long long const [8], unsigned long long [8]);

#endif // _SH_h
