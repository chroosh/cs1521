// myls.c ... my very own "ls" implementation

#include <stdlib.h>
#include <stdio.h>
// #include <bsd/string.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAXDIRNAME 100
#define MAXFNAME   200
#define MAXNAME    20

char *rwxmode(mode_t, char *);
char *username(uid_t, char *);
char *groupname(gid_t, char *);

int main(int argc, char *argv[])
{
   // string buffers for various names
   char dirname[MAXDIRNAME];
	 char uname[MAXNAME+1]; // UNCOMMENT this line
	 char gname[MAXNAME+1]; // UNCOMMENT this line
	 char mode[MAXNAME+1]; // UNCOMMENT this line

   // collect the directory name, with "." as default
   if (argc < 2)
      strlcpy(dirname, ".", MAXDIRNAME);
   else
      strlcpy(dirname, argv[1], MAXDIRNAME);

   // check that the name really is a directory
   struct stat info;
   if (stat(dirname, &info) < 0)
      { perror(argv[0]); exit(EXIT_FAILURE); }
   if ((info.st_mode & S_IFMT) != S_IFDIR)
      { fprintf(stderr, "%s: Not a directory\n",argv[0]); exit(EXIT_FAILURE); }

   // open the directory (with given name) to start reading
	 DIR *df = opendir(dirname);
	 // if failed to open dir, print program name + exit
	 if (df < 0) {
	 		perror(argv[0]);
			exit(EXIT_FAILURE);
	 }

	 // readdir(df) - where df is the pointer to the directory
	 struct dirent *entry = readdir(df);

	 // read directory entries 
	 while ((entry = readdir(df)) != NULL) {
		 if (entry->d_name[0] == '.') continue;

		 char fname[MAXFNAME];
		 snprintf(fname, MAXFNAME, "%s/%s", dirname, entry->d_name);
		 if (lstat(fname, &info) < 0) {
			 perror(fname);
		 } else {
			 printf("%s  %-8.8s %-8.8s %8lld  %s\n",
			 		rwxmode(info.st_mode, mode),
			 		username(info.st_uid, uname),
			 		groupname(info.st_gid, gname),
			 		(long long)info.st_size,
			 		entry->d_name);
		 }
	 }
	 
   // finish up
	 closedir(df); // UNCOMMENT this line
   return EXIT_SUCCESS;
}

// convert octal mode to -rwxrwxrwx string
char *rwxmode(mode_t mode, char *str)
{
	 // first char of the string
	 switch(mode & S_IFMT) {
	 		case S_IFDIR: str[0] = 'd'; break;
			case S_IFREG: str[0] = '-'; break;
			case S_IFLNK: str[0] = 'l'; break;
			default 		: str[0] = '?'; break;

	 }

	 str[1] = (mode & (1<<8)) ? 'r' : '-';
	 str[2] = (mode & (1<<7)) ? 'w' : '-';
 	 str[3] = (mode & (1<<6)) ? 'x' : '-';
	 str[4] = (mode & (1<<5)) ? 'r' : '-';
	 str[5] = (mode & (1<<4)) ? 'w' : '-';
	 str[6] = (mode & (1<<3)) ? 'x' : '-';
	 str[7] = (mode & (1<<2)) ? 'r' : '-';
	 str[8] = (mode & (1<<1)) ? 'w' : '-';
	 str[9] = (mode & (1<<0)) ? 'x' : '-';
	 str[10] = '\0';

   return str;
}

// convert user id to user name
char *username(uid_t uid, char *name)
{
   struct passwd *uinfo = getpwuid(uid);
   if (uinfo == NULL)
      snprintf(name, MAXNAME, "%d?", (int)uid);
   else
      snprintf(name, MAXNAME, "%s", uinfo->pw_name);
   return name;
}

// convert group id to group name
char *groupname(gid_t gid, char *name)
{
   struct group *ginfo = getgrgid(gid);
   if (ginfo == NULL)
      snprintf(name, MAXNAME, "%d?", (int)gid);
   else
      snprintf(name, MAXNAME, "%s", ginfo->gr_name);
   return name;
}
