#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <getopt.h>

#include "utils.h"

/* TODO implement stdin read funcionality */

/*** FLAGS ***/
#define OPTSTR "vAETn"

#define OPT_SHOWALL 'A'
#define OPT_SHOWNONP 'v'
#define OPT_SHOWTAB 'T'
#define OPT_SHOWEND 'E'
#define OPT_SHOWNUM 'n'

int opt_showall = 0;
int opt_shownonp = 0;
int opt_showtab = 0;
int opt_showend = 0;
int opt_shownum = 0;

void process_opts(int argc, char *argv[]) {

  int opt = 0;

  while (getopt(argc, argv, OPTSTR) != -1) {
    switch (opt) {
      case OPT_SHOWALL:
        opt_showall = 1;
        break;
      case OPT_SHOWNONP:
        opt_shownonp = 1;
        break;
      case OPT_SHOWTAB:
        opt_showtab = 1;
        break;
      case OPT_SHOWEND:
        opt_showend = 1;
        break;
      case OPT_SHOWNUM:
        opt_shownum = 1;
        break;
      default:
        fprintf(stderr, "%s: Unknown option \'%c\'", argv[0], optopt);
        exit(EXIT_FAILURE);
    }
  }
}

int process_text(char *buf, ssize_t bufsize) {

  for (int i = 0; i < bufsize; i++) {

    if (buf[i] == '\n') {
      printf("ha, ivan!\n");
    }
  }

  return 0;
}

int write_out(int *outfd, char *buf, ssize_t bufsize) {
  return 0;
}

int cat_file(char *file) {
  char *buffer = malloc(BUFSIZE);

  if (!buffer) {
    return -1;
  }

  int src_fd;
  ssize_t src_nrd, dst_nwr;

  if ((src_fd = open(file, O_RDONLY)) == -1) return die("src_fd");

  while ((src_nrd = read(src_fd, buffer , BUFSIZE)) > 0) {
    if ((dst_nwr = write(STDOUT_FILENO, buffer, src_nrd)) != src_nrd) return die("dst_nwr");
  }

  return 0;
}

int main(int argc, char *argv[]) {

  process_opts(argc, argv);

  if (optind >= argc) {
    fprintf(stderr, "%s: Expected arguments after options.", argv[0]);
    exit(EXIT_FAILURE);
  }

  if (cat_file(argv[1]) == -1) {
    fprintf(stderr, "%s: Error allocating memory.", argv[0]);
    exit(EXIT_FAILURE);
  }

  return 0;
}
