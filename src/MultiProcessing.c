#include "MultiProcessing.h"

void fils(int n, int* somme) {
    char str[BUF_SIZE];
    short pid = getpid();
    int tempActivite = 0;
    printf("Fils n°%d\t(mon id -> %d ,  id de mon père -> %d)\n", n, pid, getppid());
    close (tube[0]); // Fermeture lecture
    while(1) {
        *somme = *somme + 1;
        tempActivite ++;
        sleep(1);
        if(*somme%2 == 0){
            sprintf(str, "Fils n°%d :\n\tsomme -> %d \n\ttemp d'activité -> %d sec\n", n, *somme, tempActivite);
            write(tube[1], str, BUF_SIZE);
        }
    }
    // Quand il n'y a plus rien à lire : arrêt fils
    printf("Le fils n°%d (id: %d) meurt...\n", n, pid);
    close(tube[1]);
    _exit(pid);
}

void evilMonkey(){
    sleep(15);
    //printf("KILL");
}


void userLecture(){
    close (tube[0]); // Fermeture lecture
    char c[BUF_SIZE] = "  |";
    while(1){
        c[0] = getchar();
        write(tube[1], c, BUF_SIZE);
    }
}

void refreshing(){
    close (tube[0]); // Fermeture lecture
    char c[BUF_SIZE] = "  |";
    while(1){
        sleep(3);
        write(tube[1], c, BUF_SIZE);
    }
}


void pere(int* numLect, int nbLect) {
    FILE *fp;
    int i;
    int size;
    int tempActivitePere = 0;
    // Définition de la commande pour obtenir les processus fils
    char str[50] = "ps -C prog --format '%P %p'";


    // On effectue réellement l'action du père qu'après avoir créé les nbLect lecteurs
    if (*numLect == nbLect) {
        // Création du processus Evil Monkey
        pid_t pid_bis = fork();
        if(pid_bis == 0){
            evilMonkey();
            exit(EXIT_SUCCESS);
        }

        // Création du processus qui lit ce que l'utilisateur veut
        pid_bis = fork();
        if(pid_bis == 0){
            userLecture();
            exit(EXIT_SUCCESS);
        }

        // Création du processus qui calcul les 3 secondes à attendre pour afficher le moniteur
        pid_bis = fork();
        if(pid_bis == 0){
            refreshing();
            exit(EXIT_SUCCESS);
        }

        int iterTimeHit = (alea(4))*2;
 
        printf("L'evil monkey va frapper à l'itération %d\n\n", iterTimeHit);
        
        printf("Père     \t(mon id -> %d)\n", getpid());

        close(tube[1]); // Fermeture ecriture
        char **save_children = creerTableau2DChar(nbLect, BUF_SIZE);
        char *sommes = creerTableauEntier(nbLect);
        char buf[BUF_SIZE];

        while(read(tube[0], buf, sizeof(buf))!=0) {
            if(buf[2] == '|') {
                system("@cls||clear");
                printf("Voici l'état des %d fils :\n\n", nbLect);
                int total_somme = 0;
                for(int numero_fils = 0; numero_fils < nbLect; numero_fils++){
                    printf("%s\n", save_children[numero_fils]);
                    total_somme += sommes[numero_fils];
                }
                printf("La somme totale calculé est : %d\n\n", total_somme);
                printf("Appuyez sur \"entrer\" pour actualiser le moniteur...\n");
            }
            else {
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

            
                //int targetedSonNumber = alea(size); //Remplacer val par la taille de la liste des fils
                //int targetedSon = child_list[targetedSonNumber];
                //printf("L'evil monkey va frapper le fils %d\n", targetedSon);

                // Affichage des processus fils
                /*
                for (int j = 0; j < size; j++) {
                    printf("%d\n", child_list[j]);
                }
                */
                
                //récupération du numéro du fils
                int current_char = 9;
                while(buf[current_char] != ' '){
                    current_char++;
                }
                int number_size = current_char-8;
                char* number = creerTableauChar(number_size);
                for (int j = 0; j < number_size; j++) {
                    number[j] = buf[8+j];
                }
                //sauvegarde de l'état du fils
                int num_fils = atoi(number);
                strncpy(save_children[num_fils-1], buf, BUF_SIZE);
                free(number);

                //récupération de la somme
                current_char = 23;
                while(buf[current_char] != ' '){
                    current_char++;
                }
                number_size = current_char-22;
                number = creerTableauChar(number_size);
                for (int j = 0; j < number_size; j++) {
                    number[j] = buf[22+j];
                }
                //sauvegarde de la somme
                sommes[num_fils-1]= atoi(number);
                free(number);
                
                // Arrête du père quand les fils sont arrêtés
                if(0) {
                    sleep(1); // On laisse les fils terminer
                    close(tube[0]);// On ferme tube, les fils ne liront plus
                    wait(NULL); // On attend la mort des fils
                    printf("Le pere meurt\n");
                    exit(EXIT_SUCCESS); // On meurt
                }
                tempActivitePere ++;
                //printf("\tTemps écoulé du père : %d\n", tempActivitePere);
            }
        }
        freeTableau2DChar(save_children, nbLect);
        free(sommes);
    }
    // On incrémente nbLect pour savoir où on en est
    *numLect = *numLect + 1;
}


int alea(int taille){
    int res;
    res = rand() % taille;
    return res;
}