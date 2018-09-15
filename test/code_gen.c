#include<stdlib.h>
#include<time.h>

void main()
{
   int j,i;
   srand(time(NULL));
   for(i=0;i<100;i++)
   {
       j= (int)(rand() % (999999 - 100000) + 1000000);
 
       char buffer[20] = {0};
       sprintf(buffer, "%d", j);
       strcpy(buffer, buffer + 1);

       printf("%s\n", buffer);
       //int code = j - 1000000;
       //printf("%d\n", code);
   }
}
