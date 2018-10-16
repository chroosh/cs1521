// COMP1521 18s2 Week 10 Lab
// dns.c ... simple DNS lookup client

#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


void show_aliases(struct hostent *in);
void show_IP_address(struct hostent *in);

int main(int argc, char **argv)
{

	// struct in_addr {
   //  unsigned long s_addr;  // load with inet_aton()
	// };
	//
	//
	// struct hostent {
	//   char  *h_name;
	//   char  **h_aliases;
	//   short h_addrtype;
	//   short h_length;
	//   char  **h_addr_list;
	// } 
	
   struct in_addr ip;   // see "man 3 inet_aton"
   struct hostent *hp;  // see "man 3 gethostbyname"

   if (argc < 2) {
      printf("Usage: %s HostName|IPaddr\n", argv[0]);
      return 1;
  	} 

   // TODO
	// gets internet host address from ipv4 number and dots notation and stores into 
	// binary form
	if (inet_aton(argv[1], &ip)) {
      if ((hp = gethostbyaddr(&ip, sizeof(ip), AF_INET)) != NULL) {
         printf("%s -> %s\n", inet_ntoa(ip), hp->h_name);
         // show_aliases(hp);
         // show_IP_address(hp);
      } else {
         printf("No name associated with %s\n", inet_ntoa(ip));
      }
   } else {
	// if inet_aton is invalid && returns 0 use gethostbyname instead of gethostbyaddr
      if ((hp = gethostbyname(argv[1])) != NULL) {
         printf("%s -> %s\n", hp->h_name, inet_ntoa(*((struct in_addr *) hp->h_addr)));
         // show_aliases(hp);
         // show_IP_address(hp);
      } else {
         printf("Can't resolve %s\n", argv[1]);
      }
   }

   return 0;
}


void show_aliases(struct hostent *in) {
   char **alias = in->h_aliases;
   printf("Alternate aliases:\n");
   while (*alias != NULL) {
      printf("%s\n", *alias);
      alias++;
   }
}
 
void show_IP_address(struct hostent *in) {
   char **addr = in->h_addr_list;
   addr++;
   printf("Alternate addresses:\n");
   while (*addr != NULL) {
      // inet_ntoa(*((struct in_addr *) hp->h_addr)) i don't 100% get why this works
      printf("%s\n", inet_ntoa(*(struct in_addr *) *addr));
      addr++;
   }
}
