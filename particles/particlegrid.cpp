#include "particlegrid.h"

particlegrid::particlegrid(int n, double cutoff, double size){
	matris = malloc(sizeof(particle_t)*n*n);
}


