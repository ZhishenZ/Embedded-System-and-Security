#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
char * output_str ="";
int str_len;

//string concatenate function 
char* str_concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}


char * get_morse(char input_letter){
    for (size_t i = 0; i < char_len; i++) {
        if (input_letter==letter[i]){
        lookuptable_counter = 0;
        //printf("%s\n",morse_code[i]);
        return morse_code[i];
    }
    }
}

// void creat_morse_code(){
// int i;

// for (i = 0; i < strlen(user_input); i++){

//     char * return_value;
//     return_value = get_morse(user_input[i]);
//     //printf("%s-> %s\n",e.key, return_value);
//     output_str = str_concat(output_str,return_value);
//     if(i<(strlen(user_input)-1)){
//         char * three_0 = "000";
//         output_str = str_concat(output_str,three_0);
//     }
//   }
// }
char * creat_morse_code(char * user_input_){
  int i;

  for (i = 0; i < strlen(user_input_); i++){

    char * return_value;
    return_value = get_morse(user_input_[i]);
    output_str = str_concat(output_str,return_value);
    if(i<(strlen(user_input_)-1)){
        char * three_0 = "000";
        output_str = str_concat(output_str,three_0);
    }
  }
  return output_str;
}


char * creat_morse_code(char * user_input_){
  int i;
  char * output_str_;

  for (i = 0; i < strlen(user_input_); i++){

    char * return_value;
    return_value = get_morse(user_input_[i]);
    output_str_ = str_concat(output_str_,return_value);
    if(i<(strlen(user_input_)-1)){
        char * three_0 = "000";
        output_str_ = str_concat(output_str_,three_0);
    }
  }
  return output_str;
}



int main(void) {
output_str = creat_morse_code(user_input);
//0x555555559760 "101000000011101011101000101110001110100000001110111000111011101110001011101000101010001"
 //                "1010000000111010111010001011100011101000000011101110001110111011100010111010001010100011110111011101110111"
return 0;
}
