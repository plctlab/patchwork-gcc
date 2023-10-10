// Verify we note other candidates when a deleted function is
// selected by overload resolution.
// { dg-do compile { target c++11 } }

void f(int) = delete; // { dg-message "declared here|candidate" }
void f(...); // { dg-message "candidate" }
void f(int, int); // { dg-message "candidate" }

int main() {
  f(0); // { dg-error "deleted" }
}
