#include "pch.h"
#include "Paczka.h"

Paczka::Paczka()
{
	Paczka::operacja = "NULL";
	Paczka::status = "NULL";
	Paczka::identyfikator = 0;
	Paczka::znacznikCzasu = "NULL";
	argumenty.clear();

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
	this->argumenty.clear();
	this->operacja = "NULL";

	char op;
	char bufor;
	unsigned int liczba = 0;
	unsigned int cyfra;

	while (wejscie.size() > 0)
	{
		this->usunSpcaje(&wejscie);
		while (1)
		{
			bufor = wejscie[0];
			wejscie.erase(0, 1);

			if (!(bufor >= 48 && bufor <= 57))
			{
				this->argumenty.push_back(liczba);
				break;
			}
			
			liczba = liczba*10 + std::stoi(&bufor, nullptr, 10);
		}

		this->usunSpcaje(&wejscie);
		if (wejscie.size() == 0);
		{
			break;
		}
		bufor = wejscie[0];
		wejscie.erase(0, 1);
		if (this->operacja == "NULL")
		{
			op = bufor;
			if (bufor == '+')
			{
				this->operacja = "DODAWANIE";
			}

			if (bufor == '-')
			{
				this->operacja = "ODEJMOWANIE";
			}

			if (bufor == '*')
			{
				this->operacja = "MNOZENIE";
			}

			if (bufor == ':')
			{
				this->operacja = "DZIELENIE";
			}
		}
		else
		{
			if (op != bufor)
			{
				//b³¹d!
			}
		}
	}




}

std::string Paczka::dajPaczke()
{
	std::string paczka = "";

	return paczka;
}

void Paczka::usunSpcaje(std::string *tekst)
{
	while ((*tekst)[0] == ' ')
	{
		tekst->erase(0, 1);
	}
}

void Paczka::parsujPaczke(std::string wejscie) 
{
	std::map<std::string, std::string> zParsowanyPakiet; //will hold the packet information extracted from the string

   //using regex to split the data into kgroups of- (key)(: )(data)(;) and put the key-data pair into the map
	std::smatch pary;
	std::regex wzorzecKluczWartosc("(\\w+)(:\\s{1})(\\S*)(;)");


	bool correctPacketFlag = 1;
	while (wejscie.length() != 0 && correctPacketFlag) { //iterates over the packet string and finds data matching the (Key: data;) pattern
		if (std::regex_search(wejscie, pary, wzorzecKluczWartosc))
		{
			zParsowanyPakiet.insert_or_assign(pary[1], pary[3]);
			wejscie.erase(0, pary[0].length());
		}
		else { correctPacketFlag = 0; } // THROW EXCEPTION! if the regex doesn't match on any itteration, the packet is semantically incorect

		this->identyfikator = (unsigned)std::stoi(zParsowanyPakiet.find("Identyfikator")->second);
		this->operacja = zParsowanyPakiet.find("Operacja")->second;

		//this->argumenty
	}
}
