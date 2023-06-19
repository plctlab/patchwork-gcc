/* { dg-options "-O2 -fdump-ipa-fnsummary"  } */
int
test(int a)
{
	if (a > 10)
		__builtin_unreachable ();
}
/* { dg-final { scan-ipa-dump "Conditional guarding __builtin_unreachable" "fnsummary"  } } */
