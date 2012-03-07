#!/bin/bash
args=("$@")

g++ -O3 serialStandard.cpp -o serialStandard -lm common.o
g++ -O3 pthreadsStandard.cpp -o pthreadsStandard -lm common.o -lpthread
g++ -O3 openmpStandard.cpp -o openmpStandard -lm common.o
mpicc -cc=g++44 -c -O3 mpiStandard.cpp
mpicc -cc=g++44 -o mpiStandard mpi.o common.o


for k in 0 1 2 3
do
	echo "********* ${args[k]} *********" 
	echo "O(n^2/p)"
		for i in 2000 1000
		do
			for j in 1 2 4 8
			do
				if [ $k = 3 ]; then
					mpirun -np $j ./${args[k]}Standard - n $i | grep "n = "
				else
					./${args[k]}Standard -n $i -p $j | grep "n = "
				fi
				
				if [$k = 0 ]; then
					break
				fi
			done
		echo
		done
	echo "O(n/p)"
		for i in 2000 1000
		do
			for j in 1 2 4 8
			do
				if [ $k = 3 ]; then
					mpirun -np $j ./${args[k]} - n $i | grep "n = "
				else
					./${args[k]} -n $i -p $j | grep "n = "
				fi
				
				if [$k = 0 ]; then
					break
				fi
			done
		echo
	done
done
