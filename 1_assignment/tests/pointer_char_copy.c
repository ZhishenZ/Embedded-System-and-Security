#include <stdio.h>

int main(void)
{
    char *smth = "aaa";
    int someInt = 368;
    char temp_str[10];

    sprintf(temp_str, "%d", someInt);

    smth = temp_str;
    someInt++;
    printf("hallo");
    return 0;
}