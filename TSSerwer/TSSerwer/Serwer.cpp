#include "pch.h"
#include "Serwer.h"


Serwer::Serwer(){}
Serwer::~Serwer(){}

unsigned int Serwer::losowaLiczba() {
	static std::default_random_engine e{};
	std::uniform_int_distribution<unsigned int> d(0, 100);
	return d(e);
}



void Serwer::startUp()
{
	int errorHandling = 0;

	aktualnaPaczka = Paczka(); //Initializuje pusta paczke

	// Initializuje winsock wersja 2.2
	errorHandling = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (errorHandling != 0) {}
	std::cout <<
R"(
    _____ ______ _______           ________ _____
   / ____ | ____ | __  \ \        / /  ____ | __ \
   | (___ | |__  | |__) \ \ / \  / /| |__  | |__) |
   \___ \ | __|  |  _  / \ \ / \/ / |  __| |  _  /
    ____) | |____| | \ \  \ / \  /  | |____| | \ \
  | _____/|_______ |  \_\  \ / \/   |______| |  \_\
____________________________________________________
Wariant 16, Mateusz Stelmasiak, 140783
		)";
	std::cout << "\n";
	nasluchujKlienta();
}





void Serwer::nasluchujKlienta()
{

	int errorHandling = 0;
	struct addrinfo *result = NULL;
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; // IPv4
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	errorHandling = getaddrinfo(NULL, PORT_NASLUCHUJACY, &hints, &result);
	if (errorHandling != 0) {
		printf("getaddrinfo failed with error: %d\n", errorHandling);
		WSACleanup();
	}

	// Create a SOCKET for connecting to server
	SocketSluchajacy = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (SocketSluchajacy == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
	}

	// Setup the TCP listening socket
	errorHandling = bind(SocketSluchajacy, result->ai_addr, (int)result->ai_addrlen);
	if (errorHandling == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(SocketSluchajacy);
		WSACleanup();
	}

	wypisz("Oczekiwanie na polaczenie...", generujInformacjeOSerwerze());

	errorHandling = listen(SocketSluchajacy, SOMAXCONN);
	if (errorHandling == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(SocketSluchajacy);
		WSACleanup();
	}

	// Accept a client socket

	SOCKADDR_IN addr;
	int addrlen = sizeof(addr);

	SocketKlienta = accept(SocketSluchajacy, (SOCKADDR*)&addr, &addrlen);
	if (SocketKlienta == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(SocketSluchajacy);
		WSACleanup();
	}
	wypisz("Nowy klient polaczony!", generujInformacjeOKliencie(addr)); 
	
	// zamkniecie nasluchujacego socket'a i rozpoczecie przyjmowania danych
	closesocket(SocketSluchajacy);
	otrzymujDane();
}



void Serwer::otrzymujDane()
{
	int dataHandling = 0; //over 0 for recieved bytes, less then zero for errors

	do {
		dataHandling = recv(SocketKlienta, buffer, bufferDlugosc, 0);
		if (dataHandling > 0) 
		{
			std::string bufferData = buffer;
			ZeroMemory(&buffer, DLUGOSC_BUFFERU);

			aktualnaPaczka.zerujPaczke();
			std::string error = aktualnaPaczka.parsujPaczke(bufferData);

			if(error=="OK")
			{
				wypisz("Otrzymano Paczke!", aktualnaPaczka.dajDoWyswietlenia());
				odpowiedzNaAktualnaPaczke();
			}
			else 
			{
				wypisz("Otrzymano bledna paczke!", error);
				wyslijPaczkeBledu(error);
			}

		}
		else if (dataHandling == 0) 
		{
			wypisz("Klient rozlaczyl sie", "powrot do nasluchiwania");
			nasluchujKlienta();
		}
		else {
			wypisz("SOCKET ERROR", "powrot do nasluchiwania");
			nasluchujKlienta();
		}

	} while (dataHandling > 0);
}


void Serwer::wyslijPaczkeBledu(std::string blad) {
	Paczka reply = Paczka();
	reply.dodajStatus(blad);

	int rezultatWysylki = 0;
	rezultatWysylki = send(SocketKlienta, reply.dajPaczke().c_str(), reply.dajPaczke().length(), 0);
	if (rezultatWysylki == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(SocketKlienta);
		WSACleanup();
	}
	wypisz("Wyslano paczke!", reply.dajDoWyswietlenia());
}

void Serwer::odpowiedzNaAktualnaPaczke()
{
		Paczka reply = aktualnaPaczka;

		if (aktualnaPaczka.dajIdentyfikator() == 0) { //if the identificator field is null meaning its the first message between serwer and client
			unsigned int identyfikator = losowaLiczba();
			reply.dodajIdentyfikator(identyfikator);
			this->identyfikator = identyfikator;
			aktualnaPaczka.dodajIdentyfikator(identyfikator) ;
		}

		if (aktualnaPaczka.dajIdentyfikator() == this->identyfikator)
		{
			int wynik=-1;
			std::string operacja = aktualnaPaczka.dajOperacje();
			std::string status = "OK";
			std::vector<unsigned int> argumenty= aktualnaPaczka.dajArgumenty();

			if (operacja != "NULL") 
			{
				if (argumenty.size() > 1) {
					if (operacja == "Dodawanie") { wynik = obliczDodawanie(argumenty, status); }
					else if (operacja == "Logarytm") { wynik = obliczDodawanie(argumenty, status); }
					else if (operacja == "Odejmowanie") { wynik = obliczOdejmowanie(argumenty, status); }
					else if (operacja == "Dzielenie") { wynik = obliczDzielenie(argumenty, status); }
					else if (operacja == "Mnozenie") { wynik = obliczMnozenie(argumenty, status); }
					else if (operacja == "Potega") { wynik = obliczPotege(argumenty, status); }
					else if (operacja == "Pierwiastek") { wynik = obliczPierwiastek(argumenty, status); }
					else if (operacja == "Modulo") { wynik = obliczModulo(argumenty, status); }
					else { status = "ERROR: Nieznana operajca"; }
				}
				else { status = "ERROR: Za malo argumentow"; }
			}else { status = "ERROR: Brak operacji"; }
	
			if (wynik > -1) { reply.nadpiszArgumenty(wynik); }
			else { reply.zerujArgumenty(); }
			
			reply.dodajStatus(status);
			reply.dajZnacznikCzasu();
		}

		//wysylanie odpowiedzi do klienta
		int iSendResult = 0;
		iSendResult = send(SocketKlienta, reply.dajPaczke().c_str(), reply.dajPaczke().length(), 0);
		if (iSendResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(SocketKlienta);
			WSACleanup();
		}
		wypisz("Odeslano Paczke!", reply.dajDoWyswietlenia());
}

