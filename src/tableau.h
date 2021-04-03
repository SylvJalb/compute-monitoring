#ifndef __TABLEAU_H_
#define __TABLEAU_H_

//onutilise la librairie utile ici pour la gestion de la mémoire et des "exit"
#include <stdlib.h>
//on utilise la librairie utile pour les interactions avec l'utilisateur
#include <stdio.h>

#define ERREUR_ALLOCATION 1

int *creerTableauEntier(int int_taille);

char *creerTableauChar(int taille);

char **creerTableau2DChar(int tailleX, int tailleY);

void freeTableau2DChar(char** tableau, int taille_tableau);

#endif
