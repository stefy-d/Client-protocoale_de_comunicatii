#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>

#include "nlohmann/json.hpp"
using json = nlohmann::json;
using namespace std;

#define PORT 8080
#define BUFF_SIZE 4096

// functie pentru a deschide conexiunea
int open_connection(const char *host, int port, int iptype, int socktype, int flag) {
    // creez un socket
    struct sockaddr_in server;
    int sock = socket(iptype, socktype, flag);
    if (sock < 0) {
        perror("ERROR opening socket");
        exit(1);
    }
    // setez datele serverului
    memset(&server, 0, sizeof(server));
    server.sin_family = iptype;
    server.sin_port = htons(port);
    inet_aton(host, &server.sin_addr);
    // incerc sa ma conectez la server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }
    // returnez socketul
    return sock;
}

// functie pentru a inchide conexiunea
void close_connection(int socket) {
    close(socket);
}

// functie pentru a trimite requestul la server
void send_to_server(int socket, char *message) {
    int bytes, sent = 0;
    int total = strlen(message);
    // trimit tot mesajul
    do {
        bytes = write(socket, message + sent, total - sent);
        if (bytes < 0) {
            perror("ERROR writing message");
            exit(1);
        }
        if (bytes == 0) {
            break;
        }
        sent = sent + bytes;
    } while (sent < total);
}

// functie de cautare a unui substring
int find_substring(const string& buffer, const char* str) {
    size_t poz = buffer.find(str);
    if (poz != string::npos) {
        return (int)poz;
    }
    return -1;
}

// functie pentru a primi raspunsul de la server
char *receive_from_server(int sockfd) {
    char response[BUFF_SIZE];   // raspunsul serverului
    string buffer;  // string pentru a construi raspunsul complet
    int header_end = 0;     // pozitia de sfarsit a header-ului
    int content_length = 0;     // lungimea continutului
    // citesc datele de la server
    do {
        int bytes = read(sockfd, response, BUFF_SIZE);
        if (bytes < 0) {
            perror("ERRORS reading response from socket");
            exit(1);
        }

        if (bytes == 0) {
            break;
        }
        buffer.append(response, bytes);     // adaug datele citite la buffer
        // gasesc sfarsitul header-ului
        header_end = find_substring(buffer, "\r\n\r\n");
        if (header_end >= 0) {
            header_end += 4;    // modific pozitia sfarsitului header-ului
            // gasesc lungimea continului
            int content_length_start = find_substring(buffer, "Content-Length: ");
            if (content_length_start >= 0) {
                content_length_start += 16;     // modific pozitia de inceput a lungimii continutului
                content_length = strtol(buffer.c_str() + content_length_start, NULL, 10);
                break;
            }
        }
    } while (1);

    size_t total = content_length + header_end;     // totalul datelor care trebuie citite

    // citesc restul continutului de la server
    while (buffer.size() < total) {
        int bytes = read(sockfd, response, BUFF_SIZE);

        if (bytes < 0) {
            perror("ERROR reading response from socket");
            exit(1);
        }

        if (bytes == 0) {
            break;
        }

        buffer.append(response, bytes);     // adaug datele citite la buffer
  }
  
  return strdup(buffer.c_str());
}

// functie pentru a crea requestul la comanda de login si register
char *register_and_login(const char* host, const char* url, const char* content_type, const json& content) {
    string message;
    message = message + "POST " + string(url) + " HTTP/1.1\r\n";    // tipul request-ului
    message = message + "Host: " + string(host) + "\r\n";   // host-ul
    message = message + "Content-Type: " + string(content_type) + "\r\n";   // tipul continutului
    message = message + "Content-Length: " + to_string(content.dump().length()) + "\r\n";   // lungimea continutului
    message = message + "\r\n";     // linie goala    
    message = message + content.dump();     // continutul

    return strdup(message.c_str());
}

// functie pentru a crea requestul la comanda de entry-library
char *enter_library(const char* host, const char* url, const char* cookies) {
    string message;
    message = message + "GET " + string(url) + " HTTP/1.1\r\n";    // tipul request-ului
    message = message + "Host: " + string(host) + "\r\n";   // host-ul
    // adauga cookies-ul in request
    message = message + "Cookie: " + string(cookies) + "\r\n";
    message = message + "\r\n";     // linie goala

    return strdup(message.c_str());
}

