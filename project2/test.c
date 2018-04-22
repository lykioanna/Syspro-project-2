#include <dirent.h>
#include <stdio.h>

int main(int argc,char* argv[]) {
  DIR *d;
  struct dirent *dir;
  char* doc= argv[1];
  d = opendir(doc);
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      printf("%s\n", dir->d_name);
    }
    closedir(d);
  }
  return(0);
}
