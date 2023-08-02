/* { dg-do run } */
/* { dg-options "-O1 -save-temps" } */
/* { dg-require-effective-target lp64 } */
/* { dg-additional-options "-mbranch-protection=bti+all" { target { ! default_branch_protection } } } */

static int __attribute__((noinline))
func(int w) {
        return 37;
}

int __attribute__((section(".main.text")))
main(int argc, char **argv)
{
        return func(argc) == 37 ? 0 : 1;
}

/* { dg-final { scan-assembler-times "hint\t34" 2 } } */
