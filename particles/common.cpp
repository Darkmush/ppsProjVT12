#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <float.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include "common.h"
#include <iostream>
#include <vector>

using namespace std;

double size;

//
//  tuned constants
//
#define density 0.0005
#define mass    0.01
#define cutoff  0.01
#define min_r   (cutoff/100)
#define dt      0.0005

//
//  timer
//
double read_timer( )
{
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized )
    {
        gettimeofday( &start, NULL );
        initialized = true;
    }
    gettimeofday( &end, NULL );
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

//
//  keep density constant
//
double set_size( int n )
{
    size = sqrt( density * n );
 	//cout << size << endl;
    return size;
}

//
//  Initialize the particle positions and velocities
//
void init_particles( int n, particle_t *p )
{
    srand48( time( NULL ) );
        
    int sx = (int)ceil(sqrt((double)n));
    int sy = (n+sx-1)/sx;
    
    int *shuffle = (int*)malloc( n * sizeof(int) );
    for( int i = 0; i < n; i++ )
        shuffle[i] = i;
    
    for( int i = 0; i < n; i++ ) 
    {
        //
        //  make sure particles are not spatially sorted
        //
        int j = lrand48()%(n-i);
        int k = shuffle[j];
        shuffle[j] = shuffle[n-i-1];
        
        //
        //  distribute particles evenly to ensure proper spacing
        //
        p[i].x = size*(1.+(k%sx))/(1+sx);
        p[i].y = size*(1.+(k/sx))/(1+sy);

        //
        //  assign random velocities within a bound
        //
        p[i].vx = drand48()*2-1;
        p[i].vy = drand48()*2-1;
    }
    free( shuffle );
}

//
//  interact two particles
//
int lol = 0;
void apply_force( particle_t &particle, particle_t &neighbor )
{

    double dx = neighbor.x - particle.x;
    double dy = neighbor.y - particle.y;
    double r2 = dx * dx + dy * dy;
    if( r2 > cutoff*cutoff ){
        return;
	}
    r2 = fmax( r2, min_r*min_r );
    double r = sqrt( r2 );

    //
    //  very simple short-range repulsive force
    //
    double coef = ( 1 - cutoff / r ) / r2 / mass;
    particle.ax += coef * dx;
    particle.ay += coef * dy;
    
    lol++;
}

int getLol(){
	return lol;
}

//
//  integrate the ODE
//
void move( particle_t &p )
{
    //
    //  slightly simplified Velocity Verlet integration
    //  conserves energy better than explicit Euler method
    //
    p.vx += p.ax * dt;
    p.vy += p.ay * dt;
    p.x  += p.vx * dt;
    p.y  += p.vy * dt;

    //
    //  bounce from walls
    //
    while( p.x < 0 || p.x > size )
    {
        p.x  = p.x < 0 ? -p.x : 2*size-p.x;
        p.vx = -p.vx;
    }
    while( p.y < 0 || p.y > size )
    {
        p.y  = p.y < 0 ? -p.y : 2*size-p.y;
        p.vy = -p.vy;
    }
	//cout << p.x << " " << p.y << endl;
}

//
//  I/O routines
//
void save( FILE *f, int n, particle_t *p )
{
    static bool first = true;
    if( first )
    {
        fprintf( f, "%d %g\n", n, size );
        first = false;
    }
    for( int i = 0; i < n; i++ )
        fprintf( f, "%g %g\n", p[i].x, p[i].y );
}

//
//  command line option processing
//
int find_option( int argc, char **argv, const char *option )
{
    for( int i = 1; i < argc; i++ )
        if( strcmp( argv[i], option ) == 0 )
            return i;
    return -1;
}

int read_int( int argc, char **argv, const char *option, int default_value )
{
    int iplace = find_option( argc, argv, option );
    if( iplace >= 0 && iplace < argc-1 )
        return atoi( argv[iplace+1] );
    return default_value;
}

char *read_string( int argc, char **argv, const char *option, char *default_value )
{
    int iplace = find_option( argc, argv, option );
    if( iplace >= 0 && iplace < argc-1 )
        return argv[iplace+1];
    return default_value;
}

//
//Our functions
//


//
// This function returns a pointer to a numberOfRows * numberOfRows array-matrix
// that contains empty vectors.
//
vector<particle_t> **createGrid(int n, int numberOfRows){
	//Create the matrix itself
	vector<particle_t> **temp;
	temp = (vector<particle_t>**)malloc(sizeof(vector<particle_t>*)*n);
	for(int i = 0; i < n; i++){
		temp[i] = (vector<particle_t>*)malloc(sizeof(vector<particle_t>)*n);
	}
	
	//And fill it with empty vectors
	for(int i = 0; i < numberOfRows; i++){
		for(int j = 0; j < numberOfRows; j++){
			temp[i][j] = std::vector<particle_t>();
		}
	}

	return temp;
}

//
// This function finds all particles within <cutoff> range of temp in the vector-matrix matrix.
// It returns a vector containing all particles within range.
//
vector<particle_t> findNearest(particle_t temp, int numberOfRows, double size, vector<particle_t> **matrix){
	//Figre out in which column and row we shall search
	int column = floor(temp.x/size*numberOfRows);
	int row = floor(temp.y/size*numberOfRows);
	int i = -1,j = -1;
	int iend = 2, jend = 2;
	
	//This is special case stuff, if the particle is in the corner of the grid for instance...
	if(row < 1) i = 0;
	if(row == numberOfRows-1) iend = 1;
	if(column < 1) j = 0;
	if(column == numberOfRows-1) jend = 1;

	int x = j;

	//A vector to hold all the particles we find
	vector<particle_t> val;
	
	//And search thorugh the part of the amtric where we may find particles in range
	for(i; i < iend; i++){
		for(j = x; j < jend; j++){
			vector<particle_t> hej = matrix[row+i][column+j];
			for(int z = 0; z < hej.size(); z++){
				val.push_back(hej[z]);
			}
		}
		
	}
	return val; //Return the vector, sure one should perhaps return a pointer but since the vector will only contain one or two elements most of the time this is still okay. 
}

