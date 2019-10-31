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
	std::vector<int> godzinaMinutySekundyDzienMiesiacRokInt{ data.tm_hour ,data.tm_min,data.tm_sec,data.tm_mday,data.tm_mon + 1, data.tm_year + 1900 };
	std::vector<std::string> wynikString;
	for (int i = 0; i < 6; i++)
	{
		int temp = godzinaMinutySekundyDzienMiesiacRokInt[i];
		if (temp < 10) { wynikString.push_back("0" + std::to_string(temp)); }
		else { wynikString.push_back(std::to_string(temp)); }
	}

	znacznikCzasu = wynikString[0] + ":" + wynikString[1] + ":" + wynikString[2]+ "/" + wynikString[3] + '.' + wynikString[4] + '.' + wynikString[5];
}

void Paczka::dodajArgument(unsigned int argument) { this->argumenty.push_back(argument); }
void Paczka::dodajZnacznikCzasu(std::string ZC) { this->znacznikCzasu = ZC; }
void Paczka::dodajStatus(std::string status) { this->status = status; }
void Paczka::dodajOperacje(std::string operacja) { this->operacja = operacja; }
void Paczka::dodajIdentyfikator(int identyfikator) { this->identyfikator = identyfikator; }

void Paczka::nadpiszArgumenty(unsigned int argument)
{
	this->argumenty.clear();
	this->argumenty.push_back(argument);
}

void Paczka::zerujArgumenty(){ this->argumenty.clear();}


void Paczka::odczytaj(std::string wejscie)
{
	this->argumenty.clear();
	this->operacja = "NULL";

	char op;
	char bufor;
	unsigned int bliczba = 0;

	while (wejscie.size() > 0)
	{

		this->usunSpacaje(&wejscie);
		while (1)
		{
			this->usunSpacaje(&wejscie);
			bufor = wejscie[0];
			wejscie.erase(0, 1);

			if (!(bufor >= 48 && bufor <= 57))
			{
				this->argumenty.push_back(bliczba);
				bliczba = 0;
				break;
			}

			bliczba = bliczba * 10 + std::stoul(&bufor, nullptr, 10);
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
				this->operacja = "Dodawanie";
			}

			if (bufor == '-')
			{
				this->operacja = "Odejmowanie";
			}

			if (bufor == '*')
			{
				this->operacja = "Mnozenie";
			}

			if (bufor == ':')
			{
				this->operacja = "Dzielenie";
			}
		}
		else
		{
			if (op != bufor)
			{

				std::cout << "Bledzne dzialanie! " << std::endl;

			}
		}
	}


}

unsigned int Paczka::dajIdentyfikator() { return this->identyfikator; }
std::string Paczka::dajZnacznikCzasu() { return this->znacznikCzasu; }
std::string Paczka::dajStatus() { return this->status; }

std::string Paczka::dajPaczke()
{
	std::string paczka = "";
	paczka = "Operacja: " + this->operacja + ";Status: " + this->status + ";Identyfikator: " + std::to_string(this->identyfikator) + ";ZnacznikCzasu: "
		+ this->znacznikCzasu + ";";

	for (int i = 0; i < this->argumenty.size(); i++)
	{
		paczka = paczka + "Liczba" + std::to_string(i + 1) + ": " + std::to_string(argumenty[i]) + ";";
	}

	paczka = paczka + "Koniec: TAK;";

	return paczka;
}

void Paczka::usunSpacaje(std::string *tekst)
{
	while ((*tekst)[0] == ' ')
	{
		tekst->erase(0, 1);
	}
}

