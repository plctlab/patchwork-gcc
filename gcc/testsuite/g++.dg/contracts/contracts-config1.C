// Small test to ensure that the level and role information printed by various
// contract configurations is correct.
// { dg-do run }
// { dg-options "-std=c++2a -fcontracts -fcontract-role=default:maybe,maybe,ignore" }

int fun(int n)
  [[ post default r: r > 0 ]]
{
  return -n;
}

int main(int, char **)
{
  [[ assert default: false ]];
  [[ assert: false ]];
  [[ assert audit: false ]];
  [[ assert default %new_role: false ]];
  [[ assert %new_role: false ]];
  [[ assert audit %new_role: false ]];
  [[ assert check_maybe_continue: false ]];
  [[ assert %default: false ]];
  [[ assert audit %default: false ]];
  fun(5);
  return 0;
}

// { dg-output "contract violation in function main at .*C:14: false(\n|\r\n|\r)*\\\[level:default, role:default, continuation mode:maybe\\\](\n|\r\n|\r)*" }
// { dg-output "contract violation in function main at .*C:15: false(\n|\r\n|\r)*\\\[level:default, role:default, continuation mode:maybe\\\](\n|\r\n|\r)*" }
// { dg-output "contract violation in function main at .*C:16: false(\n|\r\n|\r)*\\\[level:audit, role:default, continuation mode:maybe\\\](\n|\r\n|\r)*" }
// { dg-output "contract violation in function main at .*C:17: false(\n|\r\n|\r)*\\\[level:default, role:new_role, continuation mode:maybe\\\](\n|\r\n|\r)*" }
// { dg-output "contract violation in function main at .*C:18: false(\n|\r\n|\r)*\\\[level:default, role:new_role, continuation mode:maybe\\\](\n|\r\n|\r)*" }
// { dg-output "contract violation in function main at .*C:19: false(\n|\r\n|\r)*\\\[level:audit, role:new_role, continuation mode:maybe\\\](\n|\r\n|\r)*" }
// { dg-output "contract violation in function main at .*C:20: false(\n|\r\n|\r)*\\\[level:, role:, continuation mode:maybe\\\](\n|\r\n|\r)*" }
// { dg-output "contract violation in function main at .*C:21: false(\n|\r\n|\r)*\\\[level:default, role:default, continuation mode:maybe\\\](\n|\r\n|\r)*" }
// { dg-output "contract violation in function main at .*C:22: false(\n|\r\n|\r)*\\\[level:audit, role:default, continuation mode:maybe\\\](\n|\r\n|\r)*" }
// { dg-output "contract violation in function fun at .*C:7: r > 0(\n|\r\n|\r)*\\\[level:default, role:default, continuation mode:maybe\\\](\n|\r\n|\r)*" }
