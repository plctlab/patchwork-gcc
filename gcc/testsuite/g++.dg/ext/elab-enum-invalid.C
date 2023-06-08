// { dg-do compile { target c++11 } }
typedef long CFIndex;
typedef enum CFComparisonResult : CFIndex CFComparisonResult;
// { dg-error "declaration of enumeration with fixed underlying type" "" {target *-*-*} .-1 }
