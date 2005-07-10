; The pages of NES memory.
; The IO read/write functions will look into this table based on the high
;  byte of the address. The address in the table will be added to the
;  value that was used to look it up.

npt_init:
; the read table
        dcw     0x08,mem
        dcw     0x08,mem-0x800
        dcw     0x08,mem-0x1000
        dcw     0x08,mem-0x1800
        dcw     0x20,0 ; registers
        dcw     0x20,0 ; registers and 'expansion ROM'?
        dcw     0x20,sram-0x6000
        dcw     0x40,prgrom-0x8000
        dcw     0x40,prgrom-0xC000

; the write table
        dcw     0x08,mem
        dcw     0x08,mem-0x800
        dcw     0x08,mem-0x1000
        dcw     0x08,mem-0x1800
        dcw     0x20,0 ; registers
        dcw     0x20,0 ; registers and 'expansion ROM'?
        dcw     0x20,sram-0x6000
        dcw     0x40,0 ; a write to ROM? it must be a mapper!
        dcw     0x40,0 ; ditto
