#include <stdio.h>

#ifdef linux
#include <unistd.h>

void
currmem_linux(FILE *fp)
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
#else

#include <mach/mach.h>
#include <stdio.h>

void
currmem_macos(FILE *fp)
{
  task_vm_info_data_t vm_info;
  mach_msg_type_number_t count = TASK_VM_INFO_COUNT;

  /* Request virtual memory info for the current Mach task */
  kern_return_t result = task_info (mach_task_self (),
				    TASK_VM_INFO,
				    (task_info_t) & vm_info,
				    &count);
  
  if (result == KERN_SUCCESS)
    {
      /* phys_footprint is returned in bytes */
      uint64_t footprint_bytes = vm_info.phys_footprint;
      double footprint_mb = (double) footprint_bytes / (1024.0 * 1024.0);
      fprintf (fp, "Current Memory: Physical = %.2f MiB\n", footprint_mb);
    }
  else
    {
      fprintf (stderr, "Error retrieving task info: %s\n",
	       mach_error_string (result));
    }
}
#endif

#if 0
#include "currmem.h"
int
main (int argc, const char *argv[])
{
  currmem(stderr);
  return 0;
}
#endif
