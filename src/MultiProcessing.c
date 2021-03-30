#include "MultiProcessing.h"

void fils(int n) {
    char buf;
    short pid = getpid();
    printf("Je suis fils n°%d-%d, mon père est %d\n", n, pid, getppid());
    close (tube[1]); // Fermeture écriture
    // Lecture du pipe
    while(read(tube[0], &buf, sizeof(buf))!=0) {
        printf("fils %d : %c\n", n, buf);
    }
    // Quand il n'y a plus rien à lire : arrêt fils
    printf("Je suis le fils %d-%d, je meurs\n", n, pid);
    close(tube[0]);
    _exit(pid);
}