// Verify we explain why all three candidates failed to match.
// The presence of the first two non-strictly viable candidates
// used to make us not explain the third unviable candidate.

void f(int, int*); // { dg-message "candidate" }
void f(int*, int); // { dg-message "candidate" }
void f(int, int, int); // { dg-message "candidate" }

int main() {
  f(1, 2); // { dg-error "no match|invalid conversion" }
}
