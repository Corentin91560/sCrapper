#include <stdio.h>
#include <stdlib.h>
int main()
{
    FILE* config = fopen("config.sconf","r");
    char* recup=malloc(sizeof(char)*200);
    if(config != NULL){
        char* ch=malloc(sizeof(char)*200);
        while((ch = fgetc(config)) != EOF){
                recup=ch;
            printf("%c", recup);
        }
        fclose(config);

    }else{
        printf("file not open");
    }

    return 0;
}
