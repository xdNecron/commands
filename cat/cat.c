#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "utils.h"

/* TODO implement stdin read funcionality */

/*** FLAGS ***/
#define OPTSTR "vAETn"

#define OPT_SHOWALL 'A' /* equivalent to vET  DONE */
#define OPT_SHOWNONP 'v' /* use ^ and M- notation, except for LFD and TAB  DONE */
#define OPT_SHOWTAB 'T' /* display TAB characters as ^I  DONE */
#define OPT_SHOWEND 'E' /* display $ at the end of each line  DONE */
#define OPT_SHOWNUM 'n'/* show line numbers  NS */

int opt_shownonp = 0;
int opt_showtab = 0;
int opt_showend = 0;
int opt_shownum = 0;

void process_opts(int argc, char *argv[]) {

  int opt = 0;

  while ((opt = getopt(argc, argv, OPTSTR)) != -1) {
    switch (opt) {
      case OPT_SHOWALL:
        opt_shownonp = 1;
        opt_showtab = 1;
        opt_showend = 1;
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
        fprintf(stderr, "%s: Unknown option \'%c\'.\n", argv[0], optopt);
        exit(EXIT_FAILURE);
    }
  }
}

char *to_caret(unsigned char c) {
  char buf[3];

  if (c <= 0x1F && c != 0x0A) {
    buf[0] = '^';
    buf[1] = c + '@';
    buf[2] = '\0';
    printf("%s\n", buf);
  } else if (c == 0x7F) {
    buf[0] = '^';
    buf[1] = '?';
    buf[2] = '\0';
  } else {
    return NULL;  // Not a control char
  }

  return buf;
}

int process_text(char *buf, ssize_t bufsize) {

  for (int i = 0; i < bufsize; i++) {

    if (opt_showend) {
      if (buf[i] == '\n') {
        printf("$");
      }
    }

    if (opt_showtab) {
      if (buf[i] == 0x09) {
        printf("^I");
        continue;
      }
    }

    if (opt_shownonp) {
      char *caret = to_caret(buf[i]);
      if (caret) {
        printf("%s", caret);
        continue;
      }
    }

    if (opt_shownum) {

    }

    printf("%c", buf[i]);
  }

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

  while ((src_nrd = read(src_fd, buffer, BUFSIZE)) > 0) {
    process_text(buffer, src_nrd);
  }

  return 0;
}

int main(int argc, char *argv[]) {

  process_opts(argc, argv);

  if (optind >= argc) {
    fprintf(stderr, "%s: Expected arguments after options.", argv[0]);
    exit(EXIT_FAILURE);
  }

  if (cat_file(argv[optind]) == -1) {
    fprintf(stderr, "%s: Error allocating memory.", argv[0]);
    exit(EXIT_FAILURE);
  }

  return 0;
}
