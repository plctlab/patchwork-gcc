#include "riscv_vector.h"

vint32m1_t test_vmv_overloaded_0 (vint32m1_t src, size_t vl) {
  return __riscv_vmv_v (src, vl);
}

vfloat16m1_t test_vmv_overloaded_1 (vfloat16m1_t src, size_t vl) {
  return __riscv_vmv_v (src, vl);
}

vint32m1_t test_vmv_non_overloaded_0 (vint32m1_t src, size_t vl) {
  return __riscv_vmv_v_v_i32m1 (src, vl);
}

vfloat16m1_t test_vmv_non_overloaded_1 (vfloat16m1_t src, size_t vl) {
  return __riscv_vmv_v_v_f16m1 (src, vl);
}
