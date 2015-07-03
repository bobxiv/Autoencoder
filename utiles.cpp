#include "utiles.h"

void tic() {
	gettimeofday(&t_ini, NULL);
}

double toc() {
	gettimeofday(&t_fin, NULL);
	return (double)(t_fin.tv_sec+(double)t_fin.tv_usec/1000000)-(double)(t_ini.tv_sec + (double)t_ini.tv_usec/1000000);
}
