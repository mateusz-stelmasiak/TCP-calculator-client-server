﻿#include "pch.h"
#include <iostream>
#include <string>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <ws2tcpip.h>
#include "Paczka.h"
#include <cmath>

#pragma comment(lib, "ws2_32.lib")

void wyswietlLogo()
{
	std::cout << R"(  Wariant 16
  _   __  ___   _      _   __ _   _  _       ___   _____  _____ ______ 
 | | / / / _ \ | |    | | / /| | | || |     / _ \ |_   _||  _  || ___ \
 | |/ / / /_\ \| |    | |/ / | | | || |    / /_\ \  | |  | | | || |_/ /
 |    \ |  _  || |    |    \ | | | || |    |  _  |  | |  | | | ||    / 
 | |\  \| | | || |____| |\  \| |_| || |____| | | |  | |  \ \_/ /| |\ \ 
 \_| \_/\_| |_/\_____/\_| \_/ \___/ \_____/\_| |_/  \_/   \___/ \_| \_| 

                                               Maciej Stefaniak, 140782
                                                                      )" << std::endl;
}

int main()
{
	
	wyswietlLogo(); // metoda wyświetlkająca logo, używana po wyczyszczeniu ekranu 
	Paczka paczka = Paczka();	 //
	Paczka otrzymana = Paczka(); // paczki, otrzymane i wysłane
	char buffer[4096];


	std::string ipAdress;
	int port = 54000;
	SOCKET newSocket;
	std::string userInput;
	int result;
	int bytesRecived;
	WSAData data;
	WORD ver;




	//inicjacja biblioteki
	ver = MAKEWORD(2, 2);
	result = WSAStartup(ver, &data);
	if (result != 0)
	{
		std::cout << "Klient-> Nie mozna uruchomic winsocka!" << std::endl;
		return 1;
	}

	
	//tworzenie socketu
	newSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (newSocket == INVALID_SOCKET)
	{
		std::cout << "Klient-> Nie mozna podlaczyc sie do socketu!" << std::endl;
		WSACleanup();
		return 2;
	}

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	
	//łączenie się z serwerem
	do
	{
		std::cout << std::endl << std::endl << std::endl << std::endl << std::endl;
		std::cout << "Klient-> Podaj IP Serwera: ";
		getline(std::cin, ipAdress);
		system("cls");
		wyswietlLogo();
		inet_pton(AF_INET, ipAdress.c_str(), &hint.sin_addr); //konwertuje string na numeryczny adres ip

		if (connect(newSocket, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR)
		{
			std::cout << "Klient-> Nie mozna polaczyc sie z serwerem! " << std::endl;
		}
		else
		{
			break;
		}
	} while (1);
	 

	std::cout << std::endl << std::endl << std::endl << std::endl << "Palaczono z serwerem" << std::endl;

	//działanie (wysyłanie i odbieranie), główna pętla programu
	do
	{
		userInput = "";
		std::cout << "Klient-> ";
		getline(std::cin, userInput);
		if (userInput == "") //po braku otrzymania polecenia program się wyłącza 
		{
			std::cout << "Klient-> Konczenie sesji" << std::endl;
			break;
		}
		system("cls");	//
		wyswietlLogo(); // dla zachowania przejrzystości działania programu ekran jest czyszczony
		std::cout << std::endl << std::endl << std::endl <<"Klient-> Dzialanie: " << userInput << std::endl;
		result = paczka.odczytaj(userInput);
		if (result != 0) // sprawdzanie błędów wynikających z pomyłki użytkownika przy wpisywaniu
		{
			if(result == 1)
			std::cout << "Klient-> Bledne dzialanie, podaj tylko jeden operator!" << std::endl;
			if (result == 2)
			std::cout << "Klient-> Za duzy argument!" << std::endl;
			continue;
		}
		paczka.dodajZnacznikCzasu();
		userInput = paczka.dajPaczke();


		result = send(newSocket, userInput.c_str(), userInput.size(), 0);
		if (result != SOCKET_ERROR)
		{
			ZeroMemory(buffer, 4096);

			bytesRecived = recv(newSocket, buffer, 4096, 0);
			if (bytesRecived > 0)
			{

				otrzymana = Paczka();
				otrzymana.parsujPaczke(std::string(buffer, 0, bytesRecived));
				

				/*sprawdzanie czy wiadomośc zawiera informację o wystąpieniu błędu na podstawie tego czy zawiera argumenty*/
				if (paczka.dajIdentyfikator() == 0)
				{
					paczka.dodajIdentyfikator(otrzymana.dajIdentyfikator());
				}

				if (paczka.dajIdentyfikator() != otrzymana.dajIdentyfikator())
				{
					std::cout << "Identyfikator nie jest zgodny!" << std::endl;
					continue;
				}
				
				if (otrzymana.dajArgumenty().size() == 0)
				{
					std::cout << "Serwer-> Blad: " << otrzymana.dajStatus() << std::endl;
				}
				else
				{
					std::cout << "Wynik z serwera-> " << otrzymana.dajArgumenty()[0] << std::endl;
				}

			}
		}



	} while (1);

	closesocket(newSocket);
	WSACleanup();

}
