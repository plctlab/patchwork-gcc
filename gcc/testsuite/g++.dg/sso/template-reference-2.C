/* { dg-do compile } */
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
template <typename T> void g(T &);


void f(Rec *a)
{
  g(a->p); /* { dg-error "" } */
}
