Course: ID1217 Concurrent Programming (Spring term 2012, period 3)
Project: Parallelize Particle Simulation
Authors: Dara Reyahi & Carl Regårdh

Structure:
- benchmarks/
	contains benchmark data
- particles/
	contains code, makefile and script for benchmarking
- report/
	our project report

Run:
To compile our implementations, simply execute the makefile in particles/

Automatically compile our code, the given code and benchmark them by running the script in particles/ with arguments:
"./script.sh serial pthreads openmp mpi"
