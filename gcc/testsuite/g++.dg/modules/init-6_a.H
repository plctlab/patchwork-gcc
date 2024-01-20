// { dg-additional-options "-fmodule-header" }
// { dg-module-cmi {} }

template <bool _DecOnly>
struct __from_chars_alnum_to_val_table {
  static inline int value = 42;
};

inline unsigned char
__from_chars_alnum_to_val() {
  return __from_chars_alnum_to_val_table<false>::value;
}
