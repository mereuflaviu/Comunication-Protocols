#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

/* Computes a GET request. */
char *compute_get_request(char *host, char *url, char *query_params,
                            char **cookies, int cookies_count, char *token)
{
    /* Allocate memory. */
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    /* Write the method name, URL, request params (if any) and protocol type. */
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }
    compute_message(message, line);

    /* Add the host. */
    sprintf(line, "Host: %s", host);
    compute_message(message, line); 

    /* Add cookies, according to the protocol format. */
    if (cookies != NULL) {
       strcpy(line, "Cookie: ");
       for (int i = 0; i < cookies_count - 1; i++) {
            sprintf(line, "%s; ", cookies[i]);
       }
       strcat(line, cookies[cookies_count - 1]);
       compute_message(message, line);
    }

    /* Add authorisation header. */
    if (token != NULL) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    /* Add final new line. */
    compute_message(message, "");

    /* Free memory. */
    free(line);

    /* Return the message. */
    return message;
}

/* Computes a POST request. */
char *compute_post_request(char *host, char *url, char* content_type, char **body_data,
                            int body_data_fields_count, char **cookies, int cookies_count, char *token)
{
    /* Allocate memory. */
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));

    /* Write the method name, URL and protocol type. */
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    /* Add the host. */
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    /* Add necessary headers (Content-Type and Content-Length are mandatory).
     * In order to write Content-Length you must first compute the message size. */
    for(int i = 0; i < body_data_fields_count; i++) {
        if(i != 0) {
            strcat(body_data_buffer, "&");
        }
        strcat(body_data_buffer, body_data[i]);
    }

    sprintf(line, "Content-Length: %lu", strlen(body_data_buffer));
    compute_message(message, line);
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    /* Add cookies. */
    if (cookies != NULL) {
       strcpy(line, "Cookie: ");
       for (int i = 0; i < cookies_count - 1; i++) {
            sprintf(line, "%s; ", cookies[i]);
       }
       strcat(line, cookies[cookies_count - 1]);
       compute_message(message, line);
    }

    /* Add authorisation header. */
    if (token != NULL) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    /* Add final new line. */
    compute_message(message, "");

    /* Add the payload data. */
    compute_message(message, body_data_buffer);

    /* Free memory. */
    free(body_data_buffer);
    free(line);

    /* Return the message. */
    return message;
}

/* Computes a DELETE request. */
char *compute_delete_request(char *host, char *url, char *query_params, char *token)
{
    /* Allocate memory. */
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    /* Write the method name, URL, request params (if any) and protocol type. */
    if (query_params != NULL) {
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }
    compute_message(message, line);

    /* Add the host. */
    sprintf(line, "Host: %s", host);
    compute_message(message, line); 
    
    /* Add authorisation header. */
    if (token != NULL) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    /* Add final new line. */
    compute_message(message, "");

    /* Free memory. */
    free(line);

    /* Return the message. */
    return message;
}
