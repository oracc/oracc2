#include <dx.h>
#include <pwd.h>
#include <grp.h>
#include <osh.h>
#include <oraccsys.h>

static int oradmin_group(void);

/**osh_project validates the project/login relationship and changes
  directory to the project.

  There are two classes of login which are recognized: project-users
  and oradmin.

  For project-users, the login occurs via ssh and the login is only
  allowed to operate on the main project or subprojects thereof.

  For oradmin, the login may occur via ssh or by invoking osh from the
  command line. In either case the user logging in must be a member
  of group oradmin.

  The project must always be given as the first non-option argument in
  the arguments supplied to osh.
 */
int
osh_project(char **optv)
{
  uid_t uid = getuid();
  struct passwd *pwd;
  char *rname = NULL;
  char *projecthome = NULL;
  char *projectworkdir = NULL;
  if ((pwd = getpwuid(uid)))
    {
      rname = strdup(pwd->pw_name);

      int oradmintrue = oradmin_group();

      if (oradmintrue < 0)
	goto error;
      
      if (verbose)
	fprintf(stderr, "%s: user %s%s requesting %s in project %s\n",
		progname,
		rname,
		oradmintrue ? " (group oradmin)" : "",
		optv[1], optv[0]);
      
      if (!oradmintrue)
	{
	  /* For a project user the project must start with the rname;
	     you're only allowed to work on your own project or
	     subproject */
	  int namelen = strlen(rname);
	  if (strncmp(optv[0], rname, namelen)
	      || (optv[0][namelen] && '/' != optv[0][namelen]))
	    {
	      fprintf(stderr, "%s: project-users can only work in their own projects (%s vs %s)\n",
		      progname, rname, optv[0]);
	      goto error;
	    }
	  projecthome = strdup(rname);
	}
      else
	projecthome = strdup(optv[0]);

      /* For any user the project home (e.g., /home/oracc/ogsl) must also exist */
      char projdir[strlen(oracc_builds())+strlen(projecthome)+2];
      sprintf(projdir, "%s/%s", oracc_builds(), projecthome);
      struct stat st;
      if (stat(projdir, &st))
	{
	  fprintf(stderr, "%s: project directory %s failed stat\n", progname, projdir);
	  goto error;
	}

      /* And it must be a directory */
      if ((st.st_mode&S_IFMT) != S_IFDIR)
	{
	  fprintf(stderr, "%s: project directory %s is not a directory!\n", progname, projdir);
	  goto error;
	}

      /* If the project arg is a subproject that must also exist */
      if (strchr(optv[0], '/'))
	{
	  char subprojdir[strlen(oracc_builds())+strlen(optv[0])+1];
	  sprintf(subprojdir, "%s/%s", oracc_builds(), optv[0]);
	  if (stat(subprojdir, &st))
	    {
	      fprintf(stderr, "%s: subproject directory %s failed stat\n", progname, subprojdir);
	      goto error;
	    }
	      
	  /* And it must be a directory */
	  if ((st.st_mode&S_IFMT) != S_IFDIR)
	    {
	      fprintf(stderr, "%s: subproject directory %s is not a directory!\n", progname, subprojdir);
	      goto error;
	    }	      
	  projectworkdir = strdup(subprojdir);
	}
      else
	projectworkdir = strdup(projdir);
	  
      /* TO DO: check access of projectworkdir */

      /* Now we can set the CWD for the command */
      if (chdir(projectworkdir) < 0)
	{
	  fprintf(stderr, "%s: failed to change directory to %s\n", progname, projectworkdir);
	  goto error;
	}
      if (verbose)
	fprintf(stderr, "%s: current working directory is now %s\n", progname, projectworkdir);
    }
  else
    {
      fprintf(stderr, "%s: failed to get user name from uid %ld\n", progname, (long)uid);
      goto error;
    }
#define cleanup() \
  if (rname) free(rname); \
  if (projecthome) free(projecthome); \
  if (projectworkdir) free(projectworkdir)
  cleanup();
  return 0;
 error:
  cleanup();
  return 1;
}

/**oradmin_group: return 1 if caller is in group oradmin; 0 if not; -1 on error
 */
static int
oradmin_group(void)
{
  gid_t dummy[1];
  int i, n = getgroups(0, dummy);
  gid_t glist[n];
  if ((n == getgroups(n, glist)))
    {
      for (i = 0; i < n; ++i)
	{
	  struct group *gp = getgrgid(glist[i]);
	  if (gp && !strcmp(gp->gr_name, "oradmin"))
	    return 1;
	}
    }
  else
    {
      perror("getgroups failed");
      return -1;
    }
  return 0;
}
