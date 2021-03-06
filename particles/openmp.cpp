#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "common.h"
#include <vector>
#include <iostream>

#define cutoff 0.01

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
        printf( "-n <int> to set number of particles\n" );
        printf( "-o <filename> to specify the output file name\n" );
        printf( "-p <int> number of threads\n" );
        return 0;
    }

    int n = read_int( argc, argv, "-n", 1000 );
    int numThreads = read_int( argc, argv, "-p", 2 );
    char *savename = read_string( argc, argv, "-o", NULL );

    FILE *fsave = savename ? fopen( savename, "w" ) : NULL;

    particle_t *particles = (particle_t*) malloc( n * sizeof(particle_t) );
    double size = set_size( n );
    init_particles( n, particles );

    //
    //  simulate a number of time steps
    //
	int numberOfRows = floor(size/cutoff);
	cout<<numberOfRows*numberOfRows<<endl;
 
    double simulation_time = read_timer( );
	vector<particle_t> **matrix = createGrid(n, numberOfRows); //Create the grid
	
	//OK, lets go!
    #pragma omp parallel num_threads(numThreads)
    for( int step = 0; step < 1000; step++ ){
    	
    	//Empty the matrix
		#pragma omp for
		for(int i = 0; i < numberOfRows; i++){
			for(int j = 0; j < numberOfRows; j++){
				matrix[i][j].clear();
			}
		}
	
		//Place each particle in its place in the matrix
		#pragma omp single
		for(int i = 0; i < n; i++){	
			particle_t temp = particles[i];
			int column = floor(temp.x/size*numberOfRows);
			int row = floor(temp.y/size*numberOfRows);
			matrix[row][column].push_back(temp);
		}

        //
        //  compute all forces
        //
        #pragma omp for
        for(int i = 0; i < n; i++ ){
            particles[i].ax = particles[i].ay = 0;
            
            //Find particles in range
			vector<particle_t> nearest = findNearest(particles[i], numberOfRows, size, matrix);
			for(int j = 0; j < nearest.size(); j++) {
				if(!(nearest[j].x == particles[i].x && nearest[j].y == particles[i].y)){ //Dont apply for to the curretn particle, i.e dont do apply_force(particles[i], particles[i]);
					apply_force(particles[i], nearest[j]);	//DO IT!			
				}			
			}
		}

        
        //
        //  move particles
        //
        #pragma omp for
        for( int k = 0; k < n; k++ ) 
            move( particles[k] );
        
        //
        //  save if necessary
        //
        #pragma omp master
        if( fsave && (step%SAVEFREQ) == 0 )
            save( fsave, n, particles );
            
    }
    simulation_time = read_timer( ) - simulation_time;
    
    printf("n = %d, n_threads = %d, simulation time = %g seconds\n", n, numThreads, simulation_time );
    
    free( particles );
    if( fsave )
        fclose( fsave );
    
    cout<<getLol()<<endl;
    
    return 0;
}
