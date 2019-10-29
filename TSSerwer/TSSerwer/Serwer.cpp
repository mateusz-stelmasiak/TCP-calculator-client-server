#include "pch.h"
#include "Serwer.h"


Serwer::Serwer(){}
Serwer::~Serwer(){}

int Serwer::startUp()
{
	int flagaError = 0;
	aktualnaPaczka = Paczka(); //Inicjalizuje pusta paczke

	// Inicjalizuje winsock wersja 2.2
	flagaError = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (flagaError != 0) { wypisz("Nie mozna zainicjowac biblioteki winsock", "zamykanie serwera"); return 0; }

	wyswietlLogo();

	nasluchujKlienta();	
}

void Serwer::nasluchujKlienta()
{
	//kod zaadaptowany z dokumentacji winsock https://docs.microsoft.com/en-us/windows/win32/winsock/creating-a-socket-for-the-server

	int flagaError = 0;
	struct addrinfo *result = NULL; //bedzie przetrzymywac dane o serwerze
	struct addrinfo hints;	//uzywane do konfiguracji serwera


	ZeroMemory(&hints, sizeof(hints)); //wypelnia miejsce zarezerwowane dla struktury zerami
	//Konfiguracja serwera
	hints.ai_family = AF_INET; // uzywanie IPv4
	hints.ai_socktype = SOCK_STREAM; // jako socket "potokowy" 
	hints.ai_protocol = IPPROTO_TCP; //uzywanie prokolu TCP
	hints.ai_flags = AI_PASSIVE; //flaga wskazujaca na to, ze chce sie uzyc zwracanej struktury result do przypisania danych 

	// Odczytuje adres serwera i port
	flagaError = getaddrinfo(NULL, PORT_NASLUCHUJACY, &hints, &result);
	if (flagaError != 0) {
		wypisz("Blad funkcji getaddrinfo", "Numer bledu- " +std::to_string(flagaError));
		WSACleanup();
		wypisz("Restartowanie serwera...");
		startUp();
	}

	// Tworzenie socket'a nasluchujacego
	SocketSluchajacy = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (SocketSluchajacy == INVALID_SOCKET) {
		wypisz("Blad socket'a nasluchujacego", "Numer bledu- " + std::to_string(WSAGetLastError()));
		freeaddrinfo(result);
		WSACleanup();
		wypisz("Restartowanie serwera...");
		startUp();
	}

	// Przypisuje dane serwera do socketu nasluchujacego
	flagaError = bind(SocketSluchajacy, result->ai_addr, (int)result->ai_addrlen);
	if (flagaError == SOCKET_ERROR) {
		wypisz("Blad funkcji bind", "Numer bledu- " + std::to_string(WSAGetLastError()));
		freeaddrinfo(result);
		closesocket(SocketSluchajacy);
		WSACleanup();
		wypisz("Restartowanie serwera...");
		startUp();
	}

	wypisz("Oczekiwanie na polaczenie...", generujInformacjeOSerwerze());

	//nasluchuje na polaczenie klienta
	flagaError = listen(SocketSluchajacy, SOMAXCONN);
	if (flagaError == SOCKET_ERROR) {
		wypisz("Blad funkcji bind", "Numer bledu- " + std::to_string(WSAGetLastError()));
		closesocket(SocketSluchajacy);
		WSACleanup();
		wypisz("Restartowanie serwera...");
		startUp();
	}

	//Akceptuje polaczenie klienta i zapisuje jego dane w strukturze SOCKADDR_IN
	SOCKADDR_IN addr;
	int addrlen = sizeof(addr);
	SocketKlienta = accept(SocketSluchajacy, (SOCKADDR*)&addr, &addrlen);
	if (SocketKlienta == INVALID_SOCKET) {
		wypisz("Blad funkcji bind", "Numer bledu- " + std::to_string(WSAGetLastError()));
		closesocket(SocketSluchajacy);
		WSACleanup();
		wypisz("Restartowanie serwera...");
		startUp();
	}
	wypisz("Nowy klient polaczony!", generujInformacjeOKliencie(addr)); 
	
	// zamkniecie nasluchujacego socket'a i rozpoczecie przyjmowania danych
	closesocket(SocketSluchajacy);
	otrzymujDane();
}


