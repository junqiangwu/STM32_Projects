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
// Program layout for starting in ROM, copying test and data to RAM,
// and then running out of RAM.
//

SECTIONS
{

//
// ROM SECTIONS
//

    .ROM.boottext   		        ROM(.boottext ) : > flash_memory
    .ROM.syscall 		          ROM(.syscall) : > . 
    .ROM.intercall 		       ROM(.intercall)	: > .
    .ROM.interfunc 		       ROM(.interfunc)	: > .
    
    .rodata 					        : > flash_memory
    .sdata2 					        : > .

    .secinfo 					        : > . 
    .fixaddr 					        : > .
    .fixtype 					        : > .

    .CROM.sdata  		           CROM(.sdata)	: > .
    .CROM.data 			            CROM(.data) : > .
    .CROM.text  		            CROM(.text) : > .

//
// RAM SECTIONS
//

    .boottext : {
	  // All .text which is reachable between
	  // _start and __ghs_ind_crt1 needs to be
	  // pulled into .boottext and left
	  // uncompressed.
	  crt0.o(.text)
	  libboardinit.a(*)(.text) // optional library
	  libstartup.a(*)(.text)
	  libsys.a(ind_crt1.o)(.text)
     }                                                    > dram_memory
    .syscall					        : > .
    .intercall					        : > .
    .interfunc  				        : > .
    .text 					        : > .
   
    .sdabase 					        : > .
    .sbss 					        : > .
    .sdata 					        : > .
    .data 					        : > .
    .bss 					        : > .
    .heap                  ALIGN(16) PAD(heap_reserve)  : > .
    .stack                 ALIGN(16) PAD(stack_reserve) : > .

//
// These special symbols mark the bounds of RAM and ROM images of boot code.
// They are used by the GHS startup code (_start and __ghs_ind_crt0).
//
    __ghs_rombootcodestart  =  ADDR(.ROM.boottext);
    __ghs_rombootcodeend    =  ENDADDR(.fixtype);
    __ghs_rambootcodestart  =  ADDR(.boottext);
    __ghs_rambootcodeend    =  ENDADDR(.stack);	

//
// These special symbols mark the bounds of RAM and ROM memory.
// They are used by the MULTI debugger.
//
    __ghs_romstart  = MEMADDR(flash_rsvd1);
    __ghs_romend    = MEMENDADDR(flash_rsvd2);
    __ghs_ramstart  = MEMADDR(dram_rsvd1);
    __ghs_ramend    = MEMENDADDR(dram_rsvd2);

//
// This special symbol marks the the first address executed after the
// ROM to RAM copy is complete.  It is used by the MULTI debugger.
//
    __ghs_after_romcopy = __ghs_ind_crt1;
}

