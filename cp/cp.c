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

/*** FLAG PROCESSING ***/

#define CP_OPTSTR "sfrvP"

#define OPT_RECURSIVE 'r'
#define OPT_FORCE 'f'
#define OPT_VERBOSE 'v'
#define OPT_SYMLINK 's'
#define OPT_NODEREF 'P'

int opt_recursive = 0; /* recursive mode  DONE*/
int opt_verbose = 0; /* verbose  DONE */
int opt_noderef = 0; /* no-dereference  DONE */
int opt_symlink = 0; /* make symbolic links instead of copying  DONE */
int opt_force = 0; /* force: if an existing destination file cannot be
                                 opened, remove it and try again (this option
                                 is ignored when the -n option is also used)  DONE */

/*** GLOBALS ***/
char srcpath[PATH_MAX];
char dstpath[PATH_MAX];

int process_opts(int argc, char *argv[]) {

  int opt;

  while ((opt = getopt(argc, argv, CP_OPTSTR)) != -1) {
    switch (opt) {
      case OPT_RECURSIVE:
        opt_recursive = 1;
        break;
      case OPT_FORCE:
        opt_force = 1;
        break;
      case OPT_VERBOSE:
        opt_verbose = 1;
        break;
      case OPT_SYMLINK:
        opt_symlink = 1;
        break;
      case OPT_NODEREF:
        opt_noderef = 1;
        break;
      default:
        fprintf(stderr, "%c: not a valid option\n", optopt);
        return -1;
    }
  }

  return 0; 
}

char *get_new_path(char *old_path, char *old_prefix, char *new_prefix) {

  if (strncmp(old_path, old_prefix, strlen(old_prefix)) != 0) return NULL;

  ssize_t new_len = strlen(new_prefix) + strlen(old_path) - strlen(old_prefix) + 2;
  char *new_path = malloc(new_len);

  if (!new_path) return NULL;

  snprintf(new_path, new_len, "%s/%s", new_prefix, old_path + (strlen(old_prefix) + 1));
  return new_path;
}

int copy_file(char *srcpath, char *dstpath) {

  int opensrc_flags = O_RDONLY | O_LARGEFILE;
  int opendst_flags = O_WRONLY | O_TRUNC | O_CREAT | O_LARGEFILE;
  int opendst_mode = S_IRUSR | S_IWUSR ;

  int src_fd, dst_fd;
  int bytes_read;

  char readbuf[BUFSIZE];

  src_fd = open(srcpath, opensrc_flags);
  dst_fd = open(dstpath, opendst_flags, opendst_mode);

  if (src_fd == -1) return diefn("src_open");
  if (dst_fd == -1) return -2; /* -2 for the purpose of distinguishing src and dst failure */

  while ((bytes_read = read(src_fd, readbuf, BUFSIZE)) > 0) {
    if (write(dst_fd, readbuf, bytes_read) == -1) return diefn("write");
  }

  close(src_fd);
  close(dst_fd);

  return 0;

}

int copy_symlink(char *src, char *dst) {

  /* Creates a symlink at dst that points to file pointed to by symlink at src. */

  char srcref[PATH_MAX + NULLB];
  int nread;

  if ((nread = readlink(src, srcref, PATH_MAX)) == -1) return diefn("copy_symlink");
  srcref[nread] = '\0';

  if (symlink(srcref, dst) == -1) return diefn("symlink");

  return 0;
}

int nftw_copy_target(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {

  /* 
  Executed by nftw during recursive copying. Handles tree structure and handling of individual nodes (files)
  based on their type.
  */

  char *dest = get_new_path(fpath, srcpath, dstpath);
  if (!dest) return diefn("nftw_copy_target");

  if (opt_verbose) printf("\'%s\' -> \'%s\'\n", fpath, dest);

  if (typeflag == FTW_F) {

    if (!dest) diefn("nftw_copy_target");
    if (copy_file(fpath, dest) == -2) {
      if (opt_force) {
        if (remove(dest) == -1) return diefn("unlink");
        if (copy_file(fpath, dest) < 0) return diefn("force");
      } else {
        return diefn("nftw_copy_target");
      }
    }

  } else if (typeflag == FTW_D) {

    if (mkdir(dest, S_IRUSR | S_IWUSR | S_IXUSR) == -1) return diefn("mkdir");

  } else if (FTW_SL) {
    if (opt_noderef) {
      if (copy_symlink(fpath, dest) == -1) return diefn("FTW_SL");
    } else {
      if (copy_file(fpath, dest) == -1) return diefn("nftw_copy_target");
    }
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

  if (opt_recursive) {
    if (nftw(srcpath, nftw_copy_target, 20, ftw_flags) == -1)
      return diefn("nftw");
  } else {
    if (opt_noderef) {
      if (copy_symlink(srcpath, dstpath) == -1) return die("copy_symlink");
    } else if (opt_symlink) {
      if (symlink(srcpath, dstpath) == -1) return die("symlink_target");
    } else {
      if (copy_file(srcpath, dstpath) == -1) return die("copy_file");
    }
  }

  return 0;
}
