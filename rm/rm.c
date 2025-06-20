#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "utils.h"

/* rm command
  Removes target file
  */

int main(int argc, char *argv[]) {

  if (argc < 2) return 1;

  for (int i = 1; i <= argc - 1; i++) {
    remove(argv[i]);
  }

  return 0;
}
