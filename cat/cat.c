#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "utils.h"

/* cat command
  Concatenate srcfd.

  Steps:
    1. Open and read srcfd.
    2. Write contents of srcfd to stdout.
 */

#define BUFSIZE 1024

int die(char *err) {
  perror(err);
  return 1;
}

int main(int argc, char *argv[]) {

  if (argc != 2) return 1;

  int len = BUFSIZE;
  char *c = malloc(len);

  int src_fd;
  ssize_t src_rd;

  // STEP 1: Open and read srcfd
  if ((src_fd = open(argv[1], O_RDONLY)) == -1) return die("src_fd");

  while ((src_rd = read(src_fd, c, len)) > 0) {
    if (src_rd == BUFSIZE) {
        len += BUFSIZE;
        c = realloc(c, len);
    }
  }

  fprintf(stdout, "%s", c);

  return 0;
}
