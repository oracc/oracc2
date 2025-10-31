#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

char **
dirarray (const char *path, int *count)
{
  DIR *dir;
  struct dirent *entry;
  char **files = NULL;
  int file_count = 0;
  int capacity = 10;

  if (count == NULL)
    {
      return NULL;
    }
  *count = 0;

  dir = opendir (path);
  if (dir == NULL)
    {
      perror ("opendir failed");
      return NULL;
    }

  files = (char **) malloc (capacity * sizeof (char *));
  if (files == NULL)
    {
      perror ("malloc failed");
      closedir (dir);
      return NULL;
    }

  while ((entry = readdir (dir)) != NULL)
    {
      if (strcmp (entry->d_name, ".") == 0
	  || strcmp (entry->d_name, "..") == 0)
	{
	  continue;
	}

      if (file_count >= capacity)
	{
	  capacity *= 2;
	  char **temp_files =
	    (char **) realloc (files, capacity * sizeof (char *));
	  if (temp_files == NULL)
	    {
	      perror ("realloc failed");
	      free(files);
	      closedir (dir);
	      return NULL;
	    }
	  files = temp_files;
	}

      files[file_count] = (char *) malloc (strlen (entry->d_name) + 1);
      if (files[file_count] == NULL)
	{
	  perror ("malloc failed for filename");
	  free(files);
	  closedir (dir);
	  return NULL;
	}
      strcpy (files[file_count], entry->d_name);
      file_count++;
    }

  closedir (dir);
  *count = file_count;

  return files;
}
