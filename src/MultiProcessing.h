#ifndef __MULTIPROC_H_
#define __MULTIPROC_H_

#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int tube[2];
int somme = 0;
void fils(int n);

#endif