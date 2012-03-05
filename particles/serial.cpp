#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "common.h"
#include <iostream>
#include <set>
#include <vector>

using namespace std;

vector<particle_t> **matrix;


vector<particle_t> **createGrid(int n);

vector<particle_t> findNearest(particle_t temp, int numberOfRows, double size){
	int column = floor(temp.x/size*numberOfRows);
	//cout<<temp.y<<endl;
	int row = floor(temp.y/size*numberOfRows);
	//vector<particle_t> *matrix[numberOfRows][numberOfRows];
	//coutt<<"Grannar för 4 "<<cout<<row<<" "<<column<<" "<<temp.y<<" "<<temp.x<<endl;
	int i = -1,j = -1;
	int iend = 2, jend = 2;
	if(row < 1) i = 0;
	if(row == numberOfRows-1) iend = 1;
	if(column < 1) j = 0;
	if(column == numberOfRows-1) jend = 1;

	int x = j;

	vector<particle_t> val;

	for(i; i < iend; i++){
		for(j = x; j < jend; j++){
			//cout<<row+i<<" "<<column+j<<" "<<row<<" "<<column<<" "<<numberOfRows<<endl;
			vector<particle_t> hej = matrix[row+i][column+j];
			for(int z = 0; z < hej.size(); z++){
				//cout<<row+i<<" "<<column+j<<" "<<temp[z].y<<" "<<temp[z].x<<endl;
				val.push_back(hej[z]);
			}
		}
		
	}

	return val;
}

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

	/////////////////////////////////////////////////
	//particle_t **matrix = createGrid(n);
	//matrix[5][] = particles[5];
	//matrix[0][n] = particles[7];
	int numberOfRows = floor(size/0.01);
	cout<<numberOfRows*numberOfRows<<endl;
	//vector<particle_t> matrix[numberOfRows][numberOfRows];
	matrix = createGrid(n);
	//matrix[6][60] = set<particle_t>();
	for(int i = 0; i < numberOfRows; i++){
		for(int j = 0; j < numberOfRows; j++){
			matrix[i][j] = vector<particle_t>();
		}
	}



	cout<<endl;



	////////////////////////////////////////////////
    
    //
    //  simulate a number of time steps
    //
    double simulation_time = read_timer( );
    for( int step = 0; step < NSTEPS; step++ )
    {
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
			//cout<<row<<" "<<column<<" "<<temp.y<<" "<<temp.x<<endl;
		}
        //
        //  compute forces
        //
        for( int i = 0; i < n; i++ ){
            particles[i].ax = particles[i].ay = 0;
			
			//cout<<"asdasd "<<particles[i].y<<endl;
			vector<particle_t> nearest = findNearest(particles[i], numberOfRows, size);
			for(int j = 0; j < nearest.size(); j++) {
				if(!(nearest[j].x == particles[i].x && nearest[j].y == particles[i].y)){
					apply_force(particles[i], nearest[j]);				
				}			
			}

			
			/*
            for (int j = 0; j < n; j++ ){
                apply_force( particles[i], particles[j] );
				//cout << particles[i].ax << " " << particles[i].ay << endl;
			}*/
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
    
	cout<<"LOL: ";
	getLol();

    return 0;
}



vector<particle_t> **createGrid(int n){
	vector<particle_t> **temp;
	temp = (vector<particle_t>**)malloc(sizeof(vector<particle_t>*)*n);
	for(int i = 0; i < n; i++){
		temp[i] = (vector<particle_t>*)malloc(sizeof(vector<particle_t>)*n);
	}

	return temp;
}



