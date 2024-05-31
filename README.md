# mpp

The project involves developing an instant messaging system for computers on a local network using the C++ programming language. The goal is to create a robust and professional application, encompassing all essential stages of a well-structured project, such as complete documentation, the use of a GitHub repository, a Makefile to facilitate compilation, and organizing the source code into .cpp and .h files. The server for this system will run on a Raspberry Pi 4, while clients will run on other computers within the same local network.

The system's architecture will be based on the client-server model, using the TCP/IP protocol for communication between devices. The server will be responsible for managing connections, receiving messages from clients, and retransmitting them to the appropriate recipients. Additionally, the server may maintain a log of online users and store message logs if necessary. Clients will be responsible for sending messages to the server and receiving messages from other users.

Initially, the project will be developed with a simple command-line interface to facilitate implementation and testing. In the future, a graphical interface will be developed to make the application more user-friendly and accessible.

The project will begin with setting up an appropriate development environment, including installing a C++ compiler (such as g++), a version control system (Git), and a code editor (such as VSCode or CLion). Next, a repository will be created on GitHub to host the code and allow for collaboration and version control. The basic project structure will include directories for source code, header files, documentation, and unit tests.

A Makefile will be created to automate the compilation process, defining rules for compiling .cpp files into object files and linking them into an executable. Development will start with the implementation of the server, which will listen for client connections, accept connections, and manage message exchange. In parallel, a basic client will be developed to connect to the server, send messages, and display received messages.

Documentation will be a crucial part of the project, including a README.md file with installation, usage, and contribution instructions, as well as explanatory comments in the code to facilitate understanding and maintenance.

After the initial implementation, the server will be tested on the Raspberry Pi 4, and clients will be tested on computers within the local network. Future improvements will include implementing security with message encryption, developing a graphical interface using libraries such as Qt or GTK, and adding extra functionalities like multi-user support and message history.

Throughout development, frequent commits will be made to the GitHub repository to ensure version control and continuous collaboration. The ultimate goal is to create an efficient, secure, and user-friendly instant messaging system with a solid foundation of well-structured and documented code.


Of course, here is the detailed explanation in Markdown in English:


# Server Functionality

## Including Necessary Libraries

The server includes standard libraries for input/output, string manipulation, sockets, threads, and STL data structures.

```cpp
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <map>
#include <string>
```

## Defining Constants and Global Variables

Defines the server port and creates a map to associate usernames with client sockets. A mutex is also created to safely manage access to the map in a multi-threaded environment.

```cpp
#define PORT 8080

std::map<std::string, int> clients;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
```

## Message Sending Function

This function is responsible for sending messages to the recipient user. It locks access to the clients map while searching for the recipient's socket to ensure thread safety.

```cpp
void send_message(const std::string& user, const std::string& message) {
    pthread_mutex_lock(&clients_mutex);
    if (clients.find(user) != clients.end()) {
        int user_socket = clients[user];
        send(user_socket, message.c_str(), message.size(), 0);
    }
    pthread_mutex_unlock(&clients_mutex);
}
```

## Client Handling Function

This function runs in a new thread for each connected client. It manages communication with the client, receiving the username, adding it to the clients map, receiving and forwarding messages.

```cpp
void *handle_client(void *client_socket) {
    int sock = *(int*)client_socket;
    char buffer[1024] = {0};
    std::string username;

    // Receive the username
    read(sock, buffer, 1024);
    username = std::string(buffer);
    
    pthread_mutex_lock(&clients_mutex);
    clients[username] = sock;
    pthread_mutex_unlock(&clients_mutex);

    std::cout << username << " has connected\n";

    // Loop to read and forward messages
    while (true) {
        memset(buffer, 0, 1024);
        int bytes_read = read(sock, buffer, 1024);
        if (bytes_read <= 0) {
            std::cout << username << " has disconnected\n";
            pthread_mutex_lock(&clients_mutex);
            clients.erase(username);
            pthread_mutex_unlock(&clients_mutex);
            break;
        }

        std::string message(buffer);
        size_t delimiter_pos = message.find(":");
        if (delimiter_pos != std::string::npos) {
            std::string recipient = message.substr(0, delimiter_pos);
            std::string msg = message.substr(delimiter_pos + 1);
            send_message(recipient, username + ": " + msg);
        }
    }

    close(sock);
    free(client_socket);
    return nullptr;
}
```

## Main Function

The `main` function sets up the server to accept client connections and creates threads to handle each client individually.

### Creating the Server Socket

Creates a socket for the server.

```cpp
int server_fd, new_socket;
struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);

if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
}
```

### Configuring the Socket

Configures the socket to reuse the address and port.

```cpp
if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
}
```

### Defining the Server Address

Defines the address family, IP address, and port for the server.

```cpp
address.sin_family = AF_INET;
address.sin_addr.s_addr = INADDR_ANY;
address.sin_port = htons(PORT);
```

### Binding the Socket

Binds the socket to the defined address and port.

```cpp
if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
}
```

### Listening for Connections

Puts the socket in listening mode, allowing it to accept client connections.

```cpp
if (listen(server_fd, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
}
```

### Accepting Connections and Creating Threads

Enters an infinite loop to accept client connections. For each new connection, a new thread is created to handle communication with the client.

```cpp
while (true) {
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    int *client_socket = (int*)malloc(sizeof(int));
    *client_socket = new_socket;

    pthread_t client_thread;
    if (pthread_create(&client_thread, nullptr, handle_client, (void*)client_socket) != 0) {
        perror("pthread_create");
        free(client_socket);
    }

    pthread_detach(client_thread);
}

close(server_fd);
return 0;
```

This server can manage multiple client connections simultaneously, forward messages to the correct recipients, and handle client disconnections robustly.

