/* { dg-options "-O2 -fdbg-cnt=sched_block:1" } */
/* { dg-prune-output {\*\*\*dbgcnt:.*limit.*reached} } */

/* Verify there is no ICE.  */

int a, b, c, e, f;
float d;

void
g ()
{
  float h, i[1];
  for (; f;)
    if (c)
      {
	d *e;
	if (b)
	  {
	    float *j = i;
	    j[0] += 0;
	  }
	h += d;
      }
  if (h)
    a = i[0];
}
