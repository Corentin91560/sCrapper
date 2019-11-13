#include <stdio.h>
#include <stdlib.h>

#include <curl/curl.h>

struct Action {
    char *nameAction;
    char *urlAction;

    int maxDepthOption;
    char *versioningOption;
    char **typeOption;
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

/** Lit le fichier et enregistre les informations "name" et "url" dans la structure "action" */
int readFileAction (Action *action, int nbrAction, char *data, long fileLength);

/** Lit le fichier et enregistre les options dans la structure "action" */
int readFileOption (Action *action, int nbrAction, char *data, long fileLength);

int main()
{
    /** Lecture binaire du fichier pour vérifier sa taille*/
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

    /** Lecture du fichier pour voir ce qu'il contient */
    config = fopen("config.sconf","r");

    if(config == NULL){
        printf("File not open");
        return 0;

    }
    /** Récupération de l'intégralité du fichier de config */
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

        int validateReadAction = 0;
        int validateReadOption = 0;

        /** Lecture des actions */
        validateReadAction = readFileAction(action, nbrAction, data, fileLength);
        if (validateReadAction == 0) {
            printf("Error in the file, make sure you have enter good information\n");
            return 0;
        }
        /** C'est parti ! */
        else {
            printf("File is good\n");
            validateReadOption = readFileOption(action, nbrAction, data, fileLength);
            CURL *curl;
            CURLcode res;
            FILE *file;

          curl = curl_easy_init();
          file = fopen("file.html","wb");
          if(curl) {
            printf("%s\n", action[0].urlAction);
            /** Désactive les certificats */
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, FALSE);

            curl_easy_setopt(curl, CURLOPT_URL, action[0].urlAction);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA,file);

            /** Forcing HTTP/3 will make the connection fail if the server isn't
               accessible over QUIC + HTTP/3 on the given host and port.
               Consider using CURLOPT_ALTSVC instead! */
            curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, 30);

            /** Perform the request, res will get the return code */
            res = curl_easy_perform(curl);
            /** Check for errors */
            if(res != CURLE_OK)
              fprintf(stderr, "curl_easy_perform() failed: %s\n",
                      curl_easy_strerror(res));

            /** always cleanup */
            curl_easy_cleanup(curl);
            fclose(file);
          }
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
int readFileAction (Action *action, int nbrAction, char *data, long fileLength) {
    int occurence = 0;
    int counter = 0;

        /** Récupère les noms des actions */
        for (int i = 0; i < fileLength; i++) {
            //printf("%c", data[i]);
            if (data[i] == '#') {
                while (data[i] != '\n') {
                    i++;
                }
            }

            else {
                //printf("%c", data[i]);

                /** Détermine le premier caractère à lire */
                if (occurence == 0) {
                    if (data[i] == '=' && data[i+1] != '=' && data[i-1] != '=') {
                        occurence = 1;
                        i--;
                    }
                    else {
                        printf("Error, no \'=\' recognized in the file\n");
                        return 0;
                    }
                }

                /** Lecture classique ensuite */
                else {

                    /** Retour à la ligne */
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
                        /** Lecture du nom et du site web*/
                        if (data[i] == 'n' && data[i+1] == 'a' && data[i+2] == 'm' && data[i+3] == 'e' && data[i+4] == ' ' && data[i+5] == '-' && data[i+6] == '>' && data[i+7] == ' ') {

                            /** On se place à la donnée utilisateur */
                            i += 8;
                            int allocName = 0;
                            int j = i;
                            while (data[j] != '}') {
                                j++;
                                if (data[j] == '\n') {
                                    return 0;
                                }
                            }

                            /** Allocation du nom */
                            allocName = j - i;
                            action[counter].nameAction = malloc((allocName + 1) * sizeof(char));

                            /** Ajout de la donnée */
                            int k = 0;
                            while (allocName != k) {
                                action[counter].nameAction[k] = data[i];
                                k++;
                                i++;
                            }
                            action[counter].nameAction[allocName] = '\0';
                            i++;

                            /** Vérifie la donnée après le symbole '}' du nom */
                            while (data[i] != '\n') {
                                if (data[i] == ' ' || data[i] == 9) {
                                    i++;
                                }
                                else if (data[i] == '#') {
                                    while (data[i] != '\n') {
                                        i++;
                                    }
                                }
                                else {
                                    printf("Error, be careful you wrote comment with the character '#' and not outside. Error in the action \"name\" number %d.)\n", counter + 1);
                                    return 0;
                                }
                            }

                            /** On se place au début de la ligne qui contient l'URL */
                            i+=2;

                            /** Verification de la présence de l'URL */
                            if (data[i] == 'u' && data[i+1] == 'r' && data[i+2] == 'l' && data[i+3] == ' ' && data[i+4] == '-' && data[i+5] == '>' && data[i+6] == ' ') {

                                /** On se place à la donnée utilisateur */
                                i += 7;
                                int allocURL = 0;
                                j = i;
                                while (data[j] != '}') {
                                    j++;
                                    if (data[j] == '\n') {
                                        return 0;
                                    }
                                }

                                /** Allocation de l'URL */
                                allocURL = j - i;
                                action[counter].urlAction = malloc((allocURL + 1) * sizeof(char));
                                k = 0;
                                while (allocURL != k) {
                                    action[counter].urlAction[k] = data[i];
                                    k++;
                                    i++;
                                }
                                action[counter].urlAction[allocURL] = '\0';

                                i++;

                                /** Vérifie la donnée après le symbole '}' de l'url */
                                while (data[i] != '\n') {
                                    if (data[i] == ' ' || data[i] == 9) {
                                        i++;
                                    }
                                    else if (data[i] == '#') {
                                        while (data[i] != '\n') {
                                            i++;
                                        }
                                    }
                                    else {
                                        printf("Error, be careful you wrote comment with the character '#' and not outside. Error in the action \"url\" number %d.)\n", counter + 1);
                                        return 0;
                                    }
                                }

                                /** Compteur des actions */
                                counter++;
                            }

                            /** URL erreur */
                            else {
                                printf("Error, the url section of the action number %d is incorrect (make sure the line is correct {url -> site.com})\n", counter + 1);
                                return 0;
                            }
                        }
                    }
                    else {
                        //printf("Error, \n", counter + 1);
                        return 0;
                    }
                }
            }
        }
    }
    return 1;
}

