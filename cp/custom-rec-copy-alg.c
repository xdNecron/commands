#define _LARGEFILE64_SOURCE
#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

#define BUFSIZE 1024
#define C_VERBOSE 0x01

int copy(char *pathname, char *dstname, int flags) {

  DIR *dir = opendir(pathname);
  struct dirent *entry;
  struct stat st;

  if (stat(pathname, &st) == -1) {  // derefs symlinks for now
    perror("fstat");
    return -1;
  }

  if (!dir && errno == ENOTDIR) {

    int src_fd;
    int dst_fd;
    int dst_flags = O_WRONLY | O_TRUNC | O_CREAT | O_EXCL;
    ssize_t bytes_read;

    char buf[BUFSIZE];

    if ((src_fd = open(pathname, O_RDONLY | O_LARGEFILE)) == -1) {
      perror("src_fd");
      return -1;
    }

    if ((dst_fd = open(dstname, dst_flags, st.st_mode) == -1)) {
      perror("dst_fd");
      close(src_fd);
      return -1;
    }


    while ((bytes_read = read(src_fd, buf, BUFSIZE)) != 0) {
      if (bytes_read == -1) {
        perror("read");
        close(src_fd);
        close(dst_fd);
        return -1;
      }
      if (write(dst_fd, buf, bytes_read) != bytes_read) {
        perror("write");
        close(src_fd);
        close(dst_fd);
        return -1;
      }
    }

    close(src_fd);
    close(dst_fd);

    if (flags & C_VERBOSE) printf("\'%s\' -> \'%s\'\n", pathname, dstname);

    return 0;

  } else {
    if (mkdir(dstname, st.st_mode) == -1) {
      perror("mkdir");
      if (dir) closedir(dir);
      return -1;
    }

    if (flags & C_VERBOSE) printf("\'%s\' -> \'%s\'\n", pathname, dstname);
  }

  // while there are children to be copied
  while ((entry = readdir(dir)) != NULL) {

    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {

      char *new_pathname = malloc(strlen(pathname) + strlen(entry->d_name) + 2);
      char *new_dstname = malloc(strlen(dstname) + strlen(entry->d_name) + 2);

      sprintf(new_pathname, "%s/%s", pathname, entry->d_name);
      sprintf(new_dstname, "%s/%s", dstname, entry->d_name);

      if ((copy(new_pathname, new_dstname, flags)) == -1) {
        free(new_pathname);
        free(new_dstname);
        closedir(dir);
        return -1;
      }

      free(new_pathname);
      free(new_dstname);
    }
  }

  closedir(dir);

  return 0;
}

int main(int argc, char *argv[]) {

  if (argc < 3) {
    fprintf(stderr, "Usage: ./run <src> <dst>\n");
    exit(EXIT_FAILURE);
  }

  int result = copy(argv[1], argv[2], C_VERBOSE);
  if (result == -1) exit(EXIT_FAILURE);

  return 0;
}
