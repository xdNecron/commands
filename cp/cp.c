#define _XOPEN_SOURCE 500
#define _LARGEFILE64_SOURCE

#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include "utils.h"
#include <ftw.h>
#include <limits.h>
#include <errno.h>

/*** DEFINES ***/

#define CP_OPTSTR "sfrv"

#define OPT_R 'r'
#define OPT_F 'f'
#define OPT_V 'v'
#define OPT_S 's'

/*** FLAG PROCESSING ***/

int opt_r = 0; /* recursive mode */

int opt_v = 0; /* verbose */
int opt_d = 0; /* no-dereference */
int opt_s = 0; /* make symbolic links instead of copying */
int opt_f = 0; /* force: if an existing destination file cannot be
                                 opened, remove it and try again (this option
                                 is ignored when the -n option is also used) */

/*** GLOBALS ***/
char srcpath[PATH_MAX];
char dstpath[PATH_MAX];

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
      case OPT_V:
        opt_v = 1;
        break;
      case OPT_S:
        opt_s = 1;
        break;
      default:
        fprintf(stderr, "%c: not a valid option\n", optopt);
        return -1;
    }
  }

  return 0; // returns success for now
}

// Is this the most optimal way to keep track of the cp destination?
char *get_new_path(char *old_path, char *old_prefix, char *new_prefix) {

  if (strncmp(old_path, old_prefix, strlen(old_prefix)) != 0) return NULL;

  ssize_t new_len = strlen(new_prefix) + strlen(old_path) - strlen(old_prefix) + 2;
  char *new_path = malloc(new_len);

  if (!new_path) return NULL;

  snprintf(new_path, new_len, "%s/%s\n", new_prefix, old_path + strlen(old_prefix));
  return new_path;

}

int copy_file(char *srcpath, char *dstpath) {

  int opensrc_flags = O_RDONLY | O_LARGEFILE;
  int opendst_flags = O_WRONLY | O_TRUNC | O_CREAT | O_LARGEFILE;
  int opendst_mode = S_IRUSR | S_IWUSR ;

  int src_fd, dst_fd;
  int bytes_read;

  char readbuf[BUFSIZE];

  if ((src_fd = open(srcpath, opensrc_flags)) == -1) return diefn("src_open");
  if ((dst_fd = open(dstpath, opendst_flags, opendst_mode)) == -1) return diefn("dst_open");

  while ((bytes_read = read(src_fd, readbuf, BUFSIZE)) > 0) {
    if (write(dst_fd, readbuf, bytes_read) == -1) return diefn("write");
  }

  close(src_fd);
  close(dst_fd);

  return 0;

}

int nftw_copy_target(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {

  /* handling typeflag logic */

  if (typeflag == FTW_F) {

    char *dest = get_new_path(fpath, srcpath, dstpath);

    if (opt_v) printf("I'm handling a file :3 hehe\n");

    if (!dest) diefn("nftw_copy_target");
    if (copy_file(fpath, dest) == -1) return diefn("nftw_copy_target");

  } else if (typeflag == FTW_D) {

    char *newpath = get_new_path(fpath, srcpath, dstpath);

    if (opt_v) printf("I'm handling a directory :3 hehe\n");
    if (mkdir(newpath, S_IRUSR | S_IWUSR | S_IXUSR) == -1) return diefn("mkdir");
  }

  return 0;
}

int main(int argc, char *argv[]) {

  struct stat dst_sb;

  int ftw_flags = FTW_PHYS;

  // TODO this check fucking sucks
  if (argc < 3) {
    fprintf(stderr, "Usage: cp [ARGS] file1 file2\n");
    return 1;
  }

  if ((process_opts(argc, argv)) == -1) return 1;

  strncpy(srcpath, argv[optind], strlen(argv[optind]));
  strncpy(dstpath, argv[optind + 1], strlen(argv[optind + 1]));

  errno = 0;

  if (stat(argv[optind + 1], &dst_sb) == -1) {
    if (errno != ENOENT) return die("stat");
  } else {
    if (S_ISDIR(dst_sb.st_mode)) {
      memset(dstpath, 0, sizeof(dstpath));
      snprintf(dstpath, sizeof(dstpath), "%s/%s", argv[optind + 1], argv[optind]);
    }
  }

  if (opt_r) {
    if (opt_v) printf("I am going recursive >:3\n");
    if (nftw(srcpath, nftw_copy_target, 20, ftw_flags) == -1)
      return diefn("nftw");
  } else {
    if (copy_file(srcpath, dstpath) == -1) return die("copy_file");
  }

  return 0;
}
