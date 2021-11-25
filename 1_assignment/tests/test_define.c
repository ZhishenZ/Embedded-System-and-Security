#include <stdio.h>
# define P 1
# define PI P:3.14159


double my_sum(double a, double b){

    return a+b;
}
int main(void)
{
    double r, s;
    r = 1.1;
    s = my_sum(PI);
    printf("s=PI*r^2 = %.6f\n", s);  //PI不会被宏替换
    return 0;
}
