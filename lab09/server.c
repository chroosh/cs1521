//
// server.c -- A webserver written in C
// Based on code from LambdaSchool
// 
// Test with curl
//    curl -D - http://localhost:3490/
//    curl -D - http://localhost:3490/date
//    curl -D - http://localhost:3490/hello
//    curl -D - http://localhost:3490/hello?You
// 
// You can also test the above URLs in your browser! They should work!

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <sys/file.h>
#include <fcntl.h>

#define PORT 3490  // the port users will be connecting to
#define BACKLOG 10	 // how many pending connections queue will hold

#define BUF_SIZE 65536

void failed(char *);

// Send an HTTP response
//
// header:       "HTTP/1.1 404 NOT FOUND" or "HTTP/1.1 200 OK", etc.
// content_type: "text/plain", etc.
// body:         the data to send.
// 
int send_response(int fd, char *header, char *content_type, char *body)
{
   char response[BUF_SIZE];
   int response_length; // Total length of header plus body

   sprintf(response, "%s\n%s\r\n\r\n%s\n", header, content_type, body);
   response_length = strlen(response);
   int rv = send(fd, response, response_length, 0);
   if (rv < 0) failed("send");
   return rv;
}

// Send a 404 response
void send_404(int fd)
{
   send_response(fd, "HTTP/1.1 404 NOT FOUND",
                     "text/html",
                     "<h1>404 Page Not Found</h1>");
}

// Send a 500 response
void send_500(int fd)
{
   send_response(fd, "HTTP/1.1 500 Internal Server Error",
                     "text/html",
                     "<h1>Mangled request</h1>");
}

// Send response for server root
void send_root(int fd)
{
   // TODO
	send_response(fd, "HTTP/1.1 200 OK",
                     "text/html",
                     "<h1>Server running ...</h1>");
}

// Send a /date endpoint response
void send_date(int fd)
{
   // TODO
	char str_time[1024];
   time_t cur_time;
   cur_time = time(NULL);
   strcpy(str_time, "<h1>");
   strcat(str_time, asctime(localtime(&cur_time)));
   strcat(str_time, "<h1>");
   send_response(fd, "HTTP/1.1 200 OK",
                     "text/html",
                     str_time);
}

// Send a /date endpoint response
void send_hello(int fd, char *req)
{
   // TODO
	char name[1017]; //variable /hello?[name]
   char body[1032]; //<h1>Hello,[name]<h1>
 
   if (strncmp(req, "/hello?", 7) == 0) {
      if (sscanf(req, "/hello?%s", name) == 1) {
         strcpy(body, "<h1>Hello, ");
         strcat(body, name);
         strcat(body, "!<h1>");
         send_response(fd, "HTTP/1.1 200 OK",
                           "text/html",
                           body);
      } else {
         send_response(fd, "HTTP/1.1 200 OK",
                           "text/html",
                           "<h1>Name not found</h1>");
      }
   } else if (strcmp(req, "/hello") == 0) {
      send_response(fd, "HTTP/1.1 200 OK",
                        "text/html",
                        "<h1>Hello</h1>");
   }

}

// sen
void got_data(int fd, char *req) {
   char msg[1045];
   char data[1024];
   char *substr = strstr(req, "data=");
   if (substr != NULL) {
      sscanf(substr, "data=%s", data);
      strcpy(msg, "Thanks for the data: ");
      strcat(msg, data);
      send_response(fd, "HTTP/1.1 200 OK",
                        "text/plain",
                        msg);
   } else {
      send_response(fd, "HTTP/1.1 200 OK",
                        "text/plain",
                        "Paramaters: <data>");
   }
}

// Handle HTTP request and send response
void handle_http_request(int fd)
{
   char request[BUF_SIZE];
   char req_type[8]; // GET or POST
   char req_path[1024]; // /info etc.
   char req_protocol[128]; // HTTP/1.1
 
   // Read the request
   int nbytes = recv(fd, request, BUF_SIZE-1, 0);
   if (nbytes < 0) failed("recv");
   request[nbytes] = '\0';

   // for logging/debugging
   printf("Request: ");
   for (char *c = request; *c != '\n'; c++)
      putchar(*c);
   putchar('\n');
   // if you want to see the whole HTTP header, use
   // printf("%s\n", request);
 
   // Get the request type, path and protocol from the first line
   // If you can't decode the request, generate a 500 error response
   // Otherwise call appropriate handler function, based on path
   // Hint: use sscanf() and strcmp()
	
	// TODO
	if (sscanf(request, "%s %s %s", req_type, req_path, req_protocol) == 3) {
      if (strcmp(req_type, "GET") == 0) {
         if (strcmp(req_path, "/") == 0) {
            send_root(fd);
         } else if (strncmp(req_path, "/hello", 6) == 0) {
            send_hello(fd, req_path);
         } else if (strcmp(req_path, "/date") == 0) {
            send_date(fd);
         } else {
            send_404(fd);
         }
      } else if (strcmp(req_type, "POST") == 0 || strcmp(req_type, "PUT") == 0) {
         got_data(fd, request);
      } else {
         send_500(fd);
      }
   } else {
      send_500(fd);
   }
}

// fatal error handler
void failed(char *msg)
{
   char buf[100];
   sprintf(buf, "WebServer: %s", msg);
   perror(buf);
   exit(1);
}

int main(int argc, char *argv[])
{
   int listenfd, newfd, portno;
   struct sockaddr_in serv_addr, cli_addr;
   struct sockaddr *addr;

   // set up a socket
   listenfd = socket(AF_INET, SOCK_STREAM, 0);
   if (listenfd < 0) failed("opening socket");
   memset((char *) &serv_addr, 0, sizeof(serv_addr));
   // bind the socket to an address localhost:PortNo
   portno = PORT;
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   addr = (struct sockaddr *) &serv_addr;
   if (bind(listenfd, addr, sizeof(serv_addr)) < 0)
          failed("binding socket");
   // listen for connections to that address
   listen(listenfd,BACKLOG);
   printf("WebServer: waiting for connections...\n");

   socklen_t cli_len = sizeof(cli_addr);
   addr = (struct sockaddr *) &cli_addr;
   while (1) {
      // accept a connection from a client
      newfd = accept(listenfd, addr, &cli_len);
      if (newfd < 0) failed("accept");

      // Print out a message that we got the connection
      printf("WebServer: got connection\n");

      // newfd is a new socket descriptor for the new connection.
      // listenfd is still listening for new connections.
      handle_http_request(newfd);
      close(newfd);
   }
   close(listenfd);
   return 0; /* we never get here */
}

