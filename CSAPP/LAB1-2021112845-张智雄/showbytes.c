#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    FILE *fp = NULL;
    fp = fopen("hello.c","r");
    char ch;int i = 0;
    if(fp == NULL)
	printf("The file is empty!");
    else{
    	fseek(fp,0,SEEK_END);
    	int length = ftell(fp);
    	int file[length];
    	rewind(fp);
	while((ch = fgetc(fp)) != EOF)
	{
            file[i] = ch;
            i++;
	}
        fclose(fp);
        int x = 0;
        for(int j = 0; j < length; j++)
        {
          if(j == 0) printf("%.6x: ",0);
          if(j%16 != 0 || j == 0){
            if(file[j] == '\n')
              printf("\\n   ");
            else
              printf("%c    ",file[j]);
          }
          if(j % 16 == 0 && j != 0){
            printf("\n");
            int s = j;
            printf("        ");
            for(int m = x; m < s; m++)
            {
              printf("%.2x   ",file[m]);
            }
            x = s;
            printf("\n");
            printf("%.6x: %c    ",j,file[j]);
          }
          if(j == length -1 && j % 16 != 0){ 
            printf("\n");
            int s = j;
            printf("        ");
            for(int m = x; m <= s; m++)
            {
              printf("%.2x   ",file[m]);
            }
            printf("\n");
          }
        }
            
    }
    return 0;
}
