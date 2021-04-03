#include "MultiProcessing.h"

void fils(int n, int* somme) {
    char str[BUF_SIZE];
    short pid = getpid();
    int tempActivite = 0;
    printf("Fils n°%d\t(mon id -> %d ,  id de mon père -> %d)\n", n, pid, getppid());
    close (tube[0]); // Fermeture écriture
    while(1) {
        *somme = *somme + 1;
        tempActivite ++;
        sleep(1);
        if(*somme%2 == 0){
            sprintf(str, "Fils n°%d :\n\tsomme -> %d\n\ttemp d'activité -> %d sec\n", n, *somme, tempActivite);
            write(tube[1], str, BUF_SIZE);
        }
    }
    // Quand il n'y a plus rien à lire : arrêt fils
    printf("Le fils n°%d (id: %d) meurt...\n", n, pid);
    close(tube[1]);
    _exit(pid);
}

void pere(int* numLect, int nbLect) {
    char str_monkey[BUF_SIZE_MONKEY];
    FILE *fp;
    int i;
    int size;
    int tempActivitePere = 0;
    // Définition de la commande pour obtenir les processus fils
    char str[50] = "ps -C prog --format '%P %p'";


    // On effectue réellement l'action du père qu'après avoir créé les nbLect lecteurs
    if (*numLect == nbLect) {

        pid_t pid_bis = fork();
        if(pid_bis == 0){
            evilMonkey();
            exit(EXIT_SUCCESS);
        }
        
        printf("Père     \t(mon id -> %d)\n", getpid());

        close(tube[1]); // Fermeture lecture

        close(tubeMonkey[0]); // Fermeture lecture

        char buf[BUF_SIZE];
        while(read(tube[0], buf, sizeof(buf))!=0) {
            
            // Exécution de la commande ps
            fp = popen(str, "r");
            if (fp == NULL)
            {
                printf("Erreur d'execution de popen()\n");
            }

            // On recrée les tableaux pour ne pas avoir de conflit mémoire
            char parentID[256];
            char processID[256];
            int child_list[nbLect];
            i = 0;

            // On récupère les process id
            while (fscanf(fp, "%s %s", parentID, processID) != EOF)
            {
                // On vérifie que la ligne récupérée est bien un ID de processus
                if (strcmp(processID, "PID") && atoi(processID) != getpid()) {
                    child_list[i] = atoi(processID);
                    i++;
                }
            }

            pclose(fp);

            size = sizeof(child_list) / sizeof(child_list[0]);

            sprintf(str_monkey, "%d",size);

            // Affichage des processus fils
            for (int j = 0; j < size; j++) {
                printf("%d\n", child_list[j]);
                sprintf(str_monkey + strlen(str_monkey), "-%d",child_list[j]);
            }

            write(tubeMonkey[1], str_monkey, BUF_SIZE_MONKEY);

            //printf("Le message à envoyer %s\n",str_monkey);

            printf("%s", buf);
            
            // Arrête du père quand les fils sont arrêtés
            if(0) {
                sleep(1); // On laisse les fils terminer
                close(tube[0]);// On ferme tube, les fils ne liront plus
                wait(NULL); // On attend la mort des fils
                printf("Le pere meurt\n");
                exit(EXIT_SUCCESS); // On meurt
            }
            tempActivitePere ++;
            printf("\tTemps écoulé du père : %d\n", tempActivitePere);

        }
    }
    // On incrémente nbLect pour savoir où on en est
    *numLect = *numLect + 1;
}

void evilMonkey(){
    // Variable aléatoire : temps de lancement du evil monkey (le x de la consigne)
    int sleepTime = alea(10); 
    //printf("Hmmm... je dors pendant %d secondes\n", sleepTime);
    sleep(sleepTime);


    close(tubeMonkey[1]); // Ouverture du pipe Père -> Evil Monkey lecture

    int compteur = 1; //Compteur pour lancer la lecture du pipe uniquement sur les dernières informations reçues (toutes les mêmes)
    int x = 0; //element extrait de message du pipe
    int passage = 0; //permet de savoir si on a déjà iniatialisé le nombre de processus.
    int *elements; //tableau des fils (leur PID)
    char buf_Monkey[BUF_SIZE_MONKEY]; //message (char) stocké dans le buffer du Evil Monkey
    int size = 0; //taille du tableau élements (ou nombre de processus)
    int reponse; //variable de retour pour le SIGKILL
    void erreur(const char *); //void de gestion d'erreur
    char delim[] = "-"; //delimiter utilisé dans le message du pipe (séparant les infos envoyées par le père)

    while(read(tubeMonkey[0], buf_Monkey, sizeof(buf_Monkey))!=0) { //ouverture en lecture du pipe
        if(compteur == sleepTime){
            char *ptr = strtok(buf_Monkey,delim);
            while(ptr != NULL){

                x = atoi(ptr);

                if(passage == 0){
                    elements = creerTableauEntier(x);
                    size = x;
                }
                else{
                    elements[passage-1] = x;
                }
                passage ++;
                //printf("\t'%s'\n",ptr);
                ptr = strtok(NULL,delim);
            }

            for(int i=0; i<size; i++){
                printf("element : %d %d\n ", elements[i], i);
            }
            compteur = 1;

            int targetedSonNumber = alea(size);
            int targetedSon = elements[targetedSonNumber];

            printf("\nL'evil monkey va frapper le fils %d !\n", targetedSon);
            if ((reponse = kill(targetedSon,SIGKILL)) == -1)
                erreur("SIGKILL kill");

            printf("Le processus %d a été tué !\n",targetedSon);
            if(size>0){
            free(elements);
            }
        }
        compteur ++;
    }
    close(tubeMonkey[0]);
}

void erreur(const char * message){
    perror(message);
    exit(-1);
}

int alea(int taille){
    int res;
    res = rand() % taille;
    return res;
}