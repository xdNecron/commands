#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "utils.h"

/* cp command
   Takes in 2 args (no flags for now): srcdir, dstdir. Copies contents of srcdir to dstdir.

   TODO
   [ ] Add usage error handling
   [ ] Add flag handling
   [ ] Add recursive copying

   Steps:
    1. Open srcdir.
    2. Read srcdir contents to new buffer.
    3. Close srcdir.
    4. Open (or create) dstdir.
    5. Write srcdir contents to dstdir.
    6. Close dstdir.
 */

#define BUFSIZE 1024

int die(char *error) {
  perror(error);
  return 1;
}

int main(int argc, char *argv[]) {

  int src_fd, dst_fd;
  int open_flags = O_CREAT | O_WRONLY | O_TRUNC;
  mode_t file_perms  = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
  ssize_t src_rd, dst_wr;

  char buffer[BUFSIZE];

  // STEP 1: Open srcdir
  src_fd = open(argv[1], O_RDONLY);
  if (src_fd == -1) return die("src_fd");

  // STEP 2: Open (or create) dstdir
  dst_fd = open(argv[2], open_flags, file_perms);
  if (dst_fd == -1) return die("dst_fd");

  while ((src_rd = read(src_fd, buffer, BUFSIZE)) > 0) {
    if ((dst_wr = write(dst_fd, buffer, src_rd)) != src_rd) return die("dst_wr");
  }

  // STEP 6: Close dstdir
  close(src_fd);
  close(dst_fd);

  return 0;
}
