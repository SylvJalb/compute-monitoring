#include "MultiProcessing.h"


int main(int argc, char ** argv) {
    system("@cls||clear");
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

    if (pipe(tubeMonkey) == -1) {
        perror("Pipe Monkey");
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
            pere(&numLect, nbLect);
        }
        // Action des fils
        else if (pid == 0) {
            int depart = BORNE_INF / nbLect;
            fils(numLect, depart);
        }
    } while(pid != 0 && numLect <= nbLect);
    return 0;

}
