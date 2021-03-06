#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <iostream>
#include <vector>

#include "common.h"

#define cutoff		0.01

using namespace std;

//
//  benchmarking program
//
int main( int argc, char **argv )
{    
    if( find_option( argc, argv, "-h" ) >= 0 )
    {
        printf( "Options:\n" );
        printf( "-h to see this help\n" );
        printf( "-n <int> to set the number of particles\n" );
        printf( "-o <filename> to specify the output file name\n" );
        return 0;
    }
    
    int n = read_int( argc, argv, "-n", 1000 );

    char *savename = read_string( argc, argv, "-o", NULL );
    
    FILE *fsave = savename ? fopen( savename, "w" ) : NULL;
    particle_t *particles = (particle_t*) malloc( n * sizeof(particle_t) );
    double size = set_size( n );
    init_particles( n, particles );

	int numberOfRows = floor(size/cutoff);
	cout<<numberOfRows*numberOfRows<<endl;

	vector<particle_t> **matrix = createGrid(n, numberOfRows);
    
    //
    //  simulate a number of time steps
    //
    double simulation_time = read_timer( );
    for( int step = 0; step < NSTEPS; step++ ){
    
		for(int i = 0; i < numberOfRows; i++){
			for(int j = 0; j < numberOfRows; j++){
				matrix[i][j].clear();
			}
		}

		for(int i = 0; i < n; i++){
			particle_t temp = particles[i];
			int column = floor(temp.x/size*numberOfRows);
			int row = floor(temp.y/size*numberOfRows);
			matrix[row][column].push_back(temp);
		}
		
        //
        //  compute forces
        //
        for( int i = 0; i < n; i++ ){
            particles[i].ax = particles[i].ay = 0;
			
			vector<particle_t> nearest = findNearest(particles[i], numberOfRows, size, matrix);
			for(int j = 0; j < nearest.size(); j++) {
				if(!(nearest[j].x == particles[i].x && nearest[j].y == particles[i].y)){
					apply_force(particles[i], nearest[j]);				
				}			
			}
        }
        
        //
        //  move particles
        //
        for( int i = 0; i < n; i++ ) {
            move( particles[i] );
		}
        
        //
        //  save if necessary
        //
        if( fsave && (step%SAVEFREQ) == 0 )
            save( fsave, n, particles );
    }
    
    simulation_time = read_timer( ) - simulation_time;
    
    printf( "n = %d, simulation time = %g seconds\n", n, simulation_time );
    
    free( particles );
    if( fsave )
        fclose( fsave );

    return 0;
}




