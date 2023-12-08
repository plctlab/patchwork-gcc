// PR c++/108286
// { dg-do run }
// { dg-additional-options "-fdump-tree-gimple" }

struct S {
  int
  foo ()
  {
    int res = 0;
#pragma omp target map(size, ptr[:size], res) nowait
    res = ptr[size - 1];
#pragma omp taskwait
    return res;
  }

  unsigned size;
  int *ptr;
};

int
main ()
{
  S s;
  int buf[5];
  s.size = 5;
  s.ptr = buf;
  buf[4] = 42;
  if (s.foo () != 42)
    __builtin_abort ();
}

/* Ensure that 'this' is mapped but only once and not additionally via 'this[:0]'.  */
/* { dg-final { scan-tree-dump "#pragma omp target num_teams\\(-2\\) thread_limit\\(0\\) map\\(tofrom:\\*this \\\[len: \[0-9\]+\\\]\\) map\\(firstprivate:this \\\[pointer assign, bias: 0\\\]\\) nowait map\\(tofrom:res \\\[len: \[0-9\]+\\\]\\) map\\(tofrom:\\*_\[0-9\]+ \\\[len: _\[0-9\]+\\\]\\) map\\(attach:this->ptr \\\[bias: 0\\\]\\)\[\r\n\]" "gimple" } } */
