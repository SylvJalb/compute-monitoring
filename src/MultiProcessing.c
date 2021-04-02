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
    char str_monkey[BUF_SIZE];
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
        close(tubeMonkey[0]); // Ouverture lecture

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

            sprintf(str_monkey, "%d_",size);

            // Affichage des processus fils
            for (int j = 0; j < size; j++) {
                printf("%d\n", child_list[j]);
                sprintf(str_monkey, "|%d|",child_list[j]);
            }
            write(tubeMonkey[1], str_monkey, BUF_SIZE_MONKEY);
            close(tubeMonkey[1]);

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

    sleep(15);

    close(tubeMonkey[0]); // Fermeture lecture
    char buf_Monkey[BUF_SIZE_MONKEY];

        while(read(tubeMonkey[0], buf_Monkey, sizeof(buf_Monkey))!=0) {
            
        }

    int targetedSonNumber = alea(size); //Remplacer val par la taille de la liste des fils
    int targetedSon = child_list[targetedSonNumber];
    printf("L'evil monkey va frapper le fils %d\n", targetedSon);
    printf("KILL");
}


int alea(int taille){
    int res;
    res = rand() % taille;
    return res;
}