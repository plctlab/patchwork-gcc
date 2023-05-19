/* PR libgomp/109875  */
/* { dg-set-target-env-var OMP_NUM_TEAMS_ALL 7 } */
/* { dg-set-target-env-var OMP_NUM_TEAMS 4 } */
/* { dg-set-target-env-var OMP_NUM_TEAMS_DEV 8 } */
/* { dg-set-target-env-var OMP_NUM_TEAMS_DEV_0 5 } */
/* { dg-set-target-env-var OMP_NUM_TEAMS_DEV_1 11 } */

#include "teams-nteams-icv-1.c"
