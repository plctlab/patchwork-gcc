// PR c++/12341
/* { dg-do compile } */
/* { dg-options -Wshadow } */

class A { 
protected: 
  int aaa; /* { dg-line A_def_aaa } */
}; 

// Check simple inheritance is checked for.
class B : public A { 
public: 
  int aaa; /* { dg-line B_shadowing_aaa } */
  /* { dg-warning "'B::aaa' might shadow 'A::aaa'" "" { target *-*-* } .-1 } */
  /* { dg-note "'A::aaa' name already in use here" "" { target *-*-* } A_def_aaa } */
private:
  int bbb; /* { dg-line B_def_bbb } */
  int eee; /* { dg-line B_def_eee } */
};

// Check that visibility of base classes's fields is of no concern.
class C : public B {
public:
  int bbb; /* { dg-warning "'C::bbb' might shadow 'B::bbb'" } */
  /* { dg-note "'B::bbb' name already in use here" "" { target *-*-* } B_def_bbb } */
};

class D {
protected:
  int bbb; /* { dg-line D_def_bbb } */
  int ddd; /* { dg-line D_def_ddd } */
};

class E : protected D {
private:
  int eee; /* { dg-line E_def_eee } */
};

// all first-level base classes must be considered. 
class Bi : protected B, public E {
protected:
  /* Check that we stop on first ambiguous match, 
  that we don't walk the hierarchy deeper. */
  int aaa; /* { dg_bogus "'Bi::aaa' might shadow 'A::aaa'" } */
  /* { dg-warning "'Bi::aaa' might shadow 'B::aaa'" "" { target *-*-* } .-1 } */
  /* { dg-note "'B::aaa' name already in use here" "" { target *-*-* } B_shadowing_aaa } */

  // All branches of a multiple inheritance tree must be explored.
  int bbb; /* { dg-warning "'Bi::bbb' might shadow 'D::bbb'" } */
  /* { dg-note "'D::bbb' name already in use here" "" { target *-*-* } D_def_bbb } */
  /* { dg-warning "'Bi::bbb' might shadow 'B::bbb'" "" { target *-*-* } .-2 } */
  
  // Must continue beyond the immediate parent, even in the case of multiple inheritance. 
  int ddd; /* { dg-warning "'Bi::ddd' might shadow 'D::ddd'" } */
  /* { dg-note "'D::ddd' name already in use here" "" { target *-*-* } D_def_ddd } */
};

class BiD : public Bi {
  /* If the base class does not cause a warning,
  then look into each base classes of the parent's multiple inheritance */
  int eee; /* { dg-warning "'BiD::eee' might shadow 'B::eee'" } */
  /* { dg-note "'B::eee' name already in use here" "" { target *-*-* } B_def_eee } */
  /* { dg-warning "'BiD::eee' might shadow 'E::eee'" "" { target *-*-* } .-2 } */
  /* { dg-note "'E::eee' name already in use here" "" { target *-*-* } E_def_eee } */
};