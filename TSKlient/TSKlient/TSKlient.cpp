#include "pch.h"
#include <iostream>
#include <string>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <ws2tcpip.h>
#include "Paczka.h"
#include <cmath>

#pragma comment(lib, "ws2_32.lib")

void wyswietlLogo()
{
	std::cout << R"(
 _   __  ___   _      _   __ _   _  _       ___   _____  _____ ______ 
| | / / / _ \ | |    | | / /| | | || |     / _ \ |_   _||  _  || ___ \
| |/ / / /_\ \| |    | |/ / | | | || |    / /_\ \  | |  | | | || |_/ /
|    \ |  _  || |    |    \ | | | || |    |  _  |  | |  | | | ||    / 
| |\  \| | | || |____| |\  \| |_| || |____| | | |  | |  \ \_/ /| |\ \ 
\_| \_/\_| |_/\_____/\_| \_/ \___/ \_____/\_| |_/  \_/   \___/ \_| \_| 

                                                                      )" << std::endl;
}

int main()
{
	
	wyswietlLogo();
	Paczka paczka = Paczka();
	Paczka otrzymana = Paczka();
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

	} while (connect(newSocket, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR);
	 

	std::cout << std::endl << std::endl << std::endl << std::endl << "Palaczono z serwerem" << std::endl;
	//działanie (wysyłanie i odbieranie)
	do
	{
		userInput = "";
		std::cout << "Klient-> ";
		getline(std::cin, userInput);
		if (userInput == "")
		{
			std::cout << "Klient-> Konczenie sesji" << std::endl;
			break;
		}
		system("cls");
		wyswietlLogo();
		std::cout << std::endl << std::endl << std::endl <<"Klient-> Dzialanie: " << userInput << std::endl;
		result = paczka.odczytaj(userInput);
		if (result != 0)
		{
			if(result == 1)
			std::cout << "Klient-> Bledne dzialanie, podaj tylko jeden operator!" << std::endl;
			if (result == 2)
			std::cout << "Klient-> Za duzy argument!" << std::endl;
			continue;
		}
		paczka.dodajZnacznikCzasu();
		userInput = paczka.dajPaczke();

		//std::cout << userInput << std::endl; //testy
		result = send(newSocket, userInput.c_str(), userInput.size() + 1, 0);

		if (result != SOCKET_ERROR)
		{
			ZeroMemory(buffer, 4096);

			bytesRecived = recv(newSocket, buffer, 4096, 0);
			if (bytesRecived > 0)
			{
				//std::cout << "SERWER-> " << std::string(buffer, 0, bytesRecived) << std::endl; //testy

				otrzymana = Paczka();
				otrzymana.parsujPaczke(std::string(buffer, 0, bytesRecived));
				

				
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
