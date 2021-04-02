#ifndef __MULTIPROC_H_
#define __MULTIPROC_H_

#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>

#define BUF_SIZE 200

int tube[2];
void fils(int n, int* somme);
void pere(int* numLect, int nbLect);

#endif