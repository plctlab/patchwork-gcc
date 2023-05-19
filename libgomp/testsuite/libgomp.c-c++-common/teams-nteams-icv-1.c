/* Check that the nteams ICV is honored. */
/* PR libgomp/109875  */

/*  This base version of testcases is supposed to be run with all
    OMP_NUM_TEAMS* env vars being unset.

    The variants teams-nteams-icv-{2,3,4}.c test it by setting the
    various OMP_NUM_TEAMS* env vars. However, as DejaGNU's remote testing
    does not handle dg-set-target-env-var, the testcase has been written
    such that it will still work in that case.

    Once fixed in DejaGNU, the getenv could be replaced by using #define'd
    values.  */

/* OpenMP currently has:
   - nteams-var ICV is initialized to 0; one ICV per device
   - OMP_NUM_TEAMS(_DEV(_<dev-num>)) overrides it
     OMP_NUM_TEAMS_ALL overrides it
   - Number of teams is:
     -> the value specific by num_teams([lower:]upper)
	with lower := upper if unspecified
     -> Otherwise, if nteams-var ICV > 0, #teams <= nteams-var ICV
     -> Otherwise, if nteams-var ICV <= 0, #teams > 1
 GCC uses 3 as default on the host and 1 for host fallback.
 For offloading, it is device specific >> 1.  */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int
main ()
{
  int num_teams_env = -1, num_teams_env_dev = -1, *num_teams_env_devs = NULL;
  const char *env_s;

  /* Get the OMP_NUM_TEAMS environment variables.  */

  env_s = getenv ("OMP_NUM_TEAMS_ALL");
  if (env_s)
    {
      num_teams_env = num_teams_env_dev = atoi (env_s);
      printf ("DEBUG: OMP_NUM_TEAMS_ALL='%s' -> nteams-var = %d\n",
	      env_s, num_teams_env);
    }
  env_s = getenv ("OMP_NUM_TEAMS");
  if (env_s)
    {
      num_teams_env = atoi (env_s);
      printf ("DEBUG: OMP_NUM_TEAMS='%s' -> nteams-var = %d\n",
	      env_s, num_teams_env);
    }
  env_s = getenv ("OMP_NUM_TEAMS_DEV");
  if (env_s)
    {
      num_teams_env_dev = atoi (env_s);
      printf ("DEBUG: OMP_NUM_TEAMS_DEV='%s' -> nteams-var = %d\n",
	      env_s, num_teams_env_dev);
    }
  if (omp_get_num_devices () > 0)
    {
      num_teams_env_devs = (int*) malloc (sizeof (int) * omp_get_num_devices ());
      for (int i = 0; i < omp_get_num_devices (); i++)
	{
	  char tmp[18+5+1];
	  snprintf (tmp, sizeof (tmp), "OMP_NUM_TEAMS_DEV_%d", i);
	  env_s = getenv (tmp);
	  if (env_s)
	    {
	      num_teams_env_devs[i] = atoi (env_s);
	      printf ("DEBUG: %s='%s' -> nteams-var = %d\n",
		      tmp, env_s, num_teams_env_devs[i]);
	    }
	  else if (num_teams_env_dev > 0)
	    num_teams_env_devs[i] = num_teams_env_dev;
	  else
	    num_teams_env_devs[i] = -1;
	}
    }

  /* Check that the number of teams (initial device and in target) is
     >= 1 and, if omp_get_max_teams() > 0, it does not
     exceed omp_get_max_teams (). */

  int nteams, num_teams;

  /* Assume that omp_get_max_teams (); returns the ICV, i.e. 0 as default init
     and not the number of teams that would be run; hence: '>='.  */
  nteams = omp_get_max_teams ();
  if (nteams < 0 || (num_teams_env >= 0 && nteams != num_teams_env))
    abort ();
  num_teams = -1;

  #pragma omp teams
   if (omp_get_team_num () == 0)
     num_teams = omp_get_num_teams ();
  if (num_teams < 1 || (nteams > 0 && num_teams > nteams))
    abort ();

  /* GCC hard codes 3 teams - check for it.  */
  if (nteams <= 0 && num_teams != 3)
    abort ();

  /* For each device, including host fallback.  */
  for (int dev = 0; dev <= omp_get_num_devices (); dev++)
    {
      int num_teams_icv = ((dev == omp_get_num_devices ())
			   ? num_teams_env : num_teams_env_devs[dev]);
      nteams = -1;
      #pragma omp target device(dev) map(from: nteams)
	nteams = omp_get_max_teams ();
      if (nteams < 0 || (num_teams_icv >= 0 && nteams != num_teams_icv))
	abort ();

      num_teams = -1;
      #pragma omp target teams device(dev) map(from: num_teams)
	if (omp_get_team_num () == 0)
	  num_teams = omp_get_num_teams ();

      if (num_teams < 1 || (nteams > 0 && num_teams > nteams))
	abort ();

      /* GCC hard codes 1 team for host fallback - check for it.  */
      if (dev == omp_get_num_devices () && num_teams != 1)
	abort ();
    }

  /* Now set the nteams-var ICV and check that omp_get_max_teams()
     returns the set value and that the following holds:
     num_teams >= 1 and num_teams <= nteams-var ICV.

     Additionally, implementation defined, assume:
     - num_teams == (not '<=') nteams-var ICV, except:
     - num_teams == 1 for host fallback.  */

  omp_set_num_teams (5);

  nteams = omp_get_max_teams ();
  if (nteams != 5)
    abort ();
  num_teams = -1;

  #pragma omp teams
   if (omp_get_team_num () == 0)
     num_teams = omp_get_num_teams ();
  if (num_teams != 5)
    abort ();

  /* For each device, including host fallback.  */
  for (int dev = 0; dev <= omp_get_num_devices (); dev++)
    {
      #pragma omp target device(dev) firstprivate(dev)
	omp_set_num_teams (7 + dev);

      #pragma omp target device(dev) map(from: nteams)
	nteams = omp_get_max_teams ();
      if (nteams != 7 + dev)
	abort ();

      num_teams = -1;
      #pragma omp target teams device(dev) map(from: num_teams)
	if (omp_get_team_num () == 0)
	  num_teams = omp_get_num_teams ();

      if (dev == omp_get_num_devices ())
	{
	  if (num_teams != 1)
	    abort ();
	}
      else
	{
	  if (num_teams != 7 + dev)
	    abort ();
	}
    }

  /* Now use the num_teams clause explicitly.  */

  num_teams = -1;
  #pragma omp teams num_teams(6)
   if (omp_get_team_num () == 0)
     num_teams = omp_get_num_teams ();
  if (num_teams != 6)
    abort ();

  /* For each device, including host fallback.  */
  for (int dev = 0; dev <= omp_get_num_devices (); dev++)
    {
      num_teams = -1;
      #pragma omp target teams device(dev) map(from: num_teams) num_teams(dev+3)
	if (omp_get_team_num () == 0)
	  num_teams = omp_get_num_teams ();

      /* This must match the set value, also with host fallback.  */
      if (num_teams != 3 + dev)
	abort ();
    }

  free (num_teams_env_devs);
  return 0;
}
