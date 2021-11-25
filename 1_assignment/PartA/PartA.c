#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xmc_gpio.h>
#include <xmc_ccu4.h>

static char letter[] =     {  ' ', 'A', 'B', 'C', 'D', 
                              'E', 'F', 'G', 'H', 'I',
                              'J', 'K', 'L', 'M', 'N',
                              'O', 'P', 'Q', 'R', 'S',
                              'T', 'U', 'V', 'W', 'X',
                              'Y', 'Z' }; 

static char *morse_code[] = { "0", "10111","111010101","11101011101","1110101",
                              "1", "101011101","111011101","1010101", "101",
                              "1011101110111","111010111","101110101","1110111","11101",
                              "11101110111"  ,"10111011101", "1110111010111","1011101","10101",
                              "111", "1010111","101010111","101110111","11101010111",
                              "1110101110111","11101110101"}; 
int lookuptable_counter = 0;
int char_len = sizeof(letter)/sizeof(letter[0]);

char * user_input = "I CAN MORSE";
char * output_str = "";
int str_len;
int str_counter = 0;
int pause_ticks = 50;// we set by prescaler one tick as 100 ms, so 5 sec should be 50 ticks

char * get_morse(char input_letter){
    for (size_t i = 0; i < char_len; i++) {
        if (input_letter==letter[i]){
        lookuptable_counter = 0;
        return morse_code[i];
        };
    };
}

//string concatenate function 
char* str_concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void creat_morse_code(){
int i;

for (i = 0; i < strlen(user_input); i++){

    char * return_value;
    return_value = get_morse(user_input[i]);
    output_str = str_concat(output_str,return_value);
    if(i<(strlen(user_input)-1)){
        char * three_0 = "000";
        output_str = str_concat(output_str,three_0);
    }
  }
}

const XMC_GPIO_CONFIG_t LED_config = \
    {.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL ,\
     .output_level = XMC_GPIO_OUTPUT_LEVEL_LOW ,\
     .output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE };

//volatile uint32_t CCU4_periods = 0;

XMC_CCU4_SLICE_COMPARE_CONFIG_t CCU_compare_config =
    {.timer_mode = 0U /*XMC_CCU4_SLICE_TIMER_COUNT_MODE_EA*/,
    .monoshot = false ,
    .shadow_xfer_clear = 0U ,
    .dither_timer_period = 0U ,
    .dither_duty_cycle = 0U ,
    .prescaler_mode = XMC_CCU4_SLICE_PRESCALER_MODE_NORMAL ,
    .mcm_enable = 0U ,
    .prescaler_initval = 0xAU , /* 1/1024 ( Table 22 -7 p . 22 -54) */
    .float_limit = 0U ,
    .dither_limit = 0U ,
    .passive_level = 0U/*XMC_CCU4_SLICE_OUTPUT_PASSIVE_LEVEL_LOW*/ ,
    .timer_concatenation = 0U
    };


int main(void) {
   

  creat_morse_code();
  str_len = strlen(output_str);
  XMC_GPIO_Init(XMC_GPIO_PORT1 , 1 , & LED_config ) ;
  /* Initialize CCU40 , the MCMS transfer is irrelevant for our
  * application . Calls EnableModule ( SCU enable clock , SCU ungate
  * clock , SCU deassert reset ) and StartPrescaler ( GIDLC . SPRB ) */
  XMC_CCU4_Init( CCU40 , XMC_CCU4_SLICE_MCMS_ACTION_TRANSFER_PR_CR ) ;
  XMC_CCU4_SLICE_CompareInit ( CCU40_CC40 , & CCU_compare_config ) ;
  XMC_CCU4_SLICE_SetTimerPeriodMatch ( CCU40_CC40 , 0x2DC7 ) ;//1719 as 0x
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

while(1){


  if(str_counter < str_len){
    if(output_str[str_counter]=='1'){
      XMC_GPIO_SetOutputLevel(XMC_GPIO_PORT1, 1, XMC_GPIO_OUTPUT_LEVEL_HIGH);//XMC_GPIO_OUTPUT_LEVEL_HIGH
    }else{ 
      XMC_GPIO_SetOutputLevel(XMC_GPIO_PORT1, 1, XMC_GPIO_OUTPUT_LEVEL_LOW);//XMC_GPIO_OUTPUT_LEVEL_HIGH
  }

  }else{

    if(str_counter < (str_len+pause_ticks)){
      XMC_GPIO_SetOutputLevel(XMC_GPIO_PORT1, 1, XMC_GPIO_OUTPUT_LEVEL_LOW);
    }else{
      str_counter = 0;
    }
    
  }

}

return 0;
}

void CCU40_0_IRQHandler ( void ) {
  str_counter++;

}


