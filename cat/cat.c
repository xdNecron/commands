#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "utils.h"

/* cat command
  Concatenate srcfd.

  Steps:
    1. Open srcf
    2. Incrementally write contents of srcf to stdout
 */

int main(int argc, char *argv[]) {

  if (argc != 2) return 1;

  char buffer[BUFSIZE];

  int src_fd;
  ssize_t src_rd, dst_wr;

  // STEP 1: Open and read srcfd
  if ((src_fd = open(argv[1], O_RDONLY)) == -1) return die("src_fd");

  while ((src_rd = read(src_fd, buffer , BUFSIZE)) > 0) {
    if ((dst_wr = write(STDOUT_FILENO, buffer, src_rd)) != src_rd) return die("dst_wr");
  }

  return 0;
}
