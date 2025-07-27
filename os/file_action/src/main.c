#include <stdlib.h>
#include <stdio.h>

#include "actions.h"
#include "util.h"

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("Error: no argument provided.\n");
    return EXIT_FAILURE;
  }

  if (!argcmp(argv[0], "touch_dir"))
  {
    if (!touch_dir(argv[1]))
    {
      printf("Error: failed to create directory %s\n", argv[1]);
      return EXIT_FAILURE;
    }
    printf("Directory created successfully.\n");
    return EXIT_SUCCESS;
  }
  if (!argcmp(argv[0], "ls_dir"))
  {
    if (!ls_dir(argv[1]))
    {
      printf("Error: failed to list directory %s\n", argv[1]);
      return EXIT_FAILURE;
    }
    printf("Directory listed successfully.\n");
    return EXIT_SUCCESS;
  }
  if (!argcmp(argv[0], "rm_dir"))
  {
    if (!rm_dir(argv[1]))
    {
      printf("Error: failed to remove directory %s\n", argv[1]);
      return EXIT_FAILURE;
    }
    printf("Directory removed successfully.\n");
    return EXIT_SUCCESS;
  }

  // if (!argcmp(argv[0], "touch_file"))
  // {
  //   if (!touch_file(argv[1]))
  //   {
  //     printf("Error: failed to create file %s\n", argv[1]);
  //     return EXIT_FAILURE;
  //   }
  //   printf("File created successfully.\n");
  //   return EXIT_SUCCESS;
  // }
  // if (!argcmp(argv[0], "cat_file"))
  // {
  //   if (!cat_file(argv[1]))
  //   {
  //     printf("Error: failed to list contents of file %s\n", argv[1]);
  //     return EXIT_FAILURE;
  //   }
  //   printf("File contents printed successfully.\n");
  //   return EXIT_SUCCESS;
  // }
  // if (!argcmp(argv[0], "rm_file"))
  // {
  //   if (!rm_file(argv[1]))
  //   {
  //     printf("Error: failed to remove file %s\n", argv[1]);
  //     return EXIT_FAILURE;
  //   }
  //   printf("File removed successfully.\n");
  //   return EXIT_SUCCESS;
  // }

  // if (!argcmp(argv[0], "touch_slink"))
  // {
  //   if (!touch_slink(argv[1]))
  //   {
  //     printf("Error: failed to create symlink %s\n", argv[1]);
  //     return EXIT_FAILURE;
  //   }
  //   printf("Symlink created successfully.\n");
  //   return EXIT_SUCCESS;
  // }
  // if (!argcmp(argv[0], "cat_slink"))
  // {
  //   if (!cat_slink(argv[1]))
  //   {
  //     printf("Error: failed to list contents of symlink %s\n", argv[1]);
  //     return EXIT_FAILURE;
  //   }
  //   printf("Symlink contents printed successfully.\n");
  //   return EXIT_SUCCESS;
  // }
  // if (!argcmp(argv[0], "cat_slink_file"))
  // {
  //   if (!cat_slink_file(argv[1]))
  //   {
  //     printf("Error: failed to list file contents of symlink %s\n", argv[1]);
  //     return EXIT_FAILURE;
  //   }
  //   printf("Symlink file printed successfully.\n");
  //   return EXIT_SUCCESS;
  // }
  // if (!argcmp(argv[0], "rm_slink"))
  // {
  //   if (!rm_slink(argv[1]))
  //   {
  //     printf("Error: failed to remove symlink %s\n", argv[1]);
  //     return EXIT_FAILURE;
  //   }
  //   printf("Symlink removed successfully.\n");
  //   return EXIT_SUCCESS;
  // }

  // if (!argcmp(argv[0], "touch_hlink"))
  // {
  //   if (!touch_hlink(argv[1]))
  //   {
  //     printf("Error: failed to create hardlink %s\n", argv[1]);
  //     return EXIT_FAILURE;
  //   }
  //   printf("Hardlink created successfully.\n");
  //   return EXIT_SUCCESS;
  // }
  // if (!argcmp(argv[0], "cat_perm"))
  // {
  //   if (!cat_perm(argv[1]))
  //   {
  //     printf("Error: failed to list file permissions %s\n", argv[1]);
  //     return EXIT_FAILURE;
  //   }
  //   printf("File permission listed successfully.\n");
  //   return EXIT_SUCCESS;
  // }
  // if (!argcmp(argv[0], "ch_perm"))
  // {
  //   if (!ch_perm(argv[1]))
  //   {
  //     printf("Error: failed to change file permissions %s\n", argv[1]);
  //     return EXIT_FAILURE;
  //   }
  //   printf("File permissions changed successfully.\n");
  //   return EXIT_SUCCESS;
  // }
  // if (!argcmp(argv[0], "rm_hlink"))
  // {
  //   if (!rm_hlink(argv[1]))
  //   {
  //     printf("Error: failed to remove hardlink\n");
  //     return EXIT_FAILURE;
  //   }
  //   printf("Hardlink removed successfully.\n");
  //   return EXIT_SUCCESS;
  // }

  printf("Error: unknown file operation.\n");
  return EXIT_FAILURE;
}
