#include <stdio.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

/*	- Pliki �r�d�owe w C najlepiej kompilowa� poleceniem:
*		gcc -O2 -W -Wall -pedantic -std=c99 -o main.o mainc
*		zapobiega to b��dowi kompilacji "outside C99 mode"
*		i w��cza ostrze�enia widoczne w terminalu.
**/



/*	#define nazwa zamie�_na - spos�b dzia�ania define == zamieni wszystkie fragmentu kodu "nazwa" na "zmie�_na"	
*	W naszym przypadku fragment kodu "MAX" zostanie zast�piony przez 16384
**/

#define MAX 16384 //Rozmiar naszego bufora(bajty)

void showTab(char[], int);
/*
 * +=====================+
 * |#####C###|####C++####|
 * |  _Bool ==   bool    |
 * +---------------------+
 *
 * */
_Bool isChar(char[], int);


/* Do zrozumienia dzia�ania (int argc, char* argv[]) b�dziemy musieli wiedzie� jak dzia�aj� tablice i wska�niki w C. (Bardzo pomocny artyku� na wiki https://goo.gl/6PohAA)
 *
 *
 *		1.) Wyobra�my sobie, �e pami�� kt�r� dysponujemy jest jak jednowymiarowa o�, zbudowana z kom�rek o rozmiarze jednego bajta, te kt�re zapiszemy do pamici oznaczymy jako "O".
 *		O� jest pusta dop�ki nie zapsizaemy w niej �adnych danych a pozycj� dowolnej kom�rki mo�emy okre�li� poprzez podanie odleg�o�ci (ilo�ci kom�rek) od pocz�tku osi.
 *
 *		Pusta o�:
 *
 *		----------------------------------------------------------------------------------------------------------------------->
 *		^												Jednowymiarowa o� reprezentuj�ca pami��
 *		a ===> Adres kom�rki "a" b�dziemy mogli umownie zapisa� jako 0 (zero).	
 *
 *
 *		2.) Tablic� wyobra�my sobie jako u�o�one obok siebie zmienne, "O" b�dzie symbolizowa� 1 bajt w pami�ci / 1 kom�rk�.
 * 
 *		Dla umownej tablicy czterech int�w ===> int t[4]			//int na 64-bitowym komputerze to 4 bajty ===> dlatego jeden int to cztery "O"
 *
 *		t[0] t[1] t[2] t[3]
 *		|OOOO|OOOO|OOOO|OOOO|-------------------------------------------------------------------------------------------------->
 *														Jednowymiarowa o� reprezentuj�ca pami��
 *
 *			a.) Adres dowolnej zmiennej pobieramy "&"
 *			b.) W naszym modelu adres t[0] to 0(zero)
 *			c.) t[0] zajmuje 4 bajty dlatego nast�pna (t[1]) b�dzie opatrzona adresem 4, bo 0 + 4 = 4
 *			d.) W �atwy spos�b mo�emy obliczy� adres dowolnego elementu tablicy, np 3 element tablicy int to (3 * (pami�� zajmowana przez jednego inta)) = 3 * 4 = 12
 *			e.) tab jest r�wnoznaczne z tab[0] - adres na pocz�tek tablicy
 *
 *		3.) Wska�nik to zmienna w kt�rej nie trzymamy tradycyjnej warto�ci ale ADRES, np do innej zmiennej.
 *			a.) "*" (gwiazdka) oznacza "id� pod podany adres" lub wska�nik
 *				a.1) int *wskaznik - to jest wska�nik (zmienna przechowuj�ca adres)
 *				a.2) printf("%d\n", *x); - id� pod adres podany we wska�niku x i pobierz warto��
 *			b.) "&" s�u�y do pobierania adresu, przyk�ad kodu:
 *
 *		1| int a = 37; //Zmienna a pryjmuje warto�� 37
 *		2| int *b = &a; //Do wska�nika b zapisz adres zmiennej a
 *		3| printf("%d\n", *b); //Wy�wietli 37
 *
 *
 *
 *	int main(int argc, char* argv[]){...} - przekazujemy do maina argc i *argv[]
 *
 *	int atgc - iczba argumentow przekazanych do programu, z czego pierwszym argumentem jest nazwa programu, wiec jej wartos�� 
 *																		w sytuacji, gdy nie podajemy �adnego argumentu w terminalu == 1
 *	char* argv[] - tablica wska�nik�w na char
 *		- Czym jest tabica wska�nik�w? 
 *			> Najpo�ciej rzecz ujmuj�c, mo�na to sobie wyobrazi� jako tablic� dwuwymiarow� (argv[][])
 *		- Pierwszym argumentem ZAWSZE jest nazwa programu -> jest przechowywana w char[0] 

 *
 * */
