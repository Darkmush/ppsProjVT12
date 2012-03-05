#include <iostream>

int main(){
	int matris[10][10];
	
	for(int i = 0; i < 10; i++){
		for(int j = 0; j < 10; j++)
			std::cout<<matris[i][j];
	}

	return 0;
}
