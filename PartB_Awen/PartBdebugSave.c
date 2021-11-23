#include <xmc_gpio.h>
#include <xmc_ccu4.h>
#include <stdio.h>
#include <string.h>
//#include <stdlib.h>
//#include <search.h>

#define GPIO_BUTTON1  XMC_GPIO_PORT1, 14
#define GPIO_BUTTON2  XMC_GPIO_PORT1, 15

volatile uint8_t time_stuck_error = 0;
volatile uint8_t button_press_status=0;
volatile uint32_t button1_first_press_time = 0;
volatile uint32_t button1_second_press_time = 0;

static char characters[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','U','X','Y','Z',' ',
'0','1','2','3','4','5','6','7','8','9'};
static char *morsecode[] = {".-","-...","-.-.","-..",".","..-.","--.","....","..",".---","-.-",".-..","--","-.","---",".--.","--.-",".-.","...","-","..-","...-",".--","-..-","-.--","--..","0"
"-----",".----","..---","...--","....-",".....","-....","--...","---..","----."};
static char strInput[] = "I CAN MORSE";
volatile uint8_t led_k = 0;
uint8_t intoutput[100];
uint8_t intoutputk=0;
uint8_t waitTickNumber = 0;
uint8_t morse_output_busy=0;

const XMC_GPIO_CONFIG_t LED_config = \
    {.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL ,\
     .output_level = XMC_GPIO_OUTPUT_LEVEL_LOW ,\
     .output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE };

const XMC_GPIO_CONFIG_t input_config = \
      {.mode=XMC_GPIO_MODE_INPUT_TRISTATE,\
       .output_level=XMC_GPIO_OUTPUT_LEVEL_LOW,\
       .output_strength=XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE};

volatile uint32_t CCU4_periods = 0;
volatile uint32_t CCU4_periods_lastone = 0;


XMC_CCU4_SLICE_COMPARE_CONFIG_t CCU_config =
    {.timer_mode = 0U /*XMC_CCU4_SLICE_TIMER_COUNT_MODE_EA*/,
    .monoshot = false ,
    .shadow_xfer_clear = 0U ,
    .dither_timer_period = 0U ,
    .dither_duty_cycle = 0U ,
    .prescaler_mode = XMC_CCU4_SLICE_PRESCALER_MODE_NORMAL ,
    .mcm_enable = 0U ,
    .prescaler_initval = 0xAU, /* 1/1024 ( Table 22 -7 p . 22 -54) */
    .float_limit = 0U ,
    .dither_limit = 0U ,
    .passive_level = 0U/*XMC_CCU4_SLICE_OUTPUT_PASSIVE_LEVEL_LOW*/ ,
    .timer_concatenation = 0U
    };
    

void CCU40_0_IRQHandler ( void ) {
  CCU4_periods++;
  //XMC_GPIO_ToggleOutput(XMC_GPIO_PORT1, 1);
}

