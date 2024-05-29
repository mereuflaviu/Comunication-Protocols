#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>  /* inet_addr */
#include <ctype.h>      /* isdigit */
#include "helpers.h"
#include "requests.h"
#include "parson.h"

#define HOST "34.246.184.49"
#define PORT 8080
#define JSON "application/json"
#define REGISTER "/api/v1/tema/auth/register"
#define LOGIN "/api/v1/tema/auth/login"
#define ACCESS "/api/v1/tema/library/access"
#define BOOKS "/api/v1/tema/library/books"
#define LOGOUT "/api/v1/tema/auth/logout"

#define LINELEN 1000
#define BAD_REQUEST "HTTP/1.1 400 Bad Request"
#define GOOD_REQUEST "HTTP/1.1 200 OK"
#define UNAUTHORIZED "HTTP/1.1 401 Unauthorized"
#define FORBIDDEN "HTTP/1.1 403 Forbidden"
#define SUCCESSFUL_REGISTRATION "HTTP/1.1 200 OK - Utilizator înregistrat cu succes!"

char* remove_newline(char* buffer) {
    size_t len = strlen(buffer);
    if (len > 0) {
        if (buffer[len - 1] == '\n' || buffer[len - 1] == '\r') {
            buffer[len - 1] = '\0';
            len--; // Update length after removing the character
        }
        if (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
            buffer[len - 1] = '\0';
        }
    }
    return buffer;
}

char **allocate_data(int fields_count) {
    char **data = (char **)malloc(fields_count * sizeof(char *));
    if (data == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < fields_count; i++) {
        data[i] = (char *)malloc(256);
        if (data[i] == NULL) {
            perror("malloc failed");
            exit(EXIT_FAILURE);
        }
        memset(data[i], 0, 256); // Optional: Initialize to 0
    }
    return data;
}

void register_user(int sockfd, char **data, int fields_count, char **cookie) {
    char username[40];
    char password[40];
    char *message = NULL;
    char *response = NULL;

    if (*cookie) {
        printf("You are logged in! Please logout and re-enter the register command.\n");
        return;
    }

    // Prompt for username
    printf("username=");
    fgets(username, 40, stdin);
    remove_newline(username);

    // Validate username
    if (strchr(username, ' ') != NULL || strlen(username) == 0) {
        printf("Username cannot contain spaces or must not be empty! Re-enter the register command and try again.\n");
        return;
    }

    // Prompt for password
    printf("password=");
    fgets(password, 40, stdin);
    remove_newline(password);

    // Validate password
    if (strchr(password, ' ') != NULL || strlen(password) == 0) {
        printf("Password cannot contain spaces or must not be empty! Re-enter the register command and try again.\n");
        return;
    }

    // Prepare JSON data for the request
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    json_object_set_string(root_object, "username", username);
    json_object_set_string(root_object, "password", password);
    char* serialized_string = json_serialize_to_string_pretty(root_value);

    memset(data[0], 0, 256);
    strcpy(data[0], serialized_string);
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);

    // Send the request
    message = compute_post_request(HOST, REGISTER, JSON, data, fields_count, NULL, 0, NULL);
    send_to_server(sockfd, message);
    free(message);

    // Receive and process the response
    response = receive_from_server(sockfd);
    char *server_response = strstr(response, "is taken!");
    if (server_response) {
        printf("The username is taken! Re-enter the register command and try a different one.\n");
        free(response);
        return;
    }

    server_response = strstr(response, "429 Too Many Requests");
    if (server_response) {
        printf("Server is busy! Try again later.\n");
        free(response);
        return;
    }

    printf("Your account has been successfully registered!\n");
    free(response);
}

