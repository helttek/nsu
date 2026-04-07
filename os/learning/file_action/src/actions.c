#include "actions.h"
#include "util.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int touch_dir(const char *dir_name)
{
  struct stat st = {0};
  if (stat(dir_name, &st) == -1)
  {
    perror(strerror(errno));

    struct stat pst = {0};
    char dest[255];
    if (!get_parent_path(dir_name, dest))
    {
      printf("Warning: failed to get parent path from directory path\n");
      if (mkdir(dir_name, 0777))
      {
        perror(strerror(errno));
        return 0;
      }
      return 1;
    }

    if (stat(dest, &pst) == -1)
    {
      if (mkdir(dir_name, 0777))
      {
        perror(strerror(errno));
        return 0;
      }
    }
    else
    {
      if (mkdir(dir_name, pst.st_mode))
      {
        perror(strerror(errno));
        return 0;
      }
    }
  }
  printf("Directory %s already exists.\n", dir_name);
  return 1;
}

int ls_dir(const char *dir_name)
{
  DIR *dir = opendir(dir_name);
  if (!dir)
  {
    perror(strerror(errno));
    return 0;
  }
  struct dirent *d;
  errno = 0;
  while ((d = readdir(dir)) != NULL)
  {
    switch (d->d_type)
    {
    case DT_REG:
      printf("-");
      break;

    case DT_DIR:
      printf("d");
      break;

    case DT_FIFO:
      printf("p");
      break;

    case DT_SOCK:
      printf("s");
      break;

    default:
      break;
    }
    printf(" %s\n", d->d_name);
  }
  if (errno)
  {
    perror(strerror(errno));
    return 0;
  }
  return 1;
}

int rm_dir(const char *dir_name)
{
  if (rmdir(dir_name))
  {
    perror(strerror(errno));
    return 0;
  }
  return 1;
}

int touch_file(const char *file_name)
{
  struct stat st = {0};
  if (!fopen(file_name, "w"))
  {
    perror(strerror(errno));
    return 0;
  }
  return 1;
}

int cat_file(const char *file_name)
{
  const int BUF_SIZE = 256;
  const int fd = fopen(file_name, "r");
  if (!fd)
  {
    perror(strerror(errno));
    return 0;
  }

  char buf[BUF_SIZE];
  size_t ret;
  while (!(ret = fread(buf, sizeof(char), BUF_SIZE, fd)))
  {
    // write_n(fd, buf, BUF_SIZE);
  }
}

int rm_file(const char *file_name)
{
  if (remove(file_name))
  {
    perror(strerror(errno));
    return 0;
  }
  return 1;
}

int touch_slink(const char *slink_file_name)
{
  char slink_name[256];
  char postfix[7] = "_slink";

  if (!add_postfix(slink_file_name, strlen(slink_file_name), slink_name, 256, postfix, 7))
  {
    printf("Failed to add postfix, when creating symlink\n");
    return 0;
  }

  if (symlink(slink_file_name, slink_name))
  {
    perror(strerror(errno));
    return 0;
  }

  return 1;
}

int cat_slink(const char *slink_name) {}

int cat_slink_file(const char *slink_name) {}

int rm_slink(const char *slink_name) {}

int touch_hlink(const char *hlink_name) {}

int ch_perm(const char *hlink_name) {}

int cat_perm(const char *hlink_name) {}

int rm_hlink(const char *hlink_name) {}