void Serwer::otrzymujDane()
{
	int flagaDanych = 0; //przechowuje dane o stanie otrzymywania danych

	//iteruje tak dlugo az klient jest podlaczony
	do { 
		//otrzymuje dane i zapisuje je w bufferze, zwraca wartosc do flagiDanych
		//JEZELI ZWRACA: >0 oznacza ilosc przyjetych bajtow, <0 oznacza kod bledu, =0 oznacza rozlaczenie klienta
		flagaDanych = recv(SocketKlienta, buffer, bufferDlugosc, 0);  
		if (flagaDanych > 0) //jezeli przyjeto jakies dane
		{
			//Przepisywanie danych do string'a i zerowanie buffera i aktualnej paczki
			std::string daneBuffera = buffer; 
			ZeroMemory(&buffer, DLUGOSC_BUFFERU);
			aktualnaPaczka.zerujPaczke();

			//przepisywanie danych buffera do paczki, zwraca "OK" jezeli danebuffera nie zawieraly bledow
			//w przeciwnym wypdaku zwraca string z bledem
			std::string error = aktualnaPaczka.parsujPaczke(daneBuffera);

			if(error=="OK") //jezeli otrzymana paczka nie zawierala bledow
			{
				wypisz("Otrzymano Paczke!", aktualnaPaczka.dajDoWyswietlenia());
				odpowiedzNaAktualnaPaczke();
			}
			else //jezeli otrzymana paczka zawierala bledy
			{
				wypisz("Otrzymano bledna paczke!", error);
				wyslijPaczkeBledu(error);
			}

		}
		else if (flagaDanych == 0) //jezeli klient rozlaczyl sie
		{
			wypisz("Klient rozlaczyl sie", "powrot do nasluchiwania");
			this->identyfikator = 0; //resetowanie identyfikatora sesji
			nasluchujKlienta();
		}
		else { //jezeli wystapil blad socket'a
			wypisz("Blad socket'a z kodem: " +std::to_string(flagaDanych), "powrot do nasluchiwania");
			this->identyfikator = 0; //resetowanie identyfikatora sesji
			nasluchujKlienta();
		}
	} while (flagaDanych > 0);
}

void Serwer::odpowiedzNaAktualnaPaczke()
{
		//kopiowanie pol z otrzymanej paczki do paczki odpowiedzi
		Paczka paczkaOdpowiedz = aktualnaPaczka;

		//Handling pierwszej wymiany miedzy Serwerem a Klientem
		///jezeli identyfikator paczki wyslanej przez klienta to NULL, i 
		///i serwer nie wygenerowal jeszcze dla aktualnego klienta zadnego identyfikatora
		///ten identyfikator jest generowany i odsylany w paczce do klienta, razem z odpowiedzia
		if (aktualnaPaczka.dajIdentyfikator() == 0 && identyfikator==0) { 
			unsigned int identyfikator = generujLosowyIdentyfikator();
			paczkaOdpowiedz.dodajIdentyfikator(identyfikator);
			this->identyfikator = identyfikator;
		}

		if (paczkaOdpowiedz.dajIdentyfikator() == this->identyfikator)
		{
			//zmienne pomocnicze
			long long int wynik=-1; //wynik ustawiony na -1 jako znacznik bledu
			std::string operacja = aktualnaPaczka.dajOperacje();
			std::string status = "OK"; //przechowuje dane o bledach, OK dla braku bledow
			std::vector<unsigned int> argumenty= aktualnaPaczka.dajArgumenty();

			//sprawdzanie i wykonywanie operacji zawartej w paczce
			if (operacja != "NULL") 
			{
				if (argumenty.size() > 1) //kazda operacja musi miec conajmniej 2 argumenty 
				{ 
					//kazda funkcja obliczeniowa zwraca dane o bledach do zmiennej status, przekazywanej przez referencje
					//i wynik do zmiennej wynik, w przypadku bledu wynik zawsze =-1
					if (operacja == "Dodawanie") { wynik = obliczDodawanie(argumenty, status); }
					else if (operacja == "Logarytm") { wynik = obliczLogarytm(argumenty, status); }
					else if (operacja == "Odejmowanie") { wynik = obliczOdejmowanie(argumenty, status); }
					else if (operacja == "Dzielenie") { wynik = obliczDzielenie(argumenty, status); }
					else if (operacja == "Mnozenie") { wynik = obliczMnozenie(argumenty, status); }
					else if (operacja == "Potega") { wynik = obliczPotege(argumenty, status); }
					else if (operacja == "Pierwiastek") { wynik = obliczPierwiastek(argumenty, status); }
					else if (operacja == "Modulo") { wynik = obliczModulo(argumenty, status);}
					else { status = "Nieznana operajca"; }
				}else { status = "Za_malo_argumentow"; }
			}else { status = "Brak_operacji"; }
	
			//jezeli w odpowiedzi nie poprawil sie blad, wpisuje sie wynik jako argument paczkiOdpowiedzi
			if (status=="OK") { paczkaOdpowiedz.nadpiszArgumenty(wynik); }
			//w przeciwnym wypadku paczkaOdpowiedzi nie zawiera zadnych argumentow
			else {paczkaOdpowiedz.zerujArgumenty(); } 
			
			paczkaOdpowiedz.dodajStatus(status);
			paczkaOdpowiedz.dajZnacznikCzasu();
		}
		else {wyslijPaczkeBledu("Niepoprawny_identyfikator"); }

		//wysylanie odpowiedzi do klienta
		int rezultatWysylki = 0;
		rezultatWysylki = send(SocketKlienta, paczkaOdpowiedz.dajPaczke().c_str(), paczkaOdpowiedz.dajPaczke().length(), 0);
		if (rezultatWysylki == SOCKET_ERROR) {
			wypisz("Blad funkcji send", "Numer bledu- " + std::to_string(WSAGetLastError()));
			closesocket(SocketKlienta);
			WSACleanup();
		}
		else { wypisz("Odeslano Paczke!", paczkaOdpowiedz.dajDoWyswietlenia()); }
}

