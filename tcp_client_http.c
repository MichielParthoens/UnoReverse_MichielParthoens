#ifdef _WIN32
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#include <winsock2.h> //for all socket programming
#include <ws2tcpip.h> //for getaddrinfo, inet_pton, inet_ntop
#include <stdio.h>    //for fprintf, perror
#include <unistd.h>   //for close
#include <stdlib.h>   //for exit
#include <string.h>   //for memset
void httpclient_OSInit(void)
{
    WSADATA wsaData;
    int WSAError = WSAStartup(MAKEWORD(2, 0), &wsaData);
    if (WSAError != 0)
    {
        fprintf(stderr, "WSAStartup errno = %d\n", WSAError);
        exit(-1);
    }
}
void httpclient_OSCleanup(void)
{
    WSACleanup();
}
#define perror(string) fprintf(stderr, string ": WSA errno = %d\n", WSAGetLastError())
#else
#include <sys/socket.h> //for sockaddr, socket, socket
#include <sys/types.h>  //for size_t
#include <netdb.h>      //for getaddrinfo
#include <netinet/in.h> //for sockaddr_in
#include <arpa/inet.h>  //for htons, htonl, inet_pton, inet_ntop
#include <errno.h>      //for errno
#include <stdio.h>      //for fprintf, perror
#include <unistd.h>     //for close
#include <stdlib.h>     //for exit
#include <string.h>     //for memset
void httpclient_OSInit(void) {}
void httpclient_OSCleanup(void) {}
#endif
int httpclient_initialization();
void httpclient_execution(int internet_socket, const char *ip_adress);
void httpclient_cleanup(int internet_socket);

const char *http_api(const char *ip_adress)
{

    //////////////////
    // Initialization//
    //////////////////

    httpclient_OSInit();

    int internet_socket = httpclient_initialization();

    /////////////
    // Execution//
    /////////////

    httpclient_execution(internet_socket, ip_adress);

    ////////////
    // Clean up//
    ////////////

    httpclient_cleanup(internet_socket);

    httpclient_OSCleanup();

    return 0;
}

int httpclient_initialization()
{
    // Step 1.1
    struct addrinfo internet_address_setup;
    struct addrinfo *internet_address_result;
    memset(&internet_address_setup, 0, sizeof internet_address_setup);
    internet_address_setup.ai_family = AF_UNSPEC;
    internet_address_setup.ai_socktype = SOCK_STREAM;
    int getaddrinfo_return = getaddrinfo("208.95.112.1", "80", &internet_address_setup, &internet_address_result);
    if (getaddrinfo_return != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(getaddrinfo_return));
        exit(1);
    }

    int internet_socket = -1;
    struct addrinfo *internet_address_result_iterator = internet_address_result;
    while (internet_address_result_iterator != NULL)
    {
        // Step 1.2
        internet_socket = socket(internet_address_result_iterator->ai_family, internet_address_result_iterator->ai_socktype, internet_address_result_iterator->ai_protocol);

        if (internet_socket == -1)
        {
            perror("socket");
        }
        else
        {
            // Step 1.3
            int connect_return = connect(internet_socket, internet_address_result_iterator->ai_addr, internet_address_result_iterator->ai_addrlen);
            if (connect_return == -1)
            {
                perror("connect");
                close(internet_socket);
            }
            else
            {
                break;
            }
        }
        internet_address_result_iterator = internet_address_result_iterator->ai_next;
    }

    freeaddrinfo(internet_address_result);

    if (internet_socket == -1)
    {
        fprintf(stderr, "socket: no valid socket address found\n");
        exit(2);
    }

    return internet_socket;
}

void httpclient_execution(int internet_socket, const char *ip_adress)
{

    // Step 2.1
    char first[] = "GET /json/";
    char third[] = " HTTP/1.1\r\nHost: ip-api.com\r\nAccept: application/json\r\n\r\n";
    // override van het ip adres voor te testen
    // ip_adress = "84.192.107.138";

    int string_length = strlen(first) + strlen(third) + strlen(ip_adress);
    char http_header[string_length];
    strcat(http_header, first);
    strcat(http_header, ip_adress);
    strcat(http_header, third);
    // werkende test met ip dat als voorbeeld gebruikt wordt op ip-api.com
    //     char first[] = "GET /json/84.192.107.138 HTTP/1.1\r\nHost: ip-api.com\r\nAccept: application/json\r\n\r\n";

    int number_of_bytes_send = 0;

    number_of_bytes_send = send(internet_socket, http_header, strlen(http_header), 0);
    if (number_of_bytes_send == -1)
    {
        perror("send");
    }

    // Step 2.2
    int number_of_bytes_received = 0;
    char buffer[1000];

    number_of_bytes_received = recv(internet_socket, buffer, (sizeof buffer) - 1, 0);

    if (number_of_bytes_received == -1)
    {
        perror("recv");
    }
    else
    {
        buffer[number_of_bytes_received] = '\0';
        printf("Received : %s\n", buffer);
    }
    char resultaat[strlen(buffer)];
    int y = 0;
    int x = 0;
    while (buffer[x] != '{')
    {
        x++;
    }
    for (x; x <= strlen(buffer); x++)
    {
        if (buffer[x] == '{' || buffer[x] == '}')
        {
            continue;
        }
        else if (buffer[x] == '\"')
        {
            while (buffer[x] != '\"')
            {
                resultaat[y] = buffer[x];
                y++;
                x++;
            }
        }
        else if (buffer[x] == ',')
        {
            resultaat[y] = '\n';
            y++;
        }
        else
        {
            resultaat[y] = buffer[x];
            y++;
        }
    }
    FILE *fp;
    fp = fopen("data.log", "a");
    fprintf(fp, "\nJSON RESPONSE: \n%s", resultaat);
    fclose(fp);
}

void httpclient_cleanup(int internet_socket)
{
    // Step 3.2
    int shutdown_return = shutdown(internet_socket, SD_SEND);
    if (shutdown_return == -1)
    {
        perror("shutdown");
    }

    // Step 3.1
    close(internet_socket);
}