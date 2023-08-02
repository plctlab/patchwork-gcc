#ifdef __x86_64__
extern int do_spin_for_count (int *, int, unsigned long long) ;
#else
# include "../spin.h"
#endif
