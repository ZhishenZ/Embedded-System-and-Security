#include <xmc_common.h>

void initCCU4(void);
void connectLED(void);

int main(void) {


  
  initCCU4();

  while(1);
  return 0;
}

void initCCU4() {
  // release reset of th CCU4
  // we have to configerate the CCU40RS in the PRCLR0 register
  // base address of SCU is 0x50004000 
  // the offset address of PRCLR0 is 0414 (in0x)
  // *(0x50004414) = 0b100;
  SCU_RESET->PRCLR0 = SCU_RESET_PRCLR0_CCU40RS_Msk;
  
  //step 3
  SCU_CLK->CLKSET = SCU_CLK_CLKSET_CCUCEN_Msk;

  //step 4 
  CCU40->GIDLC = CCU8_GIDLC_SPRB_Msk; 
  
  //step 5 the lecturer said it's not necessary to configure this. just skip it. 

  //step 6 we have to configure period and compare in CCU4 

  //about glitch: The glitch occurs because the period and compare value are changed at an improper pointer in time.
  // As I understood it, this occurs because the period and compare value changed inside a duty cycle, to aviod this,
  // we have to introduce the prescaler shadow compare and the time period shadow value.
  CCU40_CC42->PRS = 0xFFFF;
  //CCU40_CC42->PSC = 0b1011; // configure the prescaler to make the LED blinking
  CCU40_CC42->CRS = 0xFFFF*(1-0.01);
  // the "2" from S2SE means "slide 2" from CC42
  // we have to enable this register if we want to request a shadow transfer 
  CCU40->GCSS = CCU4_GCSS_S2SE_Msk;
  

  //step 7 we can ommit this step, because the initial value is not important for this example. 

  //step 8
  CCU40->GIDLC = CCU4_GIDLC_CS2I_Msk; 
  //step 9 Since we do not need to start multiple slices in parallel, it is sufficient to use the TCSET register of the slice
  CCU40_CC42->TCSET = CCU4_CC4_TCSET_TRBS_Msk;

  connectLED();

}

void connectLED() {
  
  // To modify the P1.1 we need to set up the modus of this pin, which should be ALT3 and pull-push 
  // This can be found in the table 25-5
  static const uint8_t PP_ALT3 = 0b10011;


  //we need PORT1 because the LED is P1.1 
  // And for PORT1 we need pin 1
  // In the reference manuel P1.1 can be modified by the register Input/Output Control Register 0 Pn_IOCR0 

  // PORT0_IOCR0_PC1_Msk can be searched by tipping "IOCR0"
  PORT1->IOCR0 = (PORT1->IOCR0 & ~PORT0_IOCR0_PC1_Msk)|(PP_ALT3<<PORT0_IOCR0_PC1_Pos);
  // (PORT1->IOCR0 & ~PORT0_IOCR0_PC1_Msk) means that we delete the content from bit 11 to bit 15 in this register 
  //  we move PP_ALT3 = ob10011 to the 11th bit.
}

