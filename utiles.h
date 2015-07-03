#ifndef utiles_H
#define utiles_H
#include <stdio.h>
#include <fstream>
#include <sys/time.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>

static timeval t_ini, t_fin;

void tic();

double toc();

#ifdef DEBUG
	#define _revienta(cond) if (cond) asm("int3"); asm("nop");
#else
	#define _revienta(cond) ;
#endif

#endif
