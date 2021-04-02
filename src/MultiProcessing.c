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
    // On effectue réellement l'action du père qu'après avoir créé les nbLect lecteurs
    if (*numLect == nbLect) {
        printf("Père     \t(mon id -> %d)\n", getpid());
        close(tube[1]); // Fermeture lecture
        char buf[BUF_SIZE];
        while(read(tube[0], buf, sizeof(buf))!=0) {
            printf("%s", buf);
            // Arrête du père quand les fils sont arrêtés
            if(0) {
                sleep(1); // On laisse les fils terminer
                close(tube[0]);// On ferme tube, les fils ne liront plus
                wait(NULL); // On attend la mort des fils
                printf("Le pere meurt\n");
                exit(EXIT_SUCCESS); // On meurt
            }
        }
    }
    // On incrémente nbLect pour savoir où on en est
    *numLect = *numLect + 1;
}