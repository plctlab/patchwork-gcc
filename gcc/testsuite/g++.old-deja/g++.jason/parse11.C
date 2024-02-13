// { dg-do assemble  }
// PRMS Id: 6825

class aClass 
{ 
  ; // { dg-error "extra" "" { target c++11_down } } missing declaration
private: 
  ; // { dg-error "extra" "" { target c++11_down } } missing declaration
}; 
