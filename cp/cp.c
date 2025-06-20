#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
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

#define CP_OPTSTR "fr"

#define OPT_R 'r'
#define OPT_F 'f'

/*** FLAGS ***/

int opt_r = 0; /* recursive mode */
int opt_f = 0; /* force */

int process_opts(int argc, char *argv[]) {

  int opt;

  while ((opt = getopt(argc, argv, CP_OPTSTR)) != -1) {
    switch (opt) {
      case OPT_R:
        opt_r = 1;
        break;
      case OPT_F:
        opt_f = 1;
        break;
      default:
        fprintf(stderr, "%c: not a valid option\n", optopt);
        return -1;
    }
  }
  // Is it even necessary to return the index of first FILE argument?
  /* return optind; */
  return 0; // returns success for now
}

int reccpy(int *src, int *dst) {
  printf("Tisknu kundy\n");
  return 0;
}

int copy(int *src, int *dst) {

  ssize_t src_rd, dst_wr;
  char buffer[BUFSIZE];

  if (opt_r == 1) {
    if (reccpy(src, dst) == -1) return diefn("reccpy");
  } else {
    while ((src_rd = read(*src, buffer, BUFSIZE)) > 0) {
      if ((dst_wr = write(*dst, buffer, src_rd)) != src_rd) return diefn("dst_wr");
    }
  }

  return 0;
}


int main(int argc, char *argv[]) {

  struct stat finfo;

  if (stat(argv[argc - 2], &finfo) == -1) return die("stat");

  if (argc < 3) {
    fprintf(stderr, "Usage: cp [ARGS] file1 file2\n");
    return 1;
  }

  int src_fd, dst_fd;
  int open_flags = O_CREAT | O_WRONLY | O_TRUNC;
  mode_t file_perms  = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

  if ((process_opts(argc, argv)) == -1) return 1;

  if ((S_ISDIR(finfo.st_mode)) != 0 && opt_r == 0) {
    fprintf(stderr, "%s: is a directory, use recursive mode\n", argv[1]);
    return 1;
  }

  // STEP 1: Open srcdir
  src_fd = open(argv[argc - 2], O_RDONLY);
  if (src_fd == -1) return die("src_fd");

  // STEP 2: Open (or create) dstdir
  dst_fd = open(argv[argc - 1], open_flags, file_perms);
  if (dst_fd == -1) return die("dst_fd");

  if ((copy(&src_fd, &dst_fd)) == -1) return die("copy");

  // STEP 6: Close dstdir
  close(src_fd);
  close(dst_fd);

  return 0;
}
