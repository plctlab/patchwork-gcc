// { dg-do compile { target c++14 } }

constexpr const double& test() {
  const double& local = 3.0;  // { dg-message "note: declared here" }
  return local;
}

static_assert(test() == 3.0, "");  // { dg-error "non-constant condition|accessing object outside its lifetime" }

// no deference, shouldn't error
static_assert((test(), true), "");
