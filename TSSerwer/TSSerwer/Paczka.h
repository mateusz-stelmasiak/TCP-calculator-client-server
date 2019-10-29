#pragma once
#include <string>
#include <vector>
#include <ctime>   
#include <iostream>

#include <map>
#include <regex>



/*
BUDOWA PAKIETU:
----------NAG£ÓWEK---------------
Operacja: ...; (
Status: ...;
Identyfikator: ...;
Czas: ...;
---------DANE-------
Liczba1: ..;
Liczba2: ..;
.
.
LiczbaN: ..;
Koniec: TAK;

*/

class Paczka
{
public:

	Paczka();
	Paczka(std::string wejscie);
	Paczka(std::string wejscie, std::string status, int identyfikator, std::string znacznikCzasu);

	void dodajIdentyfikator(int identyfikator);
	void dodajZnacznikCzasu();
	void dodajArgument(unsigned int argument);
	void dodajZnacznikCzasu(std::string ZC);
	void dodajStatus(std::string status);
	void nadpiszArgumenty(unsigned int argument);
	void zerujArgumenty();

	unsigned int dajIdentyfikator();
	std::string dajZnacznikCzasu();
	std::string dajStatus();
	std::string dajPaczke();
	std::string dajOperacje();
	std::vector <unsigned int> dajArgumenty();

	//! Paczka do wyswietlenia
	/*! Zwraca paczke jako string w przejrzystej do czytania formie  */
	std::string dajDoWyswietlenia();

	void odczytaj(std::string wejscie);


	//! Parsowanie string na paczke 
	/*! Pobiera dane z paczki w formie string i uzupelnia pola w klasie
		Zwraca OK dla poprawnych paczek, i opis bledu dla niepoprawnych
	*/
	std::string parsujPaczke(std::string wejscie);

	void zerujPaczke();

	

private:

	std::string operacja;
	std::string status;
	unsigned int identyfikator;
	std::string znacznikCzasu;
	std::vector <unsigned int> argumenty;

	void usunSpacaje(std::string *tekst);

};
