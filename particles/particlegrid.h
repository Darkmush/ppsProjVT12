#ifndef PARTICLE_H
#define PARTICLE_H
#include "common.h"

class particlegrid {
	private:
		particle_t *matris;

		//particlegrid(){}
	public:
		particlegrid(int n, double cutoff, double size);

};

#endif
