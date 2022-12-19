#include <sys/stat.h>  /* For mkdir + permission bits.  */
#include <unistd.h>  /* For rmdir.  */
#include <errno.h>  /* For errno.  */
#include <stdio.h>  /* For perror.  */
#include <stdlib.h>  /* For abort.  */
 

void
my_mkdir (const char *dir)
{
  int err;
  struct stat path_stat;

  /* Check whether 'dir' exists and is a directory.  */
  err = stat (dir, &path_stat);
  if (err && errno != ENOENT)
    {
      perror ("my_mkdir: failed to call stat for directory");
      abort ();
    }
  if (err == 0 && !S_ISDIR (path_stat.st_mode))
    {
      printf ("my_mkdir: pathname %s is not a directory\n", dir);
      abort ();
    }

  err = mkdir (dir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
  if (err != 0)
    {
      perror ("my_mkdir: failed to create directory");
      abort ();
    }    
}

void
my_rmdir (const char *dir)
{
  rmdir (dir);
}
