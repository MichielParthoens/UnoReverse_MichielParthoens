Project: Uno Reverse Michiel Parthoens

Files:

-tcp_server.c     --> code bestand voor de tcp server
-tcp_server.exe   --> executable voor de tcp server
tcp_client_http.C --> http client voor de requist naar IP-API
tcp_client_http.h --> header voor de http client
client.c          --> code bestand voor de aanvaller te simuleren (gaat ook met packetsender)
client.exe        --> executable voor de client (aanvaller)
data.log          --> logbestand voor de server


TCP SERVER OPSTARTEN:

-open cmd
-ga naar de path waar de bestanden staan
-setlocal
-gcc tcp_server.c tcp_client_http.c -l ws2_32 -o tcp_server.exe
-tcp_server

start dan de client op of stuur via packetsender op poort 22

--------------------------------------------------------------------------------------------------------


