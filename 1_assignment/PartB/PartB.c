#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xmc_gpio.h>
#include <xmc_ccu4.h>


#define GPIO_BUTTON1  XMC_GPIO_PORT1, 14 //define GPIO_BUTTON1 as a input which is XMC_GPIO_PORT1, 14 
#define GPIO_BUTTON2  XMC_GPIO_PORT1, 15

static char letter[] =     {  ' ', 'A', 'B', 'C', 'D', 
                              'E', 'F', 'G', 'H', 'I',
                              'J', 'K', 'L', 'M', 'N',
                              'O', 'P', 'Q', 'R', 'S',
                              'T', 'U', 'V', 'W', 'X',
                              'Y', 'Z', '0', '1', '2',
                              '3', '4', '5', '6', '7',
                              '8', '9'}; 

static char *morse_code[] = { "0", "10111","111010101","11101011101","1110101",
                              "1", "101011101","111011101","1010101", "101",
                              "1011101110111","111010111","101110101","1110111","11101",
                              "11101110111"  ,"10111011101", "1110111010111","1011101","10101",
                              "111", "1010111","101010111","101110111","11101010111",
                              "1110101110111","11101110101","1110111011101110111","10111011101110111","101011101110111",
                              "1010101110111","10101010111","101010101","11101010101","1110111010101",
                              "111011101110101","11101110111011101"}; 
int lookuptable_counter = 0;
int char_len = sizeof(letter)/sizeof(letter[0]);

char * user_input = "I CAN MORSE";//the user input of LED 1 when button 1 is pressed 
char * time_span = "";
char * output_str = "";
int str_len;
int time_string_len;
int autoreload_counter = 0;//auto_reload_counter for 100 ms
volatile uint32_t global_autoreload_counter = 0;// global_autoreload_counter for the whole time period
int time_counter = 0;
int pause_ticks = 50;// we set by prescaler one tick as 100 ms, so 5 sec should be 50 ticks
volatile uint32_t pre_time = 0;
volatile uint32_t cur_time = 0;

char * get_morse(char input_letter){
    for (size_t i = 0; i < char_len; i++) {
        if (input_letter==letter[i]){
        lookuptable_counter = 0;
        return morse_code[i];
        };
    };
  return "";
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

// void creat_morse_code(char * user_input){
// int i;

// for (i = 0; i < strlen(user_input); i++){

//     char * return_value;
//     return_value = get_morse(user_input[i]);
//     output_str = str_concat(output_str,return_value);
//     if(i<(strlen(user_input)-1)){
//         char * three_0 = "000";
//         output_str = str_concat(output_str,three_0);
//     }
//   }
// }

// char * creat_morse_code(char * user_input_){
//   int i;

//   for (i = 0; i < strlen(user_input_); i++){

//     char * return_value;
//     return_value = get_morse(user_input_[i]);
//     output_str = str_concat(output_str,return_value);
//     if(i<(strlen(user_input_)-1)){
//         char * three_0 = "000";
//         output_str = str_concat(output_str,three_0);
//     }
//   }
//   return output_str;
// }


char * creat_morse_code(char * user_input_){
  int i;
  char * output_str_ = "";
  for (i = 0; i < strlen(user_input_); i++){

    char * return_value;
    return_value = get_morse(user_input_[i]);
    output_str_ = str_concat(output_str_,return_value);
    if(i<(strlen(user_input_)-1)){
        char * three_0 = "000";
        output_str_ = str_concat(output_str_,three_0);
    }
  }
  return output_str_;
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



void send_morse(){

if(autoreload_counter < str_len){
    if(output_str[autoreload_counter]=='1'){
      XMC_GPIO_SetOutputLevel(XMC_GPIO_PORT1, 1, XMC_GPIO_OUTPUT_LEVEL_HIGH);//XMC_GPIO_OUTPUT_LEVEL_HIGH
    }else{ 
      XMC_GPIO_SetOutputLevel(XMC_GPIO_PORT1, 1, XMC_GPIO_OUTPUT_LEVEL_LOW);//XMC_GPIO_OUTPUT_LEVEL_HIGH
  }

  }else{

    if(autoreload_counter < (str_len+pause_ticks)){
      XMC_GPIO_SetOutputLevel(XMC_GPIO_PORT1, 1, XMC_GPIO_OUTPUT_LEVEL_LOW);
    }else{
      autoreload_counter = 0;
    }
    
  };

};

void send_time(){

if(autoreload_counter < time_string_len){
    if(time_span[autoreload_counter]=='1'){
      XMC_GPIO_SetOutputLevel(XMC_GPIO_PORT1, 1, XMC_GPIO_OUTPUT_LEVEL_HIGH);//XMC_GPIO_OUTPUT_LEVEL_HIGH
    }else{ 
      XMC_GPIO_SetOutputLevel(XMC_GPIO_PORT1, 1, XMC_GPIO_OUTPUT_LEVEL_LOW);//XMC_GPIO_OUTPUT_LEVEL_HIGH
  }

  }else{

    if(autoreload_counter < (time_string_len+pause_ticks)){
      XMC_GPIO_SetOutputLevel(XMC_GPIO_PORT1, 1, XMC_GPIO_OUTPUT_LEVEL_LOW);
    }else{
      autoreload_counter = 0;
    }
    
  };
  
}

void time_span_trans(char * time_span_str){

  //output_str = creat_morse_code(user_input);
  time_span = "";//make it 0
  time_span = creat_morse_code(time_span_str);
  time_string_len = strlen(time_span);

}


int main(void) {
   
  output_str = creat_morse_code(user_input);
  str_len = strlen(output_str);
  //->21:41
  // for calculation of the output string 
  int only_once = 0;
  char * time_span_str = "0";
  // <- 21:41

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
  // if button pressed
      // send_morse
  //else
      // autoreload_counter = 0;
  // 16:54 -->
  //time_span = creat_morse_code(user_input);
  if (XMC_GPIO_GetInput(GPIO_BUTTON1) == 0 ){
    
    pre_time = cur_time;
    cur_time = global_autoreload_counter;
    char str_tem[10];
    sprintf(str_tem,"%ld",(cur_time-pre_time));
    time_span_str = str_tem;
    only_once = 0;
    //time_span_trans(input);
    //time_span = creat_morse_code(user_input);
    //strcpy(time_span,str_tem);
    //time_string_len = strlen(time_span);
    send_morse();
  }else if (XMC_GPIO_GetInput(GPIO_BUTTON2) == 0){   // 16:54 <--
  // is BUTTON2 is pressed 
    //XMC_GPIO_SetOutputLevel(XMC_GPIO_PORT1, 1, XMC_GPIO_OUTPUT_LEVEL_HIGH);
    time_string_len = strlen(time_span);
    send_time();
  }else{
    XMC_GPIO_SetOutputLevel(XMC_GPIO_PORT1, 1, XMC_GPIO_OUTPUT_LEVEL_LOW);
    if (!only_once){
    time_span_trans(time_span_str);
    only_once=1;
    }
    autoreload_counter = 0;
  }

  
  // else{
  //   XMC_GPIO_SetOutputLevel(XMC_GPIO_PORT1, 1, XMC_GPIO_OUTPUT_LEVEL_LOW);
  //   autoreload_counter = 0;
  // }


  
}

return 0;
}

void CCU40_0_IRQHandler ( void ) {
  autoreload_counter++;
  global_autoreload_counter++;
}