void morse() {

  int arrayLenth = strlen(strInput);
  char *output[100];
  intoutputk=0;
  
  for(int i=0;i<arrayLenth;i++){
    for(int k=0;k<37;k++){
      if (strInput[i] == characters[k]){
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

void clearStrInputArray(){
  for(int i=0; i<strlen(strInput); i++){
    strInput[i]=0;
  }
  for(int i=0; i<intoutputk; i++){
    intoutput[i]=0;
  }
  led_k=0;
  intoutputk=0;
  morse_output_busy=0;
}

void morseOutput(){
  if (morse_output_busy ==1 )
  {
    /* when there are things to output */
    if((CCU4_periods/100)==(CCU4_periods_lastone/100)+1){
      if(led_k < intoutputk){
        if(intoutput[led_k]==1)
          XMC_GPIO_SetOutputLevel (XMC_GPIO_PORT1, 1, XMC_GPIO_OUTPUT_LEVEL_HIGH);//XMC_GPIO_OUTPUT_LEVEL_HIGH
        else 
          XMC_GPIO_SetOutputLevel (XMC_GPIO_PORT1, 1, XMC_GPIO_OUTPUT_LEVEL_LOW);//XMC_GPIO_OUTPUT_LEVEL_HIGH

        led_k++;
      }else{
          XMC_GPIO_SetOutputLevel (XMC_GPIO_PORT1, 1, XMC_GPIO_OUTPUT_LEVEL_LOW);//Only output once
          clearStrInputArray();
        /*if(led_k < intoutputk+waitTickNumber){
          led_k++;//dont wait, just shut down
          XMC_GPIO_SetOutputLevel (XMC_GPIO_PORT1, 1, XMC_GPIO_OUTPUT_LEVEL_LOW);
        }else{
          led_k=0;
        }*/
      }
      CCU4_periods_lastone = CCU4_periods;
    }/*else if (CCU4_periods >CCU4_periods_lastone+1)
    {
      time_stuck_error +=1;//debug use
    }*/
  }
}

void checkbutton(){
  if(XMC_GPIO_GetInput(GPIO_BUTTON1) == 0 && ~morse_output_busy && button_press_status==0){
    button1_first_press_time=button1_second_press_time;
    button1_second_press_time = CCU4_periods;
    button_press_status=1;
    morse_output_busy=1;
    strcpy(strInput,"I CAN MORSE");
    morse();
  }
  if(XMC_GPIO_GetInput(GPIO_BUTTON2) == 0 && ~morse_output_busy && button_press_status==0){
    button_press_status=1;
    morse_output_busy=1;
    char tempstr[10];
    sprintf(tempstr,"%ld",button1_second_press_time-button1_first_press_time);
    strcpy(strInput,tempstr);
    morse();
  }

  if (XMC_GPIO_GetInput(GPIO_BUTTON1) != 0 && XMC_GPIO_GetInput(GPIO_BUTTON2) != 0)
  {
    /* button are all up */
    button_press_status=0;
  }
  
}


int main ( void ) {
  
  morse();

  XMC_GPIO_Init(XMC_GPIO_PORT1, 1, &LED_config);
  XMC_GPIO_Init(GPIO_BUTTON1,  &input_config);
  XMC_GPIO_Init(GPIO_BUTTON2,  &input_config);
  /* Initialize CCU40 , the MCMS transfer is irrelevant for our
  * application . Calls EnableModule ( SCU enable clock , SCU ungate
  * clock , SCU deassert reset ) and StartPrescaler ( GIDLC . SPRB ) */
  XMC_CCU4_Init( CCU40 , XMC_CCU4_SLICE_MCMS_ACTION_TRANSFER_PR_CR ) ;
  XMC_CCU4_SLICE_CompareInit ( CCU40_CC40 , & CCU_config ) ;
  XMC_CCU4_SLICE_SetTimerPeriodMatch ( CCU40_CC40 , 0x75/*0x2DC6*/ ) ;
  XMC_CCU4_EnableShadowTransfer ( CCU40 , XMC_CCU4_SHADOW_TRANSFER_SLICE_0 ) ;
  /* Map IRQ to NVIC IRQ input , enable IRQ in CCU4 */
  XMC_CCU4_SLICE_SetInterruptNode( CCU40_CC40 ,\
      XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH ,\
      XMC_CCU4_SLICE_SR_ID_0);
  XMC_CCU4_SLICE_EnableEvent( CCU40_CC40 ,\
      XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH ) ;
  /* Enable IRQ input in NVIC ( IRQ number is in XMC4500.h ) */
  NVIC_EnableIRQ( CCU40_0_IRQn );
  /* enable clock to slice CC40 */
  XMC_CCU4_EnableClock(CCU40, 0);
  /* start timer */
  XMC_CCU4_SLICE_StartTimer(CCU40_CC40);
  


  while (1){

    checkbutton();
    morseOutput();
    

  }
  return 0;
}




