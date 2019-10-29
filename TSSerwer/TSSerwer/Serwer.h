#pragma once
#include "pch.h"
#include <iostream>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WS2tcpip.h>
#include <windows.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "Paczka.h"
#include <string>
#include <random>

#pragma comment (lib, "ws2_32.lib") //dodawanie biblioteki winsock

#define PORT_NASLUCHUJACY "54000"
#define DLUGOSC_BUFFERU 4096 

class Serwer
{
public:
	Serwer();
	~Serwer();


	//! Uruchamianie serwera
	/** Inicjalizuje dane biblioteki winsock i wypisuje naglowek serwera
	 	nastepnie wywoluje nasluchujKlienta()
	*/
	int startUp();

private:
	char buffer[DLUGOSC_BUFFERU];
	int bufferDlugosc = DLUGOSC_BUFFERU;

	
	WSADATA wsaData; //dane winSock'a
	SOCKET SocketSluchajacy = INVALID_SOCKET; //socket oczekujacy na polaczenie	
	SOCKET SocketKlienta = INVALID_SOCKET; //socket clienta

	unsigned int identyfikator = 0; //identyfikator sesji
	Paczka aktualnaPaczka;



	//! Czekanie na polaczenie z klientem
	/*! Ustawia parametry socket'u nasluchujacego, wypisuje jego dane do konsoli
		i oczekuje na polaczenie ze strony klienta
	*/
	void nasluchujKlienta();

	//! Nasluchuje klientow
	/*! Pobiera dane z paczki w formie string i uzupelnia pola w klasie
		Zwraca OK dla poprawnych paczek, i opis bledu dla niepoprawnych
	*/
	void otrzymujDane();


	unsigned int generujLosowyIdentyfikator();
	void wyslijPaczkeBledu(std::string blad);
	void odpowiedzNaAktualnaPaczke();


	//FUNKCJE OPERACJI MATEMATYCZNYCH
	unsigned int obliczDodawanie(std::vector<unsigned int> argumenty, std::string & status);
	unsigned int obliczLogarytm(std::vector<unsigned int> argumenty, std::string & status);
	unsigned int obliczOdejmowanie(std::vector<unsigned int> argumenty, std::string & status);
	unsigned int obliczMnozenie(std::vector<unsigned int> argumenty, std::string & status);
	unsigned int obliczDzielenie(std::vector<unsigned int> argumenty, std::string & status);
	unsigned int obliczModulo(std::vector<unsigned int> argumenty, std::string & status);
	unsigned int obliczPotege(std::vector<unsigned int> argumenty, std::string & status);
	unsigned int obliczPierwiastek(std::vector<unsigned int> argumenty, std::string & status);



	//GENEROWANIE DANYCH
	std::string generujZnacznikCzasu();
	std::string generujInformacjeOSerwerze();
	std::string generujInformacjeOKliencie(SOCKADDR_IN addr);

	//WYPISYWANIE DO KONSOLI
	void wypisz(std::string doWypisania);
	void wypisz(std::string naglowek, std::string cialo);
};




