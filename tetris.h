#ifndef TETRIS_H
#define TETRIS_H

struct game;

/*
 Funkcja wyczyszcza strumień wejścia
*/
void areaCleanupIo();

/*
 Funkcja aktualizuje wyświetlaną informację
*/
void areaSetIoconfig();

/*
 Inicjalizacja boiska do gry
 @param t struktura zawierająca parametry dla gry oraz boiska
 @param width szerokość boiska
 @param height wysokość boiska
*/
void gameInit(struct game *t, int width, int height);

/*
 Funkcja wyczyszcza tablice przedstawiającą boisko
 @param t struktura zawierająca boisko
*/
void gameClean(struct game *t);

/*
 Funkcja wyświetla w konsoli bisko w aktualnym stanie
 @param t struktura zawierająca parametry dla gry oraz boiska
*/
void areaPrint(struct game *t);

/*
 Funkcja sprawdza czy blok dotyka scianek boiska lub wysokość jest przekroczona
 @param t struktura zawierająca parametry dla gry oraz boiska
 @return zwraca jedynkę jeżeli występuje dotknięcie
*/
int hitCheck(struct game *t);

/*
 Funkcja losowo generuje nowy blok oraz sprwdza czy gra nie skończe się po tym
 @param t struktura zawierająca parametry dla gry oraz boiska
*/
void newBlock(struct game *t);

/*
 Funkcja drukuje wygenerowany wcześniej blok
 @param t struktura zawierająca parametry dla gry oraz boiska
*/
void printBlock(struct game *t);

/*
 Funkcja obraca aktualny blok
 @param t struktura zawierająca parametry dla gry oraz boiska
*/
void rotateBlock(struct game *t);

/*
 Funkcja przespiesza spadanie bloka
 @param t struktura zawierająca parametry dla gry oraz boiska
*/
void gravityBlock(struct game *t);

/*
 Funkcja przesuwa w dól wszustkie leżące bloki kiedy jeden wiersz znika
 @param t struktura zawierająca parametry dla gry oraz boiska
 @param line numer znikającego wiersza
*/
void allBlocksFall(struct game *t, int line);

/*
 Funkcja sprawdza, czy trzeba usunąć już wypełniony wirszy
 @param t struktura zawierająca parametry dla gry oraz boiska
*/
void checkLines(struct game *t);

/*
 Funkcja zwiększa prędkość spadania bloków odpowiednio ilości punktów
 @param t struktura zawierająca parametry dla gry oraz boiska
 @return czas spadania bloka w nsek
*/
int gameLevel(struct game *t);

/*
 Funkcja uruchamia grę
 @param width szerokość boiska
 @param height wysokość boiska
*/
void palyGame(int width, int height);

#endif