std::string Paczka::parsujPaczke(std::string wejscie) 
{ 
	std::map<std::string, std::string> zParsowanyPakiet; //bedzie trzymac informacje wyluskane z string wejscie

	//uzywam regex do podzielenia wejscia na grupy- [1]("klucz")[2](: )[3]("wartosc")[4](;)  
	//i zapisanie [1] i [3] grupy jako klucz i wartosc w mapie
	std::smatch para;
	std::regex wzorzecKluczWartosc("(\\w+)(:\\s)(\\S+)(;)");

	bool flagaPoprawnyPakiet = 1;
	while (wejscie.length() != 0 && flagaPoprawnyPakiet) //iteruje po wejsciu znajdujac jedna pare klucz-wartosc na raz
	{
		if (std::regex_search(wejscie, para, wzorzecKluczWartosc))
		{
			zParsowanyPakiet.insert_or_assign(para[1], para[3]);
			wejscie.erase(0, para[0].length()); //odcinanie od wejscia znalezionej juz pary
		}
		else {flagaPoprawnyPakiet = 0; return "Blad_struktury_paczki"; } 
		//jezeli nie znaleziono dopasowania, paczka jest niepoprawana strukturalnie
	}

	//Wypisywanie zawartosci mapy- debug
	/*std::cout << "\n-- MAPA--\n";
	for (auto it = zParsowanyPakiet.begin(); it != zParsowanyPakiet.end(); ++it){ std::cout << it->first << " " << it->second <<"\n";}*/

	//UZUPELNIANIE POL PACZKI
	auto szukaneWMapie = zParsowanyPakiet.find("Identyfikator"); //zmienna temp zeby uniknac kilkukrotnego wywolywania find z mapy
	//Identyfikator
	if (szukaneWMapie != zParsowanyPakiet.end()) 
	{
		if (szukaneWMapie->second != "NULL") { this->identyfikator = (unsigned)std::stoi(szukaneWMapie->second); }
	}else { return "Brak identyfikatora"; }
	//Operacja
	szukaneWMapie = zParsowanyPakiet.find("Operacja");
	if (szukaneWMapie != zParsowanyPakiet.end())
	{
		this->operacja = szukaneWMapie->second;
	}else { return "Brak operacji";}
	//Status
	szukaneWMapie = zParsowanyPakiet.find("Status");
	if (szukaneWMapie != zParsowanyPakiet.end())
	{
		this->status = szukaneWMapie->second;
	}else { return "Brak statusu"; }
	//Znacznik czasu
	szukaneWMapie = zParsowanyPakiet.find("ZnacznikCzasu");
	if (szukaneWMapie != zParsowanyPakiet.end())
	{
		this->znacznikCzasu = szukaneWMapie->second;
	}else { return "Brak znacznika czasu"; }
	//DODAWANIE ARGUMENTOW OPERACJI

	std::string aktualnaLiczba{ "Liczba1" };
	bool wiecejLiczb = 1; //flaga| 1- jezeli nalezy szukac jeszcze kolejnej liczby do dodania |0 -jezeli nie
	int licznik = 1;	//licznik dodanych liczb, sluzacy do modyfikacji zmiennej aktualna liczba
	while (wiecejLiczb)
	{
		//jezeli w mapie znajduje sie Liczba o kluczu wskazanym przez aktualnaLiczba, dodaj ja do vektora argumentow
		auto paraLiczbaWartosc = zParsowanyPakiet.find(aktualnaLiczba);
		if (paraLiczbaWartosc != zParsowanyPakiet.end()) {
			//sprawdzanie czy liczba nie jest za duza na unsigned int int
			try {
				unsigned int temp = std::stoul(paraLiczbaWartosc->second);
				if (temp > UINT_MAX) { throw "out_of_range"; }
				dodajArgument(temp); //dodaje liczbe do vektora argumentow paczki
			}
			catch (std::string s) { return "Liczby_poza_zakresem"; }
			catch (std::invalid_argument) { return "Niepoprawne_argumenty"; }
		}
		else { wiecejLiczb = 0; }
		//szukaj nastepnej liczby
		licznik++;
		aktualnaLiczba = "Liczba" + std::to_string(licznik);
	}

	return "OK";
}

void Paczka::zerujPaczke()
{
	Paczka::operacja = "NULL";
	Paczka::status = "NULL";
	Paczka::identyfikator = 0;
	Paczka::znacznikCzasu = "NULL";
	argumenty.clear();
}


std::string Paczka::dajOperacje() { return this->operacja; }
std::vector<unsigned int> Paczka::dajArgumenty() { return this->argumenty; }

std::string Paczka::dajDoWyswietlenia()
{
	std::string paczka = "\n\t";
	paczka = "|OPERACJA: " + this->operacja + "|STATUS: " + this->status + "|IDENTYFIKATOR: " + std::to_string(this->identyfikator)
		+ "|ZNACZNIK CZASU: " + this->znacznikCzasu;
	
	for (int i = 0; i < this->argumenty.size(); i++)
	{
		if(i==0){ paczka += +"|LICZBY: "; }
		paczka += std::to_string(argumenty[i]);
		if (i != this->argumenty.size() - 1) { paczka += ", "; }
	}
	paczka = paczka + "|";

	return paczka;
}
