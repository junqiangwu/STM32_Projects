#include "sys.h"


void inttohex(u16 value,u8 *hex)
{
	u16 x1;
	hex[0]=value/(16*16*16)+'0';
	x1=value%(16*16*16);
	hex[1]=x1/(16*16)+'0';
	x1=value%(16*16);
	hex[2]=x1/16+'0';
	x1=value%16;
	hex[3]=x1+'0';	
	hex[4]=' ';		 

}

