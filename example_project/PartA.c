#include <xmc_gpio.h>
#include <xmc_ccu4.h>
#include <stdio.h>
#include <string.h>
//#include <stdlib.h>
//#include <search.h>

static char characters[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','U','X','Y','Z',' '};
static char *morsecode[] = {".-","-...","-.-.","-..",".","..-.","--.","....","..",".---","-.-",".-..","--","-.","---",".--.","--.-",".-.","...","-","..-","...-",".--","-..-","-.--","--..","0"};
static char site[] = "ABC";
volatile uint8_t led_on_off_k=0;
uint8_t intoutput[88];
uint8_t intoutputk=0;
/*10100000001110101
1101000101
1100011101
0000000111
0111000111
0111011100
0101110100
0101010001
*/

const XMC_GPIO_CONFIG_t LED_config = \
    {.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL ,\
     .output_level = XMC_GPIO_OUTPUT_LEVEL_LOW ,\
     .output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE };

volatile uint32_t CCU4_periods = 0;

XMC_CCU4_SLICE_COMPARE_CONFIG_t CCU_compare_config =
    {.timer_mode = 0U /*XMC_CCU4_SLICE_TIMER_COUNT_MODE_EA*/,
    .monoshot = false ,
    .shadow_xfer_clear = 0U ,
    .dither_timer_period = 0U ,
    .dither_duty_cycle = 0U ,
    .prescaler_mode = XMC_CCU4_SLICE_PRESCALER_4 /*XMC_CCU4_SLICE_PRESCALER_MODE_NORMAL*/ ,
    .mcm_enable = 0U ,
    .prescaler_initval = 0xAU , /* 1/1024 ( Table 22 -7 p . 22 -54) */
    .float_limit = 0U ,
    .dither_limit = 0U ,
    .passive_level = 0U/*XMC_CCU4_SLICE_OUTPUT_PASSIVE_LEVEL_LOW*/ ,
    .timer_concatenation = 0U
    };
    

void CCU40_0_IRQHandler ( void ) {
  //CCU4_periods ++;
  //XMC_GPIO_ToggleOutput ( XMC_GPIO_PORT1 , 0) ;
  if(led_on_off_k < intoutputk){
    if(intoutput[led_on_off_k]==1)
      XMC_GPIO_SetOutputLevel (XMC_GPIO_PORT1, 1, XMC_GPIO_OUTPUT_LEVEL_HIGH);//XMC_GPIO_OUTPUT_LEVEL_HIGH
    else 
      XMC_GPIO_SetOutputLevel (XMC_GPIO_PORT1, 1, XMC_GPIO_OUTPUT_LEVEL_LOW);//XMC_GPIO_OUTPUT_LEVEL_HIGH

    led_on_off_k++;
  }else{
    led_on_off_k = 0;
    XMC_GPIO_SetOutputLevel (XMC_GPIO_PORT1, 1, XMC_GPIO_OUTPUT_LEVEL_LOW);
  }
}

int main ( void ) {
  
  morse();


  XMC_GPIO_Init(XMC_GPIO_PORT1 , 1 , & LED_config ) ;
  /* Initialize CCU40 , the MCMS transfer is irrelevant for our
  * application . Calls EnableModule ( SCU enable clock , SCU ungate
  * clock , SCU deassert reset ) and StartPrescaler ( GIDLC . SPRB ) */
  XMC_CCU4_Init( CCU40 , XMC_CCU4_SLICE_MCMS_ACTION_TRANSFER_PR_CR ) ;
  XMC_CCU4_SLICE_CompareInit ( CCU40_CC40 , & CCU_compare_config ) ;
  XMC_CCU4_SLICE_SetTimerPeriodMatch ( CCU40_CC40 , 0x7530 ) ;
  XMC_CCU4_EnableShadowTransfer ( CCU40 , XMC_CCU4_SHADOW_TRANSFER_SLICE_0 ) ;
  /* Map IRQ to NVIC IRQ input , enable IRQ in CCU4 */
  XMC_CCU4_SLICE_SetInterruptNode( CCU40_CC40 ,\
      XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH ,\
      XMC_CCU4_SLICE_SR_ID_0 ) ;
  XMC_CCU4_SLICE_EnableEvent( CCU40_CC40 ,\
      XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH ) ;
  /* Enable IRQ input in NVIC ( IRQ number from XMC4500 . h ) */
  NVIC_EnableIRQ( CCU40_0_IRQn ) ;
  /* enable clock to slice CC40 */
  XMC_CCU4_EnableClock( CCU40 , 0) ;
  /* start timer */
  XMC_CCU4_SLICE_StartTimer( CCU40_CC40 ) ;
  


  while (1){



  }
  return 0;
}



void morse() {

    int arrayLenth = strlen(site);
    char *output[100];
    
    for(int i=0;i<arrayLenth;i++){
        for(int k=0;k<27;k++){
            if (site[i] == characters[k]){
                output[i]=morsecode[k];
                break;
            }
        }
    }

    for (int i = 0; i < arrayLenth; i++)
    {
        int charMorseCodeLenth = strlen(output[i]);
        for (int k = 0; k < charMorseCodeLenth; k++)
        {
            if(output[i][k]=='-'){
                intoutput[intoutputk]=1;
                intoutputk++;
                intoutput[intoutputk]=1;
                intoutputk++;
                intoutput[intoutputk]=1;
                intoutputk++;
                
            }else if (output[i][k]=='.')
            {
                intoutput[intoutputk]=1;
                intoutputk++;

            }else if (output[i][k]=='0'){
                intoutput[intoutputk]=0;
                intoutputk++;
            }
            
            if (k!=(charMorseCodeLenth-1))
            {
                intoutput[intoutputk]=0;
                intoutputk++;
            }
            
        }
        if (i!=(arrayLenth-1))
        {
            intoutput[intoutputk]=0;
            intoutputk++;
            intoutput[intoutputk]=0;
            intoutputk++;
            intoutput[intoutputk]=0;
            intoutputk++;
        }
    }
}
