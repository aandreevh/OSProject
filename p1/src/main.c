/* va_start example */

#include<stdio.h>
#include <unistd.h>

int main (int argc, char** argv)
{

    int out =  execlp("echo","echo","HI",NULL);
    
    fprintf(stdout,"%d\n",out);
    return 0;
}