//functie pentru a crea requestul la comanda de get_books
char *get_books(const char* host, const char* url, const char* token) {
    string message;
    message = message + "GET " + string(url) + " HTTP/1.1\r\n";    // tipul request-ului
    message = message + "Host: " + string(host) + "\r\n";   // host-ul
    message = message + "Authorization: Bearer " + string(token) + "\r\n";   // token-ul
    message = message + "\r\n";     // linie goala

    return strdup(message.c_str());
}

// functie pentru a crea requestul la comanda de get_book
char *get_book(const char* host, const char* url, const char* token) {
    string message;
    message = message + "GET " + string(url) + " HTTP/1.1\r\n";    // tipul request-ului
    message = message + "Host: " + string(host) + "\r\n";   // host-ul
    message = message + "Authorization: Bearer " + string(token) + "\r\n";   // token-ul
    message = message + "\r\n";     // linie goala

    return strdup(message.c_str());
}

// functie pentru a crea requestul la comanda de add_book
char *add_book(const char* host, const char* url, const char* content_type, const json& content, const char* token) {
    string message;
    message = message + "POST " + string(url) + " HTTP/1.1\r\n";    // tipul request-ului
    message = message + "Host: " + string(host) + "\r\n";   // host-ul
    message = message + "Content-Type: " + string(content_type) + "\r\n";   // tipul continutului
    message = message + "Content-Length: " + to_string(content.dump().length()) + "\r\n";   // lungimea continutului
    message = message + "Authorization: Bearer " + string(token) + "\r\n";   // token-ul
    message = message + "\r\n";     // linie goala    
    message = message + content.dump();     // continutul

    return strdup(message.c_str());
}

// functie pentru a crea requestul la comanda de delete_book
char *delete_book(const char* host, const char* url, const char* token) {
    string message;
    message = message + "DELETE " + string(url) + " HTTP/1.1\r\n";    // tipul request-ului
    message = message + "Host: " + string(host) + "\r\n";   // host-ul
    message = message + "Authorization: Bearer " + string(token) + "\r\n";   // token-ul
    message = message + "\r\n";     // linie goala

    return strdup(message.c_str());
}

// functie pentru a crea requestul la comanda de logout
char *logout(const char* host, const char* url, const char* cookies) {
    string message;
    message = message + "GET " + string(url) + " HTTP/1.1\r\n";    // tipul request-ului
    message = message + "Host: " + string(host) + "\r\n";   // host-ul
    // adauga cookies-ul in request
    message = message + "Cookie: " + string(cookies) + "\r\n";
    message = message + "\r\n";     // linie goala

    return strdup(message.c_str());
}

// functie pentru a verifica daca page_count este un numar
int valid_page_count(const char* page_count) {
    for (size_t i = 0; i < strlen(page_count); i++) {
        if (page_count[i] < '0' || page_count[i] > '9') {
            return 0;
        }
    }
    return 1;
}

