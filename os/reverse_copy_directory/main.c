#include "util.h"

int main(int argc, char **argv)
{
  if (argc == 1)
  {
    fprintf(stderr, "Error, no arguments provided\n");
    return EXIT_FAILURE;
  }
  struct FileMetaData srcDir, destDir, srcFile, destFile;
  AllocFileMetaData(&srcDir), AllocFileMetaData(&srcFile), AllocFileMetaData(&destDir), AllocFileMetaData(&destFile);
  strcpy(srcDir.path, argv[1]);
  srcDir.pathSize = strlen(srcDir.path);
  destDir.path = CreateReverseDirectory(&srcDir, &destDir);
  // printf("%s\n", destDir.path);
  if (!destDir.path || !ReverseCopyFiles(srcDir.path, destDir.name))
  {
    FreeFileMetaData(&srcDir), FreeFileMetaData(&srcFile), FreeFileMetaData(&destDir), FreeFileMetaData(&destFile);
    return EXIT_FAILURE;
  }
  FreeFileMetaData(&srcDir), FreeFileMetaData(&srcFile), FreeFileMetaData(&destDir), FreeFileMetaData(&destFile);
  return EXIT_SUCCESS;
}
