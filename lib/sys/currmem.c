#include <stdio.h>
#include <unistd.h>

#ifdef linux
int i_am_linux = 1;
#else
int i_am_linux = 0;
#endif

void
show_current_memory (FILE *fp, unsigned long *vm_arg, unsigned long *rss_arg)
{
  FILE *file = fopen ("/proc/self/statm", "r");
  if (!file)
    {
      perror ("Failed to open /proc/self/statm");
      return;
    }

  unsigned long vm_pages = 0;
  unsigned long rss_pages = 0;

  // Read the first two space-separated fields
  if (fscanf (file, "%lu %lu", &vm_pages, &rss_pages) != 2)
    {
      fclose (file);
      return;
    }
  fclose (file);

  // Get the system page size in bytes (usually 4096 bytes)
  long page_size_bytes = sysconf (_SC_PAGESIZE);

  // Convert pages to total Megabytes (MB)
  unsigned long vm, rss;
  vm = (vm_pages * page_size_bytes) / (1024 * 1024);
  rss = (rss_pages * page_size_bytes) / (1024 * 1024);

  if (vm_arg)
    *vm_arg = vm;
  if (rss_arg)
    *rss_arg = rss;

  if (fp)
    fprintf(fp, "Current Memory:  Virtual=%lu MB; Physical RAM (RSS): %lu MB\n", vm, rss);
}