char* login_user(int sockfd, char **data, int fields_count, char *cookie) {
    char username[40];
    char password[40];
    char *message = NULL;
    char *response = NULL;
    char *server_response;

    if (cookie) {
        printf("You are already logged in! Please logout and re-enter the login command.\n");
        return cookie;
    }

    // Prompt for username
    printf("username=");
    fgets(username, 40, stdin);
    remove_newline(username);
    // Validate username
    if (strchr(username, ' ') != NULL || strlen(username) == 0) {
        printf("Username cannot contain spaces or must not be empty! Re-enter the login command and try again.\n");
        return NULL;
    }

    // Prompt for password
    printf("password=");
    fgets(password, 40, stdin);
    remove_newline(password);
    // Validate password
    if (strchr(password, ' ') != NULL || strlen(password) == 0) {
        printf("Password cannot contain spaces or must not be empty! Re-enter the login command and try again.\n");
        return NULL;
    }

    // Prepare JSON data for the request
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *serialized_string = NULL;
    json_object_set_string(root_object, "username", username);
    json_object_set_string(root_object, "password", password);
    serialized_string = json_serialize_to_string_pretty(root_value);

    memset(data[0], 0, 256);
    strcpy(data[0], serialized_string);
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);

    // Send the request
    message = compute_post_request(HOST, LOGIN, JSON, data, fields_count, NULL, 0, NULL);
    send_to_server(sockfd, message);
    free(message);

    // Receive and process the response
    response = receive_from_server(sockfd);
    puts(response);
    server_response = strstr(response, "No account with this username!");
    if (server_response) {
        printf("No account with this username! Re-enter the login command and try again.\n");
        free(response);
        return NULL;
    }
    server_response = strstr(response, "Credentials are not good!");
    if (server_response) {
        printf("Wrong password! Re-enter the login command and try again.\n");
        free(response);
        return NULL;
    }

    // Extract cookie from the response
    char *start = strstr(response, "Set-Cookie: ");
    char *end = strstr(start, ";");
    int start_len = strlen(start);
    int end_len = strlen(end);
    cookie = (char *)malloc(256);
    memcpy(cookie, start + strlen("Set-Cookie: "), start_len - end_len - strlen("Set-Cookie: "));
    cookie[start_len - end_len - strlen("Set-Cookie: ")] = '\0';
    printf("Successfully logged in.\n");

    free(response);
    return cookie;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* enter_library(int sockfd, char *cookie, char **data, int fields_count, char **library_access) {
    if (!cookie) {
        printf("You are not logged in! Please authenticate before requesting access.\n");
        return NULL;
    }
    if (*library_access) {
        printf("You already have access to the library.\n");
        return *library_access;
    }

    close_connection(sockfd);
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    char *message, *response;
    memset(data[0], 0, 256);
    strcpy(data[0], cookie);
    message = compute_get_request(HOST, ACCESS, NULL, data, fields_count, NULL);
    send_to_server(sockfd, message);
    free(message);

    response = receive_from_server(sockfd);
    if (response == NULL) {
        perror("Failed to receive response");
        return NULL;
    }

    char *start = strstr(response, "{\"token\":\"");
    if (start != NULL) {
        start += strlen("{\"token\":\"");
        char *end = strstr(start, "\"}");
        if (end != NULL) {
            size_t token_length = end - start;
            *library_access = (char *)malloc(token_length + 1);
            if (*library_access == NULL) {
                perror("malloc failed");
                free(response);
                return NULL;
            }
            strncpy(*library_access, start, token_length);
            (*library_access)[token_length] = '\0';
        } else {
            printf("Failed to parse token from response\n");
            free(response);
            return NULL;
        }
    } else {
        printf("Failed to parse token from response\n");
        free(response);
        return NULL;
    }
    free(response);
    printf("You now have access to the library.\n");
    return *library_access;
}


void get_books(int sockfd, char *message, char *response, char *cookie, char *library_access, char **data, int fields_count) {
    if (!cookie) {
        printf("You are not logged in! Please authenticate before requesting books information.\n");
        return;
    }
    if (!library_access) {
        printf("You don't have access to the library! Please request access and try again.\n");
        return;
    }
    
    close_connection(sockfd);
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    message = compute_get_request(HOST, BOOKS, NULL, NULL, 0, library_access);
    send_to_server(sockfd, message);
    free(message);

    response = receive_from_server(sockfd);
    puts(response);

    // Check for valid JSON response
    if (strstr(response, "[") != NULL) {
        JSON_Value *root_value = json_parse_string(response);
        if (json_value_get_type(root_value) == JSONArray) {
            char *serialized_books = json_serialize_to_string_pretty(root_value);
            printf("%s\n", serialized_books);
            json_free_serialized_string(serialized_books);
        } else {
            printf("Failed to parse books information.\n");
        }
        json_value_free(root_value);
    } else {
        printf("You don't have any books in the library.\n");
    }

    free(response);
}

