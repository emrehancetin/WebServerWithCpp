#include "server.h"
#define PORT 8080


// this method creates a Server.
int createServer(){
    // this create a Server Socket. If this is successfully,then this sign the integer which is bigger than 0.
    int serverSocket = socket(AF_INET,SOCK_STREAM,0);
    // this controls that the Server Socket is created successfully, if not serverSocket will equal -1, than this returns error for socket.
    if(serverSocket < 0){
        perror("webserver (socket)");
        exit(1);
    }
    return serverSocket;
}
// this method binds the server to a exclusive address and port.
int bindServer(int Server,struct sockaddr_in *host_addr){

    // these arrange server info.
    host_addr->sin_family = AF_INET;
    host_addr->sin_port = htons(PORT);
    host_addr->sin_addr.s_addr = INADDR_ANY;

    // this controls that the it is connected successfully, if not this will equal -1, than this returns error for bind.
    // bind method try to connect Server. If this is successfully,then this sign the integer which is bigger than 0.
    if (bind(Server, (struct sockaddr *)host_addr, sizeof(*host_addr)) < 0) {
        perror("webserver (bind)");
        return 1;
    }
    return 0;
}


// this method deals with the file.
void file(int client, const std::string& filename){
    // this opens the file in binary mode.
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    // this controls if the file is opened successfully, then this returns true.
    if(file.is_open()){

        // this gets file size.
        std::streampos fileSize = file.tellg();

        // this sets the file position indicator to the beginning.
        file.seekg(0, std::ios::beg);

        std::string contentType;

        // this controls content according to the file extension.
        if (filename.find(".html") != std::string::npos) {
            contentType = "text/html";
        } else if (filename.find(".css") != std::string::npos) {
            contentType = "text/css";
        } else if (filename.find(".png") != std::string::npos){
            contentType = "text/png";
        }
        // this constructs the HTTP response.
        std::string response = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(fileSize) + "\r\nContent-Type: " + contentType + "; charset=utf-8\r\n\r\n";

        // this sends the response.
        send(client, response.c_str(), response.size(), 0);

        char buffer[1024];
        while (!file.eof()) {
            // this reads stack.
            file.read(buffer, sizeof(buffer));
            // this sends stack.
            send(client, buffer, file.gcount(), 0);
        }
    } else {// if this works,whics means that the file is not opened. Then returns 404 Not Found Error.
        std::string response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
        send(client, response.c_str(), response.size(), 0);
    }
    
}



int main(){
    // create a our main variables
    int server,client;
    struct sockaddr_in host_addr, client_addr;
    socklen_t addrLen = sizeof(client_addr);
    
    // this calls createServer() methods than sign at server.
    server = createServer();


    // this controls adress and port. If not ,than this returns error for bind.
    if (bindServer(server, &host_addr) < 0) {
        perror("webserver (bind)");
        return 1;
    }
    
    // this controls that listening is started successfully,if not this will equal -1, than this returns error for listen.
    if (listen(server, 5) < 0) {
        perror("webserver (listen)");
        return 1;
    }
    

    for(;;){
        // this accepts incoming connection request.
        client = accept(server, (struct sockaddr *)&host_addr, &addrLen);
        if (client < 0) {
            perror("Error accepting connection");
            return 1;
        }
        char request[1024];

        // this receives the HTTP request from the client.
        recv(client, request, sizeof(request), 0);

        std::istringstream requestStream(request);
        std::string requestLine;
        // this gets the first line of the request.
        getline(requestStream, requestLine);

        // this separates request due to choose which file will serve.
        if (requestLine.find("GET") != std::string::npos) {
            size_t start = requestLine.find_first_of(" ");
            size_t end = requestLine.find_last_of(" ");
            std::string requestedFile = requestLine.substr(start+1, end-start-1);

            if (requestedFile == "/") {
                file(client, "site/index.html"); // serve the index.html file
            } else if (requestedFile == "/style.css") {
                file(client, "site/style.css"); // serve the style.css file
            } else if(requestedFile == "/rover.png"){
                file(client, "site/rover.png"); // serve the style.css file
            }else {
                std::string response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
                send(client, response.c_str(), response.size(), 0); // send a 404 Not Found response
            }
        }
        // this closes the client.
        close(client);
    }
    //this closes the server.
    close(server);


    // Finally, after everything works successfully, it allows you to exit the program.
    return 0;
}

// g++ src/server.cpp -o bin/server
// bin/server