void Serwer::wyslijPaczkeBledu(std::string blad) {
	Paczka reply = Paczka();
	reply.dodajStatus(blad);
	reply.dodajIdentyfikator(identyfikator);
	reply.dodajOperacje("BLEDNY_PAKIET");

	int rezultatWysylki = 0;
	rezultatWysylki = send(SocketKlienta, reply.dajPaczke().c_str(), reply.dajPaczke().length(), 0);
	if (rezultatWysylki == SOCKET_ERROR) {
		wypisz("Blad funkcji send", "Numer bledu- " + std::to_string(WSAGetLastError()));
		closesocket(SocketKlienta);
		WSACleanup();
	}
	else { wypisz("Wyslano paczke bledu!", reply.dajDoWyswietlenia()); }
}

//FUNKCJE OBLICZENIOWE

long long int Serwer::obliczOdejmowanie(std::vector <unsigned int> argumenty, std::string& status) {
	long long int wynik = argumenty[0];
	for (int i = 1; i < argumenty.size();i++) {
		wynik -= argumenty[i];
		if (wynik < 0) { status = "Wynik_mniejszy_od_zera"; return -1; }
	}
	return wynik;
}

long long int Serwer::obliczMnozenie(std::vector <unsigned int> argumenty, std::string& status) {
	unsigned int wynik = 1;
	for (auto i : argumenty) {
		if (wynik!=0 && i >= UINT_MAX/wynik ) { status = "Przepelnienie"; return -1; }
		wynik *= i;
	}
	return wynik; 
}

long long int Serwer::obliczDzielenie(std::vector <unsigned int> argumenty, std::string& status) {
	unsigned int wynik = argumenty[0];
	for (int i = 1; i < argumenty.size(); i++) {
		if (argumenty[i] == 0) { status = "Dzielenie_przez_zero";  return -1; }
		wynik /= argumenty[i];
	}
	return wynik; 
}

long long int Serwer::obliczModulo(std::vector<unsigned int> argumenty, std::string & status)
{
	unsigned int wynik = argumenty[0];
	for (int i = 1; i < argumenty.size(); i++) {
		if (argumenty[i] == 0) { status = "Dzielenie_przez_zero";  return -1; }
		wynik %= argumenty[i];
	}
	return wynik;
}

long long int Serwer::obliczPotege(std::vector<unsigned int> argumenty, std::string & status)
{
	long long int wynik = argumenty[0];
	unsigned int wykladnik = 1;
	for (int i = 1; i < argumenty.size(); i++) {
		if (wykladnik == 0) { continue; }
		if (argumenty[i] >= UINT_MAX / wykladnik) { status = "Przepelnienie_Wykladnika"; return -1; }
		wykladnik *= argumenty[i];
	}
	if (wykladnik == 0) { return 1; } //kazda cyfra do zerowej to 1
	if (wynik == 0) { return 0; } //zero do kazdej potegi (oprocz 0) to 0

	for (int i = 1; i < wykladnik; i++) {
		if (argumenty[0] >= UINT_MAX/wynik) { status = "Przepelnienie"; return -1; }
		wynik *= argumenty[0];
	}
	return wynik;
}

