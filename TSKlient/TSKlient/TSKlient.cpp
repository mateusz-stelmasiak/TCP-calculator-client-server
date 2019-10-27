#include "pch.h"
#include <iostream>
#include <string>
#include <ws2tcpip.h>
#include "Paczka.h"


#pragma comment(lib, "ws2_32.lib")

int main()
{
	Paczka paczka = Paczka();



	std::string ipAdress = "192.168.43.72";
	int port = 54000;

	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int WSResult = WSAStartup(ver, &data);
	if (WSResult != 0)
	{
		std::cout << "Nie można uruchomic winsocka!" << std::endl;
		return 1;
	}

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		std::cout << "Nie można podłączyć się do socketu!" << std::endl;
		WSACleanup();
		return 2;
	}

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAdress.c_str(), &hint.sin_addr);

	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		std::cout << "Nie mozna polaczyc się z serwerem! " << std::endl;
		closesocket(sock);
		WSACleanup();
		return 3;
	}

	char buffer[4096];
	std::string userInput;
	int sendResult;
	int bytesRecived;
	do
	{
		std::cout << "> ";
		getline(std::cin, userInput);
		paczka.odczytaj(userInput);
		paczka.dodajZnacznikCzasu();
		userInput = paczka.dajPaczke();

		std::cout << userInput << std::endl;
		sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);

		if (sendResult != SOCKET_ERROR)
		{
			ZeroMemory(buffer, 4096);

			bytesRecived = recv(sock, buffer, 4096, 0);
			if (bytesRecived > 0)
			{
				std::cout << "SERVER> " << std::string(buffer, 0, bytesRecived) << std::endl;
			}
		}



	} while (userInput.size() > 0);

	closesocket(sock);
	WSACleanup();

}
