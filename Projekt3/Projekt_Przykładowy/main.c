#include <unistd.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdbool.h>

#define BUFF_SIZE 16384 // 16kB -> 1/4 bufora pipe

//	Do zrobienia:
//	char* quickSort(char*);
//
//-----------------------------------
//
//  Przykład włączenia:
// ./main.o <plik wejściowy>

const unsigned long long knownPrimes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563, 569, 571, 577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, 701, 709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 947, 953, 967, 971, 977, 983, 991, 997, 18446744073709551533, 18446744073709551557};
const unsigned short int amountOfKnownPrimes = sizeof(knownPrimes) / sizeof(knownPrimes[0]);

bool isDigit(char);
bool isPrime(unsigned long long);

int main(int argc, char** argv){
	
	if(argc < 2) { // Sprawdza czy podano argument
		puts("Nie podano pliku wejściowego. \n");
		return -2;
	}
	
	int pipeDescriptor[2];
	pipe(pipeDescriptor); //Pipuje

	if(fork()) 			//Rodzic
		if(fork()){}    //Rodzic
		else{			//Potomek 1
			FILE *file = fopen(argv[1], "r"); //Tylko do odczytu
			char* readBuffer = malloc(BUFF_SIZE);
			int loadedBytes = 0;
		
			close(pipeDescriptor[0]); //Zamyka końcówkę
			nice(2); //Zmniejsza priorytet procesu o 2
			printf("Proces 1. Moja wartość nice: %d\n", nice(0));

			if(file == NULL) { //Sprawdza czy plik otworzono poprawnie
				puts("Błąd otwarcia pliku");
				exit(3);
			}

			while (loadedBytes = fread(readBuffer, sizeof(char), BUFF_SIZE, file)){ //Czyta w pętli po 'BUFF_SIZE' bajtów do bufora a następnie wysyła je potokiem do drugiego procesu
				//printf("Loaded: %d\n", loadedBytes);
				write(pipeDescriptor[1], readBuffer, loadedBytes);
			}
			
			fclose(file);

			free(readBuffer);
			exit(0);
		}
	else {              //Potomek 2
		char *bufferForRecivedData = malloc(BUFF_SIZE);
		int recivedBytes = 0;
		
		printf("Proces 2. Moja wartość nice: %d\n", nice(0));
		close(pipeDescriptor[1]);
	
		while(recivedBytes = read(pipeDescriptor[0], bufferForRecivedData, BUFF_SIZE)){
		//	printf("Second Proccess recived: %d\n", recivedBytes);
		}

		free(bufferForRecivedData);
		exit(0);
	}

	for (int i = 0; i < 1; i++) wait(NULL); //Dlaczego nie i < 2 ?

	return 0;
}

bool isDigit(char c){
	if((int) c < 58 && (int) c > 47) return true;
	else return false;
}

bool isPrime(unsigned long long suspected){
	if(suspected < 2) return false;

	for(int i = 0; i < amountOfKnownPrimes; i++){
		if(suspected == knownPrimes[i]) return true;
		if((suspected % knownPrimes[i]) == 0) return false;
	}

	puts("Wyjście poza znane liczby pierwsze");

	unsigned long long divisor = knownPrimes[amountOfKnownPrimes - 1];
	unsigned long long halfSuspected = (suspected + 1) / 2;

	while((suspected % divisor) != 0){
		if(divisor > halfSuspected) return true;
		divisor += 2;
	}
	
	return false;
}