long long int Serwer::obliczPierwiastek(std::vector<unsigned int> argumenty, std::string & status)
{
	if (argumenty.size() > 2) { status = "Za_duzo_argumentow"; return -1; }
	if(argumenty[0]==0 ||argumenty[1]==0){ status = "Niepoprawny_pierwiastek"; return -1; }
	return std::pow(argumenty[1], 1.0 / argumenty[0]);
}

long long int Serwer::obliczDodawanie(std::vector <unsigned int> argumenty,std::string& status) {
	unsigned int wynik = 0;
	for (auto i : argumenty) {
		if ((i >= UINT_MAX - wynik)) {status = "Przepelnienie"; return -1;}	//zapobieganie overflow
		wynik += i;
	}
	return wynik; 
}

long long int Serwer::obliczLogarytm(std::vector <unsigned int> argumenty, std::string& status) {
	if (argumenty.size() > 2) { status = "Za_duzo_argumentow"; return -1;}
	if (argumenty[0] == 1 || argumenty[0] == 0) { status = "Niepoprawny_Logarytm"; return -1; }
	return (unsigned int) (std::log(argumenty[1])/ std::log(argumenty[0]));
	
}

//GENERATORY

std::string Serwer::generujZnacznikCzasu() {
	std::time_t t = std::time(0);
	std::tm data;
	localtime_s(&data, &t);

	//formatowanie danych w taki sposob aby liczby mniejsze od 10 byly poprzedzone 0, (np. 10:06:03 zamiast 10:6:3)
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

std::string Serwer::generujInformacjeOSerwerze() 
{
	char szBuffer[1024];
	unsigned char cip[4];
	if (gethostname(szBuffer, sizeof(szBuffer)) == SOCKET_ERROR)
	{
		WSACleanup();
		return false;
	}
	
	struct hostent *host = gethostbyname(szBuffer);
	if (host == NULL)
	{
		WSACleanup();
		return false;
	}
	std::string ip;
	cip[0] = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b1;
	cip[1] = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b2;
	cip[2] = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b3;
	cip[3] = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b4;
	ip = std::to_string((int)cip[0]) + "." + std::to_string((int)cip[1]) + "." + std::to_string((int)cip[2]) + "." + std::to_string((int)cip[3]);

	return "IP serwera: " + ip + "| Port nasluchujacy: " + PORT_NASLUCHUJACY;
}

std::string Serwer::generujInformacjeOKliencie(SOCKADDR_IN addr)
{
	char adresIP[NI_MAXHOST];	// adres ip klienta
	inet_ntop(AF_INET, &addr.sin_addr, adresIP, NI_MAXHOST);

	std::string adresIPString = adresIP;
	std::string portString = std::to_string(ntohs(addr.sin_port));

	return "Adres ip: " + adresIPString + "| Port: " +  portString;
}

unsigned int Serwer::generujLosowyIdentyfikator() {
	std::srand(std::time(0));
	return (rand()%100)+1;
}


//WYPISYWANIE DANYCH DO KONSOLI
void Serwer::wypisz(std::string doWypisania)
{
	std::cout << "\n<" << generujZnacznikCzasu() << ">" << doWypisania;
	std::cout << "\n";
}

void Serwer::wypisz(std::string naglowek, std::string cialo)
{
	std::cout << "\n<" << generujZnacznikCzasu() << ">" << naglowek;
	std::cout << "\n  " << cialo;
	std::cout << "\n";
}

void Serwer::wyswietlLogo() {
	std::cout <<
		R"(
    ______ ______ _______           ________ _____
   /  ____| ____ |  __ \ \         / /  ____|  __ \
  |  (___ | |__  | |__) \ \   /\  / /| |__  | |__) |
   \___  \| ___| |  _  / \ \ /  \/ / |  __| |  _  /
    ____) | |____| | \ \  \   /\  /  | |____| | \ \
   |_____/|_______ |  \_\  \ /  \/   |______| |  \_\
____________________________________________________
Wariant 16, Mateusz Stelmasiak, 140783
		)";
	std::cout << "\n";
}
