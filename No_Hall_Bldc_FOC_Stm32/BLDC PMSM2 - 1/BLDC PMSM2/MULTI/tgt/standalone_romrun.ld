MEMORY {

// 20KB of 32 bit internal RAM starting at 0x20000000

    dram_rsvd1   : ORIGIN = 0x20000000, LENGTH = 0
    dram_memory  : ORIGIN = .,	        LENGTH = 20k
    dram_rsvd2   : ORIGIN = ., 	        LENGTH = 0

// 128KB bit flash starting at 0x08000000

    flash_rsvd1  : ORIGIN = 0x08000000, LENGTH = 0
    flash_memory : ORIGIN = .,          LENGTH = 128k
    flash_rsvd2  : ORIGIN = .,          LENGTH = 0

}
DEFAULTS {

    stack_reserve = 2K
    heap_reserve = 512

}
//
// Program layout for starting in ROM, copying data to RAM,
// and continuing to execute out of ROM.
//

SECTIONS
{

//
// ROM SECTIONS
//

    .vectors						: > flash_memory
    .text 					        : > flash_memory
    .syscall					        : > .
    .intercall					        : > .
    .interfunc  				        : > .

    .rodata 					        : > .
    .sdata2 					        : > .

    .secinfo 					        : > .
    .fixaddr 					        : > .
    .fixtype 					        : > .
    
    .CROM.sdata  	                   CROM(.sdata) : > .
    .CROM.data 	                            CROM(.data) : > .

//
// RAM SECTIONS
//

    .sdabase 					        : > dram_memory	
    .sbss 					        : > .
    .sdata 					        : > .
    .data 					        : > .
    .bss 					        : > .
    .heap                  ALIGN(16) PAD(heap_reserve)  : > .
    .stack                 ALIGN(16) PAD(stack_reserve) : > .


//
// These special symbols mark the bounds of RAM and ROM memory.
// They are used by the MULTI debugger.
//
    __ghs_romstart  = MEMADDR(flash_rsvd1);
    __ghs_romend    = MEMENDADDR(flash_rsvd2);
    __ghs_ramstart  = MEMADDR(dram_rsvd1);
    __ghs_ramend    = MEMENDADDR(dram_rsvd2);

//
// These special symbols mark the bounds of RAM and ROM images of boot code.
// They are used by the GHS startup code (_start and __ghs_ind_crt0).
//
    __ghs_rombootcodestart  = ADDR(.text);
    __ghs_rombootcodeend    = ENDADDR(.fixtype);
    __ghs_rambootcodestart  = 0;
    __ghs_rambootcodeend    = 0;

}