unsigned int Serwer::obliczOdejmowanie(std::vector <unsigned int> argumenty, std::string& status) {
	int wynik = argumenty[0];
	for (int i = 1; i < argumenty.size();i++) {
		wynik -= argumenty[i];
		if (wynik < 0) { status = "ERROR: Mniejsze od zera!"; return -1; }
	}
	return wynik;
}

unsigned int Serwer::obliczMnozenie(std::vector <unsigned int> argumenty, std::string& status) {
	int wynik = 1;
	for (auto i : argumenty) {
		if (wynik!=0 && i > UINT_MAX/wynik ) { status = "ERROR: Overflow!"; return -1; }
		wynik *= i;
	}
	return wynik; 
}

unsigned int Serwer::obliczDzielenie(std::vector <unsigned int> argumenty, std::string& status) {
	int wynik = argumenty[0];
	for (int i = 1; i < argumenty.size(); i++) {
		if (argumenty[i] == 0) { status = "ERROR: Dzielenie przez zero!";  return -1; }
		wynik /= argumenty[i];
	}
	return wynik; 
}

unsigned int Serwer::obliczModulo(std::vector<unsigned int> argumenty, std::string & status)
{
	int wynik = argumenty[0];
	for (int i = 1; i < argumenty.size(); i++) {
		if (argumenty[i] == 0) { status = "ERROR: Dzielenie przez zero!";  return -1; }
		wynik %= argumenty[i];
	}
	return wynik;
}

unsigned int Serwer::obliczPotege(std::vector<unsigned int> argumenty, std::string & status)
{
	if (argumenty.size() > 2) { status = "ERROR: Za duzo argumentow!"; return -1; }
	if (argumenty.size() < 2) { status = "ERROR: Za malo argumentow!"; return -1; }
	int wynik = 1;
	for (int i = 1; i < argumenty[1]; i++) {
		if (argumenty[0]> UINT_MAX / wynik) { status = "ERROR: Overflow!"; return -1; }
		wynik *=argumenty[0];
	}
	return wynik;
}

unsigned int Serwer::obliczPierwiastek(std::vector<unsigned int> argumenty, std::string & status)
{
	if (argumenty.size() > 2) { status = "ERROR: Za duzo argumentow!"; return -1; }
	return std::pow(argumenty[1], 1.0 / argumenty[0]);
}

unsigned int Serwer::obliczDodawanie(std::vector <unsigned int> argumenty,std::string& status) {
	int wynik = 0;
	for (auto i : argumenty) {
		if ((i > UINT_MAX - wynik)) {status = "ERROR: Overflow!"; return -1;}	//zapobieganie overflow
		wynik += i;
	}
	return wynik; 
}

unsigned int Serwer::obliczLogarytm(std::vector <unsigned int> argumenty, std::string& status) {
	if (argumenty.size() > 2) { status = "ERROR: Za duzo argumentow!"; return -1;}
	return (int)(log(argumenty[0])/ log(argumenty[1]));
}

std::string Serwer::generujZnacznikCzasu() {
	std::time_t t = std::time(0);
	std::tm data;
	localtime_s(&data, &t);
	std::vector<int> godzinaMinutySekundyInt{ data.tm_hour ,data.tm_min,data.tm_sec };
	std::vector<std::string> godzinaMinutySekundyString;
	for(int i=0;i<3;i++)
	{
		int temp = godzinaMinutySekundyInt[i];
		if (temp < 10) { godzinaMinutySekundyString.push_back("0" + std::to_string(temp)); }
		else { godzinaMinutySekundyString.push_back(std::to_string(temp)); }
	}
	return godzinaMinutySekundyString[0] + ":" + godzinaMinutySekundyString[1] + ":" + godzinaMinutySekundyString[2];
}


std::string Serwer::generujInformacjeOSerwerze() {
	std::string ip{""};
	return "IP serwera: " + ip + " Port nasluchujacy: " + PORT_NASLUCHUJACY;
}
std::string Serwer::generujInformacjeOKliencie(SOCKADDR_IN addr)
{
	char adresIP[NI_MAXHOST];	// adres ip klienta
	inet_ntop(AF_INET, &addr.sin_addr, adresIP, NI_MAXHOST);

	std::string adresIPString = adresIP;
	std::string portString = std::to_string(ntohs(addr.sin_port));

	return "Adres ip: " + adresIPString + " Port: " +  portString;
}


void Serwer::wypisz(std::string doWypisania) 
{
	std::cout << "\n<" << generujZnacznikCzasu() << "> " << doWypisania;
	std::cout << "\n";
}

void Serwer::wypisz(std::string naglowek, std::string cialo)
{
	std::cout << "\n<" << generujZnacznikCzasu() << "> " << naglowek;
	std::cout << "\n  " << cialo;
	std::cout << "\n";
}



