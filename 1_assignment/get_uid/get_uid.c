#include <xmc_common.h>

//dimming.c
int main(void) {


g_chipid;

//uint32_t *UCIDptr;
uint32_t UniqueChipID[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint32_t Count;

//UCIDptr = SCU_GENERAL->IDCHIP;//0x50004004
// UCIDptr = 0x20000000;
  
  // initCCU4();
  for(Count=0; Count < 16; Count++)
{
UniqueChipID[Count] = g_chipid[Count];
//UCIDptr++;
}

  while(1);
  return 0;
}
