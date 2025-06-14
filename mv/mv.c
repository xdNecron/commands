#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>

/* mv command
   Move srcdir to dstdir

  Steps
    1. Open and read srcdir
    2. Create dstdir
    3. Write contents of srcdir into dstdir
    4. Close both files
    5. Delete srcdir
  */

#define BUFSIZE 1024

int die(char *err) {
  perror(err);
  return 1;
}

int main(int argc, char *argv[]) {

  int src_fd, dst_fd;
  int open_flags = O_CREAT | O_WRONLY | O_TRUNC;
  mode_t file_perms  = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
  ssize_t src_rd;

  // STEP 0: Define buffer struct

  struct buf {
    char *c;
    int len;
  };

  struct buf buf = { .c = NULL, .len = 0};

  // STEP 1: Open and read srcdir
  if (argc != 3) return 1;
  src_fd = open(argv[1], O_RDONLY);
  if (src_fd == -1) return die("src_fd");

  while ((src_rd = read(src_fd, &buf.c, buf.len)) > 0) {
    buf.len += BUFSIZE;
    buf.c = realloc(buf.c, buf.len);
  }

  if (src_rd == -1) return die("src_rd");

  close(src_fd);

  // STEP 2: Create dstdir
  dst_fd = open(argv[2], open_flags, file_perms);
  if (dst_fd == -1) return die("dst_fd");

  // STEP 3: Write contents of srcdir to dstdir
  if (write(dst_fd, &buf.c, buf.len) == -1) return die("dst_fd");

  close(dst_fd);

  // STEP 5: Delete srcdir
  if (remove(argv[1]) == -1) return die("remove");

  return 0;
}
