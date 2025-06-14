#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>

/* cp command
   Takes in 2 args (no flags for now): srcpath, dstpath. Copies contents of srcpath to dstpath.

   TODO
   [ ] Add usage error handling
   [ ] Add flag handling
   [ ] Add recursive copying

   Steps:
    1. Open srcpath.
    2. Read srcpath contents to new buffer.
    3. Close srcpath.
    4. Open (or create) dstpath.
    5. Write srcpath contents to dstpath.
    6. Close dstpath.
 */

#define BUFSIZE 1024

int die(char *error) {
  perror(error);
  return 1;
}

int main(int argc, char *argv[]) {
  /* printf("%s\n", argv[1]); */
  /* printf("%ld\n", (long) argc); */

  // STEP 1
  int src_fd = open(argv[1], O_RDONLY);
  if (src_fd == -1) return die("src_fd");

  // STEP 2: Read contents of srcpath

  int bufsize = BUFSIZE;
  char *buffer = malloc(bufsize);

  while (1) {
    int src_rd = read(src_fd, buffer, bufsize);
    if (src_rd == -1) return die("read");
    if (src_rd != BUFSIZE) {
      break;
    } else {
      bufsize += BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (*buffer == -1) return die("realloc");
    }
  }

  // STEP 3: Close srcpath
  int status = close(src_fd);
  if (status == -1) return die("src_close");

  // STEP 4: Open (or create) dstpath
  int dst_fd = open(argv[2], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IWGRP | S_IRGRP | S_IROTH | S_IWOTH);
  if (dst_fd == -1) return die("dst_open");

  // STEP 5: Write buffer into dstpath
  int dst_wr = write(dst_fd, buffer, bufsize);
  if (dst_wr == -1) return die("dst_wr");

  // STEP 6: Close dstpath
  close(dst_fd);

  return 0;
}
