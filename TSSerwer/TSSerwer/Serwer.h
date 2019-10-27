#pragma once
#include "pch.h"
#include <iostream>
#include <WS2tcpip.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "Paczka.h"
#include <string>
#include <random>

#pragma comment (lib, "ws2_32.lib") //adding the winsoc library

#define PORT_NASLUCHUJACY "54000"
#define DLUGOSC_BUFFERU 4096 //4GB buffer

class Serwer
{
public:
	Serwer();
	~Serwer();
	unsigned int losowaLiczba();
	void startUp();
	void nasluchujKlienta();
	void otrzymujDane();
	void wyslijPaczkeBledu(std::string blad);
	void odpowiedzNaAktualnaPaczke();

private:
	char buffer[DLUGOSC_BUFFERU];
	int bufferDlugosc = DLUGOSC_BUFFERU;

	//winSock data
	WSADATA wsaData;
	SOCKET SocketSluchajacy = INVALID_SOCKET; //socket for listening
	SOCKET SocketKlienta = INVALID_SOCKET;

	unsigned int identyfikator = 0; //0 if no client has an established session
	Paczka aktualnaPaczka;

	//functions for the mathematical operations
	unsigned int obliczDodawanie(std::vector<unsigned int> argumenty, std::string & status);
	unsigned int obliczLogarytm(std::vector<unsigned int> argumenty, std::string & status);
	unsigned int obliczOdejmowanie(std::vector<unsigned int> argumenty, std::string & status);
	unsigned int obliczMnozenie(std::vector<unsigned int> argumenty, std::string & status);
	unsigned int obliczDzielenie(std::vector<unsigned int> argumenty, std::string & status);
	unsigned int obliczModulo(std::vector<unsigned int> argumenty, std::string & status);
	unsigned int obliczPotege(std::vector<unsigned int> argumenty, std::string & status);
	unsigned int obliczPierwiastek(std::vector<unsigned int> argumenty, std::string & status);

	std::string generujZnacznikCzasu();
	std::string generujInformacjeOSerwerze();
	std::string generujInformacjeOKliencie(SOCKADDR_IN addr);
	void wypisz(std::string doWypisania);
	void wypisz(std::string naglowek, std::string cialo);
};


/*
BUDOWA PAKIETU:
----------NAG£ÓWEK---------------
Operacja: ...;
Status: ...;
Identyfikator: ...;
Czas: ...;
---------DANE-------



OPERACJE:
„dodaj” – dodawanie
„odejmij” – odejmowanie
„mnozenie” – mno¿enie
„dzielenie” –dzielenie
"modulo" - modulo
"potega" - potêgowanie  pierwszy argument do potêgi drugiego argu
"logarytm" - logarytm z pierwszego argumentu o podstawie drugiego argumentu
"pierwiastek"


*/


