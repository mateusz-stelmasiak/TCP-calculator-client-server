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

	void dodajIdentyfikator(int identyfikator);
	void dodajZnacznikCzasu();
	void dodajArgument(unsigned int argument);
	void dodajZnacznikCzasu(std::string ZC);
	void dodajStatus(std::string status);
	void nadpiszArgumenty(unsigned int argument);

	unsigned int dajIdentyfikator();
	std::string dajZnacznikCzasu();
	std::string dajStatus();
	std::string dajPaczke();
	std::string dajOperacje();
	std::vector <unsigned int> dajArgumenty();

	void odczytaj(std::string wejscie);
	std::string dajPaczke();


	//! Parsowanie string na paczke 
	/*! Pobiera dane z paczki w formie string i uzupelnia pola w klasie */
	unsigned int parsujPaczke(std::string wejscie); 

	void zerujPaczke();

private:

	std::string operacja;
	std::string status;
	unsigned int identyfikator;
	std::string znacznikCzasu;
	std::vector <unsigned int> argumenty;

	void usunSpacaje(std::string *tekst);

};