void get_book(int sockfd, char *message, char *response, char *url, char *library_access, char *cookie, char *input) {
    if (!cookie) {
        printf("You are not logged in! Please authenticate before requesting information about a book.\n");
        return;
    }
    if (!library_access) {
        printf("You don't have access to the library! Please request access and try again.\n");
        return;
    }
    printf("id=");
    fgets(input, 256, stdin);
    remove_newline(input);
    size_t length = strlen(input);
    // Check if input is empty or contains non-digit characters
    int valid = 1;
    if (length == 0) {
        valid = 0;
    }
    for (size_t i = 0; i < length; i++) {
        if (!isdigit(input[i])) {
            valid = 0;
            break;
        }
    }
    if (!valid) {
        printf("Input id cannot contain letters or be empty! Re-enter the get_book command and try again.\n");
        return;
    }
    int id = atoi(input);
    sprintf(url, "/api/v1/tema/library/books/%d", id);
    close_connection(sockfd);
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    char *server_response;
    message = compute_get_request(HOST, url, NULL, NULL, 0, library_access);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    server_response = strstr(response, "error");
    if (!server_response) {
        char *book = NULL, *serialized_book = NULL;
        book = strstr(response, "{\"");
        if (book) {
            JSON_Value *root_value = json_parse_string(book);
            serialized_book = json_serialize_to_string_pretty(root_value);
            printf("%s\n", serialized_book);
            json_free_serialized_string(serialized_book);
            json_value_free(root_value);
        } else {
            printf("Error parsing book information.\n");
        }
    } else {
        printf("Error! There is no book with the given id. Try again.\n");
    }
    free(message);
    free(response);
}


void add_book(int sockfd, char *message, char *response, char **data, int fields_count, char *library_access, char *cookie) {
    if (!cookie) {
        printf("You are not logged in! Please authenticate before trying to add a book.\n");
        return;
    }
    if (!library_access) {
        printf("You don't have access to the library! Please request access and try again.\n");
        return;
    }

    close_connection(sockfd);
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *serialized_string = NULL;
    
    char input[256];

    printf("title=");
    fgets(input, 256, stdin);
    remove_newline(input);
    json_object_set_string(root_object, "title", input);

    printf("author=");
    fgets(input, 256, stdin);
    remove_newline(input);
    json_object_set_string(root_object, "author", input);

    printf("genre=");
    fgets(input, 256, stdin);
    remove_newline(input);
    json_object_set_string(root_object, "genre", input);

    printf("publisher=");
    fgets(input, 256, stdin);
    remove_newline(input);
    json_object_set_string(root_object, "publisher", input);

    int page_count = 0;
    while (1) {
        printf("page_count=");
        fgets(input, 256, stdin);
        remove_newline(input);
        // Check if input is empty or contains non-digit characters
        int valid = 1;
        size_t length = strlen(input);
        if (length == 0) {
            valid = 0;
        }
        for (size_t i = 0; i < length; i++) {
            if (!isdigit(input[i])) {
                valid = 0;
                break;
            }
        }
        if (!valid) {
            printf("Input page_count cannot contain letters or be empty! Re-enter the input.\n");
            continue;
        }
        page_count = atoi(input);
        if (page_count == 0) {
            printf("Page_count cannot be zero!\n");
            continue;
        }
        break;
    }
    json_object_set_string(root_object, "page_count", input);

    serialized_string = json_serialize_to_string_pretty(root_value);

    memset(data[0], 0, 256);
    strcpy(data[0], serialized_string);
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);

    char *server_response;
    message = compute_post_request(HOST, BOOKS, JSON, data, fields_count, NULL, 0, library_access);

    send_to_server(sockfd, message);
    free(message);

    response = receive_from_server(sockfd);
    server_response = strstr(response, "200 OK");
    if (!server_response) {
        printf("Error! Try again.\n");
    } else {
        printf("Book added successfully!\n");
    }
    free(response);
}

