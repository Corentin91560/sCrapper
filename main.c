#include <stdio.h>
#include <stdlib.h>

struct Action {
    char **nameAction;
    char **urlAction;
    char **options[3];
};
typedef struct Action Action;

struct Tache {
    char **nomTache;
    char **temps[3];
    char **options;
};
typedef struct Tache Tache;

/** Permet de retourner le nombre d'action qui a été rentré dans le fichier de config */
int getNbrAction(char *data, long lenghFile);

/** Permet de retourner le nombre de tâches qui a été rentré dans le fichier de config */
int getNbrTache(char *data, long lenghFile);

int main()
{
    /* Lecture binaire du fichier pour vérifier sa taille*/
    FILE* config = fopen("config.sconf","rb");
    if (config == NULL) {
        printf("File not open");
        return 0;
    }
    long fileLength;
    fseek(config, 0, SEEK_END);
    fileLength = ftell(config);
    char* data=malloc(sizeof(char)*fileLength);
    fclose(config);
    //printf("%ld", fileLength);

    /* Lecture du fichier pour voir ce qu'il contient */
    config = fopen("config.sconf","r");

    if(config == NULL){
        printf("File not open");
        return 0;

    }
    /* Récupération de l'intégralité du fichier de config */
    else {
        for (int i = 0; i < fileLength; i++) {
            data[i] = fgetc(config);
        }
        printf("%s", data);
        fclose(config);

        int nbrAction = getNbrAction(data, fileLength);
        //printf("%d", nbrAction);

        int nbrTache = getNbrTache(data, fileLength);
        printf("%d", nbrTache);

    }

    return 0;
}

/** Permet de retourner le nombre d'action qui a été rentré dans le fichier de config */
int getNbrAction(char *data, long lenghFile) {
    int nbrAction = 0;
    for (int i = 0; i < lenghFile; i++) {
        if((data[i] == '=') && (data[i+1] != '=') && (data[i-1] != '=')) {
            nbrAction++;
        }
    }
    return nbrAction;
}

/** Permet de retourner le nombre de tâches qui a été rentré dans le fichier de config */
int getNbrTache(char *data, long lenghFile) {
    int nbrTache = 0;
    for (int i = 0; i < lenghFile; i++) {
        if ((data[i] == '=') && (data[i+1] == '=')) {
            nbrTache++;
        }
    }
    return nbrTache;
}
