#include <asm-generic/errno-base.h>
#include <linux/limits.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <getopt.h>
#include <errno.h>

#include "utils.h"

#define HELPSTR \
"Usage: mv [OPTION]... SOURCE DEST\n\
  or: mv [OPTION]... SOURCE... DIRECTORY\n\
  or: mv [OPTION]... -t DIRECTORY SOURCE...\n\
\n\
Available options:\n\
  -f                            do not prompt before overwrite\n\
  -i                            prompt before overwrite\n\
  -n                            do not overwrite an existing file\n\
If you specify more than one of -i, -f, -n, only the final one takes effect\n\
\n\
  -t DIRECTORY                  move all SOURCE rguments into DIRECTORY\n\
  -u                            move only when the source file is newer\n\
                                  than the destination file or when the\n\
                                  destination file is missing\n\
  -v                            explain what is being done\n\
  -h                            display this help and exit\n"

/*** FLAGS ***/
#define OPTSTR "uvt:infh"

#define OPT_UPDATE 'u' /* move only when the SOURCE file is newer
                          than the destination file or when the
                          destination file is missing  NS */
#define OPT_VERBOSE 'v' /* explain what is being done  DONE */
#define OPT_TARGETDIR 't' /* move all SOURCE arguments into DIRECTORY  DONE */
#define OPT_INTERACTIVE 'i' /* prompt before overwrite  NS */
#define OPT_NOCLOB 'n' /* do not overwirte an existing file  NS */
#define OPT_FORCE 'f' /* do not prompt before overwriting  NS */
#define OPT_HELP 'h'
/* If you specify more than one of -i, -f, -n, only the final one takes effect. */

int opt_update = 0;
int opt_verbose = 0;
int opt_targetdir = 0;
int opt_interactive = 0;
int opt_noclob = 0;
int opt_force = 0;
int opt_help = 0;

char oarg_targetdir[BUFSIZE];

void process_flags(int argc, char *argv[]) {

  int opt = 0;

  while ((opt = getopt(argc, argv, OPTSTR)) != -1) {
    switch (opt) {
      case OPT_UPDATE:
        opt_update = 1;
        break;
      case OPT_VERBOSE:
        opt_verbose = 1;
        break;
      case OPT_TARGETDIR:
        opt_targetdir = 1;
        snprintf(oarg_targetdir, sizeof(oarg_targetdir), "%s", optarg);
        break;
      case OPT_INTERACTIVE:
        opt_interactive = 1;
        opt_noclob = 0;
        opt_force = 0;
        break;
      case OPT_NOCLOB:
        opt_interactive = 0;
        opt_noclob = 1;
        opt_force = 0;
        break;
      case OPT_FORCE:
        opt_interactive = 0;
        opt_noclob = 0;
        opt_force = 1;
        break;
      case OPT_HELP:
        opt_help = 1;
        break;
      default:
        fprintf(stderr, HELPSTR);
        exit(EXIT_FAILURE);
    }
  }
}

int get_lastsrc(int argc) {

  int endindex;
  int lastsrc = optind - 1;

  if (opt_targetdir) {
    endindex = argc;
  } else {
    endindex = argc -1;
  }

  for (int i = optind; i < endindex; i++) {
    lastsrc += 1;
  }

  return lastsrc;
}

int get_scount(int argc) {

  int endindex;
  int scount = 0;

  if (opt_targetdir) {
    endindex = argc;
  } else {
    endindex = argc - 1;
  }

  for (int i = optind; i < endindex; i++) {
    scount += 1;
  }

  return scount;
}

int move_source(char *src, char *dst, int tflag) {

  struct stat srcsb, dstsb;
  int srcstat, dststat;
  int dsterr = 0;

  srcstat = stat(src, &srcsb);
  if (srcstat == -1) diefn("srcstat");

  dststat = stat(dst, &dstsb);
  if (dststat == -1) {
    dsterr = errno;
    if (dsterr != ENOENT) {
      printf("%d\n", dsterr);
      diefn("dststat");
    }
  }

  if (tflag) {
    if (dsterr == ENOENT) {
      if (mkdir(dst, S_IRUSR | S_IWUSR | S_IXUSR) == -1) diefn("mkdir");
    }
    char newdst[PATH_MAX];
    snprintf(newdst, PATH_MAX, "%s/%s", dst, src);
    if (rename(src, newdst) == -1) diefn("rename");
    if (opt_verbose)
      printf("renamed \'%s\' -> \'%s\'\n", src, newdst);
    return 0;
  }

  if (rename(src, dst) == -1) diefn("rename");
  if (opt_verbose)
    printf("renamed \'%s\' -> \'%s\'\n", src, dst);

  return 0;
}

int main(int argc, char *argv[]) {

  int scount;
  int lastsrc = 0;
  char *dst = malloc(PATH_MAX);
  int dst_istarget = 0; /* treat dst as target directory */

  process_flags(argc, argv);

  if (opt_help) {
    printf(HELPSTR);
    exit(EXIT_SUCCESS);
  }

  if (optind >= argc) {
    fprintf(stderr, HELPSTR);
    exit(EXIT_FAILURE);
  }

  lastsrc = get_lastsrc(argc);
  scount = get_scount(argc);

  // printf("firstsrc: %d, lastsrc: %d, scount: %d\n", optind, lastsrc, scount);

  if (scount > 1) dst_istarget = 1;

  if (opt_targetdir) {
    snprintf(dst, PATH_MAX, "%s", oarg_targetdir);
    dst_istarget = 1;
  } else {
    snprintf(dst, PATH_MAX, "%s", argv[argc - 1]);
  }

  for (int i = optind; i <= lastsrc; i++) {
    if (move_source(argv[i], dst, dst_istarget) == -1) exit(EXIT_FAILURE);
  }

  free(dst);

  return 0;
}
