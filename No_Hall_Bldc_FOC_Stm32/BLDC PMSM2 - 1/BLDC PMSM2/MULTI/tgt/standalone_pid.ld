DEFAULTS {

    stack_reserve = 2K
    heap_reserve = 512

}
//
// Program layout for PID built programs.
//
//

SECTIONS
{

//
// The text segment
//

    .picbase                      0x00010000 : 
    .text                                    :
    .syscall                                 :
    .intercall                               :
    .interfunc                               :
    .fixaddr                                 :
    .fixtype                                 :
    .rodata                                  :   
    .secinfo                                 :

//
// The data segment
//
    
    .pidbase                     0x000000000 :
    .sdabase                                 :
    .sbss                                    :
    .sdata                                   :
    .data                                    :
    .bss                                     :
    .heap       ALIGN(16) PAD(heap_reserve)  :
    .stack      ALIGN(16) PAD(stack_reserve) :
}
