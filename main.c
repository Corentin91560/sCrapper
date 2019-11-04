#include <stdio.h>
#include <stdlib.h>

struct Action {
    char *nameAction;
    char *urlAction;
    char *options[3];
};
typedef struct Action Action;

struct Tache {
    char *nomTache;
    char *temps[3];
    char *options;
};
typedef struct Tache Tache;

/** Permet de retourner le nombre d'action qui a été rentré dans le fichier de config */
int getNbrAction(char *data, long lenghFile);

/** Permet de retourner le nombre de tâches qui a été rentré dans le fichier de config */
int getNbrTache(char *data, long lenghFile);

/** Lit le fichier et enregistre les informations dans les structures appropriées */
int readFile (Action *action, Tache *tache, int nbrAction, int nbrTache, char *data, long fileLength);

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
        //printf("%s", data);
        fclose(config);

        int nbrAction = getNbrAction(data, fileLength);
        //printf("%d", nbrAction);

        int nbrTache = getNbrTache(data, fileLength);
        //printf("%d", nbrTache);

        Action action[nbrAction];
        Tache tache[nbrTache];

        int validateRead = 0;

        validateRead = readFile(action, tache, nbrAction, nbrTache, data, fileLength);
        if (validateRead == 0) {
            printf("Error in the file, make sure you have enter good information\n");
            return 0;
        }
        else {
            printf("File is good\n");
        }
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

/** Lit le fichier et enregistre les informations dans les structures appropriées */
int readFile (Action *action, Tache *tache, int nbrAction, int nbrTache, char *data, long fileLength) {
    int occurence = 0;
    int counter = 0;
        /* Récupère les noms des actions */
        for (int i = 0; i < fileLength; i++) {
            //printf("%c", data[i]);
            if (data[i] == '#') {
                while (data[i] != '\n') {
                    i++;
                }
            }
            else {
                //printf("%c", data[i]);
                /* Détermine le premier caractère à lire */
                if (occurence == 0) {
                    if (data[i] == '=') {
                        occurence = 1;
                        i--;
                    }
                    else {
                        printf("Error, no \'=\' recognized in the file\n");
                        return 0;
                    }
                }
                else {
                    /* Retour à la ligne */
                    if (data[i] == '=' && data[i+1] != '=' && data[i-1] != '=') {
                    if (data[i] != '\n') {
                        while (data[i] != '\n') {
                            i++;
                        }
                    }
                    i++;
                    //printf("%c\n", data[i]);
                    if (data[i] == '{') {
                        i++;
                        /* Lecture du nom et du site web*/
                        if (data[i] == 'n' && data[i+1] == 'a' && data[i+2] == 'm' && data[i+3] == 'e' && data[i+4] == ' ' && data[i+5] == '-' && data[i+6] == '>' && data[i+7] == ' ') {
                            i += 8;
                            int allocName = 0;
                            int j = i;
                            while (data[j] != '}') {
                                j++;
                                if (data[j] == '\n') {
                                    return 0;
                                }
                            }
                            allocName = j - i;
                            action[counter].nameAction = malloc((allocName + 1) * sizeof(char));
                            int k = 0;
                            while (allocName != k) {
                                action[counter].nameAction[k] = data[i];
                                k++;
                                i++;
                            }
                            action[counter].nameAction[allocName] = '\0';

                            while (data[i] != '\n') {
                                i++;
                            }
                            /* On se place au début de la ligne qui contient l'URL */
                            i+=2;

                            /* Verification de la présence de l'URL */
                            if (data[i] == 'u' && data[i+1] == 'r' && data[i+2] == 'l' && data[i+3] == ' ' && data[i+4] == '-' && data[i+5] == '>' && data[i+6] == ' ') {
                                printf ("IN");
                                i += 8;
                                int allocURL = 0;
                                j = i;
                                while (data[j] != '}') {
                                    j++;
                                    if (data[j] == '\n') {
                                        return 0;
                                    }
                                }
                                allocURL = j - i;
                                action[counter].urlAction = malloc((allocURL + 1) * sizeof(char));
                                k = 0;
                                while (allocURL != k) {
                                    action[counter].urlAction[k] = data[i];
                                    k++;
                                    i++;
                                }
                                action[counter].urlAction[allocURL + 1] = '\0';

                                counter++;
                            }
                            else {
                                printf("Error, the url section of the action number %d is incorrect (make sure the line is correct {url -> site.com})\n", counter + 1);
                                return 0;
                            }
                        }
                    }
                }
            }
        }
    }
    return 1;
}
