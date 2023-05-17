#define NOINLINE __attribute__ ((noinline))
UINT NOINLINE
opt_u1 (UINT x)
{
  if (x < (M * N) - GAP)
    return 0;
  UINT a = x - (M * N);
  UINT b = a / N;
  return b + M;
}

UINT NOINLINE
opt_u2 (UINT x)
{
  if (x > (UMAX - (M * N) + GAP))
    return 0;
  UINT a = x + (M * N);
  UINT b = a / N;
  return b - M;
}

INT NOINLINE
opt_s1 (INT x)
{
  if (x < (M * N) - GAP)
    return 0;
  INT a = x - (M * N);
  INT b = a / N;
  return b + M;
}

INT NOINLINE
opt_s2 (INT x)
{
  if (x < IMIN + (M * N) - GAP || x > 0)
    return 0;
  INT a = x - (M * N);
  INT b = a / N;
  return b + M;
}

INT NOINLINE
opt_s3 (INT x)
{
  if (x < (M * N) - GAP)
    return 0;
  INT a = x - (M * N);
  INT b = a / -N;
  return b + -M;
}

INT NOINLINE
opt_s4 (INT x)
{
  if (x < IMIN + (M * N) - GAP || x > 0)
    return 0;
  INT a = x - (M * N);
  INT b = a / -N;
  return b + -M;
}

INT NOINLINE
opt_s5 (INT x)
{
  if (x > (-M * N) + GAP)
    return 0;
  INT a = x - (-M * N);
  INT b = a / N;
  return b + -M;
}

INT NOINLINE
opt_s6 (INT x)
{
  if (x > IMAX - (M * N) + GAP || x < 0)
    return 0;
  INT a = x - (-M * N);
  INT b = a / N;
  return b + -M;
}

INT NOINLINE
opt_s7 (INT x)
{
  if (x > (M * -N) + GAP)
    return 0;
  INT a = x - (M * -N);
  INT b = a / -N;
  return b + M;
}

INT NOINLINE
opt_s8 (INT x)
{
  if (x > IMAX - (M * N) + GAP || x < 0)
    return 0;
  INT a = x - (M * -N);
  INT b = a / -N;
  return b + M;
}

UINT NOINLINE
opt_u3 (UINT x)
{
  if (x < (M << N) - GAP)
    return 0;
  UINT a = x - (M << N);
  UINT b = a >> N;
  return b + M;
}

UINT NOINLINE
opt_u4 (UINT x)
{
  if (x > (UMAX - (M << N)) + GAP)
    return 0;
  UINT a = x + (M << N);
  UINT b = a >> N;
  return b - M;
}

INT NOINLINE
opt_s9 (INT x)
{
  if (x < (M << N) - GAP)
    return 0;
  INT a = x - (M << N);
  INT b = a >> N;
  return b + M;
}

INT NOINLINE
opt_s10 (INT x)
{
  if (x < IMIN + (M << N) - GAP || x > 0)
    return 0;
  INT a = x - (M << N);
  INT b = a >> N;
  return b + M;
}

INT NOINLINE
opt_s11 (INT x)
{
  if (x > (-M << N) + GAP)
    return 0;
  INT a = x - (-M << N);
  INT b = a >> N;
  return b + -M;
}

INT NOINLINE
opt_s12 (INT x)
{
  if (x > IMAX - (M << N) + GAP || x < 0)
    return 0;
  INT a = x - (-M << N);
  INT b = a >> N;
  return b + -M;
}
