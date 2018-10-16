// COMP1521 18s2 Week 10 Lab
// dns.c ... simple DNS lookup client

#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int main(int argc, char **argv)
{
   struct in_addr ip;   // see "man 3 inet_aton"
   struct hostent *hp;  // see "man 3 gethostbyname"

   if (argc < 2) {
      printf("Usage: %s HostName|IPaddr\n", argv[0]);
      return 1;
   }

   // TODO

   return 0;
}
