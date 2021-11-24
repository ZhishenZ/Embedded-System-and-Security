#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include <string.h>

// create the hash table
// static char *letter[] = { "I", " ", "C", "A",
//                           "N", "M", "O", "R",
//                           "S", "E"  };

// static char *morse_code[] = { "101", "0", "11101011101", "10111",
//                               "11101", "1110111", "11101110111", "1011101",
//                               "10101","1" };

static char *letter[] =     { " ", "A", "B", "C", "D", 
                              "E", "F", "G", "H", "I",
                              "J", "K", "L", "M", "N",
                              "O", "P", "Q", "R", "S",
                              "T", "U", "V", "W", "X",
                              "Y", "Z" }; 


static char *morse_code[] = { "0", "10111","111010101","11101011101","1110101"
                              "1", "101011101","111011101","1010101", "101",
                              "1011101110111","111010111","101110101","1110111","11101",
                              "11101110111"  ,"10111011101", "1110111010111","1011101","10101",
                              "111", "1010111","101010111","101110111","11101010111",
                              "1110101110111","11101110101"};  

// static char characters[] = {'A','B',       'C',  'D','E',   'F',  'G',   'H', 'I',   'J',  'K',   'L', 'M', 'N',  
//                             'O',     'P',   'Q',  'R',  'S','T',  'U',   'V',  'W',   'X','Y','Z',' '};
// static char *morsecode[] = {".-","-...","-.-.","-..",".","..-.","--.","....","..",".---","-.-",".-..","--","-.",
//                             "---",".--.","--.-",".-.","...","-","..-","...-",".--","-..-","-.--","--..","0"};

// initilize the content of the input 
char * user_input = "I CAN MORSE";
char * output_str = "";

//string concatenate function 
char* str_concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

// the access the hash table to get the code
char * letter_to_morse(ENTRY e_main){

   ENTRY e;
   ENTRY *ep;

   const size_t capacity = sizeof letter / sizeof letter[0];
   hcreate(capacity);

   for (size_t i = 0; i < capacity/*-2*/; i++) {
       e.key = letter[i];
       e.data = (void *) morse_code[i];

       ep = hsearch(e, ENTER);

       if (ep == NULL) {
           fprintf(stderr, "entry failed\n");
           exit(EXIT_FAILURE);
       }
   }

//    e.key = letter;
   ep = hsearch(e_main, FIND);
   //printf("%s -> %s\n", e_main.key, (char*)ep->data);

   hdestroy();
   //printf("> %s\n", (char*)ep->data);
   return  (char*)ep->data;
   //exit(EXIT_SUCCESS);

}


void creat_morse_code(){
ENTRY e;
// char * user_input = "I CAN MORSE";
// char * output_str = "";
int i;

for (i = 0; i < strlen(user_input); i++){

    // printf("%c\n", user_input[i]);
    //e.key = (&user_input)[i];
    
    // char str_temp[1] = user_input[0];
    //char str_temp[99] = 'c';
    char ch_temp[2];

    ch_temp[0]=user_input[i];
    //ch_temp[0]='R';
    ch_temp[1]='\0';

    e.key = ch_temp;
    letter_to_morse(e);
    char * return_value;
    return_value = letter_to_morse(e);
    //printf("%s-> %s\n",e.key, return_value);
    output_str = str_concat(output_str,return_value);
    if(i<(strlen(user_input)-1)){
        char * three_0 = "000";
        output_str = str_concat(output_str,three_0);
    }
  }
}



int main(void) {
   
creat_morse_code();   
//printf("\n\n");

//printf("%s",output_str);

int str_counter = 0;
int str_len = strlen(output_str);
output_str[0];
// int64_t code ;
// code = atoi(output_str);
// while(1){

// };

return 0;
}