void delete_book(int sockfd, char *message, char *response, char *url, char *library_access, char *cookie, char *input) {
    if (!cookie) {
        printf("You are not logged in! Please authenticate before trying to delete a book.\n");
        return;
    }
    if (!library_access) {
        printf("You don't have access to the library! Please request access and try again.\n");
        return;
    }

    printf("id=");
    fgets(input, 256, stdin);
    remove_newline(input);
    size_t length = strlen(input);
    // Check if input is empty or contains non-digit characters
    int valid = 1;
    if (length == 0) {
        valid = 0;
    }
    for (size_t i = 0; i < length; i++) {
        if (!isdigit(input[i])) {
            valid = 0;
            break;
        }
    }
    if (!valid) {
        printf("Input id cannot contain letters or be empty! Re-enter the delete_book command and try again.\n");
        return;
    }

    int id = atoi(input);
    sprintf(url, "/api/v1/tema/library/books/%d", id);

    close_connection(sockfd);
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    char *server_response;

    message = compute_delete_request(HOST, url, NULL, library_access);
    send_to_server(sockfd, message);

    response = receive_from_server(sockfd);
    server_response = strstr(response, "error");

    if (!server_response) {
        printf("Book deleted successfully.\n");
    } else {
        printf("Error! There is no book with the given id. Try again.\n");
    }

    free(message);
    free(response);
}

void logout(int sockfd, char *message, char *response, char **data, int fields_count, char **cookie, char **library_access) {
    if (!*cookie) {
        printf("You are not logged in!\n");
        return;
    }

    close_connection(sockfd);
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    memset(data[0], 0, 256);
    strcpy(data[0], *cookie);

    char *server_response;

    message = compute_get_request(HOST, "/api/v1/tema/auth/logout", NULL, data, fields_count, NULL);
    send_to_server(sockfd, message);

    response = receive_from_server(sockfd);
    server_response = strstr(response, "error");

    if (!server_response) {
        printf("Logout successful.\n");
    } else {
        printf("Error! Try again.\n");
    }

    free(message);
    free(response);
    free(*cookie);
    *cookie = NULL;
    free(*library_access);
    *library_access = NULL;
}

int main() {
    char command[256], *cookie = NULL, *library_access = NULL;
    char url[40];
    char *message = NULL, *response = NULL;
    char input[256];
    int sockfd, fields_count = 1;

    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error opening socket");
        exit(EXIT_FAILURE);
    }

    char **data = allocate_data(fields_count);

    while (1) {
        printf("Enter command: ");
        fgets(command, 256, stdin);
        remove_newline(command);
        
        if (strcmp(command, "register") == 0) {
            register_user(sockfd, data, fields_count, &cookie);
        } else if (strcmp(command, "login") == 0) {
            close_connection(sockfd);
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            cookie = login_user(sockfd, data, fields_count, cookie);
        } else if (strcmp(command, "enter_library") == 0) {
            library_access = enter_library(sockfd, cookie, data, fields_count, &library_access);
        } else if (strcmp(command, "get_books") == 0) {
            get_books(sockfd, message, response, cookie, library_access, data, fields_count);
        } else if (strcmp(command, "get_book") == 0) {
            get_book(sockfd, message, response, url, library_access, cookie, input);
        } else if (strcmp(command, "add_book") == 0) {
            add_book(sockfd, message, response, data, fields_count, library_access, cookie);
        } else if (strcmp(command, "delete_book") == 0) {
            delete_book(sockfd, message, response, url, library_access, cookie, input);
        } else if (strcmp(command, "logout") == 0) {
            logout(sockfd, message, response, data, fields_count, &cookie, &library_access);
        } else if (strcmp(command, "exit") == 0) {
            break;
        } else {
            printf("Invalid command. Please try again.\n");
        }
    }

    // Cleanup
    free(message);
    free(response);
    free(cookie);
    free(library_access);
    for (int i = 0; i < fields_count; i++) {
        free(data[i]);
    }
    free(data);
    close_connection(sockfd);

    return 0;
}
