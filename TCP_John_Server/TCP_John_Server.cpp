///--- TCP SERVER ---///

#include "stdafx.h"
#include <winsock2.h>
#include <locale.h>
#include <string.h>
#include <conio.h>

#pragma comment(lib, "WS2_32.lib")

// SCHEMAT DZIALANIA KLIENTA TCP //
//	1. INICJALIZACJA BIBLIOTEKI WINSOCK										(WSAStartup)
//  2. STWORZENIE 2 GNIAZD, OCZEKUJACEGO NA PO£ACZENIE I DLA KOMUNIKACJI	(socket)
//	3. WYPELNIENIE STRUKTURY ADRESOWEJ DANYMI SERWERA						(sockaddr_in)
//	4. BINDOWANIE GNIAZDA OCZEKUJACEGO										(bind)
//  5. USTAWIENIE GNIAZDA OCZEKUJACEGO NA NAS£UCHIWANIE						(listen)
//  6. OCZEKIWANIE NA POLACZENIE SIE KLIENTA								(accept)
//  7. KOMUNIKACJA Z KLIENTEM												(send, recv)
//	8. ZAKONCZENIE POLACZENIA												(closesocket, shutdown)


int main()
{
	setlocale(LC_ALL, "POLISH");
	WSAData wsadata;

	/// --- 1. INICJALIZACJA WINSOCK --- ///

	if (WSAStartup(MAKEWORD(2, 2), &wsadata))
	{
		printf("B³¹d Inicjalizacji Winsock!: %d", WSAGetLastError());
		_getch();
		return 0;
	}

	/// --- 2. TWORZENIE GNIAZD --- ///
	SOCKET SERV_CZEKAJ = INVALID_SOCKET;
	SOCKET KLIENT_DZIALAJ = INVALID_SOCKET;

	SERV_CZEKAJ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Tworzenie Socketu Servera (czekaj¹ce na po³¹czenie)
	if (SERV_CZEKAJ == INVALID_SOCKET)
	{
		printf("B³ad przy tworzeniu gniazda: %d", WSAGetLastError());
		closesocket(SERV_CZEKAJ);
		_getch();
		return 0;
	}


	/// --- 3. TWORZENIE I WYPELNIANIE STRUKTURY ADRESOWEJ ---///

	struct sockaddr_in Adres_Servera = { 0 };
	struct sockaddr_in Adres_Klienta = { 0 };

	Adres_Servera.sin_addr.s_addr = inet_addr("127.0.0.1"); //ustawianie adresu serwera 
	Adres_Servera.sin_family = AF_INET;
	Adres_Servera.sin_port = htons(5678); //ustawianie portu serwera 


	/// --- 4. BINDOWANIE GNIAZDA ---///

	int bindResult = bind(SERV_CZEKAJ, (sockaddr*)&Adres_Servera, sizeof(Adres_Servera)); // Bindowanie Socketu Servera 
	// (server MUSI byæ zbindowany aby klient wiedzia³ na jaki port sie komunikowaæ)
	if (bindResult == SOCKET_ERROR)
	{
		printf("Bind B³¹d: %d\n", WSAGetLastError());
		closesocket(SERV_CZEKAJ);
		WSACleanup();
		getch();
		return 0;
	}


	/// --- 5. USTAWIENIE GNIAZDA  W TRYB NAS£UCHIWANIA ---///

	int ListenResult = listen(SERV_CZEKAJ, 2); // Ustawianie Socketu na czekanie...
	if (ListenResult == SOCKET_ERROR)
	{
		printf("Bind B³¹d: %d\n", WSAGetLastError());
		closesocket(SERV_CZEKAJ);
		WSACleanup();
		getch();
		return 0;
	}

	
	/// --- 6. OCZEKIWANIE NA KLIENTA ---///

	printf("Czekam Na Dane od Klienta... \n\n");
	int rozmiar = sizeof(Adres_Klienta);

	KLIENT_DZIALAJ = accept(SERV_CZEKAJ, (sockaddr*)&Adres_Klienta, &rozmiar); // Accept to jo se tu poczekom a¿ coœ przyjdzie....
	// Akceptuj Polaczenie i przekaz info na 2 gniazdko (KLIENT_DZIALAJ)
	
	closesocket(SERV_CZEKAJ); // Tego socketu juz nie trza bo  polaczenie zosta³o nawi¹zane i przekazane przez accept na gniazdo "KLIENT_DZIALAJ".

	char Otrzymano[1024] = { 0 };


	/// --- 7a. ODBIERANIE OD KLIENTA --- ///
	
	int Bajty = recv(KLIENT_DZIALAJ, Otrzymano, sizeof(Otrzymano), 0);
	printf("OD KLIENTA:\n", Bajty);
	printf("Liczba Otrzymanych Bajtow: %d\n", Bajty);

	float tab_recv[100] = { 0 };
	int LicznikBajtow = 0;
	for (int i = 0; i < (Bajty / 4); i++)
	{
		tab_recv[i] = *(float*)&Otrzymano[LicznikBajtow];
		printf("%2.0d. Dane: %0.2f\n",i+1, tab_recv[i]);
		LicznikBajtow += 4;
	}
	printf("\n//////////////////////////////////////////////////////////////////////////// \n\n");
	printf("DO KLIENTA:\n");
	printf("\n Przetwarzam dane i odst³am wynik.\n\n");

	/// --- SZUKANIE NAJWIEKSZEJ WARTOŒCI --- ///
	float MaxVal = 0;
	for (int i = 0; i < (Bajty / 4); i++)
	{
		if (MaxVal < tab_recv[i]) MaxVal = tab_recv[i];
	}

	/// --- NORMALIZACJA DANYCH OD KLIENTA --- ///
	for (int i = 0; i < (Bajty / 4); i++)
	{
		tab_recv[i] = tab_recv[i] / MaxVal;
		printf("%2.0d. Znormalizowano: %0.2f\n", i + 1, tab_recv[i]);
	}


	/// --- 7b. WYSY£ANIE DO KLIENTA --- ///

	int iloscdanych = Bajty/4;
	send(KLIENT_DZIALAJ, (char*)&iloscdanych, sizeof(iloscdanych), 0);  // wysyla ilosc pomiarow 

	send(KLIENT_DZIALAJ, (char*)&tab_recv, sizeof(tab_recv), 0);  // Wysy³anie znormalizowane pomiary


	/// --- 8. KOÑCZENIE PROGRAMU ---///

	printf("\nNacisnij Aby Zakoñczyæ.");
	_getch();

	shutdown(KLIENT_DZIALAJ, 2);
	closesocket(KLIENT_DZIALAJ);
	WSACleanup();

	return 0;
} //by Johnny