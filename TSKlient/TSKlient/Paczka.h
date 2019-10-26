#pragma once
#include <string>
#include <vector>
#include <ctime>   

class Paczka
{
public:

	Paczka();
	Paczka(std::string wejscie);
	Paczka(std::string wejscie, std::string status, int identyfikator, std::string znacznikCzasu);

	void dodajIdentyfikator(int identyfikator);
	void dodajZnacznikCzasu();
	void dodajZnacznikCzasu(std::string ZC);
	void dodajStatus(std::string status);

	void odczytaj(std::string wejscie);
	std::string dajPaczke();
	void parsujPaczke(std::string wejscie);

private:

	std::string operacja;
	std::string status;
	unsigned int identyfikator;
	std::string znacznikCzasu;
	std::vector <unsigned int> argumenty;

	void usunSpcaje(std::string *tekst);

};
