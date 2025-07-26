#define _XOPEN_SOURCE 500

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <getopt.h>
#include <ftw.h>

#include "utils.h"

#define OPTLIM 32

/*** FLAGS ***/
#define OPTSTR "rvf"

#define OPT_RECURSIVE 'r' /* WIP */
#define OPT_VERBOSE 'v' /* WIP */
#define OPT_FORCE 'f' /* ignore nonexistent files and arguments, never prompt  WIP */

int opt_recursive = 0;
int opt_verbose = 0;
int opt_force = 0;

char **gargv;

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

int remove_file(char *file, struct stat *sb) {

  if (remove(file) == -1) return diefn("remove");

  if (opt_verbose) {
    if (S_ISDIR(sb->st_mode)) {
      printf("removed directory \'%s\'\n", file);
    } else {
      printf("removed \'%s\'\n", file);
    }
  }

  return 0;
}

int nftw_handle_target(const char *fpath, const struct stat *sb, const int typeflag, struct FTW *ftwbuf) {

  if (typeflag == FTW_D) {
    if (nftw(fpath, nftw_handle_target, 20, FTW_PHYS | FTW_DEPTH) == -1) {
      perror("nftw");
      return -1;
    }
    return 0;
  }

  if (remove_file(fpath, sb) == -1) return diefn("nftw_file");

  return 0;
}

int main(int argc, char *argv[]) {

  struct stat sb;

  if (argc < 2) {
    fprintf(stderr, "Usage: rm [OPTION]... [FILE]...");
    return 1;
  }

  process_opts(argc, argv);

  if (optind >= argc) {
    fprintf(stderr, "%s: Expected arguments after options.\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  for (int i = optind; i <= argc - 1; i++) {
    if (stat(argv[i], &sb) == -1) {
      if (errno == ENOENT) {
        if (opt_force) continue;
        fprintf(stderr, "%s: no entry for file \'%s\'.\n", argv[0], argv[i]);
        exit(EXIT_FAILURE);
      }
      return diefn("stat");
    }

    if (opt_recursive) {
      if (nftw(argv[i], nftw_handle_target, 20, FTW_PHYS | FTW_DEPTH) == -1) {
        perror("nftw");
        exit(EXIT_FAILURE);
      }
      continue;
    }
    if (remove_file(argv[optind], &sb) == -1) return die("remove_file");
  }

  return 0;
}
