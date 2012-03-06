#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <iostream>
#include "common.h"

#define cutoff 0.01

using namespace std;

//
//  global variables
//
int n, n_threads;
particle_t *particles;
FILE *fsave;
pthread_barrier_t barrier;

//
//  check that pthreads routine call was successful
//
#define P( condition ) {if( (condition) != 0 ) { printf( "\n FAILURE in %s, line %d\n", __FILE__, __LINE__ );exit( 1 );}}

//
//  This is where the action happens
//
void *thread_routine( void *pthread_id )
{
    int thread_id = *(int*)pthread_id;

    int particles_per_thread = (n + n_threads - 1) / n_threads;
    int first = min(  thread_id    * particles_per_thread, n );
    int last  = min( (thread_id+1) * particles_per_thread, n );
    
    double size = set_size(n);
	int numberOfRows = floor(size/cutoff);
	
	vector<particle_t> **matrix = createGrid(n, numberOfRows);
    
    //
    //  simulate a number of time steps
    //
    for( int step = 0; step < NSTEPS; step++ ){
    //	cout << "timestep in" << endl;
    	
    //	cout << "Thread id: " <<thread_id<<" has first/last: "<< firstRow<<" "<<lastRow<<endl;
    //	cout << "for clear in" << endl;
		for(int i = 0; i < numberOfRows; i++){
			for(int j = 0; j < numberOfRows; j++){
				matrix[i][j].clear();
			}
		}
		//cout << "for clear ut" << endl;


		for(int i = 0; i < n; i++){
			particle_t temp = particles[i];
			int column = floor(temp.x/size*numberOfRows);
			int row = floor(temp.y/size*numberOfRows);
		//	cout<<"row: "<<row<<" column: "<<column<<" i: " << i<<endl;
			matrix[row][column].push_back(temp);
		}
        //
        //  compute forces
        //
        //cout << "for in" << endl;
        for( int i = first; i < last; i++ ){
        	particles[i].ax = particles[i].ay = 0;
        	
			vector<particle_t> nearest = findNearest(particles[i], numberOfRows, size, matrix);
			for(int j = 0; j < nearest.size(); j++) {
				if(!(nearest[j].x == particles[i].x && nearest[j].y == particles[i].y)){
					apply_force(particles[i], nearest[j]);				
				}			
			}
        }
       // cout << "for ut" << endl;
        
        pthread_barrier_wait( &barrier );
        
        //
        //  move particles
        //
        for( int i = first; i < last; i++ ) 
            move( particles[i] );
        
        pthread_barrier_wait( &barrier );
        
        //
        //  save if necessary
        //
        if( thread_id == 0 && fsave && (step%SAVEFREQ) == 0 )
            save( fsave, n, particles );
            
       // cout << "timestep ut" << endl;
            
    }
    
    return NULL;
}

//
//  benchmarking program
//
int main( int argc, char **argv )
{    
    //
    //  process command line
    //
    if( find_option( argc, argv, "-h" ) >= 0 )
    {
        printf( "Options:\n" );
        printf( "-h to see this help\n" );
        printf( "-n <int> to set the number of particles\n" );
        printf( "-p <int> to set the number of threads\n" );
        printf( "-o <filename> to specify the output file name\n" );
        return 0;
    }
    
    n = read_int( argc, argv, "-n", 1000 );
    n_threads = read_int( argc, argv, "-p", 2 );
    char *savename = read_string( argc, argv, "-o", NULL );
    
    //
    //  allocate resources
    //
    fsave = savename ? fopen( savename, "w" ) : NULL;

    particles = (particle_t*) malloc( n * sizeof(particle_t) );
    set_size( n );
    init_particles( n, particles );

    pthread_attr_t attr;
    P( pthread_attr_init( &attr ) );
    P( pthread_barrier_init( &barrier, NULL, n_threads ) );
    
    /////////////////////////////////////////////////////////
    

    
    /////////////////////////////////////////////////////////

    int *thread_ids = (int *) malloc( n_threads * sizeof( int ) );
    for( int i = 0; i < n_threads; i++ ) 
        thread_ids[i] = i;

    pthread_t *threads = (pthread_t *) malloc( n_threads * sizeof( pthread_t ) );
    
    //
    //  do the parallel work
    //
    double simulation_time = read_timer( );
    for( int i = 1; i < n_threads; i++ ) 
        P( pthread_create( &threads[i], &attr, thread_routine, &thread_ids[i] ) );
    
    thread_routine( &thread_ids[0] );
    
    for( int i = 1; i < n_threads; i++ ) 
        P( pthread_join( threads[i], NULL ) );
    simulation_time = read_timer( ) - simulation_time;
    
    printf( "n = %d, n_threads = %d, simulation time = %g seconds\n", n, n_threads, simulation_time );
    
    //
    //  release resources
    //
    P( pthread_barrier_destroy( &barrier ) );
    P( pthread_attr_destroy( &attr ) );
    free( thread_ids );
    free( threads );
    free( particles );
    if( fsave )
        fclose( fsave );
    
    cout<<getLol()<<endl;
    return 0;
}
