// PR c++/12341 test anonymous bit field
/* { dg-do compile } */
/* { dg-options -Wshadow } */

class B {
protected:
  unsigned int x : 5;
  unsigned int : 2;
  unsigned int y : 1;

  union {
    float uuu;
    double vvv;
  };
};

// Check that anonymous bit fields do not cause spurious warnings
class D : private B {
protected:
  unsigned int x : 4; /* { dg-warning "'D::x' might shadow 'B::x'" } */
  unsigned int : 4; // If test for excess errors fails, it might be from here.
  int uuu; /* { dg-warning "'D::uuu' might shadow 'B::<unnamed union>::uuu'" } */
};

class E : public D {
public:
  /* Do not warn if inheritance is not visible to the class,
  as there is no risk even with polymorphism to mistake the fields. */
  unsigned int y; /* { dg-bogus "'E::y' might shadow 'B::y'" } */
  unsigned int vvv; /* { dg-bogus "'E::vvv' might shadow 'B::<unnamed union>::vvv'" } */

  // Do warn even if the type differs. Should be trivial, still test for it.
  double x; /* { dg-warning "'E::x' might shadow 'D::x'" } */
};