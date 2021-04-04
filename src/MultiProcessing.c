#include "MultiProcessing.h"

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


int valueinarray(int value, int* array) {
    int i;
    int size = sizeof(*array) / sizeof(array[0]);
    for(i = 0; i <= size; i++) {
        if (array[i] == value)
            return 1;
    }
    return 0;
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


void update_child_list(int* child_list, int no_kill[3]) {
    // Définition de la commande pour obtenir les processus fils
    char str[50] = "ps -C prog --format '%P %p'";
    // On recrée les tableaux pour ne pas avoir de conflit mémoire
    char parentID[256];
    char processID[256];
    int i = 0;
    FILE *fp;


    // Exécution de la commande ps
    fp = popen(str, "r");
    if (fp == NULL)
    {
        printf("Erreur d'execution de popen()\n");
    }


    // On récupère les process id
    while (fscanf(fp, "%s %s", parentID, processID) != EOF)
    {
        // On vérifie que la ligne récupérée est bien un ID de processus
        if (strcmp(processID, "PID") && atoi(processID) != getpid() && (valueinarray(atoi(processID), no_kill) == 0)) {
            child_list[i] = atoi(processID);
            i++;
        }
    }

    pclose(fp);
}

void evilMonkey(){
    // On initialise le générateur aléatoire
    srand( time( NULL ) );
    // Variable aléatoire : temps de lancement du evil monkey (le x de la consigne)
    int sleepTime = 5 + alea(5);
    printf("\n\n/!\\\tJe vais tuer un fils dans %d sec !\t/!\\\n\n", sleepTime);
    //printf("Hmmm... je dors pendant %d secondes\n", sleepTime);
    sleep(sleepTime);

    close(tubeMonkey[1]); // Ouverture du pipe Père -> Evil Monkey lecture

    int x = 0; //element extrait de message du pipe
    int passage = 0; //permet de savoir si on a déjà iniatialisé le nombre de processus.
    int *elements; //tableau des fils (leur PID)
    char buf_Monkey[BUF_SIZE_MONKEY]; //message (char) stocké dans le buffer du Evil Monkey
    int size = 0; //taille du tableau élements (ou nombre de processus)
    int reponse; //variable de retour pour le SIGKILL
    void erreur(const char *); //void de gestion d'erreur
    char delim[] = "-"; //delimiter utilisé dans le message du pipe (séparant les infos envoyées par le père)

    int lecture; // indicateur de lecture pour lire le contenu lorsqu'il y en a
    time_t depart = time(NULL); //valeur de timing pour la synchronisation (départ)
    time_t courant; //valeur de timing pour la synchronisation (courant)

    while (1) {
        // Mise à jour du temps courant
        courant = time(NULL);
        if (difftime(courant, depart) >= (double) sleepTime) { //synchronisation avec le temps courant
            lecture = 0; 
            write(tube[1], "!", BUF_SIZE);
            usleep(1000000); //sleep en microseconde
            while (lecture == 0 && read(tubeMonkey[0], buf_Monkey, sizeof(buf_Monkey))!=0) { //ouverture en lecture du pipe
                char *ptr = strtok(buf_Monkey,delim); //Initialisation tableau de char et division de cette chaine grâce au délimiteur
                while(ptr != NULL){ //Tant qu'il y a des "sous chaines" à lire

                    x = atoi(ptr); //Conversion de la chaine en int

                    if(passage == 0){ //Premier passage, on veut récupérer uniquement la première valeur (nombre de processus)
                        elements = creerTableauEntier(x); //On initiatlise un tableau pour stocker les PID des fils
                        size = x; //On sauvegarde la taille (qui est le nombre de processus récupéré)
                    }
                    else{
                        elements[passage-1] = x; //sinon, outre le premier élément, tous les autres sont les PIDs des fils.
                    }
                    passage ++; // Incrémentation du passage qui sert d'indice pour le tableau éléments.
                    //printf("\t'%s'\n",ptr);
                    ptr = strtok(NULL,delim);
                }
                passage = 0; // Réinitialisation pour le prochain appel du evil monkey

                int targetedSonNumber = alea(size); //Nombre aléatoire compris entre 0 et le nombre de processus
                int targetedSon = elements[targetedSonNumber]; //PID du fils visé par Evil Monkey

                // L'evil monkey va frapper le fils
                if ((reponse = kill(targetedSon,SIGTERM)) == -1) //kill du fils visé
                    erreur("SIGTERM");

                printf("\n\n/!\\\tLe processus %d a été tué !\t/!\\\n\n",targetedSon);

                if(size>0){
                    free(elements); // On libère éléments
                }

                // Prochain temps avant de kill
                sleepTime = 5 + alea(5);
                printf("\n\n/!\\\tJe vais tuer un fils dans %d sec !\t/!\\\n\n", sleepTime);

                // Réinitialisation du temps de départ
                depart = time(NULL);

                lecture++;
            }
        }
        usleep(500000); //sleep (en microseconde pour éviter le spam)
    }
    close(tubeMonkey[0]); //fermeture descripteur
}


void fils(int n, int depart) {
    short pid = getpid();
    

    int somme = depart;
    char str[BUF_SIZE];
    int tempActivite = 0;
    printf("Fils n°%d\t(mon id -> %d ,  id de mon père -> %d)\n", n, pid, getppid());
    close (tube[0]); // Fermeture lecture
    while(1) {
        somme = somme + 1;
        tempActivite ++;
        sleep(1);
        if(tempActivite%2 == 0){
            sprintf(str, "Fils n°%d :\n\tsomme -> %d \n\ttemp d'activité -> %d sec\n\tpid -> %d\n", n, somme, tempActivite, getpid());
            if (write(tube[1], str, BUF_SIZE) == -1) {
                perror("write\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    // Quand il n'y a plus rien à lire : arrêt fils
    printf("Le fils n°%d (id: %d) meurt...\n", n, pid);
    close(tube[1]);
    _exit(pid);
}


void pere(int* numLect, int nbLect) {
    char str_monkey[BUF_SIZE_MONKEY];
    int i;
    int status;
    int tempActivitePere = 0;
    int *child_list = creerTableauEntier(nbLect);
    int pid_fils;
    int num_fils;
    char delim[] = " ";

    // On effectue réellement l'action du père qu'après avoir créé les nbLect lecteurs
    if (*numLect == nbLect) {
        // Création du processus Evil Monkey
        pid_t pid_monkey = fork();
        if(pid_monkey == 0){
            evilMonkey();
            exit(EXIT_SUCCESS);
        }

        // Création du processus qui lit ce que l'utilisateur veut
        pid_t pid_lecture = fork();
        if(pid_lecture == 0){
            userLecture();
            exit(EXIT_SUCCESS);
        }

        // Création du processus qui calcule les 3 secondes à attendre pour afficher le moniteur
        pid_t pid_refresh_time = fork();
        if(pid_refresh_time == 0){
            refreshing();
            exit(EXIT_SUCCESS);
        }

        int no_kill[3] = {pid_monkey, pid_lecture, pid_refresh_time};
        
        printf("Père     \t(mon id -> %d)\n", getpid());

        close(tubeMonkey[0]); // Fermeture lecture
        char **save_children = creerTableau2DChar(nbLect, BUF_SIZE);
        int *sommes = creerTableauEntier(nbLect);
        char buf[BUF_SIZE];
        int total_somme = 0;

        while(read(tube[0], buf, sizeof(buf))!=0) {
            if(buf[2] == '|') {
                //clear de l'écran
                system("@cls||clear");
                //maj de la liste des fils
                update_child_list(child_list, no_kill);
                printf("Voici l'état des %d fils :\n\n", nbLect);
                //initialisation de la somme totale
                total_somme = 0;
                //parcour de tous les fils
                for(int numero_fils = 0; numero_fils < nbLect; numero_fils++){
                    //affichage de l'état du fils courant
                    printf("%s\n", save_children[numero_fils]);
                    //calcul de la somme total
                    total_somme += sommes[numero_fils];
                }
                printf("La somme totale calculé est : %d\n\n", total_somme);
                printf("Appuyez sur \"entrée\" pour actualiser le moniteur...\n");
            } else if (buf[0] == '!') {
                //maj de la liste des fils
                update_child_list(child_list, no_kill);

                sprintf(str_monkey, "%d",nbLect);

                // Génération du message pour l'evil monkey
                for (int j = 0; j < nbLect; j++) {
                    sprintf(str_monkey + strlen(str_monkey), "-%d",child_list[j]);
                }

                // Envoi du message à l'evil monkey
                write(tubeMonkey[1], str_monkey, BUF_SIZE_MONKEY);
            } else {               
                update_child_list(child_list, no_kill);
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
                char *ptr = strtok(buf, delim);
                int k = 0;
                while(ptr != NULL){
                    if (k == 4) {
                        sommes[num_fils-1] = atoi(ptr);
                    }
                    ptr = strtok(NULL,delim);
                    k++;
                }

                //calcul de la somme totale
                total_somme = 0;
                for(int numero_fils = 0; numero_fils < nbLect; numero_fils++){
                    total_somme += sommes[numero_fils];
                }
                
                // Arrête du père quand les fils sont arrêtés
                if(total_somme >= BORNE_SUP) {
                    close(tube[0]);// On ferme tube, les fils ne liront plus
                    //clear de l'écran
                    system("@cls||clear");
                    //maj de la liste des fils
                    update_child_list(child_list, no_kill);
                    printf("Voici l'état des %d fils à la fin :\n\n", nbLect);
                    //initialisation de la somme totale
                    total_somme = 0;
                    //parcour de tous les fils
                    for(int numero_fils = 0; numero_fils < nbLect; numero_fils++){
                        //affichage de l'état du fils courant
                        printf("%s\n", save_children[numero_fils]);
                        //calcul de la somme total
                        total_somme += sommes[numero_fils];
                    }
                    printf("La somme totale calculé est : %d\n\nFIN DU PROGRAMME\n", total_somme);
                    freeTableau2DChar(save_children, nbLect);
                    free(sommes);
                    free(child_list);
                    for(int j = 0; j < nbLect ; j++){
                        kill(child_list[j], SIGTERM);
                    }
                    for(long unsigned int j = 0 ; j < sizeof(no_kill) ; j++){
                        kill(no_kill[j], SIGTERM);
                    }
                    exit(EXIT_SUCCESS); // On meurt
                }
                tempActivitePere ++;
                //printf("\tTemps écoulé du père : %d\n", tempActivitePere);
            }

            for (int f = 0; f < nbLect; f++)
            {
                int wpid = waitpid(child_list[f], &status, WNOHANG);
                //printf("Index %d; pid %d ; wpid : %d ; status %d\n", f, child_list[f], wpid, status);
                if (wpid == child_list[f]) {
                    i = 0;
                    do
                    {
                        char* tmp_str = malloc(BUF_SIZE * sizeof(char));
                        strncpy(tmp_str, save_children[i], BUF_SIZE);

                        //récupération du numéro du fils
                        int current_char = 9;
                        while(tmp_str[current_char] != ' '){
                            current_char++;
                        }
                        int number_size = current_char-8;

                        char* number = creerTableauChar(number_size);
                        for (int j = 0; j < number_size; j++) {
                            number[j] = tmp_str[8+j];
                        }
                        //sauvegarde du numero du fils
                        num_fils = atoi(number);
                        free(number);

                        char *ptr = strtok(tmp_str, delim);
                        int k = 0;
                        while(ptr != NULL){
                            if (k == 11) {
                                pid_fils = atoi(ptr);
                            }
                            ptr = strtok(NULL,delim);
                            k++;
                        }
                        free(tmp_str);
                        i++;
                    } while (pid_fils != wpid);
                                        
                    pid_t nouveau_fils = fork();
                    if (nouveau_fils == 0) {
                        printf("\nNOUVEAU FILS (relance du fils %d)\n\n", num_fils);
                        fils(num_fils, sommes[f]);
                        exit(EXIT_SUCCESS);
                    }
                }
                
            }
        }
    }
    // On incrémente nbLect pour savoir où on en est
    *numLect = *numLect + 1;
}