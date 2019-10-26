#include "pch.h"
#include "Paczka.h"

Paczka::Paczka()
{
	Paczka::operacja = "NULL";
	Paczka::status = "NULL";
	Paczka::identyfikator = 0;
	Paczka::znacznikCzasu = "NULL";

}

Paczka::Paczka(std::string wejscie)
{
	Paczka::odczytaj(wejscie);
	Paczka::status = "NULL";
	Paczka::identyfikator = 0;
	Paczka::znacznikCzasu = "NULL";
}

Paczka::Paczka(std::string wejscie, std::string status, int identyfikator, std::string znacznikCzasu) : status(status), identyfikator(identyfikator), znacznikCzasu(znacznikCzasu)
{
	Paczka::odczytaj(wejscie);
}

void Paczka::dodajZnacznikCzasu()
{
	std::time_t t = std::time(0);
	std::tm data;
	localtime_s(&data, &t);
	znacznikCzasu = std::to_string(data.tm_mday) + '.' + std::to_string(data.tm_mon + 1) + '.' + std::to_string(data.tm_year + 1900);
}

void Paczka::dodajZnacznikCzasu(std::string ZC)
{
	znacznikCzasu = ZC;
}

void Paczka::dodajStatus(std::string status)
{
	this->status = status;
}

void Paczka::dodajIdentyfikator(int identyfikator)
{
	this->identyfikator = identyfikator;
}

void Paczka::odczytaj(std::string wejscie)
{


}

std::string Paczka::dajPaczke()
{
	std::string paczka = "";

	return paczka;
}