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
    int second;
    int minute;
    int hour;
    char **options;
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

/** Lit le fichier et enregistre les tâches */
int readFileTask(Tache *task, int nbrTache, char *data, long fileLength, Action *action, int nbrAction);

/** Libère la mémoire à la fin du programme */
void freeMemory(Action *action, int nbrAction, Tache *task, int nbrTache);

int main()
{
    /** Lecture binaire du fichier pour vérifier sa taille*/
    FILE* config = fopen("config.sconf","rb");
    if (config == NULL) {
        printf("File not open");
        system("pause");
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

    int nbrAction = 0;
    int nbrTache = 0;

    if(config == NULL){
        printf("File not open");
        system("pause");
        return 0;

    }
    /** Récupération de l'intégralité du fichier de config */
    for (int i = 0; i < fileLength; i++) {
        data[i] = fgetc(config);
    }
    //printf("%s", data);
    fclose(config);

    nbrAction = getNbrAction(data, fileLength);
    //printf("%d", nbrAction);

    nbrTache = getNbrTache(data, fileLength);
    //printf("%d", nbrTache);

    Action action[nbrAction];
    Tache tache[nbrTache];

    int validateReadAction = 0;
    int validateReadOption = 0;
    int validateReadTask = 0;

    /** Lecture des actions */
    validateReadAction = readFileAction(action, nbrAction, data, fileLength);
    if (validateReadAction == 0) {
        return 0;
    }

    validateReadOption = readFileOption(action, nbrAction, data, fileLength);
    if (validateReadOption == 0) {
        return 0;
    }

    validateReadTask = readFileTask(tache, nbrTache, data, fileLength, action, nbrAction);
    if (validateReadTask == 0) {
        return 0;
    }

    /** C'est parti ! */
    printf("File is good\n");

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

    free(data);
    freeMemory(action, nbrAction, tache, nbrTache);
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

        /** On lit le fichier */
        for (int i = 0; i < fileLength; i++) {
            //printf("%c", data[i]);

            /** Saute les commentaires */
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
                        printf("Error, character '{' not found in the action number %d \n", counter + 1);
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

    /** Vérifications pour éviter les multiples options */
    int okDepth = 0;
    int okVers = 0;
    int okType = 0;

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

                okDepth = 0;
                okVers = 0;
                okType = 0;

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


                    /** Go fin de ligne */
                    while(data[i] != '\n') {
                        i++;
                    }

                    /** Situé sur '{' */
                    i++;
                    while (data[i] != '\n' && data[i] != ' ' && data[i] != '=')  {

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

                                    free(tempOpt);

                                    //printf("%d\n", action[counter].maxDepthOption);

                                    /** Vérification que c'est bien un chiffre */
                                    if (action[counter].maxDepthOption == 0) {
                                        printf("Error, make sure you have enter a number > 1 for the \"max-depth\"\n");
                                        return 0;
                                    }

                                    i++;

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
                                            printf("Error, be careful you wrote comment with the character '#' and not outside. Error in the option \"max-depth\" number %d.\n", counter + 1);
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
                                        //printf("%s", action[counter].versioningOption);
                                    }
                                    else if (!strcmp(tempOpt, "off")) {
                                        action[counter].versioningOption = "off";
                                        //printf("%s", action[counter].versioningOption);
                                    }
                                    else {
                                        printf("Error : Please make sure to write \"on\" or \"off\" in the option \"versioning\" in the action number %d\n", counter + 1);
                                        return 0;
                                    }

                                    free(tempOpt);

                                    i++;

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
                                            printf("Error, be careful you wrote comment with the character '#' and not outside. Error in the option \"versioning\" number %d.\n", counter + 1);
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
                                    for (int p = 0; p < counterComma; p++) {
                                        int countOpt = 0;
                                        while (data[i] != ',' && data[i] != ')') {
                                            if(data[i] != ' ') {
                                                action[counter].typeOption[p][countOpt] = data[i];
                                                countOpt++;
                                            }
                                            i++;
                                        }
                                        i++;
                                    }

                                    /** Fin de ligne */
                                    i++;

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
                                printf("Error, no option recognized or action found multiple times in the action number %d\n", counter + 1);
                                return 0;
                            }

                            /** Go fin de ligne */
                            while(data[i] != '\n') {
                                i++;
                            }

                            /** Situé sur '{' */
                            i++;
                        }
                        else {
                            printf("Error, symbol '{' waited for the option of the action number %d\n", counter + 1);
                            return 0;
                        }
                    }
                }

                else if (data[i] == ' ' || data[i] == 9 || data[i] == '#' || data[i] == '\n') {
                    /** Rien */
                }

                else {
                    printf("Error, options bad for the action number %d\n", counter + 1);
                    return 0;
                }

                /** Si on ne trouve pas ces options, on mets par défaut */
                if (okDepth == 0) {
                    action[counter].maxDepthOption = 0;
                }
                if (okVers == 0) {
                    action[counter].versioningOption = "off";
                }
                if (okType == 0) {
                    action[counter].typeOption = malloc(sizeof(char *));
                    action[counter].typeOption[0] = malloc(5 * sizeof(char));
                    action[counter].typeOption[0] = "null";
                }

                //printf("%d, %d, %d, %d, %d\n", okDepth, okType, okVers, counter, data[i]);
            }
        }
    }
    return 1;
}

