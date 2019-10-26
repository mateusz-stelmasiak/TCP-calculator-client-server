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
	unsigned int bliczba = 0;

	while (wejscie.size() > 0)
	{
		std::cout << wejscie << std::endl;

		this->usunSpcaje(&wejscie);
		while (1)
		{
			this->usunSpcaje(&wejscie);
			bufor = wejscie[0];
			wejscie.erase(0, 1);

			if (!(bufor >= 48 && bufor <= 57))
			{
				this->argumenty.push_back(bliczba);
				bliczba = 0;
				break;
			}
			
			bliczba = bliczba*10 + std::stoi(&bufor, nullptr, 10);
		}
		

		if (wejscie.size() == 0)
		{
			break;
		}

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
				std::cout << "Bledzne dzialanie! " << std::endl;
				//b��d!
			}
		}
	}




}

std::string Paczka::dajPaczke()
{
	std::string paczka = "";
	paczka = "Operacja: " + this->operacja + ";Status: " + this->status + ";Identyfikator: " + std::to_string(this->identyfikator) + ";ZnacznikCzasu: "
		+ this->znacznikCzasu + ";";

	for (int i = 0; i < this->argumenty.size(); i++)
	{
		paczka = paczka + "Liczba" + std::to_string(i+1) + ": "  + std::to_string(argumenty[i]) + ";";
	}

	paczka = paczka + "Koniec;";

	return paczka;
}

void Paczka::usunSpcaje(std::string *tekst)
{
	while ((*tekst)[0] == ' ')
	{
		tekst->erase(0, 1);
	}
}