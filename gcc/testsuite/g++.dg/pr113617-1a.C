// { dg-do compile { target fpic } }
// { dg-require-visibility "" }
// { dg-options "-O2 -std=c++11 -fPIC -fvisibility=hidden -fvisibility-inlines-hidden" }

namespace {
template <int __v> struct integral_constant {
  static constexpr int value = __v;
};
template <bool __v> using __bool_constant = integral_constant<__v>;
using true_type = __bool_constant<true>;
template <int> struct __conditional {
  template <typename _Tp, typename> using type = _Tp;
};
template <bool _Cond, typename _If, typename _Else>
using __conditional_t = typename __conditional<_Cond>::type<_If, _Else>;
true_type __trans_tmp_1;
template <typename _Tp> struct remove_cv { using type = _Tp; };
template <typename _Up>
struct __decay_selector
    : __conditional_t<true_type ::value, remove_cv<_Up>, _Up> {};
template <typename _Tp> struct decay {
  using type = typename __decay_selector<_Tp>::type;
};
}
struct vtkCellArray {};
namespace blah {
struct _Any_data;
enum _Manager_operation {};
template <typename> class function;
struct _Function_base {
  using _Manager_type = bool (*)(_Any_data &, const _Any_data &,
                                 _Manager_operation);
  _Manager_type _M_manager;
};
template <typename, typename> class _Function_handler;
template <typename _Res, typename _Functor, typename... _ArgTypes>
struct _Function_handler<_Res(_ArgTypes...), _Functor> {
  static bool _M_manager(_Any_data &, const _Any_data &, _Manager_operation) {
    return false;
  }
  __attribute__((noipa)) static _Res _M_invoke(const _Any_data &) {}
};
template <typename _Res, typename... _ArgTypes>
struct function<_Res(_ArgTypes...)> : _Function_base {
  template <typename _Functor>
  using _Handler = _Function_handler<_Res(), _Functor>;
  template <typename _Functor> function(_Functor) {
    using _My_handler = _Handler<_Functor>;
    _M_invoker = _My_handler::_M_invoke;
    _M_manager = _My_handler::_M_manager;
  }
  using _Invoker_type = _Res (*)(const _Any_data &);
  _Invoker_type _M_invoker;
};
template <typename> class _Bind;
template <typename _Functor, typename... _Bound_args>
struct _Bind<_Functor(_Bound_args...)> {};
template <typename> using __is_socketlike = decltype(__trans_tmp_1);
template <int, typename _Func, typename... _BoundArgs> struct _Bind_helper {
  typedef _Bind<typename decay<_Func>::type(
      typename decay<_BoundArgs>::type...)>
      type;
};
template <typename _Func, typename... _BoundArgs>
__attribute__((noipa)) typename _Bind_helper<__is_socketlike<_Func>::value, _Func, _BoundArgs...>::type
bind(_Func, _BoundArgs...) { return typename _Bind_helper<__is_socketlike<_Func>::value, _Func, _BoundArgs...>::type (); }
template <typename _Tp> struct __uniq_ptr_impl {
  template <typename _Up> struct _Ptr { using type = _Up *; };
  using pointer = typename _Ptr<_Tp>::type;
};
template <typename _Tp> struct unique_ptr {
  using pointer = typename __uniq_ptr_impl<_Tp>::pointer;
  pointer operator->();
};
}
extern int For_threadNumber;
namespace vtk {
namespace detail {
namespace smp {
enum BackendType { Sequential, STDThread };
template <int> struct vtkSMPToolsImpl {
  template <typename FunctorInternal>
  __attribute__((noipa)) void For(long long, long long, long long, FunctorInternal &) {}
};
struct vtkSMPThreadPool {
  vtkSMPThreadPool(int);
  void DoJob(blah::function<void()>);
};
template <typename>
__attribute__((noipa)) void ExecuteFunctorSTDThread(void *, long long, long long, long long) {}
template <>
template <typename FunctorInternal>
void vtkSMPToolsImpl<STDThread>::For(long long, long long last, long long grain,
                                     FunctorInternal &fi) {
  vtkSMPThreadPool pool(For_threadNumber);
  for (;;) {
    auto job = blah::bind(ExecuteFunctorSTDThread<FunctorInternal>, &fi, grain,
                         grain, last);
    pool.DoJob(job);
  }
}
struct vtkSMPToolsAPI {
  static vtkSMPToolsAPI &GetInstance();
  template <typename FunctorInternal>
  void For(long first, long last, long grain, FunctorInternal fi) {
    switch (ActivatedBackend) {
    case Sequential:
      SequentialBackend->For(first, last, grain, fi);
    case STDThread:
      STDThreadBackend->For(first, last, grain, fi);
    }
  }
  BackendType ActivatedBackend;
  blah::unique_ptr<vtkSMPToolsImpl<Sequential>> SequentialBackend;
  blah::unique_ptr<vtkSMPToolsImpl<STDThread>> STDThreadBackend;
};
template <typename, bool> struct vtkSMPTools_FunctorInternal;
template <typename Functor> struct vtkSMPTools_FunctorInternal<Functor, false> {
  __attribute__((noipa)) vtkSMPTools_FunctorInternal(Functor) {}
  void For(long first, long last, long grain) {
    auto SMPToolsAPI = vtkSMPToolsAPI::GetInstance();
    SMPToolsAPI.For(first, last, grain, *this);
  }
};
template <typename Functor> struct vtkSMPTools_Lookup_For {
  typedef vtkSMPTools_FunctorInternal<Functor, 0> type;
};
}
}
}
struct vtkSMPTools {
  template <typename Functor>
  static void For(long first, long last, long grain, Functor f) {
    typename vtk::detail::smp::vtkSMPTools_Lookup_For<Functor>::type fi(f);
    fi.For(first, last, grain);
  }
  template <typename Functor>
  static void For(long first, long last, Functor f) {
    For(first, last, 0, f);
  }
};
template <typename> struct vtkStaticCellLinksTemplate {
  void ThreadedBuildLinks(long long, long long, vtkCellArray *);
  int *Offsets;
};
namespace {
template <typename> struct CountUses { __attribute__((noipa)) CountUses(vtkCellArray *, int *) {} };
}
template <typename TIds>
void vtkStaticCellLinksTemplate<TIds>::ThreadedBuildLinks(
    long long numPts, long long numCells, vtkCellArray *cellArray) {
  int counts;
  CountUses<TIds> count(cellArray, &counts);
  vtkSMPTools::For(0, numCells, count);
  long ptId, npts;
  for (ptId = 0; ptId < numPts; ptId++)
    Offsets[ptId] = Offsets[ptId - 1] + npts;
}

int For_threadNumber;
long vtkConstrainedSmoothingFilterRequestData_numPts;
void vtkConstrainedSmoothingFilterRequestData() {
  vtkCellArray lines;
  vtkStaticCellLinksTemplate<long long> links;
  links.ThreadedBuildLinks(vtkConstrainedSmoothingFilterRequestData_numPts, 0,
                           &lines);
}

// { dg-final { scan-assembler-not ".section\t\.data\.rel\.ro\.local,\"aw\"" { target { { i?86-*-linux* x86_64-*-linux* } && { ! ia32 } } } } }
// { dg-final { scan-assembler ".section\t.data\.rel\.ro\.local\..*,\"awG\"" { target { { i?86-*-linux* x86_64-*-linux* } && { ! ia32 } } } } }
