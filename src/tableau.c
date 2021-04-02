#include "tableau.h"

char *creerTableauChar(int int_taille){
  //DECLARATION DES VARIABLES
  char *tchar_tab; //le poiteur vers la premiere case du tableau

  //ALLOCATION DE LA MEMOIRE
  tchar_tab = malloc(int_taille * sizeof(char));
  //Si l'allocation c'est fini en echec
  if(tchar_tab == NULL){
    //Avertir l'utilisateur
    printf("Erreur d'allocation mémoire !");
    //Quitter le programme avec un message d'erreur
    exit(ERREUR_ALLOCATION);
  }

  //retourner l'addresse de la premiere case du tableau, soit : tint_tab
  return(tchar_tab);
}

char **creerTableau2DChar(int tailleX, int tailleY){
  char **tab2D = malloc(tailleX * sizeof(char*));
  //Si l'allocation c'est fini en echec
  if(tab2D == NULL){
    //Avertir l'utilisateur
    printf("Erreur d'allocation mémoire !");
    //Quitter le programme avec un message d'erreur
    exit(ERREUR_ALLOCATION);
  }
  for(int i = 0 ; i < tailleX ; i++){
    tab2D[i] = creerTableauChar(tailleY);
  }
  return tab2D;
}

void freeTableau2DChar(char** tableau, int taille_tableau){
  for(int i = 0 ; i < taille_tableau ; i++){
    free(tableau[i]);
  }
  free(tableau);
}