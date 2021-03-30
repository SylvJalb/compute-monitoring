#include "MultiProcessing.h"

void fils(int n) {
    char buf;
    short pid = getpid();
    int somme = 0;
    printf("Je suis fils n°%d-%d, mon père est %d\n", n, pid, getppid());
    close (tube[1]); // Fermeture écriture
    // Lecture du pipe
    while(read(tube[0], &buf, sizeof(buf))!=0) {
        printf("fils %d : %c\n", n, buf);
    }
    // Quand il n'y a plus rien à lire : arrêt fils
    printf("Je suis le fils %d-%d, je meurs\n", n, pid);
    close(tube[0]);
    _exit(pid); } int main(int argc, char ** argv) {
    int numLect = 1; // Numéro des lecteurs
    int nbLect; // Nb de lecteur
    int nbEssai = 10; // Nb d'essai pour faire un fork au cas où ça rate
    int essai;
    pid_t pid;
    assert(argc == 2);
    nbLect = atoi(argv[1]);
    if (pipe(tube) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    // Boucle que fait nbLect fois le père (au moins 1 fois)
    do {
        // Le père fait un fork, et retente nbEssai fois si ça rate
        essai=0;
        do {
            pid = fork();
            essai++;
        } while(pid == -1 && essai <= nbEssai);
        // Action du père
        if (essai != nbEssai && pid != 0) {
            // On effectue réellement l'action du père qu'après avoir créé les nbLect lecteurs
            if (numLect == nbLect) {
                printf("Je suis le père %d\n", getpid());
                close(tube[0]); // Fermeture lecture
                char c;
                while ((c=getchar()) != EOF) {
                    // Ecriture tube
                    if((c<='z')&&(c>='a')) {
                        write(tube[1], &c, 1);//ecriture dans tube
                    }
                    // Arrête du père quand les fils sont arrêtés
                    else if(atoi(&c) == 1) {
                        sleep(1); // On laisse les fils terminer
                        close(tube[1]);// On ferme tube, les fils ne liront plus
                        wait(NULL); // On attend la mort des fils
                        printf("Je suis le pere, je meurs\n");
                        exit(EXIT_SUCCESS); // On meurt
                    }
                }
            }
            // On incrémente nbLect pour savoir où on en est
            numLect++;
        }
        // Action des fils
        else if (pid == 0) {
            fils(numLect);
        }
    } while(pid != 0 && numLect <= nbLect);
    return 0; }