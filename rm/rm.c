#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "utils.h"
#include <getopt.h>


#define OPTLIM 32

/*** FLAGS ***/
#define OPTSTR "rvf"

#define OPT_RECURSIVE 'r' /* WIP */
#define OPT_VERBOSE 'v' /* WIP */
#define OPT_FORCE 'f' /* ignore nonexistent files and arguments, never prompt  NS */

int opt_recursive = 0;
int opt_verbose = 0;
int opt_force = 0;

void process_opts(int argc, char *argv[]) {

  int opt;

  while ((opt = getopt(argc, argv, OPTSTR)) != -1) {
    switch (opt) {
      case OPT_RECURSIVE:
        opt_recursive = 1;
        break;
      case OPT_VERBOSE:
        opt_verbose = 1;
        break;
      case OPT_FORCE:
        opt_force = 1;
        break;
      default:
        fprintf(stderr, "%s: \'%c\' is not a valid option\n", argv[0], optopt);
    }
  }
}

int remove_file(char *file) {

  if (remove(file) == -1) return diefn("remove");
  if (opt_verbose) printf("removed \'%s\'\n", file);

  return 0;
}

int main(int argc, char *argv[]) {

  if (argc < 2) {
    fprintf(stderr, "Usage: rm [OPTION]... [FILE]...");
    return 1;
  }

  process_opts(argc, argv);

  if (optind >= argc) {
    fprintf(stderr, "%s: Expected arguments after options.\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  if (opt_recursive) {
    printf("%s: Testing, no files removed.\n", argv[0]);
    exit(EXIT_SUCCESS);
  }

  for (int i = optind; i <= argc - 1; i++) {
    if (remove_file(argv[optind]) == -1) return die("remove_file");
  }

  return 0;
}
