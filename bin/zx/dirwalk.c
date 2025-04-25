#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

void
walk_directory(const char *path)
{
  DIR *dir = opendir(path);
  if (dir == NULL)
    {
      perror("opendir");
      return;
    }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL)
    {
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
	continue;
      }
      
      char full_path[1024];
      snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
      
      struct stat statbuf;
      if (stat(full_path, &statbuf) == 0)
	{
	  if (S_ISDIR(statbuf.st_mode)) {
	    printf("Directory: %s\n", full_path);
	    walk_directory(full_path);
	  } else {
	    printf("File: %s\n", full_path);
	  }
        }
    }
  closedir(dir);
}

int main()
{
  walk_directory(".");
  return 0;
}
