#pragma once
#include <string>
#include <vector>
#include <ctime>   
#include <iostream>
#include <map>
#include <regex>


class Paczka
{
public:

	Paczka();
	Paczka(std::string wejscie);
	Paczka(std::string wejscie, std::string status, int identyfikator, std::string znacznikCzasu);

	void dodajIdentyfikator(int identyfikator); //metoda dodaj�ca identyfikator
	void dodajZnacznikCzasu(); //metoda dodaj�ca znacznik czasowy zgodny z aktualnym czasem
	void dodajArgument(unsigned int argument); //metoda dodaj�ca kolejny argument do wektora argument�w 
	void dodajZnacznikCzasu(std::string ZC); //metoda dodaj�ca znacznik czasu podawany przez argument
	void dodajStatus(std::string status); //metoda dodaj�ca status 
	void nadpiszArgumenty(unsigned int argument); //metoda nadpisuj�ca wszystkie argumenty jednym

	unsigned int dajIdentyfikator();			   //
	std::string dajZnacznikCzasu();				   //
	std::string dajStatus();					   //
	std::string dajPaczke();					   //
	std::string dajOperacje();					   //
	std::vector <unsigned int> dajArgumenty();	   // gettery

	

	int odczytaj(std::string wejscie); //nadpisuje warto�ci argument�w i operacji na podstwie podanego dzia�ania w argumencie

	//! Paczka do wyswietlenia
	/*! Zwraca paczke jako string w przejrzystej do czytania formie  */
	std::string dajDoWyswietlenia();

	//! Parsowanie string na paczke 
	/*! Pobiera dane z paczki w formie string i uzupelnia pola w klasie */
	std::string parsujPaczke(std::string wejscie); 

	void zerujPaczke();

private:

	std::string operacja; // operacja matematyczna
	std::string status;
	unsigned int identyfikator;
	std::string znacznikCzasu;
	std::vector <unsigned int> argumenty;

	void usunSpacaje(std::string *tekst);

};