/** Lit le fichier et enregistre les options dans la structure "action" */
int readFileOption (Action *action, int nbrAction, char *data, long fileLength) {
    int counter = -1;
    for (int i = 0; i < fileLength; i++) {

        /** Vérification des commentaires */
        if (data[i] == '#') {
            while (data[i] != '\n') {
                i++;
            }
        }
        else {

            /** Si on trouve uniquement un '=' */
            if (data[i] == '=' && data[i+1] != '=' && data[i-1] != '=') {

                /** Compteur d'actions */
                counter++;

                /** Vérifications pour éviter les multiples options */
                int okDepth = 0;
                int okVers = 0;
                int okType = 0;

                /** On se place normalement au '+' */
                int j = 0;
                while (j != 3) {
                    while(data[i] != '\n') {
                        i++;
                    }
                    i++;
                    j++;
                }

                /** Si on trouve des options */
                if (data[i] == '+') {
                    while (1)  {

                        /** Go fin de ligne */
                        while(data[i] != '\n') {
                            i++;
                        }

                        /** Situé sur '{' */
                        i++;

                        /** Si on trouve bien le '{' */
                        if (data[i] == '{') {
                            i++;

                            /** Revue des différentes options */

                            /** max-depth */
                            if (data[i] == 'm' && data[i+1] == 'a' && data[i+2] == 'x' && data[i+3] == '-' && data[i+4] ==  'd' && data[i+5] == 'e' && data[i+6] == 'p' && data[i+7] == 't'
                                && data[i+8] == 'h' && data[i+9] == ' ' && data[i+10] == '-' && data[i+11] == '>' && data[i+12] == ' ' && okDepth == 0) {
                                    okDepth = 1;

                                    /** On se met juste avant la valeur utilisateur */
                                    i += 13;
                                    int allocOpt = 0;
                                    int k = i;

                                    /** Allocation mémoire jusqu'au '}' */
                                    while (data[k] != '}') {
                                        k++;
                                        if (data[k] == '\n') {
                                            return 0;
                                        }
                                    }
                                    allocOpt = k - i;
                                    char* tempOpt = malloc((allocOpt + 1) * sizeof(char));

                                    /** On écrit dans une valeur temporaire la donnée utilisateur */
                                    k = 0;
                                    while (allocOpt != k) {
                                        tempOpt[k] = data[i];
                                        k++;
                                        i++;
                                    }
                                    tempOpt[allocOpt] = '\0';

                                    /** Transformation en int */
                                    action[counter].maxDepthOption = atoi(tempOpt);

                                    printf("%d\n", action[counter].maxDepthOption);

                                    /** Vérification que c'est bien un chiffre */
                                    if (action[counter].maxDepthOption == 0) {
                                        printf("Error, make sure you have enter a number > 1 for the \"max-depth\"\n");
                                        return 0;
                                    }

                                    /** Vérification des données après le '{' */
                                    while (data[i] != '\n') {
                                        if (data[i] == ' ' || data[i] == 9) {
                                            i++;
                                        }
                                        else if (data[i] == '#') {
                                            while (data[i] != '\n') {
                                                i++;
                                            }
                                        }
                                        else {
                                            printf("Error, be careful you wrote comment with the character '#' and not outside. Error in the option \"max-depth\" number %d.)\n", counter + 1);
                                            return 0;
                                        }
                                    }


                                }

                            /** versioning */
                            else if (data[i] == 'v' && data[i+1] == 'e' && data[i+2] == 'r' && data[i+3] == 's' && data[i+4] ==  'i' && data[i+5] == 'o' && data[i+6] == 'n' && data[i+7] == 'i'
                                && data[i+8] == 'n' && data[i+9] == 'g' && data[i+10] == ' ' && data[i+11] == '-' && data[i+12] == '>' && data[i+13] == ' ' && okVers == 0) {
                                    okVers = 1;

                                    /** On se met juste avant la valeur utilisateur */
                                    i += 14;
                                    int allocOpt = 0;
                                    int k = i;

                                    /** Allocation mémoire jusqu'au '}' */
                                    while (data[k] != '}') {
                                        k++;
                                        if (data[k] == '\n') {
                                            return 0;
                                        }
                                    }
                                    allocOpt = k - i;
                                    char* tempOpt = malloc((allocOpt + 1) * sizeof(char));

                                    /** On écrit dans une valeur temporaire la donnée utilisateur */
                                    k = 0;
                                    while (allocOpt != k) {
                                        tempOpt[k] = data[i];
                                        k++;
                                        i++;
                                    }
                                    tempOpt[allocOpt] = '\0';

                                    /** Comparaison si on trouve "on" ou "off" */
                                    if (!strcmp(tempOpt, "on")) {
                                        action[counter].versioningOption = "on";
                                        printf("%s", action[counter].versioningOption);
                                    }
                                    else if (!strcmp(tempOpt, "off")) {
                                        action[counter].versioningOption = "off";
                                        printf("%s", action[counter].versioningOption);
                                    }
                                    else {
                                        printf("Error : Please make sure to write \"on\" or \"off\" in the option \"versioning\" in the action number %d\n", counter + 1);
                                        return 0;
                                    }

                                    /** Vérification des données après le '{' */
                                    while (data[i] != '\n') {
                                        if (data[i] == ' ' || data[i] == 9) {
                                            i++;
                                        }
                                        else if (data[i] == '#') {
                                            while (data[i] != '\n') {
                                                i++;
                                            }
                                        }
                                        else {
                                            printf("Error, be careful you wrote comment with the character '#' and not outside. Error in the option \"versioning\" number %d.)\n", counter + 1);
                                            return 0;
                                        }
                                    }

                                }

                            /** type */
                            else if (data[i] == 't' && data[i+1] == 'y' && data[i+2] == 'p' && data[i+3] == 'e' && data[i+4] ==  ' ' && data[i+5] == '-' && data[i+6] == '>' && data[i+7] == ' ' &&
                                    data[i+8] == '(' && okType == 0) {
                                    okType = 1;

                                    /** On se met juste avant la valeur utilisateur */
                                    i += 9;

                                    /** Compteur de virgules, va nous permettre de déterminer le nombre d'attributs pour l'option */
                                    int counterComma = 1;

                                    /** Allocation mémoire du tableau de 'char' */
                                    int k = i;
                                    while (data[k] != '}') {
                                        k++;
                                        if (data[k] == '\n') {
                                            return 0;
                                        }
                                        if (data[k] == ',') {
                                            counterComma++;
                                        }
                                    }
                                    action[counter].typeOption = malloc(counterComma * sizeof(char *));

                                    /** Allocation de chaque string */
                                    k = i;
                                    for (int p = 0; p < counterComma; p++) {
                                        int allocOpt = 0;
                                        while (data[k] != ',' && data[k] != '}') {
                                            if(data[k] != ' ') {
                                                allocOpt++;
                                            }
                                            k++;
                                        }
                                        k++;
                                        action[counter].typeOption[p] = malloc(allocOpt * sizeof(char));
                                    }

                                    /** On rentre les données dans la structure */
                                    k = i;
                                    for (int p = 0; p < counterComma; p++) {
                                        int countOpt = 0;
                                        while (data[k] != ',' && data[k] != ')') {
                                            if(data[k] != ' ') {
                                                action[counter].typeOption[p][countOpt] = data[k];
                                                countOpt++;
                                            }
                                            k++;
                                        }
                                        k++;
                                    }

                                    /** Vérification des données après le '{' */
                                    while (data[i] != '\n') {
                                        if (data[i] == ' ' || data[i] == 9) {
                                            i++;
                                        }
                                        else if (data[i] == '#') {
                                            while (data[i] != '\n') {
                                                i++;
                                            }
                                        }
                                        else {
                                            printf("Error, be careful you wrote comment with the character '#' and not outside. Error in the option \"type\" number %d.)\n", counter + 1);
                                            return 0;
                                        }
                                    }

                            }

                            /** error */
                            else {
                                printf("Error, no option recognized in the action number %d\n", counter + 1);
;                            }
                        }
                        else {
                            printf("Error, symbol '{' waited for the option of the action number %d\n", counter + 1);
                        }
                    }
                }

                else if (data[i] == ' ' || data[i] == 9 || data[i] == '#' || data[i] == '\n') {
                    action[counter].maxDepthOption = "0";
                    action[counter].versioningOption = "off";
                    action[counter].typeOption = "null";
                }
                else {
                    printf("Error, options bad for the action number %d\n", counter + 1);
                    return 0;
                }
            }
        }
    }
    return 1;
}
