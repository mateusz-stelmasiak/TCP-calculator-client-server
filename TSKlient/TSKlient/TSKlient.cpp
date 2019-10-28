#include "pch.h"
#include <iostream>
#include <string>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <ws2tcpip.h>
#include "Paczka.h"


#pragma comment(lib, "ws2_32.lib")

int main()
{
	Paczka paczka = Paczka();
	char buffer[4096];


	std::string ipAdress = "127.0.0.1";
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
		std::cout << "Nie można uruchomic winsocka!" << std::endl;
		return 1;
	}

	
	//tworzenie socketu
	newSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (newSocket == INVALID_SOCKET)
	{
		std::cout << "Nie można podłączyć się do socketu!" << std::endl;
		WSACleanup();
		return 2;
	}

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAdress.c_str(), &hint.sin_addr);

	//łączenie się z serwerem
	result = connect(newSocket, (sockaddr*)&hint, sizeof(hint));
	if (result == SOCKET_ERROR)
	{
		std::cout << "Nie mozna polaczyc się z serwerem! " << std::endl;
		closesocket(newSocket);
		WSACleanup();
		return 3;
	}

	
	//działanie (wysyłanie i odbieranie)
	do
	{
		std::cout << "- ";
		getline(std::cin, userInput);
		paczka.odczytaj(userInput);
		paczka.dodajZnacznikCzasu();
		userInput = paczka.dajPaczke();

		std::cout << userInput << std::endl;
		result = send(newSocket, userInput.c_str(), userInput.size() + 1, 0);

		if (result != SOCKET_ERROR)
		{
			ZeroMemory(buffer, 4096);

			bytesRecived = recv(newSocket, buffer, 4096, 0);
			if (bytesRecived > 0)
			{
				std::cout << "SERWER- " << std::string(buffer, 0, bytesRecived) << std::endl;
			}
		}



	} while (userInput.size() > 0);

	closesocket(newSocket);
	WSACleanup();

}
