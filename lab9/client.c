#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

int main(int argc, char *argv[])
{
    char *message;
    char *response;
    int sockfd;

    // Ex 1.1: GET dummy from main server
    sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request("34.254.242.81:8080", "/api/v1/dummy", NULL, NULL, 0);
    puts(message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    puts(response);
    //close_connection(sockfd);
    // Ex 1.2: POST dummy and print response from main server
    //sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
    char *params[] = {"are=mere", "nume=ana"};
    message = compute_post_request("34.254.242.81:8080", "/api/v1/dummy", "", params, 2, NULL, 0);
    puts(message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    puts(response);
    close_connection(sockfd);
    // Ex 2: Login into main server
    // Ex 3: GET weather key from main server
    // Ex 4: GET weather data from OpenWeather API
    // Ex 5: POST weather data for verification to main server
    // Ex 6: Logout from main server

    // BONUS: make the main server return "Already logged in!"

    // free the allocated data at the end!

    return 0;
}
