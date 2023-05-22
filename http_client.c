	#define _WIN32_WINNT _WIN32_WINNT_WIN7
	#include <winsock2.h> //for all socket programming
	#include <ws2tcpip.h> //for getaddrinfo, inet_pton, inet_ntop
	#include <stdio.h> //for fprintf, perror
	#include <unistd.h> //for close
	#include <stdlib.h> //for exit
	#include <string.h> //for memset
	#include <time.h> // voor tijd --> logs enzo
	#include <ws2tcpip.h>

#define MAX_BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    char server_ip[] = "::1"; // IP-adres van de TCP-server
    int server_port = 22; // Poortnummer van de TCP-server

    // Maak een socket voor de client
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creatie mislukt");
        exit(EXIT_FAILURE);
    }

    // Stel de servergegevens in
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);


    // Maak verbinding met de server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Verbindingsfout");
        exit(EXIT_FAILURE);
    }

    // HTTP-verzoek verzenden
    char request[] = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";
    if (send(client_socket, request, strlen(request), 0) == -1) {
        perror("Verzendfout");
        exit(EXIT_FAILURE);
    }

    // Ontvang en toon het serverantwoord
    char buffer[MAX_BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    int bytes_received = 0;
    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        printf("%s", buffer);
        memset(buffer, 0, sizeof(buffer));
    }
    if (bytes_received == -1) {
        perror("Ontvangfout");
        exit(EXIT_FAILURE);
    }

    // Sluit de verbinding
    close(client_socket);

    return 0;
}