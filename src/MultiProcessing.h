#ifndef __MULTIPROC_H_
#define __MULTIPROC_H_

#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "tableau.h"

#include <string.h>

#define BORNE_INF 36
#define BORNE_SUP 108

#define BUF_SIZE 255
#define BUF_SIZE_MONKEY 200

int tube[2];
int tubeMonkey[2];
void fils(int n, int depart);
void pere(int* numLect, int nbLect);

#endif
