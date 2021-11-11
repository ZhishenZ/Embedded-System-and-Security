#include "stdio.h"

//dimming.c
int main(void) {
    
int Count;  
int uid[16] =    {193, 128, 17, 11, 19, 96, 135, 228, 130, 6, 0, 16, 10, 0, 0, 0};

  for(Count=0; Count < 16; Count++)
{
printf("%x\n",uid[Count]);
//UCIDptr++;
}


}