int main() {
    int socket;
    char cmd[25];
    const char *HOST = "34.246.184.49";
    string cookies = "";
    string token = "";

    // bucla infinita pentru a citi comenzi
    int stop = 0;
    
    while (stop == 0) {
        fgets(cmd, 25, stdin);  // citesc comanda

        // comanda de exit
        if(strncmp(cmd, "exit", 4) == 0) {
            stop = 1;
            continue;
        }

        // comanda de register
         if (strncmp(cmd, "register", 8) == 0) {           
            // citesc username-ul si parola
            char username[100], password[100];
            cout << "username = ";
            fgets(username, 100, stdin);
            cout << "password = ";
            fgets(password, 100, stdin);
            // verific daca credentialele contin spatiu
            if (strchr(username, ' ') != NULL || strchr(password, ' ') != NULL) {
                cout << "Error! Credentials must not contain space!\n";
                continue;
            } 
            // construiesc json-ul cu datele user-ului
            json user;
            user["username"] = username;
            user["password"] = password;
            // construiesc request-ul de register
            const char *ruta = "/api/v1/tema/auth/register";
            const char *type = "application/json";
            char *request = register_and_login(HOST, ruta, type, user);
            // deschid conexiunea
            socket = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            // trimit request-ul serverului
            send_to_server(socket, request);
            // primesc raspunsul de la server
            char *response = receive_from_server(socket);
            // afisez mesajul de raspuns
            if (strstr(response, "error") != NULL) {
                cout << "Error! User already exists!\n";
            } else {
                cout << "Success! User created!\n";
            }
            // inchid conexiunea
            close_connection(socket); 
            continue;      
        }

        // comanda de login
        if (strncmp(cmd, "login", 5) == 0) {
            // verific daca sunt deja logat
            if (cookies != "") {
                cout << "You are already logged in!\n";
                continue;
            } else {
                // citesc username-ul si parola
                char username[100], password[100];
                cout << "username = ";
                fgets(username, 100, stdin);
                cout << "password = ";
                fgets(password, 100, stdin);
                // verific daca credentialele contin spatiu
                if (strchr(username, ' ') != NULL || strchr(password, ' ') != NULL) {
                    cout << "Error! Credentials must not contain space!\n";
                    continue;
                } 
                // construiesc json-ul cu datele user-ului
                json user;
                user["username"] = username;
                user["password"] = password;
                // construiesc request-ul de login
                const char *ruta = "/api/v1/tema/auth/login";
                const char *type = "application/json";
                char *request = register_and_login(HOST, ruta, type, user);
                // deschid conexiunea
                socket = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
                // trimit request-ul serverului
                send_to_server(socket, request);
                // primesc raspunsul de la server
                char *response = receive_from_server(socket);
                // afisez mesajul de raspuns
                if (strstr(response, "error") != NULL) {
                    cout << "Error! Invalid username or password!\n";
                } else {
                    cout << "Success! You are logged in!\n";
                    // extrag cookies din raspuns
                    string response_str = string(response);
                    int start = response_str.find("Set-Cookie: ") + 12;
                    int end = response_str.find("Date");
                    cookies = response_str.substr(start, end - start);
                }
                // inchid conexiunea
                close_connection(socket);
                continue;
            }
        }

        // comanda de enter_library
        if (strncmp(cmd, "enter_library", 13) == 0) {
            // verific daca sunt logat
            if (cookies == "") {
                cout << "Error! You are not logged in!\n";
                continue;
            } else {
                // construiesc request-ul de enter_library
                const char *ruta = "/api/v1/tema/library/access";
                char *request = enter_library(HOST, ruta, cookies.c_str());
                // deschid conexiunea
                socket = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
                // trimit request-ul serverului
                send_to_server(socket, request);
                // primesc raspunsul de la server
                char *response = receive_from_server(socket);
                // afisez mesajul de raspuns
                if (strstr(response, "error") != NULL) {
                    cout << "Error! You don't have access to the library!\n";
                } else {
                    cout << "Success! You have access to the library!\n";
                    // extrag token-ul din raspuns
                    string response_str = string(response);
                    int start = response_str.find("{") + 10;
                    int end = response_str.find("}");
                    token = response_str.substr(start, end - start - 1);
                }
                // inchid conexiunea
                close_connection(socket);
                continue;
            } 
        }

        // comanda de get_books
        if (strncmp(cmd, "get_books", 9) == 0) {
            // verific daca am acces la biblioteca
            if (token == "") {
                cout << "Error! You don't have access to the library!\n";
                continue;
            } else {
                // construiesc request-ul de get_books
                const char *ruta = "/api/v1/tema/library/books";
                char *request = get_books(HOST, ruta, token.c_str());
                // deschid conexiunea
                socket = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
                // trimit request-ul serverului
                send_to_server(socket, request);
                // primesc raspunsul de la server
                char *response = receive_from_server(socket);
                // afisez mesajul de raspuns
                if (strstr(response, "error") != NULL) {
                    cout << "Error! There is no response from the server!\n";
                } else {
                    // afisez cartile
                    string response_str = string(response);
                    int start = response_str.find("[");
                    int end = response_str.find("]") + 1;
                    string books = response_str.substr(start, end - start);
                    cout << books << endl;                    
                }
                // inchid conexiunea
                close_connection(socket);
                continue;     
            }
        }
        
        // comanda de get_book
        if (strncmp(cmd, "get_book", 8) == 0) {
            // verific daca am acces la biblioteca
            if (token == "") {
                cout << "Error! You don't have access to the library!\n";
                continue;
            } else {
                // citesc id-ul cartii
                string id;
                cout << "id = ";
                std::getline(std::cin, id, '\n');
                // construiesc ruta
                string ruta = "/api/v1/tema/library/books/" + id;
                // construiesc request-ul de get_book
                char *request = get_book(HOST, ruta.c_str(), token.c_str());
                // deschid conexiunea
                socket = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
                // trimit request-ul serverului
                send_to_server(socket, request);
                // primesc raspunsul de la server
                char *response = receive_from_server(socket);
                // afisez mesajul de raspuns
                if (strstr(response, "error") != NULL) {
                    cout << "Error! This id is not valid!\n";
                } else {
                    // afisez cartea
                    cout << "Success! Here is the book you are looking for!\n";
                    string response_str = string(response);
                    int start = response_str.find("{");
                    int end = response_str.find("}") + 1;
                    string book = response_str.substr(start, end - start);
                    cout << book << endl;                    
                }
                // inchid conexiunea
                close_connection(socket);
                continue;
            }
        }

        // comanda de add_book
        if (strncmp(cmd, "add_book", 8) == 0) {
            // verific daca am acces la biblioteca
            if (token == "") {
                cout << "Error! You don't have access to the library!\n";
                continue;
            } else {
                // citesc datele cartii
                char title[100], author[100], genre[100], publisher[100], page_count[100];
                cout << "title = ";
                fgets(title, 100, stdin);
                title[strlen(title) - 1] = '\0';
                cout << "author = ";
                fgets(author, 100, stdin);
                author[strlen(author) - 1] = '\0';
                cout << "genre = ";
                fgets(genre, 100, stdin);
                genre[strlen(genre) - 1] = '\0';  
                cout << "publisher = ";
                fgets(publisher, 100, stdin);
                publisher[strlen(publisher) - 1] = '\0';             
                cout << "page_count = ";
                fgets(page_count, 100, stdin);
                page_count[strlen(page_count) - 1] = '\0';
                // verific daca page_count este un numar
                if (valid_page_count(page_count) == 0) {
                    cout << "Error! Page count must be a number!\n";
                    continue;
                }
                // construiesc json-ul cu datele cartii
                json book;
                book["title"] = title;
                book["author"] = author;
                book["genre"] = genre;
                book["publisher"] = publisher;
                book["page_count"] = page_count;               
                // construiesc request-ul de add_book
                const char *ruta = "/api/v1/tema/library/books";
                const char *type = "application/json";
                char *request = add_book(HOST, ruta, type, book, token.c_str());
                // deschid conexiunea
                socket = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
                // trimit request-ul serverului
                send_to_server(socket, request);
                // primesc raspunsul de la server
                char *response = receive_from_server(socket);
                // afisez mesajul de raspuns
                if (strstr(response, "error") != NULL) {
                    cout << "Error! The book was not added!\n";
                } else {
                    cout << "Success! Book added!\n";
                }
                // inchid conexiunea
                close_connection(socket);
                continue;        
            }
        }

        // comanda de delete_book
        if (strncmp(cmd, "delete_book", 11) == 0) {
            // verific daca am acces la biblioteca
            if (token == "") {
                cout << "Error! This id is not valid!\n";
                continue;
            } else {
                // citesc id-ul cartii
                string id;
                cout << "id = ";
                std::getline(std::cin, id, '\n');
                // construiesc ruta
                string ruta = "/api/v1/tema/library/books/" + id;
                // construiesc request-ul de delete_book
                char *request = delete_book(HOST, ruta.c_str(), token.c_str());
                // deschid conexiunea
                socket = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
                // trimit request-ul serverului
                send_to_server(socket, request);
                // primesc raspunsul de la server
                char *response = receive_from_server(socket);
                // afisez mesajul de raspuns
                if (strstr(response, "error") != NULL) {
                    cout << "Error! There is no response from the server!\n";
                } else {
                    cout << "Success! Book deleted!\n";
                }
                // inchid conexiunea
                close_connection(socket);
                continue;
            }
        }

        // comanda de logout
        if (strncmp(cmd, "logout", 6) == 0) {
            // verific daca sunt logat
            if (cookies == "") {
                cout << "Error! You are not logged in!\n";
                continue;
            } else {
                // construiesc request-ul de logout
                const char *ruta = "/api/v1/tema/auth/logout";
                char *request = logout(HOST, ruta, cookies.c_str());
                // deschid conexiunea
                socket = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
                // trimit request-ul serverului
                send_to_server(socket, request);
                // primesc raspunsul de la server
                char *response = receive_from_server(socket);
                // afisez mesajul de raspuns
                if (strstr(response, "error") != NULL) {
                    cout << "Error! There is no response from the server!\n";
                } else {
                    cout << "Success! You are logged out!\n";
                    // sterg cookies si token
                    cookies = "";
                    token = "";
                }
                // inchid conexiunea
                close_connection(socket);
                continue;
            }
        }
    }

    return 0;
}