int main(int argc, char *argv[]){
	srand(time(NULL));
	int target, source, lbajt;
	
	/*Tworzymy bufor do kt�rego wczytamy dane z pliku wej�ciowego.
	* Rozmiar zosta� tak dobrany aby w tym konkretnym programie wczyta� ca�y plik do bufora.
	* Poniewa� plik jest stosunkowo ma�y (<1kB) tworzymy bufor na 16kB (16684B / 1024 = 16kB), kt�ry jest w stanie pomie�ci� ca�y plik.
	**/
	char buf[MAX];
		
	/*	Program do poprawnego dzia�ania wymaga dw�ch argument�w: �cie�ki do pliku wej�ciowego lub nazwy pliku, je�li plik znajduje si� w tym
	*	samym folderze, co nasz program i nazw� pliku do kt�rego zostan� zapisane ewentualne dane.
	*	Pierwszy if sprawdza, czy brakuje dw�ch argument�w, drugi if sprawdza, czy brakuje tylko jednego.
	*	W tym przypadku pierwszy if jest opcjonalny.
	**/
	if (argc < 2) {
		printf("\nBrak parametrow? Co ja mam z tym niby zrobic?\n\n");
		return 1;
	}
	if (argc < 3) {
		printf("\nJuż? To Wszystko? O.o Postaraj sie bardzij i podaj mi drugi parametr!\n\n");
		return 1;
	}
	
	/*	Open zwraca deskryptor (int), zapisujemy go do zmiennej "source".
	*	W sytuacji, gdy otwieranie pliku zako�czy�o si� powodzeniem, warto�� zwr�conego
	*	dekryptora b�dzie nieujemna, warto�� (-1) oznacza b��d otwarcia pliku.
	*	
	*	Open przyjmuje dwa parametry: �cie�k� do pliku (char* path) i odpowiedni� flag� - okre�la ona w jakim 
	*	trybie otworzymy plik, flaga O_RDONLY oznacza trym tylko do odczytu (Read Only).	
	**/
	source = open(argv[1], O_RDONLY);

	if (source == -1) { //W sytuacji b��du zwracamy komunikat i wychodzimy z programu (return 1 ===> warto�� inna ni� zero umownie mo�e oznacza� b��d)
		printf("Blad 1\n");
		return 1;
	}

	/*	Read w przypadku powodzenia zwraca liczb� zczytanych bajt�w, 0 oznacza koniec pliku a warto�� ujemna oznacza b��d.
	*	
	*	Read jako pierwszy parametr przyjmuje deskryptor otwierania pliku (zapisany w zmiennej source), drugim jest 
	*	bufor do kt�rego b�dziemy wczytywa� dane a trzecim parametrem jest liczba bajt�w, kt�re chcemy wczyta�.
	**/
	if((lbajt = read(source, buf, MAX)) > 0) {
		
		/*	Poniewa� w tym konkretnym zadaniu interesuj� nas 4 bajty (15-18) stworzymy sobie pomocnicz� zmienn� na nie.
		**/
		char temp[4];
		
		/*	Poniewa� interesuj� nas bajty od 15 do 18, tworzymy p�tl� for i wczytujemy po kolei z buforu buf do tablicy temp;
		**/
		for (int i = 0; i < 4; i++) temp[i] = buf[i+15];

		/*	Obliczanie liczby element�w nie jest wymagane w tym zadaniu, poniewa� znamy j� z g�ry (4), natomiast pokazuje uniwersalny
		*	algorytm do oblizcania liczby element�w w danej tablicy.
		*	
		*	sizeof(x) ===> zwr�ci liczb� bajt�w, kt�r� x zajmuje w pami�ci.
		*	
		*	Algorytm obliczania liczby element�w dzieli ilo�� pami�ci zajmowanej przez tablic� przez pami�� zajmowan� przez jeden elemen.
		*	!!! Nie polecam tworzenia funkcji do zliczania element�w w tablicy, poniewa� warto�� zajmowanej pami�ci (sizeof()) tablicy 
		*	przekazanej do funkcji zwraca nieprawid�owe warto�ci (najcz�ciej 2 razy za du�e) dok�ady opis zjawiska do znalezienia na stackoverflow
		**/
		int liczbaElementow = sizeof(temp) / sizeof(temp[0]);
		
		if(isChar(temp, liczbaElementow)) { //Sprawdzamy, czy cztery bajty (15-18) s� liczb� czy znakami.
			printf("Jest Charem\n");
			showTab(temp, liczbaElementow);
		}
		else{
			printf("Podana wartosc jest liczba!\n\n\n");

			for(int i = 0; i < liczbaElementow; i++) {
				
				/*	Zmienna char do przechowywania znak�w ASCII wykorzystuje typ int (w�a�ciwie char jest intem), dlatego znaj�c oznaczenia psozczeg�lnych
				*	symboli w kodzie ASCII (wpisa� w google: ASCII TABLE), mo�emy losowa� inty w takim przedziale (48-57), ktory wskazuje tylko na liczby, dla
				*	przyk�adu liczba 2 w kodzie ASCII to 50
				**/
				buf[15+i] = (rand()%10) + 48;
			}

			/* Otwieramy plik do kt�rego odb�dzie si� zapis, O_RDWR oznacza tryb do odczytu i zapisu (Read Write) a oddzielone "|" O_CREAT oznacza, �e w przypadku
			* gdyby nasz plik do kt�rego chcemy zapisa� dane nie istnia�, zostanie automatycznie utworzony
			**/
			target = open(argv[2], O_RDWR | O_CREAT);
			
			/* Deskryptor write zostanie zapisany do zmiennej (int) ss, je�li jego warto�� == -1 oznacza to b��d zapisu.
			*
			*	Write jako parametry przyjmuje: �cie�k� do pliku docelowego, bufor z danymi do zapisu, ilo�� bajt�w (w zmiennej typu int), kt�re maj� zosta� zapisane.
			**/
			int ss;
			if (ss = (write(target, buf, lbajt)) == -1){ //Je�li podczas zapisu powsta� b��d, wy�wietli komunikat i zako�czy prac� programu.
				printf("Error przy zapisywaniu [%i]\n", ss);
				return 1;
			}

			/* Otwarte pliki musimy zamkn�c. If sprwdza, czy oba zamkni�cia si� powiod�y. Dla prawid�owego zamkni�cia deskryptor wynosi zero.
			*	Je�li chocia� jeden "close" zwr�ci inn� warto�� ni� zero zostanie wy�wietlony komunikat o b��dzie.
			**/
			if(close(source) || close(target)) printf("Blad zamkniecia pliku");
		}
	}
	return 0;
}

void showTab(char inputTab[], int n){ //P�tla wy�wietlaj�ca n znak�w z tablicy. Dla typu Char w printf u�ywamy %c
	for (int i = 0; i < n; i++) printf("%c", inputTab[i]);

	printf("\n");
}

/*	Funkcja sprawdzaj�ca, czy w tablicy znajduje si� chocia� jedna warto�� nie b�d�ca liczb� (zak�adamy, �e tablica nie reprezentuje liczb, je�li chocia� jeden element ni� nie jest).
*	
*	Algorytm sprawdzaj�cy polega na przeszukaniu n element�w w tablicy, je�li natrafimy na element spoza zakresu cyfr (48-57 w ASCII) od razu zwracamy true i wychodzimy z p�tli, je�li wszystkie elementy
*	s� cyframi, mo�emy zwr�ci� false
**/
_Bool isChar(char in[], int n){
	for(int i = 0; i < n; i++){
		if(in[i] > 57 || in[i] < 48) return true;
	}

	return false;
}