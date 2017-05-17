///--- TCP SERVER ---///

#include "stdafx.h"
#include <winsock2.h>
#include <locale.h>
#include <string.h>
#include <conio.h>

#pragma comment(lib, "WS2_32.lib")


int main()
{
	setlocale(LC_ALL, "POLISH");

	WSAData wsadata;

	struct sockaddr_in Adres_Servera = { 0 };
	struct sockaddr_in Adres_Klienta = { 0 };

	Adres_Servera.sin_addr.s_addr = inet_addr("127.0.0.1");
	Adres_Servera.sin_family = AF_INET;
	Adres_Servera.sin_port = htons(5678);


	if (WSAStartup(MAKEWORD(2, 2), &wsadata))
	{
		printf("B³¹d Inicjalizacji Winsock!: %d", WSAGetLastError());
		_getch();
		return 0;
	}

	// SOCKET //
	SOCKET SERV_CZEKAJ = INVALID_SOCKET;
	SOCKET KLIENT_DZIALAJ = INVALID_SOCKET;

	SERV_CZEKAJ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Tworzenie Socketu Servera

	bind(SERV_CZEKAJ, (sockaddr*)&Adres_Servera, sizeof(Adres_Servera)); // Bindowanie Socketu Servera (server MUSI bindowac)

	listen(SERV_CZEKAJ, 2); // Ustawianie Socketu na czekanie...

	int rozmiar = sizeof(Adres_Klienta);

	printf("Czekam Na Dane... \n\n");
	KLIENT_DZIALAJ = accept(SERV_CZEKAJ, (sockaddr*)&Adres_Klienta, &rozmiar); // Accept to jo se tu poczekom a¿ coœ przyjdzie....
	// Akceptuj Polaczenie i przekaz info na 2 gniazdko (KLIENT_DZIALAJ)
	
	closesocket(SERV_CZEKAJ); // Tego socketu juz nie trza bo jest polaczenie

	char Otrzymano[512];
	int dlugoscOtrzymanego = 512;

	int Bajty = recv(KLIENT_DZIALAJ, Otrzymano, dlugoscOtrzymanego, 0);
	printf("Liczba Bajtow: %d\n", Bajty);

	Otrzymano[Bajty] = '\0';
	printf("Otrzymano: %s\n\n\n", Otrzymano);

	printf("Nacisnij Aby Zakoñczyæ.");
	_getch();

	closesocket(KLIENT_DZIALAJ);
	WSACleanup();

	return 0;
} //by Johnny