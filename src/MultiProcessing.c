#include "MultiProcessing.h"

void fils(int n) {
    char str[20];
    short pid = getpid();
    int somme = 0;
    printf("Je suis fils n°%d-%d, mon père est %d\n", n, pid, getppid());
    close (tube[0]); // Fermeture écriture
    // Lecture du pipe
    while(1) {
        somme = somme + 1;
        sleep(1);
        sprintf(str, "fils %d : %d\n", n, somme);
        write(tube[1], str, 20);
    }
    // Quand il n'y a plus rien à lire : arrêt fils
    printf("Je suis le fils %d-%d, je meurs\n", n, pid);
    close(tube[1]);
    _exit(pid);
}