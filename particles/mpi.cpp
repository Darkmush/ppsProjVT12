#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "common.h"
#include <iostream>
#include <vector>
#include <math.h>

#define cutoff 0.01

using namespace std;

//
//  benchmarking program
//
int main( int argc, char **argv )
{    
    //
    //  process command line parameters
    //
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
    
    //
    //  set up MPI
    //
    int n_proc, rank;
    MPI_Init( &argc, &argv );
    MPI_Comm_size( MPI_COMM_WORLD, &n_proc );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    
    //
    //  allocate generic resources
    //
    FILE *fsave = savename && rank == 0 ? fopen( savename, "w" ) : NULL;
    particle_t *particles = (particle_t*) malloc( n * sizeof(particle_t) );
    
    MPI_Datatype PARTICLE;
    MPI_Type_contiguous( 6, MPI_DOUBLE, &PARTICLE );
    MPI_Type_commit( &PARTICLE );
    
    //
    //  set up the data partitioning across processors
    //
    int particle_per_proc = (n + n_proc - 1) / n_proc;
    int *partition_offsets = (int*) malloc( (n_proc+1) * sizeof(int) );
    for( int i = 0; i < n_proc+1; i++ )
        partition_offsets[i] = min( i * particle_per_proc, n );
    
    int *partition_sizes = (int*) malloc( n_proc * sizeof(int) );
    for( int i = 0; i < n_proc; i++ )
        partition_sizes[i] = partition_offsets[i+1] - partition_offsets[i];
    
    //
    //  allocate storage for local partition
    //
    int nlocal = partition_sizes[rank];
    particle_t *local = (particle_t*) malloc( nlocal * sizeof(particle_t) );
    
    //
    //  initialize and distribute the particles (that's fine to leave it unoptimized)
    //
    double size2 = set_size( n );
    if( rank == 0 ){
        init_particles( n, particles );
    }
    MPI_Scatterv( particles, partition_sizes, partition_offsets, PARTICLE, local, nlocal, PARTICLE, 0, MPI_COMM_WORLD );
    
	int numberOfRows = floor(size2/cutoff);
	//cout<<numberOfRows*numberOfRows<<endl;
    
    vector<particle_t> **matrix = createGrid(n, numberOfRows); //Create the grid
    //
    //  simulate a number of time steps
    //
    double simulation_time = read_timer( );
    for( int step = 0; step < NSTEPS; step++ )
    {
    	
    	//cout << "timestep in" << endl;
        // 
        //  collect all global data locally 
        //
        MPI_Allgatherv( local, nlocal, PARTICLE, particles, partition_sizes, partition_offsets, PARTICLE, MPI_COMM_WORLD );
        
        //Empty the matrix from old stuff
		for(int i = 0; i < numberOfRows; i++){
			for(int j = 0; j < numberOfRows; j++){
				matrix[i][j].clear();
			}
		}
		
		//Place each particle in its respective spot in the grid
		for(int i = 0; i < n; i++){	
			particle_t temp = particles[i];
			int column = floor(temp.x/size2*numberOfRows);
			int row = floor(temp.y/size2*numberOfRows);
			matrix[row][column].push_back(temp);
		}
        
        
        //
        //  save current step if necessary (slightly different semantics than in other codes)
        //
        if( fsave && (step%SAVEFREQ) == 0 )
            save( fsave, n, particles );
        
        //
        //  compute all forces
        //
        for( int i = 0; i < nlocal; i++ ){
            local[i].ax = local[i].ay = 0;
            
            //Find the current particles neighboring particles which are within range
			vector<particle_t> nearest = findNearest(local[i], numberOfRows, size2, matrix);
			
			for(int j = 0; j < nearest.size(); j++) {
				if(!(nearest[j].x == local[i].x && nearest[j].y == local[i].y)){
					apply_force(local[i], nearest[j]);	//Apply it!			
				}			
			}
        }
        
        //
        //  move particles
        //
        for( int i = 0; i < nlocal; i++ )
            move( local[i] );
            
     	//cout << "timestep out" << endl;
    }
    simulation_time = read_timer( ) - simulation_time;
    
    if( rank == 0 )
        printf( "n = %d, n_procs = %d, simulation time = %g s\n", n, n_proc, simulation_time );
    
    //
    //  release resources
    //
    free( partition_offsets );
    free( partition_sizes );
    free( local );
    free( particles );
    if( fsave )
        fclose( fsave );
    
    MPI_Finalize( );
    
    cout<<getLol()<<endl;

    
    return 0;
}
