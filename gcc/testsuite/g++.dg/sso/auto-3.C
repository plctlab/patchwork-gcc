/* { dg-do compile { target c++14 }  } */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
struct __attribute__((scalar_storage_order("big-endian"))) Rec
{
  int p;
};
#else
struct __attribute__((scalar_storage_order("little-endian"))) Rec
{
  int p;
};
#endif

decltype(auto) f(Rec *a)
{
  return (a->p); /* { dg-error "" } */
}
