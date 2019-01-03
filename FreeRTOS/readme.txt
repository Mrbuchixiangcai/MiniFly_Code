问题解决：
1、Bad operand types (UnDefOT, Constant) for operator (
解决：找这个定义 configMAX_SYSCALL_INTERRUPT_PRIORITY 
再找 configPRIO_BITS
再找 __NVIC_PRIO_BITS
发现定为4U , 去掉U改成4就可以了