/** Lit le fichier et enregistre les tâches */
int readFileTask(Tache *task, int nbrTache, char *data, long fileLength, Action *action, int nbrAction) {

    /** Compteur de taches */
    int counter = -1;

    /** Vérifications pour éviter les multiples options */
    int okSec = 0;
    int okMin = 0;
    int okHour = 0;

    /** On lit le fichier */
    for (int i = 0; i < fileLength; i++) {
        //printf("%c", data[i]);

        /** Saute les commentaires */
        if (data[i] == '#') {
            while (data[i] != '\n') {
                i++;
            }
        }

        /** Détècte le premier '==' */
        if (data[i] == '=' && data[i+1] == '=' && data[i-1] != '=') {
            if (data[i] != '\n') {
                while (data[i] != '\n') {
                    i++;
                }
            }
            i++;

            /** Si on trouve bien le caractère '{' */
            if (data[i] == '{') {
                i++;

                /** On associe le nom */
                if (data[i] == 'n' && data[i+1] == 'a' && data[i+2] == 'm' && data[i+3] == 'e' && data[i+4] == ' ' && data[i+5] == '-' && data[i+6] == '>' && data[i+7] == ' ') {

                    /** On se place à la donnée utilisateur */
                    i += 8;

                    /** Compteur des taches */
                        counter++;

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
                    task[counter].nomTache = malloc((allocName + 1) * sizeof(char));
                    int k = 0;
                    while (allocName != k) {
                        task[counter].nomTache[k] = data[i];
                        k++;
                        i++;
                    }
                    task[counter].nomTache[allocName] = '\0';

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
                            printf("Error, be careful you wrote comment with the character '#' and not outside. Error in the action \"url\" number %d.)\n", counter + 1);
                            return 0;
                        }
                    }

                    /** Go fin de ligne */
                    while(data[i] != '\n') {
                        i++;
                    }

                    /** Situé sur '{' */
                    i++;

                    /** Vérifications pour éviter les multiples options */
                    okSec = 0;
                    okMin = 0;
                    okHour = 0;

                    /** Tant qu'on ne croise pas un '+' ou une ligne vide */
                    while (data[i] != '+' && data[i] != '\n' && data[i] != ' ' && data[i] != '#') {

                        /** Détecte si on trouve '{' */
                        if (data[i] == '{') {
                            i++;

                            /** Secondes */
                            if (data[i] == 's' && data[i+1] == 'e' && data[i+2] == 'c' && data[i+3] == 'o' && data[i+4] ==  'n' && data[i+5] == 'd' && data[i+6] == ' ' && data[i+7] == '-'
                                && data[i+8] == '>' && data[i+9] == ' ' && okSec == 0) {
                                    okSec = 1;

                                    /** On se met juste avant la valeur utilisateur */
                                    i += 10;
                                    int allocSec = 0;
                                    int k = i;

                                    /** Allocation mémoire jusqu'au '}' */
                                    while (data[k] != '}') {
                                        k++;
                                        if (data[k] == '\n') {
                                            return 0;
                                        }
                                    }
                                    allocSec = k - i;
                                    char* tempSec = malloc((allocSec + 1) * sizeof(char));

                                    /** On écrit dans une valeur temporaire la donnée utilisateur */
                                    k = 0;
                                    while (allocSec != k) {
                                        tempSec[k] = data[i];
                                        k++;
                                        i++;
                                    }
                                    tempSec[allocSec] = '\0';

                                    /** Transformation en int */
                                    task[counter].second = atoi(tempSec);

                                    free(tempSec);

                                    /** Vérification que c'est bien un chiffre */
                                    if (task[counter].second == 0) {
                                        printf("Error, make sure you have enter a number >= 1 for the \"second\" in the task number %d\n", counter + 1);
                                        return 0;
                                    }

                                    /** Vérification ce n'est pas au dessus de 60 secondes */
                                    if (task[counter].second >= 60) {
                                        printf("Error, make sure you have enter a number <= 60 for the \"second\" in the task number %d\n", counter + 1);
                                        return 0;
                                    }

                                    i++;

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
                                            printf("Error, be careful you wrote comment with the character '#' and not outside. Error in the option \"second\" number %d.\n", counter + 1);
                                            return 0;
                                        }
                                    }
                            }

                            /** Minutes*/
                            else if (data[i] == 'm' && data[i+1] == 'i' && data[i+2] == 'n' && data[i+3] == 'u' && data[i+4] ==  't' && data[i+5] == 'e' && data[i+6] == ' ' && data[i+7] == '-'
                                && data[i+8] == '>' && data[i+9] == ' ' && okMin == 0) {
                                    okMin = 1;

                                    /** On se met juste avant la valeur utilisateur */
                                    i += 10;
                                    int allocMin = 0;
                                    int k = i;

                                    /** Allocation mémoire jusqu'au '}' */
                                    while (data[k] != '}') {
                                        k++;
                                        if (data[k] == '\n') {
                                            return 0;
                                        }
                                    }
                                    allocMin = k - i;
                                    char* tempMin = malloc((allocMin + 1) * sizeof(char));

                                    /** On écrit dans une valeur temporaire la donnée utilisateur */
                                    k = 0;
                                    while (allocMin != k) {
                                        tempMin[k] = data[i];
                                        k++;
                                        i++;
                                    }
                                    tempMin[allocMin] = '\0';

                                    /** Transformation en int */
                                    task[counter].minute = atoi(tempMin);

                                    free(tempMin);

                                    /** Vérification que c'est bien un chiffre */
                                    if (task[counter].minute == 0) {
                                        printf("Error, make sure you have enter a number >= 1 for the \"minute\" in the task number %d\n", counter + 1);
                                        return 0;
                                    }

                                    /** Vérification ce n'est pas au dessus de 60 secondes */
                                    if (task[counter].minute >= 60) {
                                        printf("Error, make sure you have enter a number <= 60 for the \"minute\" in the task number %d\n", counter + 1);
                                        return 0;
                                    }

                                    i++;

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
                                            printf("Error, be careful you wrote comment with the character '#' and not outside. Error in the option \"minute\" number %d.\n", counter + 1);
                                            return 0;
                                        }
                                    }
                            }

                            /** Heures*/
                            else if (data[i] == 'h' && data[i+1] == 'o' && data[i+2] == 'u' && data[i+3] == 'r' && data[i+4] ==  ' ' && data[i+5] == '-' && data[i+6] == '>' && data[i+7] == ' '
                                && okHour == 0) {
                                    okHour = 1;

                                    /** On se met juste avant la valeur utilisateur */
                                    i += 8;
                                    int allocHour = 0;
                                    int k = i;

                                    /** Allocation mémoire jusqu'au '}' */
                                    while (data[k] != '}') {
                                        k++;
                                        if (data[k] == '\n') {
                                            return 0;
                                        }
                                    }
                                    allocHour = k - i;
                                    char* tempHour = malloc((allocHour + 1) * sizeof(char));

                                    /** On écrit dans une valeur temporaire la donnée utilisateur */
                                    k = 0;
                                    while (allocHour != k) {
                                        tempHour[k] = data[i];
                                        k++;
                                        i++;
                                    }
                                    tempHour[allocHour] = '\0';

                                    /** Transformation en int */
                                    task[counter].hour = atoi(tempHour);

                                    free(tempHour);

                                    /** Vérification que c'est bien un chiffre */
                                    if (task[counter].hour == 0) {
                                        printf("Error, make sure you have enter a number >= 1 for the \"hour\" in the task number %d\n", counter + 1);
                                        return 0;
                                    }

                                    i++;

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
                                            printf("Error, be careful you wrote comment with the character '#' and not outside. Error in the option \"hour\" number %d.\n", counter + 1);
                                            return 0;
                                        }
                                    }
                            }

                            else {
                                printf("Error, no option recognized or task found multiple times in the task number %d\n", counter + 1);
                                return 0;
                            }

                        }
                        else {
                            printf("Error, symbol '{' waited for the option of the action number %d\n", counter + 1);
                            return 0;
                        }

                        /** Go fin de ligne */
                            while(data[i] != '\n') {
                                i++;
                            }

                            /** Situé sur '{' */
                            i++;

                    }
                }
                else {
                    printf("Error, options bad for the action number %d\n", counter + 1);
                    return 0;
                }
            }

            /** Valeurs par défaut pour les champs non renseignés */
            if (okSec == 0) {
                task[counter].second = 0;
            }
            if (okMin == 0) {
                task[counter].minute = 0;
            }
            if (okHour == 0) {
                task[counter].hour = 0;
            }

            /** Si on trouve un '+', on continue */
            if (data[i] == '+') {

                /** Go fin de ligne */
                while(data[i] != '\n') {
                    i++;
                }

                /** Situé sur '{' */
                i++;

                /** Si on trouve bien le caractère '(' */
                if (data[i] == '(') {
                    i++;

                    /** Compteur de virgules, va nous permettre de déterminer le nombre d'attributs pour l'option */
                    int counterComma = 1;

                    /** Allocation mémoire du tableau de 'char' */
                    int k = i;
                    while (data[k] != ')') {
                        k++;
                        if (data[k] == '\n') {
                            return 0;
                        }
                        if (data[k] == ',') {
                            counterComma++;
                        }
                    }



                    task[counter].options = malloc(counterComma * sizeof(char *));

                    /** Allocation de chaque string */
                    k = i;
                    int allocOpt = 0;
                    for (int p = 0; p < counterComma; p++) {
                        allocOpt = 0;
                        while (data[k] != ',' && data[k] != ')') {
                            if(data[k] != ' ') {
                                allocOpt++;
                            }
                            k++;
                        }
                        k++;
                        task[counter].options[p] = malloc(allocOpt + 1 * sizeof(char));
                    }

                    /** On rentre les données dans la structure */
                    for (int p = 0; p < counterComma; p++) {
                        int countOpt = 0;
                        while (data[i] != ',' && data[i] != ')') {
                            if(data[i] != ' ') {
                                task[counter].options[p][countOpt] = data[i];
                                countOpt++;
                            }
                            i++;
                        }
                        task[counter].options[p][allocOpt] = '\0';
                        i++;
                    }

                    /** Fin de ligne */
                    i++;

                    /** Vérification des données après le '{' */
                    while (data[i] != '\n' && data[i] != EOF) {
                        if (data[i] == ' ' || data[i] == 9) {
                            i++;
                        }
                        else if (data[i] == '#') {
                            while (data[i] != '\n') {
                                i++;
                            }
                        }
                        else {
                            printf("Error, be careful you wrote comment with the character '#' and not outside. Error in the option for the task number %d.\n", counter + 1);
                            return 0;
                        }
                    }

                    /** Vérifie que les options de la tache sont bien les mêmes que les noms des actions */
                    for (int c = 0; c < counterComma; c++) {
                        int verifAction = 0;
                        for (int d = 0; d < nbrAction; d++) {
                            if (!strcmp(task[counter].options[c], action[d].nameAction)) {
                                verifAction = 1;
                            }
                        }
                        if (verifAction == 0) {
                            printf("Error, option in task number %d is not recognize \n", counter + 1);
                            return 0;
                        }
                    }

                    /*for (int lol = 0; lol < counterComma; lol++) {
                        printf("%s\n", task[counter].options[lol]);
                    }*/
                }
                else {
                    printf("Error, symbol '(' waited for the option of the task number %d\n", counter + 1);
                    return 0;
                }
            }
            else {
                printf("Error, no option for the task number %d\n", counter + 1);
                return 0;
            }
            //printf("\n%ds, %dm, %dh\n", task[counter].second, task[counter].minute, task[counter].hour);
        }
    }
    return 1;
}

/** Libère la mémoire à la fin du programme */
void freeMemory(Action *action, int nbrAction, Tache *task, int nbrTache) {
    for (int i = 0; i < nbrAction; i++) {
        free(action[i].nameAction);
        free(action[i].urlAction);
    }

    for (int i = 0; i < nbrTache; i++) {
        free(task[i].nomTache);
    }

    // Free les options aussi plz */
}
