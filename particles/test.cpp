#include <iostream>


void funct(int **matrix, int a, int b){
	/*int **matris;
	matris = (int**)malloc(sizeof(int*)*a);
	for(int i = 0; i < b; i++){
		matris[i] = (int*)malloc(sizeof(int)*b);
	}*/
	

	for(int i = 0; i < 10; i++){
		for(int j = 0; j < 10; j++)
			std::cout<<matrix[i][j]<<std::endl;
	}
}


int main(){
	int **matris = new int*[10];
	for(int i = 0; i < 10; i++)
		matris[i] = new int[10];
	
	funct(matris, 10, 10);

	return 0;
}
