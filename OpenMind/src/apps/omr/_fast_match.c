
#ifndef PY_SSIZE_T_CLEAN
#define PY_SSIZE_T_CLEAN
#endif

#if defined(Py_LIMITED_API)
  #if !defined(CYTHON_LIMITED_API)
  #define CYTHON_LIMITED_API 1
  #endif
#elif defined(CYTHON_LIMITED_API)
  #ifdef _MSC_VER
  #pragma message ("Limited API usage is enabled with 'CYTHON_LIMITED_API' but 'Py_LIMITED_API' does not define a Python target version. Consider setting 'Py_LIMITED_API' instead.")
  #else
  #warning Limited API usage is enabled with 'CYTHON_LIMITED_API' but 'Py_LIMITED_API' does not define a Python target version. Consider setting 'Py_LIMITED_API' instead.
  #endif
#endif

#include "Python.h"
#ifndef Py_PYTHON_H
    #error Python headers needed to compile C extensions, please install development version of Python.
#elif PY_VERSION_HEX < 0x03080000
    #error Cython requires Python 3.8+.
#else
#define __PYX_ABI_VERSION "3_2_4"
#define CYTHON_HEX_VERSION 0x030204F0
#define CYTHON_FUTURE_DIVISION 1

#include <stddef.h>
#ifndef offsetof
  #define offsetof(type, member) ( (size_t) & ((type*)0) -> member )
#endif
#if !defined(_WIN32) && !defined(WIN32) && !defined(MS_WINDOWS)
  #ifndef __stdcall
    #define __stdcall
  #endif
  #ifndef __cdecl
    #define __cdecl
  #endif
  #ifndef __fastcall
    #define __fastcall
  #endif
#endif
#ifndef DL_IMPORT
  #define DL_IMPORT(t) t
#endif
#ifndef DL_EXPORT
  #define DL_EXPORT(t) t
#endif
#define __PYX_COMMA ,
#ifndef PY_LONG_LONG
  #define PY_LONG_LONG LONG_LONG
#endif
#ifndef Py_HUGE_VAL
  #define Py_HUGE_VAL HUGE_VAL
#endif
#define __PYX_LIMITED_VERSION_HEX PY_VERSION_HEX
#if defined(GRAALVM_PYTHON)

  #define CYTHON_COMPILING_IN_PYPY 0
  #define CYTHON_COMPILING_IN_CPYTHON 0
  #define CYTHON_COMPILING_IN_LIMITED_API 0
  #define CYTHON_COMPILING_IN_GRAAL 1
  #define CYTHON_COMPILING_IN_CPYTHON_FREETHREADING 0
  #undef CYTHON_USE_TYPE_SLOTS
  #define CYTHON_USE_TYPE_SLOTS 0
  #undef CYTHON_USE_TYPE_SPECS
  #define CYTHON_USE_TYPE_SPECS 0
  #undef CYTHON_USE_PYTYPE_LOOKUP
  #define CYTHON_USE_PYTYPE_LOOKUP 0
  #undef CYTHON_USE_PYLIST_INTERNALS
  #define CYTHON_USE_PYLIST_INTERNALS 0
  #undef CYTHON_USE_UNICODE_INTERNALS
  #define CYTHON_USE_UNICODE_INTERNALS 0
  #undef CYTHON_USE_UNICODE_WRITER
  #define CYTHON_USE_UNICODE_WRITER 0
  #undef CYTHON_USE_PYLONG_INTERNALS
  #define CYTHON_USE_PYLONG_INTERNALS 0
  #undef CYTHON_AVOID_BORROWED_REFS
  #define CYTHON_AVOID_BORROWED_REFS 1
  #undef CYTHON_AVOID_THREAD_UNSAFE_BORROWED_REFS
  #define CYTHON_AVOID_THREAD_UNSAFE_BORROWED_REFS 0
  #undef CYTHON_ASSUME_SAFE_MACROS
  #define CYTHON_ASSUME_SAFE_MACROS 0
  #undef CYTHON_ASSUME_SAFE_SIZE
  #define CYTHON_ASSUME_SAFE_SIZE 0
  #undef CYTHON_UNPACK_METHODS
  #define CYTHON_UNPACK_METHODS 0
  #undef CYTHON_FAST_THREAD_STATE
  #define CYTHON_FAST_THREAD_STATE 0
  #undef CYTHON_FAST_GIL
  #define CYTHON_FAST_GIL 0
  #undef CYTHON_METH_FASTCALL
  #define CYTHON_METH_FASTCALL 0
  #undef CYTHON_FAST_PYCALL
  #define CYTHON_FAST_PYCALL 0
  #ifndef CYTHON_PEP487_INIT_SUBCLASS
    #define CYTHON_PEP487_INIT_SUBCLASS 1
  #endif
  #undef CYTHON_PEP489_MULTI_PHASE_INIT
  #define CYTHON_PEP489_MULTI_PHASE_INIT 1
  #undef CYTHON_USE_MODULE_STATE
  #define CYTHON_USE_MODULE_STATE 0
  #undef CYTHON_USE_SYS_MONITORING
  #define CYTHON_USE_SYS_MONITORING 0
  #undef CYTHON_USE_TP_FINALIZE
  #define CYTHON_USE_TP_FINALIZE 0
  #undef CYTHON_USE_AM_SEND
  #define CYTHON_USE_AM_SEND 0
  #undef CYTHON_USE_DICT_VERSIONS
  #define CYTHON_USE_DICT_VERSIONS 0
  #undef CYTHON_USE_EXC_INFO_STACK
  #define CYTHON_USE_EXC_INFO_STACK 1
  #ifndef CYTHON_UPDATE_DESCRIPTOR_DOC
    #define CYTHON_UPDATE_DESCRIPTOR_DOC 0
  #endif
  #undef CYTHON_USE_FREELISTS
  #define CYTHON_USE_FREELISTS 0
  #undef CYTHON_IMMORTAL_CONSTANTS
  #define CYTHON_IMMORTAL_CONSTANTS 0
#elif defined(PYPY_VERSION)
  #define CYTHON_COMPILING_IN_PYPY 1
  #define CYTHON_COMPILING_IN_CPYTHON 0
  #define CYTHON_COMPILING_IN_LIMITED_API 0
  #define CYTHON_COMPILING_IN_GRAAL 0
  #define CYTHON_COMPILING_IN_CPYTHON_FREETHREADING 0
  #undef CYTHON_USE_TYPE_SLOTS
  #define CYTHON_USE_TYPE_SLOTS 1
  #ifndef CYTHON_USE_TYPE_SPECS
    #define CYTHON_USE_TYPE_SPECS 0
  #endif
  #undef CYTHON_USE_PYTYPE_LOOKUP
  #define CYTHON_USE_PYTYPE_LOOKUP 0
  #undef CYTHON_USE_PYLIST_INTERNALS
  #define CYTHON_USE_PYLIST_INTERNALS 0
  #undef CYTHON_USE_UNICODE_INTERNALS
  #define CYTHON_USE_UNICODE_INTERNALS 0
  #undef CYTHON_USE_UNICODE_WRITER
  #define CYTHON_USE_UNICODE_WRITER 0
  #undef CYTHON_USE_PYLONG_INTERNALS
  #define CYTHON_USE_PYLONG_INTERNALS 0
  #undef CYTHON_AVOID_BORROWED_REFS
  #define CYTHON_AVOID_BORROWED_REFS 1
  #undef CYTHON_AVOID_THREAD_UNSAFE_BORROWED_REFS
  #define CYTHON_AVOID_THREAD_UNSAFE_BORROWED_REFS 1
  #undef CYTHON_ASSUME_SAFE_MACROS
  #define CYTHON_ASSUME_SAFE_MACROS 0
  #ifndef CYTHON_ASSUME_SAFE_SIZE
    #define CYTHON_ASSUME_SAFE_SIZE 1
  #endif
  #undef CYTHON_UNPACK_METHODS
  #define CYTHON_UNPACK_METHODS 0
  #undef CYTHON_FAST_THREAD_STATE
  #define CYTHON_FAST_THREAD_STATE 0
  #undef CYTHON_FAST_GIL
  #define CYTHON_FAST_GIL 0
  #undef CYTHON_METH_FASTCALL
  #define CYTHON_METH_FASTCALL 0
  #undef CYTHON_FAST_PYCALL
  #define CYTHON_FAST_PYCALL 0
  #ifndef CYTHON_PEP487_INIT_SUBCLASS
    #define CYTHON_PEP487_INIT_SUBCLASS 1
  #endif
  #if PY_VERSION_HEX < 0x03090000
    #undef CYTHON_PEP489_MULTI_PHASE_INIT
    #define CYTHON_PEP489_MULTI_PHASE_INIT 0
  #elif !defined(CYTHON_PEP489_MULTI_PHASE_INIT)
    #define CYTHON_PEP489_MULTI_PHASE_INIT 1
  #endif
  #undef CYTHON_USE_MODULE_STATE
  #define CYTHON_USE_MODULE_STATE 0
  #undef CYTHON_USE_SYS_MONITORING
  #define CYTHON_USE_SYS_MONITORING 0
  #ifndef CYTHON_USE_TP_FINALIZE
    #define CYTHON_USE_TP_FINALIZE (PYPY_VERSION_NUM >= 0x07030C00)
  #endif
  #undef CYTHON_USE_AM_SEND
  #define CYTHON_USE_AM_SEND 0
  #undef CYTHON_USE_DICT_VERSIONS
  #define CYTHON_USE_DICT_VERSIONS 0
  #undef CYTHON_USE_EXC_INFO_STACK
  #define CYTHON_USE_EXC_INFO_STACK 0
  #ifndef CYTHON_UPDATE_DESCRIPTOR_DOC
    #define CYTHON_UPDATE_DESCRIPTOR_DOC (PYPY_VERSION_NUM >= 0x07031100)
  #endif
  #undef CYTHON_USE_FREELISTS
  #define CYTHON_USE_FREELISTS 0
  #undef CYTHON_IMMORTAL_CONSTANTS
  #define CYTHON_IMMORTAL_CONSTANTS 0
#elif defined(CYTHON_LIMITED_API)
  #ifdef Py_LIMITED_API
    #undef __PYX_LIMITED_VERSION_HEX
    #define __PYX_LIMITED_VERSION_HEX Py_LIMITED_API
  #endif
  #define CYTHON_COMPILING_IN_PYPY 0
  #define CYTHON_COMPILING_IN_CPYTHON 0
  #define CYTHON_COMPILING_IN_LIMITED_API 1
  #define CYTHON_COMPILING_IN_GRAAL 0
  #define CYTHON_COMPILING_IN_CPYTHON_FREETHREADING 0
  #undef CYTHON_USE_TYPE_SLOTS
  #define CYTHON_USE_TYPE_SLOTS 0
  #undef CYTHON_USE_TYPE_SPECS
  #define CYTHON_USE_TYPE_SPECS 1
  #undef CYTHON_USE_PYTYPE_LOOKUP
  #define CYTHON_USE_PYTYPE_LOOKUP 0
  #undef CYTHON_USE_PYLIST_INTERNALS
  #define CYTHON_USE_PYLIST_INTERNALS 0
  #undef CYTHON_USE_UNICODE_INTERNALS
  #define CYTHON_USE_UNICODE_INTERNALS 0
  #ifndef CYTHON_USE_UNICODE_WRITER
    #define CYTHON_USE_UNICODE_WRITER 0
  #endif
  #undef CYTHON_USE_PYLONG_INTERNALS
  #define CYTHON_USE_PYLONG_INTERNALS 0
  #ifndef CYTHON_AVOID_BORROWED_REFS
    #define CYTHON_AVOID_BORROWED_REFS 0
  #endif
  #ifndef CYTHON_AVOID_THREAD_UNSAFE_BORROWED_REFS
    #define CYTHON_AVOID_THREAD_UNSAFE_BORROWED_REFS 0
  #endif
  #undef CYTHON_ASSUME_SAFE_MACROS
  #define CYTHON_ASSUME_SAFE_MACROS 0
  #undef CYTHON_ASSUME_SAFE_SIZE
  #define CYTHON_ASSUME_SAFE_SIZE 0
  #undef CYTHON_UNPACK_METHODS
  #define CYTHON_UNPACK_METHODS 0
  #undef CYTHON_FAST_THREAD_STATE
  #define CYTHON_FAST_THREAD_STATE 0
  #undef CYTHON_FAST_GIL
  #define CYTHON_FAST_GIL 0
  #undef CYTHON_METH_FASTCALL
  #define CYTHON_METH_FASTCALL (__PYX_LIMITED_VERSION_HEX >= 0x030C0000)
  #undef CYTHON_FAST_PYCALL
  #define CYTHON_FAST_PYCALL 0
  #ifndef CYTHON_PEP487_INIT_SUBCLASS
    #define CYTHON_PEP487_INIT_SUBCLASS 1
  #endif
  #ifndef CYTHON_PEP489_MULTI_PHASE_INIT
    #define CYTHON_PEP489_MULTI_PHASE_INIT 1
  #endif
  #ifndef CYTHON_USE_MODULE_STATE
    #define CYTHON_USE_MODULE_STATE 0
  #endif
  #undef CYTHON_USE_SYS_MONITORING
  #define CYTHON_USE_SYS_MONITORING 0
  #ifndef CYTHON_USE_TP_FINALIZE
    #define CYTHON_USE_TP_FINALIZE 0
  #endif
  #ifndef CYTHON_USE_AM_SEND
    #define CYTHON_USE_AM_SEND (__PYX_LIMITED_VERSION_HEX >= 0x030A0000)
  #endif
  #undef CYTHON_USE_DICT_VERSIONS
  #define CYTHON_USE_DICT_VERSIONS 0
  #undef CYTHON_USE_EXC_INFO_STACK
  #define CYTHON_USE_EXC_INFO_STACK 0
  #ifndef CYTHON_UPDATE_DESCRIPTOR_DOC
    #define CYTHON_UPDATE_DESCRIPTOR_DOC 0
  #endif
  #ifndef CYTHON_USE_FREELISTS
  #define CYTHON_USE_FREELISTS 1
  #endif
  #undef CYTHON_IMMORTAL_CONSTANTS
  #define CYTHON_IMMORTAL_CONSTANTS 0
#else
  #define CYTHON_COMPILING_IN_PYPY 0
  #define CYTHON_COMPILING_IN_CPYTHON 1
  #define CYTHON_COMPILING_IN_LIMITED_API 0
  #define CYTHON_COMPILING_IN_GRAAL 0
  #ifdef Py_GIL_DISABLED
    #define CYTHON_COMPILING_IN_CPYTHON_FREETHREADING 1
  #else
    #define CYTHON_COMPILING_IN_CPYTHON_FREETHREADING 0
  #endif
  #if PY_VERSION_HEX < 0x030A0000
    #undef CYTHON_USE_TYPE_SLOTS
    #define CYTHON_USE_TYPE_SLOTS 1
  #elif !defined(CYTHON_USE_TYPE_SLOTS)
    #define CYTHON_USE_TYPE_SLOTS 1
  #endif
  #ifndef CYTHON_USE_TYPE_SPECS
    #define CYTHON_USE_TYPE_SPECS 0
  #endif
  #ifndef CYTHON_USE_PYTYPE_LOOKUP
    #define CYTHON_USE_PYTYPE_LOOKUP 1
  #endif
  #ifndef CYTHON_USE_PYLONG_INTERNALS
    #define CYTHON_USE_PYLONG_INTERNALS 1
  #endif
  #if CYTHON_COMPILING_IN_CPYTHON_FREETHREADING
    #undef CYTHON_USE_PYLIST_INTERNALS
    #define CYTHON_USE_PYLIST_INTERNALS 0
  #elif !defined(CYTHON_USE_PYLIST_INTERNALS)
    #define CYTHON_USE_PYLIST_INTERNALS 1
  #endif
  #ifndef CYTHON_USE_UNICODE_INTERNALS
    #define CYTHON_USE_UNICODE_INTERNALS 1
  #endif
  #if CYTHON_COMPILING_IN_CPYTHON_FREETHREADING || PY_VERSION_HEX >= 0x030B00A2
    #undef CYTHON_USE_UNICODE_WRITER
    #define CYTHON_USE_UNICODE_WRITER 0
  #elif !defined(CYTHON_USE_UNICODE_WRITER)
    #define CYTHON_USE_UNICODE_WRITER 1
  #endif
  #ifndef CYTHON_AVOID_BORROWED_REFS
    #define CYTHON_AVOID_BORROWED_REFS 0
  #endif
  #if CYTHON_COMPILING_IN_CPYTHON_FREETHREADING
    #undef CYTHON_AVOID_THREAD_UNSAFE_BORROWED_REFS
    #define CYTHON_AVOID_THREAD_UNSAFE_BORROWED_REFS 1
  #elif !defined(CYTHON_AVOID_THREAD_UNSAFE_BORROWED_REFS)
    #define CYTHON_AVOID_THREAD_UNSAFE_BORROWED_REFS 0
  #endif
  #ifndef CYTHON_ASSUME_SAFE_MACROS
    #define CYTHON_ASSUME_SAFE_MACROS 1
  #endif
  #ifndef CYTHON_ASSUME_SAFE_SIZE
    #define CYTHON_ASSUME_SAFE_SIZE 1
  #endif
  #ifndef CYTHON_UNPACK_METHODS
    #define CYTHON_UNPACK_METHODS 1
  #endif
  #ifndef CYTHON_FAST_THREAD_STATE
    #define CYTHON_FAST_THREAD_STATE 1
  #endif
  #if CYTHON_COMPILING_IN_CPYTHON_FREETHREADING
    #undef CYTHON_FAST_GIL
    #define CYTHON_FAST_GIL 0
  #elif !defined(CYTHON_FAST_GIL)
    #define CYTHON_FAST_GIL (PY_VERSION_HEX < 0x030C00A6)
  #endif
  #ifndef CYTHON_METH_FASTCALL
    #define CYTHON_METH_FASTCALL 1
  #endif
  #ifndef CYTHON_FAST_PYCALL
    #define CYTHON_FAST_PYCALL 1
  #endif
  #ifndef CYTHON_PEP487_INIT_SUBCLASS
    #define CYTHON_PEP487_INIT_SUBCLASS 1
  #endif
  #ifndef CYTHON_PEP489_MULTI_PHASE_INIT
    #define CYTHON_PEP489_MULTI_PHASE_INIT 1
  #endif
  #ifndef CYTHON_USE_MODULE_STATE
    #define CYTHON_USE_MODULE_STATE 0
  #endif
  #ifndef CYTHON_USE_SYS_MONITORING
    #define CYTHON_USE_SYS_MONITORING (PY_VERSION_HEX >= 0x030d00B1)
  #endif
  #ifndef CYTHON_USE_TP_FINALIZE
    #define CYTHON_USE_TP_FINALIZE 1
  #endif
  #ifndef CYTHON_USE_AM_SEND
    #define CYTHON_USE_AM_SEND 1
  #endif
  #if CYTHON_COMPILING_IN_CPYTHON_FREETHREADING
    #undef CYTHON_USE_DICT_VERSIONS
    #define CYTHON_USE_DICT_VERSIONS 0
  #elif !defined(CYTHON_USE_DICT_VERSIONS)
    #define CYTHON_USE_DICT_VERSIONS  (PY_VERSION_HEX < 0x030C00A5 && !CYTHON_USE_MODULE_STATE)
  #endif
  #ifndef CYTHON_USE_EXC_INFO_STACK
    #define CYTHON_USE_EXC_INFO_STACK 1
  #endif
  #ifndef CYTHON_UPDATE_DESCRIPTOR_DOC
    #define CYTHON_UPDATE_DESCRIPTOR_DOC 1
  #endif
  #ifndef CYTHON_USE_FREELISTS
    #define CYTHON_USE_FREELISTS (!CYTHON_COMPILING_IN_CPYTHON_FREETHREADING)
  #endif
  #if defined(CYTHON_IMMORTAL_CONSTANTS) && PY_VERSION_HEX < 0x030C0000
    #undef CYTHON_IMMORTAL_CONSTANTS
    #define CYTHON_IMMORTAL_CONSTANTS 0
  #elif !defined(CYTHON_IMMORTAL_CONSTANTS)
    #define CYTHON_IMMORTAL_CONSTANTS (PY_VERSION_HEX >= 0x030C0000 && !CYTHON_USE_MODULE_STATE && CYTHON_COMPILING_IN_CPYTHON_FREETHREADING)
  #endif
#endif
#ifndef CYTHON_COMPRESS_STRINGS
  #define CYTHON_COMPRESS_STRINGS 1
#endif
#ifndef CYTHON_FAST_PYCCALL
#define CYTHON_FAST_PYCCALL  CYTHON_FAST_PYCALL
#endif
#ifndef CYTHON_VECTORCALL
#if CYTHON_COMPILING_IN_LIMITED_API
#define CYTHON_VECTORCALL  (__PYX_LIMITED_VERSION_HEX >= 0x030C0000)
#else
#define CYTHON_VECTORCALL  (CYTHON_FAST_PYCCALL)
#endif
#endif
#if CYTHON_USE_PYLONG_INTERNALS
  #undef SHIFT
  #undef BASE
  #undef MASK
  #ifdef SIZEOF_VOID_P
    enum { __pyx_check_sizeof_voidp = 1 / (int)(SIZEOF_VOID_P == sizeof(void*)) };
  #endif
#endif
#ifndef __has_attribute
  #define __has_attribute(x) 0
#endif
#ifndef __has_cpp_attribute
  #define __has_cpp_attribute(x) 0
#endif
#ifndef CYTHON_RESTRICT
  #if defined(__GNUC__)
    #define CYTHON_RESTRICT __restrict__
  #elif defined(_MSC_VER) && _MSC_VER >= 1400
    #define CYTHON_RESTRICT __restrict
  #elif defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    #define CYTHON_RESTRICT restrict
  #else
    #define CYTHON_RESTRICT
  #endif
#endif
#ifndef CYTHON_UNUSED
  #if defined(__cplusplus)

    #if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
      #if __has_cpp_attribute(maybe_unused)
        #define CYTHON_UNUSED [[maybe_unused]]
      #endif
    #endif
  #endif
#endif
#ifndef CYTHON_UNUSED
# if defined(__GNUC__)
#   if !(defined(__cplusplus)) || (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))
#     define CYTHON_UNUSED __attribute__ ((__unused__))
#   else
#     define CYTHON_UNUSED
#   endif
# elif defined(__ICC) || (defined(__INTEL_COMPILER) && !defined(_MSC_VER))
#   define CYTHON_UNUSED __attribute__ ((__unused__))
# else
#   define CYTHON_UNUSED
# endif
#endif
#ifndef CYTHON_UNUSED_VAR
#  if defined(__cplusplus)
     template<class T> void CYTHON_UNUSED_VAR( const T& ) { }
#  else
#    define CYTHON_UNUSED_VAR(x) (void)(x)
#  endif
#endif
#ifndef CYTHON_MAYBE_UNUSED_VAR
  #define CYTHON_MAYBE_UNUSED_VAR(x) CYTHON_UNUSED_VAR(x)
#endif
#ifndef CYTHON_NCP_UNUSED
# if CYTHON_COMPILING_IN_CPYTHON && !CYTHON_COMPILING_IN_CPYTHON_FREETHREADING
#  define CYTHON_NCP_UNUSED
# else
#  define CYTHON_NCP_UNUSED CYTHON_UNUSED
# endif
#endif
#ifndef CYTHON_USE_CPP_STD_MOVE
  #if defined(__cplusplus) && (\
    __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1600))
    #define CYTHON_USE_CPP_STD_MOVE 1
  #else
    #define CYTHON_USE_CPP_STD_MOVE 0
  #endif
#endif
#define __Pyx_void_to_None(void_result) ((void)(void_result), Py_INCREF(Py_None), Py_None)
#include <stdint.h>
typedef uintptr_t  __pyx_uintptr_t;
#ifndef CYTHON_FALLTHROUGH
  #if defined(__cplusplus)

    #if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
      #if __has_cpp_attribute(fallthrough)
        #define CYTHON_FALLTHROUGH [[fallthrough]]
      #endif
    #endif
    #ifndef CYTHON_FALLTHROUGH
      #if __has_cpp_attribute(clang::fallthrough)
        #define CYTHON_FALLTHROUGH [[clang::fallthrough]]
      #elif __has_cpp_attribute(gnu::fallthrough)
        #define CYTHON_FALLTHROUGH [[gnu::fallthrough]]
      #endif
    #endif
  #endif
  #ifndef CYTHON_FALLTHROUGH
    #if __has_attribute(fallthrough)
      #define CYTHON_FALLTHROUGH __attribute__((fallthrough))
    #else
      #define CYTHON_FALLTHROUGH
    #endif
  #endif
  #if defined(__clang__) && defined(__apple_build_version__)
    #if __apple_build_version__ < 7000000
      #undef  CYTHON_FALLTHROUGH
      #define CYTHON_FALLTHROUGH
    #endif
  #endif
#endif
#ifndef Py_UNREACHABLE
  #define Py_UNREACHABLE()  assert(0); abort()
#endif
#ifdef __cplusplus
  template <typename T>
  struct __PYX_IS_UNSIGNED_IMPL {static const bool value = T(0) < T(-1);};
  #define __PYX_IS_UNSIGNED(type) (__PYX_IS_UNSIGNED_IMPL<type>::value)
#else
  #define __PYX_IS_UNSIGNED(type) (((type)-1) > 0)
#endif
#if CYTHON_COMPILING_IN_PYPY == 1
  #define __PYX_NEED_TP_PRINT_SLOT  (PY_VERSION_HEX < 0x030A0000)
#else
  #define __PYX_NEED_TP_PRINT_SLOT  (PY_VERSION_HEX < 0x03090000)
#endif
#define __PYX_REINTERPRET_FUNCION(func_pointer, other_pointer) ((func_pointer)(void(*)(void))(other_pointer))

#ifndef CYTHON_INLINE
  #if defined(__clang__)
    #define CYTHON_INLINE __inline__ __attribute__ ((__unused__))
  #elif defined(__GNUC__)
    #define CYTHON_INLINE __inline__
  #elif defined(_MSC_VER)
    #define CYTHON_INLINE __inline
  #elif defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    #define CYTHON_INLINE inline
  #else
    #define CYTHON_INLINE
  #endif
#endif

#define __PYX_BUILD_PY_SSIZE_T "n"
#define CYTHON_FORMAT_SSIZE_T "z"
#define __Pyx_BUILTIN_MODULE_NAME "builtins"
#define __Pyx_DefaultClassType PyType_Type
#if CYTHON_COMPILING_IN_LIMITED_API
    #ifndef CO_OPTIMIZED
    static int CO_OPTIMIZED;
    #endif
    #ifndef CO_NEWLOCALS
    static int CO_NEWLOCALS;
    #endif
    #ifndef CO_VARARGS
    static int CO_VARARGS;
    #endif
    #ifndef CO_VARKEYWORDS
    static int CO_VARKEYWORDS;
    #endif
    #ifndef CO_ASYNC_GENERATOR
    static int CO_ASYNC_GENERATOR;
    #endif
    #ifndef CO_GENERATOR
    static int CO_GENERATOR;
    #endif
    #ifndef CO_COROUTINE
    static int CO_COROUTINE;
    #endif
#else
    #ifndef CO_COROUTINE
      #define CO_COROUTINE 0x80
    #endif
    #ifndef CO_ASYNC_GENERATOR
      #define CO_ASYNC_GENERATOR 0x200
    #endif
#endif
static int __Pyx_init_co_variables(void);
#if PY_VERSION_HEX >= 0x030900A4 || defined(Py_IS_TYPE)
  #define __Pyx_IS_TYPE(ob, type) Py_IS_TYPE(ob, type)
#else
  #define __Pyx_IS_TYPE(ob, type) (((const PyObject*)ob)->ob_type == (type))
#endif
#if PY_VERSION_HEX >= 0x030A00B1 || defined(Py_Is)
  #define __Pyx_Py_Is(x, y)  Py_Is(x, y)
#else
  #define __Pyx_Py_Is(x, y) ((x) == (y))
#endif
#if PY_VERSION_HEX >= 0x030A00B1 || defined(Py_IsNone)
  #define __Pyx_Py_IsNone(ob) Py_IsNone(ob)
#else
  #define __Pyx_Py_IsNone(ob) __Pyx_Py_Is((ob), Py_None)
#endif
#if PY_VERSION_HEX >= 0x030A00B1 || defined(Py_IsTrue)
  #define __Pyx_Py_IsTrue(ob) Py_IsTrue(ob)
#else
  #define __Pyx_Py_IsTrue(ob) __Pyx_Py_Is((ob), Py_True)
#endif
#if PY_VERSION_HEX >= 0x030A00B1 || defined(Py_IsFalse)
  #define __Pyx_Py_IsFalse(ob) Py_IsFalse(ob)
#else
  #define __Pyx_Py_IsFalse(ob) __Pyx_Py_Is((ob), Py_False)
#endif
#define __Pyx_NoneAsNull(obj)  (__Pyx_Py_IsNone(obj) ? NULL : (obj))
#if PY_VERSION_HEX >= 0x030900F0 && !CYTHON_COMPILING_IN_PYPY
  #define __Pyx_PyObject_GC_IsFinalized(o) PyObject_GC_IsFinalized(o)
#else
  #define __Pyx_PyObject_GC_IsFinalized(o) _PyGC_FINALIZED(o)
#endif
#ifndef Py_TPFLAGS_CHECKTYPES
  #define Py_TPFLAGS_CHECKTYPES 0
#endif
#ifndef Py_TPFLAGS_HAVE_INDEX
  #define Py_TPFLAGS_HAVE_INDEX 0
#endif
#ifndef Py_TPFLAGS_HAVE_NEWBUFFER
  #define Py_TPFLAGS_HAVE_NEWBUFFER 0
#endif
#ifndef Py_TPFLAGS_HAVE_FINALIZE
  #define Py_TPFLAGS_HAVE_FINALIZE 0
#endif
#ifndef Py_TPFLAGS_SEQUENCE
  #define Py_TPFLAGS_SEQUENCE 0
#endif
#ifndef Py_TPFLAGS_MAPPING
  #define Py_TPFLAGS_MAPPING 0
#endif
#ifndef Py_TPFLAGS_IMMUTABLETYPE
  #define Py_TPFLAGS_IMMUTABLETYPE (1UL << 8)
#endif
#ifndef Py_TPFLAGS_DISALLOW_INSTANTIATION
  #define Py_TPFLAGS_DISALLOW_INSTANTIATION (1UL << 7)
#endif
#ifndef METH_STACKLESS
  #define METH_STACKLESS 0
#endif
#ifndef METH_FASTCALL
  #ifndef METH_FASTCALL
     #define METH_FASTCALL 0x80
  #endif
  typedef PyObject *(*__Pyx_PyCFunctionFast) (PyObject *self, PyObject *const *args, Py_ssize_t nargs);
  typedef PyObject *(*__Pyx_PyCFunctionFastWithKeywords) (PyObject *self, PyObject *const *args,
                                                          Py_ssize_t nargs, PyObject *kwnames);
#else
  #if PY_VERSION_HEX >= 0x030d00A4
  #  define __Pyx_PyCFunctionFast PyCFunctionFast
  #  define __Pyx_PyCFunctionFastWithKeywords PyCFunctionFastWithKeywords
  #else
  #  define __Pyx_PyCFunctionFast _PyCFunctionFast
  #  define __Pyx_PyCFunctionFastWithKeywords _PyCFunctionFastWithKeywords
  #endif
#endif
#if CYTHON_METH_FASTCALL
  #define __Pyx_METH_FASTCALL METH_FASTCALL
  #define __Pyx_PyCFunction_FastCall __Pyx_PyCFunctionFast
  #define __Pyx_PyCFunction_FastCallWithKeywords __Pyx_PyCFunctionFastWithKeywords
#else
  #define __Pyx_METH_FASTCALL METH_VARARGS
  #define __Pyx_PyCFunction_FastCall PyCFunction
  #define __Pyx_PyCFunction_FastCallWithKeywords PyCFunctionWithKeywords
#endif
#if CYTHON_VECTORCALL
  #define __pyx_vectorcallfunc vectorcallfunc
  #define __Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET  PY_VECTORCALL_ARGUMENTS_OFFSET
  #define __Pyx_PyVectorcall_NARGS(n)  PyVectorcall_NARGS((size_t)(n))
#else
  #define __Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET  0
  #define __Pyx_PyVectorcall_NARGS(n)  ((Py_ssize_t)(n))
#endif
#if PY_VERSION_HEX >= 0x030900B1
#define __Pyx_PyCFunction_CheckExact(func)  PyCFunction_CheckExact(func)
#else
#define __Pyx_PyCFunction_CheckExact(func)  PyCFunction_Check(func)
#endif
#define __Pyx_CyOrPyCFunction_Check(func)  PyCFunction_Check(func)
#if CYTHON_COMPILING_IN_CPYTHON
#define __Pyx_CyOrPyCFunction_GET_FUNCTION(func)  (((PyCFunctionObject*)(func))->m_ml->ml_meth)
#elif !CYTHON_COMPILING_IN_LIMITED_API
#define __Pyx_CyOrPyCFunction_GET_FUNCTION(func)  PyCFunction_GET_FUNCTION(func)
#endif
#if CYTHON_COMPILING_IN_CPYTHON
#define __Pyx_CyOrPyCFunction_GET_FLAGS(func)  (((PyCFunctionObject*)(func))->m_ml->ml_flags)
static CYTHON_INLINE PyObject* __Pyx_CyOrPyCFunction_GET_SELF(PyObject *func) {
    return (__Pyx_CyOrPyCFunction_GET_FLAGS(func) & METH_STATIC) ? NULL : ((PyCFunctionObject*)func)->m_self;
}
#endif
static CYTHON_INLINE int __Pyx__IsSameCFunction(PyObject *func, void (*cfunc)(void)) {
#if CYTHON_COMPILING_IN_LIMITED_API
    return PyCFunction_Check(func) && PyCFunction_GetFunction(func) == (PyCFunction) cfunc;
#else
    return PyCFunction_Check(func) && PyCFunction_GET_FUNCTION(func) == (PyCFunction) cfunc;
#endif
}
#define __Pyx_IsSameCFunction(func, cfunc)   __Pyx__IsSameCFunction(func, cfunc)
#if PY_VERSION_HEX < 0x03090000 || (CYTHON_COMPILING_IN_LIMITED_API && __PYX_LIMITED_VERSION_HEX < 0x030A0000)
  #define __Pyx_PyType_FromModuleAndSpec(m, s, b)  ((void)m, PyType_FromSpecWithBases(s, b))
  typedef PyObject *(*__Pyx_PyCMethod)(PyObject *, PyTypeObject *, PyObject *const *, size_t, PyObject *);
#else
  #define __Pyx_PyType_FromModuleAndSpec(m, s, b)  PyType_FromModuleAndSpec(m, s, b)
  #define __Pyx_PyCMethod  PyCMethod
#endif
#ifndef METH_METHOD
  #define METH_METHOD 0x200
#endif
#if CYTHON_COMPILING_IN_PYPY && !defined(PyObject_Malloc)
  #define PyObject_Malloc(s)   PyMem_Malloc(s)
  #define PyObject_Free(p)     PyMem_Free(p)
  #define PyObject_Realloc(p)  PyMem_Realloc(p)
#endif
#if CYTHON_COMPILING_IN_LIMITED_API
  #define __Pyx_PyFrame_SetLineNumber(frame, lineno)
#elif CYTHON_COMPILING_IN_GRAAL && defined(GRAALPY_VERSION_NUM) && GRAALPY_VERSION_NUM > 0x19000000
  #define __Pyx_PyCode_HasFreeVars(co)  (PyCode_GetNumFree(co) > 0)
  #define __Pyx_PyFrame_SetLineNumber(frame, lineno) GraalPyFrame_SetLineNumber((frame), (lineno))
#elif CYTHON_COMPILING_IN_GRAAL
  #define __Pyx_PyCode_HasFreeVars(co)  (PyCode_GetNumFree(co) > 0)
  #define __Pyx_PyFrame_SetLineNumber(frame, lineno) _PyFrame_SetLineNumber((frame), (lineno))
#else
  #define __Pyx_PyCode_HasFreeVars(co)  (PyCode_GetNumFree(co) > 0)
  #define __Pyx_PyFrame_SetLineNumber(frame, lineno)  (frame)->f_lineno = (lineno)
#endif
#if CYTHON_COMPILING_IN_LIMITED_API
  #define __Pyx_PyThreadState_Current PyThreadState_Get()
#elif !CYTHON_FAST_THREAD_STATE
  #define __Pyx_PyThreadState_Current PyThreadState_GET()
#elif PY_VERSION_HEX >= 0x030d00A1
  #define __Pyx_PyThreadState_Current PyThreadState_GetUnchecked()
#else
  #define __Pyx_PyThreadState_Current _PyThreadState_UncheckedGet()
#endif
#if CYTHON_USE_MODULE_STATE
static CYTHON_INLINE void *__Pyx__PyModule_GetState(PyObject *op)
{
    void *result;
    result = PyModule_GetState(op);
    if (!result)
        Py_FatalError("Couldn't find the module state");
    return result;
}
#define __Pyx_PyModule_GetState(o) (__pyx_mstatetype *)__Pyx__PyModule_GetState(o)
#else
#define __Pyx_PyModule_GetState(op) ((void)op,__pyx_mstate_global)
#endif
#define __Pyx_PyObject_GetSlot(obj, name, func_ctype)  __Pyx_PyType_GetSlot(Py_TYPE((PyObject *) obj), name, func_ctype)
#define __Pyx_PyObject_TryGetSlot(obj, name, func_ctype) __Pyx_PyType_TryGetSlot(Py_TYPE(obj), name, func_ctype)
#define __Pyx_PyObject_GetSubSlot(obj, sub, name, func_ctype) __Pyx_PyType_GetSubSlot(Py_TYPE(obj), sub, name, func_ctype)
#define __Pyx_PyObject_TryGetSubSlot(obj, sub, name, func_ctype) __Pyx_PyType_TryGetSubSlot(Py_TYPE(obj), sub, name, func_ctype)
#if CYTHON_USE_TYPE_SLOTS
  #define __Pyx_PyType_GetSlot(type, name, func_ctype)  ((type)->name)
  #define __Pyx_PyType_TryGetSlot(type, name, func_ctype) __Pyx_PyType_GetSlot(type, name, func_ctype)
  #define __Pyx_PyType_GetSubSlot(type, sub, name, func_ctype) (((type)->sub) ? ((type)->sub->name) : NULL)
  #define __Pyx_PyType_TryGetSubSlot(type, sub, name, func_ctype) __Pyx_PyType_GetSubSlot(type, sub, name, func_ctype)
#else
  #define __Pyx_PyType_GetSlot(type, name, func_ctype)  ((func_ctype) PyType_GetSlot((type), Py_##name))
  #define __Pyx_PyType_TryGetSlot(type, name, func_ctype)\
    ((__PYX_LIMITED_VERSION_HEX >= 0x030A0000 ||\
     (PyType_GetFlags(type) & Py_TPFLAGS_HEAPTYPE) || __Pyx_get_runtime_version() >= 0x030A0000) ?\
     __Pyx_PyType_GetSlot(type, name, func_ctype) : NULL)
  #define __Pyx_PyType_GetSubSlot(obj, sub, name, func_ctype) __Pyx_PyType_GetSlot(obj, name, func_ctype)
  #define __Pyx_PyType_TryGetSubSlot(obj, sub, name, func_ctype) __Pyx_PyType_TryGetSlot(obj, name, func_ctype)
#endif
#if CYTHON_COMPILING_IN_CPYTHON || defined(_PyDict_NewPresized)
#define __Pyx_PyDict_NewPresized(n)  ((n <= 8) ? PyDict_New() : _PyDict_NewPresized(n))
#else
#define __Pyx_PyDict_NewPresized(n)  PyDict_New()
#endif
#define __Pyx_PyNumber_Divide(x,y)         PyNumber_TrueDivide(x,y)
#define __Pyx_PyNumber_InPlaceDivide(x,y)  PyNumber_InPlaceTrueDivide(x,y)
#if CYTHON_COMPILING_IN_CPYTHON && CYTHON_USE_UNICODE_INTERNALS
#define __Pyx_PyDict_GetItemStrWithError(dict, name)  _PyDict_GetItem_KnownHash(dict, name, ((PyASCIIObject *) name)->hash)
static CYTHON_INLINE PyObject * __Pyx_PyDict_GetItemStr(PyObject *dict, PyObject *name) {
    PyObject *res = __Pyx_PyDict_GetItemStrWithError(dict, name);
    if (res == NULL) PyErr_Clear();
    return res;
}
#elif !CYTHON_COMPILING_IN_PYPY || PYPY_VERSION_NUM >= 0x07020000
#define __Pyx_PyDict_GetItemStrWithError  PyDict_GetItemWithError
#define __Pyx_PyDict_GetItemStr           PyDict_GetItem
#else
static CYTHON_INLINE PyObject * __Pyx_PyDict_GetItemStrWithError(PyObject *dict, PyObject *name) {
#if CYTHON_COMPILING_IN_PYPY
    return PyDict_GetItem(dict, name);
#else
    PyDictEntry *ep;
    PyDictObject *mp = (PyDictObject*) dict;
    long hash = ((PyStringObject *) name)->ob_shash;
    assert(hash != -1);
    ep = (mp->ma_lookup)(mp, name, hash);
    if (ep == NULL) {
        return NULL;
    }
    return ep->me_value;
#endif
}
#define __Pyx_PyDict_GetItemStr           PyDict_GetItem
#endif
#if CYTHON_USE_TYPE_SLOTS
  #define __Pyx_PyType_GetFlags(tp)   (((PyTypeObject *)tp)->tp_flags)
  #define __Pyx_PyType_HasFeature(type, feature)  ((__Pyx_PyType_GetFlags(type) & (feature)) != 0)
#else
  #define __Pyx_PyType_GetFlags(tp)   (PyType_GetFlags((PyTypeObject *)tp))
  #define __Pyx_PyType_HasFeature(type, feature)  PyType_HasFeature(type, feature)
#endif
#define __Pyx_PyObject_GetIterNextFunc(iterator)  __Pyx_PyObject_GetSlot(iterator, tp_iternext, iternextfunc)
#if CYTHON_USE_TYPE_SPECS
#define __Pyx_PyHeapTypeObject_GC_Del(obj)  {\
    PyTypeObject *type = Py_TYPE((PyObject*)obj);\
    assert(__Pyx_PyType_HasFeature(type, Py_TPFLAGS_HEAPTYPE));\
    PyObject_GC_Del(obj);\
    Py_DECREF(type);\
}
#else
#define __Pyx_PyHeapTypeObject_GC_Del(obj)  PyObject_GC_Del(obj)
#endif
#if CYTHON_COMPILING_IN_LIMITED_API
  #define __Pyx_PyUnicode_READY(op)       (0)
  #define __Pyx_PyUnicode_READ_CHAR(u, i) PyUnicode_ReadChar(u, i)
  #define __Pyx_PyUnicode_MAX_CHAR_VALUE(u)   ((void)u, 1114111U)
  #define __Pyx_PyUnicode_KIND(u)         ((void)u, (0))
  #define __Pyx_PyUnicode_DATA(u)         ((void*)u)
  #define __Pyx_PyUnicode_READ(k, d, i)   ((void)k, PyUnicode_ReadChar((PyObject*)(d), i))
  #define __Pyx_PyUnicode_IS_TRUE(u)      (0 != PyUnicode_GetLength(u))
#else
  #if PY_VERSION_HEX >= 0x030C0000
    #define __Pyx_PyUnicode_READY(op)       (0)
  #else
    #define __Pyx_PyUnicode_READY(op)       (likely(PyUnicode_IS_READY(op)) ?\
                                                0 : _PyUnicode_Ready((PyObject *)(op)))
  #endif
  #define __Pyx_PyUnicode_READ_CHAR(u, i) PyUnicode_READ_CHAR(u, i)
  #define __Pyx_PyUnicode_MAX_CHAR_VALUE(u)   PyUnicode_MAX_CHAR_VALUE(u)
  #define __Pyx_PyUnicode_KIND(u)         ((int)PyUnicode_KIND(u))
  #define __Pyx_PyUnicode_DATA(u)         PyUnicode_DATA(u)
  #define __Pyx_PyUnicode_READ(k, d, i)   PyUnicode_READ(k, d, i)
  #define __Pyx_PyUnicode_WRITE(k, d, i, ch)  PyUnicode_WRITE(k, d, i, (Py_UCS4) ch)
  #if PY_VERSION_HEX >= 0x030C0000
    #define __Pyx_PyUnicode_IS_TRUE(u)      (0 != PyUnicode_GET_LENGTH(u))
  #else
    #if CYTHON_COMPILING_IN_CPYTHON && PY_VERSION_HEX >= 0x03090000
    #define __Pyx_PyUnicode_IS_TRUE(u)      (0 != (likely(PyUnicode_IS_READY(u)) ? PyUnicode_GET_LENGTH(u) : ((PyCompactUnicodeObject *)(u))->wstr_length))
    #else
    #define __Pyx_PyUnicode_IS_TRUE(u)      (0 != (likely(PyUnicode_IS_READY(u)) ? PyUnicode_GET_LENGTH(u) : PyUnicode_GET_SIZE(u)))
    #endif
  #endif
#endif
#if CYTHON_COMPILING_IN_PYPY
  #define __Pyx_PyUnicode_Concat(a, b)      PyNumber_Add(a, b)
  #define __Pyx_PyUnicode_ConcatSafe(a, b)  PyNumber_Add(a, b)
#else
  #define __Pyx_PyUnicode_Concat(a, b)      PyUnicode_Concat(a, b)
  #define __Pyx_PyUnicode_ConcatSafe(a, b)  ((unlikely((a) == Py_None) || unlikely((b) == Py_None)) ?\
      PyNumber_Add(a, b) : __Pyx_PyUnicode_Concat(a, b))
#endif
#if CYTHON_COMPILING_IN_PYPY
  #if !defined(PyUnicode_DecodeUnicodeEscape)
    #define PyUnicode_DecodeUnicodeEscape(s, size, errors)  PyUnicode_Decode(s, size, "unicode_escape", errors)
  #endif
  #if !defined(PyUnicode_Contains)
    #define PyUnicode_Contains(u, s)  PySequence_Contains(u, s)
  #endif
  #if !defined(PyByteArray_Check)
    #define PyByteArray_Check(obj)  PyObject_TypeCheck(obj, &PyByteArray_Type)
  #endif
  #if !defined(PyObject_Format)
    #define PyObject_Format(obj, fmt)  PyObject_CallMethod(obj, "__format__", "O", fmt)
  #endif
#endif
#define __Pyx_PyUnicode_FormatSafe(a, b)  ((unlikely((a) == Py_None || (PyUnicode_Check(b) && !PyUnicode_CheckExact(b)))) ? PyNumber_Remainder(a, b) : PyUnicode_Format(a, b))
#if CYTHON_COMPILING_IN_CPYTHON && PY_VERSION_HEX >= 0x030E0000
  #define __Pyx_PySequence_ListKeepNew(obj)\
    (likely(PyList_CheckExact(obj) && PyUnstable_Object_IsUniquelyReferenced(obj)) ? __Pyx_NewRef(obj) : PySequence_List(obj))
#elif CYTHON_COMPILING_IN_CPYTHON
  #define __Pyx_PySequence_ListKeepNew(obj)\
    (likely(PyList_CheckExact(obj) && Py_REFCNT(obj) == 1) ? __Pyx_NewRef(obj) : PySequence_List(obj))
#else
  #define __Pyx_PySequence_ListKeepNew(obj)  PySequence_List(obj)
#endif
#ifndef PySet_CheckExact
  #define PySet_CheckExact(obj)        __Pyx_IS_TYPE(obj, &PySet_Type)
#endif
#if PY_VERSION_HEX >= 0x030900A4
  #define __Pyx_SET_REFCNT(obj, refcnt) Py_SET_REFCNT(obj, refcnt)
  #define __Pyx_SET_SIZE(obj, size) Py_SET_SIZE(obj, size)
#else
  #define __Pyx_SET_REFCNT(obj, refcnt) Py_REFCNT(obj) = (refcnt)
  #define __Pyx_SET_SIZE(obj, size) Py_SIZE(obj) = (size)
#endif
enum __Pyx_ReferenceSharing {
  __Pyx_ReferenceSharing_DefinitelyUnique,
  __Pyx_ReferenceSharing_OwnStrongReference,
  __Pyx_ReferenceSharing_FunctionArgument,
  __Pyx_ReferenceSharing_SharedReference,
};
#if CYTHON_COMPILING_IN_CPYTHON_FREETHREADING && PY_VERSION_HEX >= 0x030E0000
#define __Pyx_IS_UNIQUELY_REFERENCED(o, sharing)\
    (sharing == __Pyx_ReferenceSharing_DefinitelyUnique ? 1 :\
      (sharing == __Pyx_ReferenceSharing_FunctionArgument ? PyUnstable_Object_IsUniqueReferencedTemporary(o) :\
      (sharing == __Pyx_ReferenceSharing_OwnStrongReference ? PyUnstable_Object_IsUniquelyReferenced(o) : 0)))
#elif (CYTHON_COMPILING_IN_CPYTHON && !CYTHON_COMPILING_IN_CPYTHON_FREETHREADING) || CYTHON_COMPILING_IN_LIMITED_API
#define __Pyx_IS_UNIQUELY_REFERENCED(o, sharing) (((void)sharing), Py_REFCNT(o) == 1)
#else
#define __Pyx_IS_UNIQUELY_REFERENCED(o, sharing) (((void)o), ((void)sharing), 0)
#endif
#if CYTHON_AVOID_BORROWED_REFS || CYTHON_AVOID_THREAD_UNSAFE_BORROWED_REFS
  #if __PYX_LIMITED_VERSION_HEX >= 0x030d0000
    #define __Pyx_PyList_GetItemRef(o, i) PyList_GetItemRef(o, i)
  #elif CYTHON_COMPILING_IN_LIMITED_API || !CYTHON_ASSUME_SAFE_MACROS
    #define __Pyx_PyList_GetItemRef(o, i) (likely((i) >= 0) ? PySequence_GetItem(o, i) : (PyErr_SetString(PyExc_IndexError, "list index out of range"), (PyObject*)NULL))
  #else
    #define __Pyx_PyList_GetItemRef(o, i) PySequence_ITEM(o, i)
  #endif
#elif CYTHON_COMPILING_IN_LIMITED_API || !CYTHON_ASSUME_SAFE_MACROS
  #if __PYX_LIMITED_VERSION_HEX >= 0x030d0000
    #define __Pyx_PyList_GetItemRef(o, i) PyList_GetItemRef(o, i)
  #else
    #define __Pyx_PyList_GetItemRef(o, i) __Pyx_XNewRef(PyList_GetItem(o, i))
  #endif
#else
  #define __Pyx_PyList_GetItemRef(o, i) __Pyx_NewRef(PyList_GET_ITEM(o, i))
#endif
#if CYTHON_AVOID_THREAD_UNSAFE_BORROWED_REFS && !CYTHON_COMPILING_IN_LIMITED_API && CYTHON_ASSUME_SAFE_MACROS
  #define __Pyx_PyList_GetItemRefFast(o, i, unsafe_shared) (__Pyx_IS_UNIQUELY_REFERENCED(o, unsafe_shared) ?\
    __Pyx_NewRef(PyList_GET_ITEM(o, i)) : __Pyx_PyList_GetItemRef(o, i))
#else
  #define __Pyx_PyList_GetItemRefFast(o, i, unsafe_shared) __Pyx_PyList_GetItemRef(o, i)
#endif
#if __PYX_LIMITED_VERSION_HEX >= 0x030d0000
#define __Pyx_PyDict_GetItemRef(dict, key, result) PyDict_GetItemRef(dict, key, result)
#elif CYTHON_AVOID_BORROWED_REFS || CYTHON_AVOID_THREAD_UNSAFE_BORROWED_REFS
static CYTHON_INLINE int __Pyx_PyDict_GetItemRef(PyObject *dict, PyObject *key, PyObject **result) {
  *result = PyObject_GetItem(dict, key);
  if (*result == NULL) {
    if (PyErr_ExceptionMatches(PyExc_KeyError)) {
      PyErr_Clear();
      return 0;
    }
    return -1;
  }
  return 1;
}
#else
static CYTHON_INLINE int __Pyx_PyDict_GetItemRef(PyObject *dict, PyObject *key, PyObject **result) {
  *result = PyDict_GetItemWithError(dict, key);
  if (*result == NULL) {
    return PyErr_Occurred() ? -1 : 0;
  }
  Py_INCREF(*result);
  return 1;
}
#endif
#if defined(CYTHON_DEBUG_VISIT_CONST) && CYTHON_DEBUG_VISIT_CONST
  #define __Pyx_VISIT_CONST(obj)  Py_VISIT(obj)
#else
  #define __Pyx_VISIT_CONST(obj)
#endif
#if CYTHON_ASSUME_SAFE_MACROS
  #define __Pyx_PySequence_ITEM(o, i) PySequence_ITEM(o, i)
  #define __Pyx_PySequence_SIZE(seq)  Py_SIZE(seq)
  #define __Pyx_PyTuple_SET_ITEM(o, i, v) (PyTuple_SET_ITEM(o, i, v), (0))
  #define __Pyx_PyTuple_GET_ITEM(o, i) PyTuple_GET_ITEM(o, i)
  #define __Pyx_PyList_SET_ITEM(o, i, v) (PyList_SET_ITEM(o, i, v), (0))
  #define __Pyx_PyList_GET_ITEM(o, i) PyList_GET_ITEM(o, i)
#else
  #define __Pyx_PySequence_ITEM(o, i) PySequence_GetItem(o, i)
  #define __Pyx_PySequence_SIZE(seq)  PySequence_Size(seq)
  #define __Pyx_PyTuple_SET_ITEM(o, i, v) PyTuple_SetItem(o, i, v)
  #define __Pyx_PyTuple_GET_ITEM(o, i) PyTuple_GetItem(o, i)
  #define __Pyx_PyList_SET_ITEM(o, i, v) PyList_SetItem(o, i, v)
  #define __Pyx_PyList_GET_ITEM(o, i) PyList_GetItem(o, i)
#endif
#if CYTHON_ASSUME_SAFE_SIZE
  #define __Pyx_PyTuple_GET_SIZE(o) PyTuple_GET_SIZE(o)
  #define __Pyx_PyList_GET_SIZE(o) PyList_GET_SIZE(o)
  #define __Pyx_PySet_GET_SIZE(o) PySet_GET_SIZE(o)
  #define __Pyx_PyBytes_GET_SIZE(o) PyBytes_GET_SIZE(o)
  #define __Pyx_PyByteArray_GET_SIZE(o) PyByteArray_GET_SIZE(o)
  #define __Pyx_PyUnicode_GET_LENGTH(o) PyUnicode_GET_LENGTH(o)
#else
  #define __Pyx_PyTuple_GET_SIZE(o) PyTuple_Size(o)
  #define __Pyx_PyList_GET_SIZE(o) PyList_Size(o)
  #define __Pyx_PySet_GET_SIZE(o) PySet_Size(o)
  #define __Pyx_PyBytes_GET_SIZE(o) PyBytes_Size(o)
  #define __Pyx_PyByteArray_GET_SIZE(o) PyByteArray_Size(o)
  #define __Pyx_PyUnicode_GET_LENGTH(o) PyUnicode_GetLength(o)
#endif
#if CYTHON_COMPILING_IN_PYPY && !defined(PyUnicode_InternFromString)
  #define PyUnicode_InternFromString(s) PyUnicode_FromString(s)
#endif
#define __Pyx_PyLong_FromHash_t PyLong_FromSsize_t
#define __Pyx_PyLong_AsHash_t   __Pyx_PyIndex_AsSsize_t
#if __PYX_LIMITED_VERSION_HEX >= 0x030A0000
    #define __Pyx_PySendResult PySendResult
#else
    typedef enum {
        PYGEN_RETURN = 0,
        PYGEN_ERROR = -1,
        PYGEN_NEXT = 1,
    } __Pyx_PySendResult;
#endif
#if CYTHON_COMPILING_IN_LIMITED_API || PY_VERSION_HEX < 0x030A00A3
  typedef __Pyx_PySendResult (*__Pyx_pyiter_sendfunc)(PyObject *iter, PyObject *value, PyObject **result);
#else
  #define __Pyx_pyiter_sendfunc sendfunc
#endif
#if !CYTHON_USE_AM_SEND
#define __PYX_HAS_PY_AM_SEND 0
#elif __PYX_LIMITED_VERSION_HEX >= 0x030A0000
#define __PYX_HAS_PY_AM_SEND 1
#else
#define __PYX_HAS_PY_AM_SEND 2
#endif
#if __PYX_HAS_PY_AM_SEND < 2
    #define __Pyx_PyAsyncMethodsStruct PyAsyncMethods
#else
    typedef struct {
        unaryfunc am_await;
        unaryfunc am_aiter;
        unaryfunc am_anext;
        __Pyx_pyiter_sendfunc am_send;
    } __Pyx_PyAsyncMethodsStruct;
    #define __Pyx_SlotTpAsAsync(s) ((PyAsyncMethods*)(s))
#endif
#if CYTHON_USE_AM_SEND && PY_VERSION_HEX < 0x030A00F0
    #define __Pyx_TPFLAGS_HAVE_AM_SEND (1UL << 21)
#else
    #define __Pyx_TPFLAGS_HAVE_AM_SEND (0)
#endif
#if PY_VERSION_HEX >= 0x03090000
#define __Pyx_PyInterpreterState_Get() PyInterpreterState_Get()
#else
#define __Pyx_PyInterpreterState_Get() PyThreadState_Get()->interp
#endif
#if CYTHON_COMPILING_IN_LIMITED_API && PY_VERSION_HEX < 0x030A0000
#ifdef __cplusplus
extern "C"
#endif
PyAPI_FUNC(void *) PyMem_Calloc(size_t nelem, size_t elsize);
#endif
#if CYTHON_COMPILING_IN_LIMITED_API
static int __Pyx_init_co_variable(PyObject *inspect, const char* name, int *write_to) {
    int value;
    PyObject *py_value = PyObject_GetAttrString(inspect, name);
    if (!py_value) return 0;
    value = (int) PyLong_AsLong(py_value);
    Py_DECREF(py_value);
    *write_to = value;
    return value != -1 || !PyErr_Occurred();
}
static int __Pyx_init_co_variables(void) {
    PyObject *inspect;
    int result;
    inspect = PyImport_ImportModule("inspect");
    result =
#if !defined(CO_OPTIMIZED)
        __Pyx_init_co_variable(inspect, "CO_OPTIMIZED", &CO_OPTIMIZED) &&
#endif
#if !defined(CO_NEWLOCALS)
        __Pyx_init_co_variable(inspect, "CO_NEWLOCALS", &CO_NEWLOCALS) &&
#endif
#if !defined(CO_VARARGS)
        __Pyx_init_co_variable(inspect, "CO_VARARGS", &CO_VARARGS) &&
#endif
#if !defined(CO_VARKEYWORDS)
        __Pyx_init_co_variable(inspect, "CO_VARKEYWORDS", &CO_VARKEYWORDS) &&
#endif
#if !defined(CO_ASYNC_GENERATOR)
        __Pyx_init_co_variable(inspect, "CO_ASYNC_GENERATOR", &CO_ASYNC_GENERATOR) &&
#endif
#if !defined(CO_GENERATOR)
        __Pyx_init_co_variable(inspect, "CO_GENERATOR", &CO_GENERATOR) &&
#endif
#if !defined(CO_COROUTINE)
        __Pyx_init_co_variable(inspect, "CO_COROUTINE", &CO_COROUTINE) &&
#endif
        1;
    Py_DECREF(inspect);
    return result ? 0 : -1;
}
#else
static int __Pyx_init_co_variables(void) {
    return 0;
}
#endif

#if defined(_WIN32) || defined(WIN32) || defined(MS_WINDOWS)
  #ifndef _USE_MATH_DEFINES
    #define _USE_MATH_DEFINES
  #endif
#endif
#include <math.h>
#if defined(__CYGWIN__) && defined(_LDBL_EQ_DBL)
#define __Pyx_truncl trunc
#else
#define __Pyx_truncl truncl
#endif

#ifndef CYTHON_CLINE_IN_TRACEBACK_RUNTIME
#define CYTHON_CLINE_IN_TRACEBACK_RUNTIME 0
#endif
#ifndef CYTHON_CLINE_IN_TRACEBACK
#define CYTHON_CLINE_IN_TRACEBACK CYTHON_CLINE_IN_TRACEBACK_RUNTIME
#endif
#if CYTHON_CLINE_IN_TRACEBACK
#define __PYX_MARK_ERR_POS(f_index, lineno)  { __pyx_filename = __pyx_f[f_index]; (void) __pyx_filename; __pyx_lineno = lineno; (void) __pyx_lineno; __pyx_clineno = __LINE__; (void) __pyx_clineno; }
#else
#define __PYX_MARK_ERR_POS(f_index, lineno)  { __pyx_filename = __pyx_f[f_index]; (void) __pyx_filename; __pyx_lineno = lineno; (void) __pyx_lineno; (void) __pyx_clineno; }
#endif
#define __PYX_ERR(f_index, lineno, Ln_error) \
    { __PYX_MARK_ERR_POS(f_index, lineno) goto Ln_error; }

#ifdef CYTHON_EXTERN_C
    #undef __PYX_EXTERN_C
    #define __PYX_EXTERN_C CYTHON_EXTERN_C
#elif defined(__PYX_EXTERN_C)
    #ifdef _MSC_VER
    #pragma message ("Please do not define the '__PYX_EXTERN_C' macro externally. Use 'CYTHON_EXTERN_C' instead.")
    #else
    #warning Please do not define the '__PYX_EXTERN_C' macro externally. Use 'CYTHON_EXTERN_C' instead.
    #endif
#else
  #ifdef __cplusplus
    #define __PYX_EXTERN_C extern "C"
  #else
    #define __PYX_EXTERN_C extern
  #endif
#endif

#define __PYX_HAVE___fast_match
#define __PYX_HAVE_API___fast_match

#ifdef _OPENMP
#include <omp.h>
#endif

#if defined(PYREX_WITHOUT_ASSERTIONS) && !defined(CYTHON_WITHOUT_ASSERTIONS)
#define CYTHON_WITHOUT_ASSERTIONS
#endif

#ifdef CYTHON_FREETHREADING_COMPATIBLE
#if CYTHON_FREETHREADING_COMPATIBLE
#define __Pyx_FREETHREADING_COMPATIBLE Py_MOD_GIL_NOT_USED
#else
#define __Pyx_FREETHREADING_COMPATIBLE Py_MOD_GIL_USED
#endif
#else
#define __Pyx_FREETHREADING_COMPATIBLE Py_MOD_GIL_USED
#endif
#define __PYX_DEFAULT_STRING_ENCODING_IS_ASCII 0
#define __PYX_DEFAULT_STRING_ENCODING_IS_UTF8 0
#define __PYX_DEFAULT_STRING_ENCODING ""
#define __Pyx_PyObject_FromString __Pyx_PyBytes_FromString
#define __Pyx_PyObject_FromStringAndSize __Pyx_PyBytes_FromStringAndSize
#define __Pyx_uchar_cast(c) ((unsigned char)c)
#define __Pyx_long_cast(x) ((long)x)
#define __Pyx_fits_Py_ssize_t(v, type, is_signed)  (\
    (sizeof(type) < sizeof(Py_ssize_t))  ||\
    (sizeof(type) > sizeof(Py_ssize_t) &&\
          likely(v < (type)PY_SSIZE_T_MAX ||\
                 v == (type)PY_SSIZE_T_MAX)  &&\
          (!is_signed || likely(v > (type)PY_SSIZE_T_MIN ||\
                                v == (type)PY_SSIZE_T_MIN)))  ||\
    (sizeof(type) == sizeof(Py_ssize_t) &&\
          (is_signed || likely(v < (type)PY_SSIZE_T_MAX ||\
                               v == (type)PY_SSIZE_T_MAX)))  )
static CYTHON_INLINE int __Pyx_is_valid_index(Py_ssize_t i, Py_ssize_t limit) {
    return (size_t) i < (size_t) limit;
}
#if defined (__cplusplus) && __cplusplus >= 201103L
    #include <cstdlib>
    #define __Pyx_sst_abs(value) std::abs(value)
#elif SIZEOF_INT >= SIZEOF_SIZE_T
    #define __Pyx_sst_abs(value) abs(value)
#elif SIZEOF_LONG >= SIZEOF_SIZE_T
    #define __Pyx_sst_abs(value) labs(value)
#elif defined (_MSC_VER)
    #define __Pyx_sst_abs(value) ((Py_ssize_t)_abs64(value))
#elif defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    #define __Pyx_sst_abs(value) llabs(value)
#elif defined (__GNUC__)
    #define __Pyx_sst_abs(value) __builtin_llabs(value)
#else
    #define __Pyx_sst_abs(value) ((value<0) ? -value : value)
#endif
static CYTHON_INLINE Py_ssize_t __Pyx_ssize_strlen(const char *s);
static CYTHON_INLINE const char* __Pyx_PyObject_AsString(PyObject*);
static CYTHON_INLINE const char* __Pyx_PyObject_AsStringAndSize(PyObject*, Py_ssize_t* length);
static CYTHON_INLINE PyObject* __Pyx_PyByteArray_FromString(const char*);
#define __Pyx_PyByteArray_FromStringAndSize(s, l) PyByteArray_FromStringAndSize((const char*)s, l)
#define __Pyx_PyBytes_FromString        PyBytes_FromString
#define __Pyx_PyBytes_FromStringAndSize PyBytes_FromStringAndSize
static CYTHON_INLINE PyObject* __Pyx_PyUnicode_FromString(const char*);
#if CYTHON_ASSUME_SAFE_MACROS
    #define __Pyx_PyBytes_AsWritableString(s)     ((char*) PyBytes_AS_STRING(s))
    #define __Pyx_PyBytes_AsWritableSString(s)    ((signed char*) PyBytes_AS_STRING(s))
    #define __Pyx_PyBytes_AsWritableUString(s)    ((unsigned char*) PyBytes_AS_STRING(s))
    #define __Pyx_PyBytes_AsString(s)     ((const char*) PyBytes_AS_STRING(s))
    #define __Pyx_PyBytes_AsSString(s)    ((const signed char*) PyBytes_AS_STRING(s))
    #define __Pyx_PyBytes_AsUString(s)    ((const unsigned char*) PyBytes_AS_STRING(s))
    #define __Pyx_PyByteArray_AsString(s) PyByteArray_AS_STRING(s)
#else
    #define __Pyx_PyBytes_AsWritableString(s)     ((char*) PyBytes_AsString(s))
    #define __Pyx_PyBytes_AsWritableSString(s)    ((signed char*) PyBytes_AsString(s))
    #define __Pyx_PyBytes_AsWritableUString(s)    ((unsigned char*) PyBytes_AsString(s))
    #define __Pyx_PyBytes_AsString(s)     ((const char*) PyBytes_AsString(s))
    #define __Pyx_PyBytes_AsSString(s)    ((const signed char*) PyBytes_AsString(s))
    #define __Pyx_PyBytes_AsUString(s)    ((const unsigned char*) PyBytes_AsString(s))
    #define __Pyx_PyByteArray_AsString(s) PyByteArray_AsString(s)
#endif
#define __Pyx_PyObject_AsWritableString(s)    ((char*)(__pyx_uintptr_t) __Pyx_PyObject_AsString(s))
#define __Pyx_PyObject_AsWritableSString(s)    ((signed char*)(__pyx_uintptr_t) __Pyx_PyObject_AsString(s))
#define __Pyx_PyObject_AsWritableUString(s)    ((unsigned char*)(__pyx_uintptr_t) __Pyx_PyObject_AsString(s))
#define __Pyx_PyObject_AsSString(s)    ((const signed char*) __Pyx_PyObject_AsString(s))
#define __Pyx_PyObject_AsUString(s)    ((const unsigned char*) __Pyx_PyObject_AsString(s))
#define __Pyx_PyObject_FromCString(s)  __Pyx_PyObject_FromString((const char*)s)
#define __Pyx_PyBytes_FromCString(s)   __Pyx_PyBytes_FromString((const char*)s)
#define __Pyx_PyByteArray_FromCString(s)   __Pyx_PyByteArray_FromString((const char*)s)
#define __Pyx_PyUnicode_FromCString(s) __Pyx_PyUnicode_FromString((const char*)s)
#define __Pyx_PyUnicode_FromOrdinal(o)       PyUnicode_FromOrdinal((int)o)
#define __Pyx_PyUnicode_AsUnicode            PyUnicode_AsUnicode
static CYTHON_INLINE PyObject *__Pyx_NewRef(PyObject *obj) {
#if CYTHON_COMPILING_IN_CPYTHON && PY_VERSION_HEX >= 0x030a0000 || defined(Py_NewRef)
    return Py_NewRef(obj);
#else
    Py_INCREF(obj);
    return obj;
#endif
}
static CYTHON_INLINE PyObject *__Pyx_XNewRef(PyObject *obj) {
#if CYTHON_COMPILING_IN_CPYTHON && PY_VERSION_HEX >= 0x030a0000 || defined(Py_XNewRef)
    return Py_XNewRef(obj);
#else
    Py_XINCREF(obj);
    return obj;
#endif
}
static CYTHON_INLINE PyObject *__Pyx_Owned_Py_None(int b);
static CYTHON_INLINE PyObject * __Pyx_PyBool_FromLong(long b);
static CYTHON_INLINE int __Pyx_PyObject_IsTrue(PyObject*);
static CYTHON_INLINE int __Pyx_PyObject_IsTrueAndDecref(PyObject*);
static CYTHON_INLINE PyObject* __Pyx_PyNumber_Long(PyObject* x);
#define __Pyx_PySequence_Tuple(obj)\
    (likely(PyTuple_CheckExact(obj)) ? __Pyx_NewRef(obj) : PySequence_Tuple(obj))
static CYTHON_INLINE Py_ssize_t __Pyx_PyIndex_AsSsize_t(PyObject*);
static CYTHON_INLINE PyObject * __Pyx_PyLong_FromSize_t(size_t);
static CYTHON_INLINE Py_hash_t __Pyx_PyIndex_AsHash_t(PyObject*);
#if CYTHON_ASSUME_SAFE_MACROS
#define __Pyx_PyFloat_AsDouble(x) (PyFloat_CheckExact(x) ? PyFloat_AS_DOUBLE(x) : PyFloat_AsDouble(x))
#define __Pyx_PyFloat_AS_DOUBLE(x) PyFloat_AS_DOUBLE(x)
#else
#define __Pyx_PyFloat_AsDouble(x) PyFloat_AsDouble(x)
#define __Pyx_PyFloat_AS_DOUBLE(x) PyFloat_AsDouble(x)
#endif
#define __Pyx_PyFloat_AsFloat(x) ((float) __Pyx_PyFloat_AsDouble(x))
#define __Pyx_PyNumber_Int(x) (PyLong_CheckExact(x) ? __Pyx_NewRef(x) : PyNumber_Long(x))
#if CYTHON_USE_PYLONG_INTERNALS
  #if PY_VERSION_HEX >= 0x030C00A7
  #ifndef _PyLong_SIGN_MASK
    #define _PyLong_SIGN_MASK 3
  #endif
  #ifndef _PyLong_NON_SIZE_BITS
    #define _PyLong_NON_SIZE_BITS 3
  #endif
  #define __Pyx_PyLong_Sign(x)  (((PyLongObject*)x)->long_value.lv_tag & _PyLong_SIGN_MASK)
  #define __Pyx_PyLong_IsNeg(x)  ((__Pyx_PyLong_Sign(x) & 2) != 0)
  #define __Pyx_PyLong_IsNonNeg(x)  (!__Pyx_PyLong_IsNeg(x))
  #define __Pyx_PyLong_IsZero(x)  (__Pyx_PyLong_Sign(x) & 1)
  #define __Pyx_PyLong_IsPos(x)  (__Pyx_PyLong_Sign(x) == 0)
  #define __Pyx_PyLong_CompactValueUnsigned(x)  (__Pyx_PyLong_Digits(x)[0])
  #define __Pyx_PyLong_DigitCount(x)  ((Py_ssize_t) (((PyLongObject*)x)->long_value.lv_tag >> _PyLong_NON_SIZE_BITS))
  #define __Pyx_PyLong_SignedDigitCount(x)\
        ((1 - (Py_ssize_t) __Pyx_PyLong_Sign(x)) * __Pyx_PyLong_DigitCount(x))
  #if defined(PyUnstable_Long_IsCompact) && defined(PyUnstable_Long_CompactValue)
    #define __Pyx_PyLong_IsCompact(x)     PyUnstable_Long_IsCompact((PyLongObject*) x)
    #define __Pyx_PyLong_CompactValue(x)  PyUnstable_Long_CompactValue((PyLongObject*) x)
  #else
    #define __Pyx_PyLong_IsCompact(x)     (((PyLongObject*)x)->long_value.lv_tag < (2 << _PyLong_NON_SIZE_BITS))
    #define __Pyx_PyLong_CompactValue(x)  ((1 - (Py_ssize_t) __Pyx_PyLong_Sign(x)) * (Py_ssize_t) __Pyx_PyLong_Digits(x)[0])
  #endif
  typedef Py_ssize_t  __Pyx_compact_pylong;
  typedef size_t  __Pyx_compact_upylong;
  #else
  #define __Pyx_PyLong_IsNeg(x)  (Py_SIZE(x) < 0)
  #define __Pyx_PyLong_IsNonNeg(x)  (Py_SIZE(x) >= 0)
  #define __Pyx_PyLong_IsZero(x)  (Py_SIZE(x) == 0)
  #define __Pyx_PyLong_IsPos(x)  (Py_SIZE(x) > 0)
  #define __Pyx_PyLong_CompactValueUnsigned(x)  ((Py_SIZE(x) == 0) ? 0 : __Pyx_PyLong_Digits(x)[0])
  #define __Pyx_PyLong_DigitCount(x)  __Pyx_sst_abs(Py_SIZE(x))
  #define __Pyx_PyLong_SignedDigitCount(x)  Py_SIZE(x)
  #define __Pyx_PyLong_IsCompact(x)  (Py_SIZE(x) == 0 || Py_SIZE(x) == 1 || Py_SIZE(x) == -1)
  #define __Pyx_PyLong_CompactValue(x)\
        ((Py_SIZE(x) == 0) ? (sdigit) 0 : ((Py_SIZE(x) < 0) ? -(sdigit)__Pyx_PyLong_Digits(x)[0] : (sdigit)__Pyx_PyLong_Digits(x)[0]))
  typedef sdigit  __Pyx_compact_pylong;
  typedef digit  __Pyx_compact_upylong;
  #endif
  #if PY_VERSION_HEX >= 0x030C00A5
  #define __Pyx_PyLong_Digits(x)  (((PyLongObject*)x)->long_value.ob_digit)
  #else
  #define __Pyx_PyLong_Digits(x)  (((PyLongObject*)x)->ob_digit)
  #endif
#endif
#if __PYX_DEFAULT_STRING_ENCODING_IS_UTF8
  #define __Pyx_PyUnicode_FromStringAndSize(c_str, size) PyUnicode_DecodeUTF8(c_str, size, NULL)
#elif __PYX_DEFAULT_STRING_ENCODING_IS_ASCII
  #define __Pyx_PyUnicode_FromStringAndSize(c_str, size) PyUnicode_DecodeASCII(c_str, size, NULL)
#else
  #define __Pyx_PyUnicode_FromStringAndSize(c_str, size) PyUnicode_Decode(c_str, size, __PYX_DEFAULT_STRING_ENCODING, NULL)
#endif

#if defined(__GNUC__)     && (__GNUC__ > 2 || (__GNUC__ == 2 && (__GNUC_MINOR__ > 95)))
  #define likely(x)   __builtin_expect(!!(x), 1)
  #define unlikely(x) __builtin_expect(!!(x), 0)
#else
  #define likely(x)   (x)
  #define unlikely(x) (x)
#endif

#ifdef __cplusplus
#if __cplusplus > 201103L
#include <type_traits>
#endif
template <typename T>
static void __Pyx_pretend_to_initialize(T* ptr) {
#if __cplusplus > 201103L
    if ((std::is_trivially_default_constructible<T>::value))
#endif
        *ptr = T();
    (void)ptr;
}
#else
static CYTHON_INLINE void __Pyx_pretend_to_initialize(void* ptr) { (void)ptr; }
#endif

#if !CYTHON_USE_MODULE_STATE
static PyObject *__pyx_m = NULL;
#endif
static int __pyx_lineno;
static int __pyx_clineno = 0;
static const char * const __pyx_cfilenm = __FILE__;
static const char *__pyx_filename;

static const char* const __pyx_f[] = {
  "_fast_match.pyx",
};

#include <pythread.h>
#ifndef CYTHON_ATOMICS
    #define CYTHON_ATOMICS 1
#endif
#define __PYX_CYTHON_ATOMICS_ENABLED() CYTHON_ATOMICS
#define __PYX_GET_CYTHON_COMPILING_IN_CPYTHON_FREETHREADING() CYTHON_COMPILING_IN_CPYTHON_FREETHREADING
#define __pyx_atomic_int_type int
#define __pyx_nonatomic_int_type int
#if CYTHON_ATOMICS && (defined(__STDC_VERSION__) &&\
                        (__STDC_VERSION__ >= 201112L) &&\
                        !defined(__STDC_NO_ATOMICS__))
    #include <stdatomic.h>
#elif CYTHON_ATOMICS && (defined(__cplusplus) && (\
                    (__cplusplus >= 201103L) ||\
                    (defined(_MSC_VER) && _MSC_VER >= 1700)))
    #include <atomic>
#endif
#if CYTHON_ATOMICS && (defined(__STDC_VERSION__) &&\
                        (__STDC_VERSION__ >= 201112L) &&\
                        !defined(__STDC_NO_ATOMICS__) &&\
                       ATOMIC_INT_LOCK_FREE == 2)
    #undef __pyx_atomic_int_type
    #define __pyx_atomic_int_type atomic_int
    #define __pyx_atomic_ptr_type atomic_uintptr_t
    #define __pyx_nonatomic_ptr_type uintptr_t
    #define __pyx_atomic_incr_relaxed(value) atomic_fetch_add_explicit(value, 1, memory_order_relaxed)
    #define __pyx_atomic_incr_acq_rel(value) atomic_fetch_add_explicit(value, 1, memory_order_acq_rel)
    #define __pyx_atomic_decr_acq_rel(value) atomic_fetch_sub_explicit(value, 1, memory_order_acq_rel)
    #define __pyx_atomic_sub(value, arg) atomic_fetch_sub(value, arg)
    #define __pyx_atomic_int_cmp_exchange(value, expected, desired) atomic_compare_exchange_strong(value, expected, desired)
    #define __pyx_atomic_load(value) atomic_load(value)
    #define __pyx_atomic_store(value, new_value) atomic_store(value, new_value)
    #define __pyx_atomic_pointer_load_relaxed(value) atomic_load_explicit(value, memory_order_relaxed)
    #define __pyx_atomic_pointer_load_acquire(value) atomic_load_explicit(value, memory_order_acquire)
    #define __pyx_atomic_pointer_exchange(value, new_value) atomic_exchange(value, (__pyx_nonatomic_ptr_type)new_value)
    #define __pyx_atomic_pointer_cmp_exchange(value, expected, desired) atomic_compare_exchange_strong(value, expected, desired)
    #if defined(__PYX_DEBUG_ATOMICS) && defined(_MSC_VER)
        #pragma message ("Using standard C atomics")
    #elif defined(__PYX_DEBUG_ATOMICS)
        #warning "Using standard C atomics"
    #endif
#elif CYTHON_ATOMICS && (defined(__cplusplus) && (\
                    (__cplusplus >= 201103L) ||\
\
                    (defined(_MSC_VER) && _MSC_VER >= 1700)) &&\
                    ATOMIC_INT_LOCK_FREE == 2)
    #undef __pyx_atomic_int_type
    #define __pyx_atomic_int_type std::atomic_int
    #define __pyx_atomic_ptr_type std::atomic_uintptr_t
    #define __pyx_nonatomic_ptr_type uintptr_t
    #define __pyx_atomic_incr_relaxed(value) std::atomic_fetch_add_explicit(value, 1, std::memory_order_relaxed)
    #define __pyx_atomic_incr_acq_rel(value) std::atomic_fetch_add_explicit(value, 1, std::memory_order_acq_rel)
    #define __pyx_atomic_decr_acq_rel(value) std::atomic_fetch_sub_explicit(value, 1, std::memory_order_acq_rel)
    #define __pyx_atomic_sub(value, arg) std::atomic_fetch_sub(value, arg)
    #define __pyx_atomic_int_cmp_exchange(value, expected, desired) std::atomic_compare_exchange_strong(value, expected, desired)
    #define __pyx_atomic_load(value) std::atomic_load(value)
    #define __pyx_atomic_store(value, new_value) std::atomic_store(value, new_value)
    #define __pyx_atomic_pointer_load_relaxed(value) std::atomic_load_explicit(value, std::memory_order_relaxed)
    #define __pyx_atomic_pointer_load_acquire(value) std::atomic_load_explicit(value, std::memory_order_acquire)
    #define __pyx_atomic_pointer_exchange(value, new_value) std::atomic_exchange(value, (__pyx_nonatomic_ptr_type)new_value)
    #define __pyx_atomic_pointer_cmp_exchange(value, expected, desired) std::atomic_compare_exchange_strong(value, expected, desired)
    #if defined(__PYX_DEBUG_ATOMICS) && defined(_MSC_VER)
        #pragma message ("Using standard C++ atomics")
    #elif defined(__PYX_DEBUG_ATOMICS)
        #warning "Using standard C++ atomics"
    #endif
#elif CYTHON_ATOMICS && (__GNUC__ >= 5 || (__GNUC__ == 4 &&\
                    (__GNUC_MINOR__ > 1 ||\
                    (__GNUC_MINOR__ == 1 && __GNUC_PATCHLEVEL__ >= 2))))
    #define __pyx_atomic_ptr_type void*
    #define __pyx_nonatomic_ptr_type void*
    #define __pyx_atomic_incr_relaxed(value) __sync_fetch_and_add(value, 1)
    #define __pyx_atomic_incr_acq_rel(value) __sync_fetch_and_add(value, 1)
    #define __pyx_atomic_decr_acq_rel(value) __sync_fetch_and_sub(value, 1)
    #define __pyx_atomic_sub(value, arg) __sync_fetch_and_sub(value, arg)
    static CYTHON_INLINE int __pyx_atomic_int_cmp_exchange(__pyx_atomic_int_type* value, __pyx_nonatomic_int_type* expected, __pyx_nonatomic_int_type desired) {
        __pyx_nonatomic_int_type old = __sync_val_compare_and_swap(value, *expected, desired);
        int result = old == *expected;
        *expected = old;
        return result;
    }
    #define __pyx_atomic_load(value) __sync_fetch_and_add(value, 0)
    #define __pyx_atomic_store(value, new_value) __sync_lock_test_and_set(value, new_value)
    #define __pyx_atomic_pointer_load_relaxed(value) __sync_fetch_and_add(value, 0)
    #define __pyx_atomic_pointer_load_acquire(value) __sync_fetch_and_add(value, 0)
    #define __pyx_atomic_pointer_exchange(value, new_value) __sync_lock_test_and_set(value, (__pyx_atomic_ptr_type)new_value)
    static CYTHON_INLINE int __pyx_atomic_pointer_cmp_exchange(__pyx_atomic_ptr_type* value, __pyx_nonatomic_ptr_type* expected, __pyx_nonatomic_ptr_type desired) {
        __pyx_nonatomic_ptr_type old = __sync_val_compare_and_swap(value, *expected, desired);
        int result = old == *expected;
        *expected = old;
        return result;
    }
    #ifdef __PYX_DEBUG_ATOMICS
        #warning "Using GNU atomics"
    #endif
#elif CYTHON_ATOMICS && defined(_MSC_VER)
    #include <intrin.h>
    #undef __pyx_atomic_int_type
    #define __pyx_atomic_int_type long
    #define __pyx_atomic_ptr_type void*
    #undef __pyx_nonatomic_int_type
    #define __pyx_nonatomic_int_type long
    #define __pyx_nonatomic_ptr_type void*
    #pragma intrinsic (_InterlockedExchangeAdd, _InterlockedExchange, _InterlockedCompareExchange, _InterlockedCompareExchangePointer, _InterlockedExchangePointer)
    #define __pyx_atomic_incr_relaxed(value) _InterlockedExchangeAdd(value, 1)
    #define __pyx_atomic_incr_acq_rel(value) _InterlockedExchangeAdd(value, 1)
    #define __pyx_atomic_decr_acq_rel(value) _InterlockedExchangeAdd(value, -1)
    #define __pyx_atomic_sub(value, arg) _InterlockedExchangeAdd(value, -arg)
    static CYTHON_INLINE int __pyx_atomic_int_cmp_exchange(__pyx_atomic_int_type* value, __pyx_nonatomic_int_type* expected, __pyx_nonatomic_int_type desired) {
        __pyx_nonatomic_int_type old = _InterlockedCompareExchange(value, desired, *expected);
        int result = old == *expected;
        *expected = old;
        return result;
    }
    #define __pyx_atomic_load(value) _InterlockedExchangeAdd(value, 0)
    #define __pyx_atomic_store(value, new_value) _InterlockedExchange(value, new_value)
    #define __pyx_atomic_pointer_load_relaxed(value) *(void * volatile *)value
    #define __pyx_atomic_pointer_load_acquire(value) _InterlockedCompareExchangePointer(value, 0, 0)
    #define __pyx_atomic_pointer_exchange(value, new_value) _InterlockedExchangePointer(value, (__pyx_atomic_ptr_type)new_value)
    static CYTHON_INLINE int __pyx_atomic_pointer_cmp_exchange(__pyx_atomic_ptr_type* value, __pyx_nonatomic_ptr_type* expected, __pyx_nonatomic_ptr_type desired) {
        __pyx_atomic_ptr_type old = _InterlockedCompareExchangePointer(value, desired, *expected);
        int result = old == *expected;
        *expected = old;
        return result;
    }
    #ifdef __PYX_DEBUG_ATOMICS
        #pragma message ("Using MSVC atomics")
    #endif
#else
    #undef CYTHON_ATOMICS
    #define CYTHON_ATOMICS 0
    #ifdef __PYX_DEBUG_ATOMICS
        #warning "Not using atomics"
    #endif
#endif

#if !CYTHON_COMPILING_IN_CPYTHON_FREETHREADING
#define __Pyx_PyCriticalSection void*
#define __Pyx_PyCriticalSection2 void*
#define __Pyx_PyCriticalSection_End(cs)
#define __Pyx_PyCriticalSection2_End(cs)
#else
#define __Pyx_PyCriticalSection PyCriticalSection
#define __Pyx_PyCriticalSection2 PyCriticalSection2
#define __Pyx_PyCriticalSection_End PyCriticalSection_End
#define __Pyx_PyCriticalSection2_End PyCriticalSection2_End
#endif

#if !CYTHON_COMPILING_IN_CPYTHON_FREETHREADING
#define __Pyx_PyCriticalSection_Begin(cs, arg) (void)(cs)
#define __Pyx_PyCriticalSection2_Begin(cs, arg1, arg2) (void)(cs)
#else
#define __Pyx_PyCriticalSection_Begin PyCriticalSection_Begin
#define __Pyx_PyCriticalSection2_Begin PyCriticalSection2_Begin
#endif
#if PY_VERSION_HEX < 0x030d0000 || CYTHON_COMPILING_IN_LIMITED_API
#define __Pyx_BEGIN_CRITICAL_SECTION(o) {
#define __Pyx_END_CRITICAL_SECTION() }
#else
#define __Pyx_BEGIN_CRITICAL_SECTION Py_BEGIN_CRITICAL_SECTION
#define __Pyx_END_CRITICAL_SECTION Py_END_CRITICAL_SECTION
#endif

#include <structmember.h>

struct __pyx_opt_args_11_fast_match__match_path_compact_impl;

struct __pyx_opt_args_11_fast_match__match_path_compact_impl {
  int __pyx_n;
  PyObject *pw_eval_fn;
};

#ifndef CYTHON_REFNANNY
  #define CYTHON_REFNANNY 0
#endif
#if CYTHON_REFNANNY
  typedef struct {
    void (*INCREF)(void*, PyObject*, Py_ssize_t);
    void (*DECREF)(void*, PyObject*, Py_ssize_t);
    void (*GOTREF)(void*, PyObject*, Py_ssize_t);
    void (*GIVEREF)(void*, PyObject*, Py_ssize_t);
    void* (*SetupContext)(const char*, Py_ssize_t, const char*);
    void (*FinishContext)(void**);
  } __Pyx_RefNannyAPIStruct;
  static __Pyx_RefNannyAPIStruct *__Pyx_RefNanny = NULL;
  static __Pyx_RefNannyAPIStruct *__Pyx_RefNannyImportAPI(const char *modname);
  #define __Pyx_RefNannyDeclarations void *__pyx_refnanny = NULL;
  #define __Pyx_RefNannySetupContext(name, acquire_gil)\
          if (acquire_gil) {\
              PyGILState_STATE __pyx_gilstate_save = PyGILState_Ensure();\
              __pyx_refnanny = __Pyx_RefNanny->SetupContext((name), (__LINE__), (__FILE__));\
              PyGILState_Release(__pyx_gilstate_save);\
          } else {\
              __pyx_refnanny = __Pyx_RefNanny->SetupContext((name), (__LINE__), (__FILE__));\
          }
  #define __Pyx_RefNannyFinishContextNogil() {\
              PyGILState_STATE __pyx_gilstate_save = PyGILState_Ensure();\
              __Pyx_RefNannyFinishContext();\
              PyGILState_Release(__pyx_gilstate_save);\
          }
  #define __Pyx_RefNannyFinishContextNogil() {\
              PyGILState_STATE __pyx_gilstate_save = PyGILState_Ensure();\
              __Pyx_RefNannyFinishContext();\
              PyGILState_Release(__pyx_gilstate_save);\
          }
  #define __Pyx_RefNannyFinishContext()\
          __Pyx_RefNanny->FinishContext(&__pyx_refnanny)
  #define __Pyx_INCREF(r)  __Pyx_RefNanny->INCREF(__pyx_refnanny, (PyObject *)(r), (__LINE__))
  #define __Pyx_DECREF(r)  __Pyx_RefNanny->DECREF(__pyx_refnanny, (PyObject *)(r), (__LINE__))
  #define __Pyx_GOTREF(r)  __Pyx_RefNanny->GOTREF(__pyx_refnanny, (PyObject *)(r), (__LINE__))
  #define __Pyx_GIVEREF(r) __Pyx_RefNanny->GIVEREF(__pyx_refnanny, (PyObject *)(r), (__LINE__))
  #define __Pyx_XINCREF(r)  do { if((r) == NULL); else {__Pyx_INCREF(r); }} while(0)
  #define __Pyx_XDECREF(r)  do { if((r) == NULL); else {__Pyx_DECREF(r); }} while(0)
  #define __Pyx_XGOTREF(r)  do { if((r) == NULL); else {__Pyx_GOTREF(r); }} while(0)
  #define __Pyx_XGIVEREF(r) do { if((r) == NULL); else {__Pyx_GIVEREF(r);}} while(0)
#else
  #define __Pyx_RefNannyDeclarations
  #define __Pyx_RefNannySetupContext(name, acquire_gil)
  #define __Pyx_RefNannyFinishContextNogil()
  #define __Pyx_RefNannyFinishContext()
  #define __Pyx_INCREF(r) Py_INCREF(r)
  #define __Pyx_DECREF(r) Py_DECREF(r)
  #define __Pyx_GOTREF(r)
  #define __Pyx_GIVEREF(r)
  #define __Pyx_XINCREF(r) Py_XINCREF(r)
  #define __Pyx_XDECREF(r) Py_XDECREF(r)
  #define __Pyx_XGOTREF(r)
  #define __Pyx_XGIVEREF(r)
#endif
#define __Pyx_Py_XDECREF_SET(r, v) do {\
        PyObject *tmp = (PyObject *) r;\
        r = v; Py_XDECREF(tmp);\
    } while (0)
#define __Pyx_XDECREF_SET(r, v) do {\
        PyObject *tmp = (PyObject *) r;\
        r = v; __Pyx_XDECREF(tmp);\
    } while (0)
#define __Pyx_DECREF_SET(r, v) do {\
        PyObject *tmp = (PyObject *) r;\
        r = v; __Pyx_DECREF(tmp);\
    } while (0)
#define __Pyx_CLEAR(r)    do { PyObject* tmp = ((PyObject*)(r)); r = NULL; __Pyx_DECREF(tmp);} while(0)
#define __Pyx_XCLEAR(r)   do { if((r) != NULL) {PyObject* tmp = ((PyObject*)(r)); r = NULL; __Pyx_DECREF(tmp);}} while(0)

#if CYTHON_FAST_THREAD_STATE
#define __Pyx_PyErr_ExceptionMatches(err) __Pyx_PyErr_ExceptionMatchesInState(__pyx_tstate, err)
static CYTHON_INLINE int __Pyx_PyErr_ExceptionMatchesInState(PyThreadState* tstate, PyObject* err);
#else
#define __Pyx_PyErr_ExceptionMatches(err)  PyErr_ExceptionMatches(err)
#endif

#if CYTHON_FAST_THREAD_STATE
#define __Pyx_PyThreadState_declare  PyThreadState *__pyx_tstate;
#define __Pyx_PyThreadState_assign  __pyx_tstate = __Pyx_PyThreadState_Current;
#if PY_VERSION_HEX >= 0x030C00A6
#define __Pyx_PyErr_Occurred()  (__pyx_tstate->current_exception != NULL)
#define __Pyx_PyErr_CurrentExceptionType()  (__pyx_tstate->current_exception ? (PyObject*) Py_TYPE(__pyx_tstate->current_exception) : (PyObject*) NULL)
#else
#define __Pyx_PyErr_Occurred()  (__pyx_tstate->curexc_type != NULL)
#define __Pyx_PyErr_CurrentExceptionType()  (__pyx_tstate->curexc_type)
#endif
#else
#define __Pyx_PyThreadState_declare
#define __Pyx_PyThreadState_assign
#define __Pyx_PyErr_Occurred()  (PyErr_Occurred() != NULL)
#define __Pyx_PyErr_CurrentExceptionType()  PyErr_Occurred()
#endif

#if CYTHON_FAST_THREAD_STATE
#define __Pyx_PyErr_Clear() __Pyx_ErrRestore(NULL, NULL, NULL)
#define __Pyx_ErrRestoreWithState(type, value, tb)  __Pyx_ErrRestoreInState(PyThreadState_GET(), type, value, tb)
#define __Pyx_ErrFetchWithState(type, value, tb)    __Pyx_ErrFetchInState(PyThreadState_GET(), type, value, tb)
#define __Pyx_ErrRestore(type, value, tb)  __Pyx_ErrRestoreInState(__pyx_tstate, type, value, tb)
#define __Pyx_ErrFetch(type, value, tb)    __Pyx_ErrFetchInState(__pyx_tstate, type, value, tb)
static CYTHON_INLINE void __Pyx_ErrRestoreInState(PyThreadState *tstate, PyObject *type, PyObject *value, PyObject *tb);
static CYTHON_INLINE void __Pyx_ErrFetchInState(PyThreadState *tstate, PyObject **type, PyObject **value, PyObject **tb);
#if CYTHON_COMPILING_IN_CPYTHON && PY_VERSION_HEX < 0x030C00A6
#define __Pyx_PyErr_SetNone(exc) (Py_INCREF(exc), __Pyx_ErrRestore((exc), NULL, NULL))
#else
#define __Pyx_PyErr_SetNone(exc) PyErr_SetNone(exc)
#endif
#else
#define __Pyx_PyErr_Clear() PyErr_Clear()
#define __Pyx_PyErr_SetNone(exc) PyErr_SetNone(exc)
#define __Pyx_ErrRestoreWithState(type, value, tb)  PyErr_Restore(type, value, tb)
#define __Pyx_ErrFetchWithState(type, value, tb)  PyErr_Fetch(type, value, tb)
#define __Pyx_ErrRestoreInState(tstate, type, value, tb)  PyErr_Restore(type, value, tb)
#define __Pyx_ErrFetchInState(tstate, type, value, tb)  PyErr_Fetch(type, value, tb)
#define __Pyx_ErrRestore(type, value, tb)  PyErr_Restore(type, value, tb)
#define __Pyx_ErrFetch(type, value, tb)  PyErr_Fetch(type, value, tb)
#endif

#if CYTHON_USE_TYPE_SLOTS
static CYTHON_INLINE PyObject* __Pyx_PyObject_GetAttrStr(PyObject* obj, PyObject* attr_name);
#else
#define __Pyx_PyObject_GetAttrStr(o,n) PyObject_GetAttr(o,n)
#endif

static CYTHON_INLINE PyObject* __Pyx_PyObject_GetAttrStrNoError(PyObject* obj, PyObject* attr_name);

static PyObject *__Pyx_GetBuiltinName(PyObject *name);

#define __Pyx_GetItemInt(o, i, type, is_signed, to_py_func, is_list, wraparound, boundscheck, has_gil, unsafe_shared)\
    (__Pyx_fits_Py_ssize_t(i, type, is_signed) ?\
    __Pyx_GetItemInt_Fast(o, (Py_ssize_t)i, is_list, wraparound, boundscheck, unsafe_shared) :\
    (is_list ? (PyErr_SetString(PyExc_IndexError, "list index out of range"), (PyObject*)NULL) :\
               __Pyx_GetItemInt_Generic(o, to_py_func(i))))
#define __Pyx_GetItemInt_List(o, i, type, is_signed, to_py_func, is_list, wraparound, boundscheck, has_gil, unsafe_shared)\
    (__Pyx_fits_Py_ssize_t(i, type, is_signed) ?\
    __Pyx_GetItemInt_List_Fast(o, (Py_ssize_t)i, wraparound, boundscheck, unsafe_shared) :\
    (PyErr_SetString(PyExc_IndexError, "list index out of range"), (PyObject*)NULL))
static CYTHON_INLINE PyObject *__Pyx_GetItemInt_List_Fast(PyObject *o, Py_ssize_t i,
                                                              int wraparound, int boundscheck, int unsafe_shared);
#define __Pyx_GetItemInt_Tuple(o, i, type, is_signed, to_py_func, is_list, wraparound, boundscheck, has_gil, unsafe_shared)\
    (__Pyx_fits_Py_ssize_t(i, type, is_signed) ?\
    __Pyx_GetItemInt_Tuple_Fast(o, (Py_ssize_t)i, wraparound, boundscheck, unsafe_shared) :\
    (PyErr_SetString(PyExc_IndexError, "tuple index out of range"), (PyObject*)NULL))
static CYTHON_INLINE PyObject *__Pyx_GetItemInt_Tuple_Fast(PyObject *o, Py_ssize_t i,
                                                              int wraparound, int boundscheck, int unsafe_shared);
static PyObject *__Pyx_GetItemInt_Generic(PyObject *o, PyObject* j);
static CYTHON_INLINE PyObject *__Pyx_GetItemInt_Fast(PyObject *o, Py_ssize_t i,
                                                     int is_list, int wraparound, int boundscheck, int unsafe_shared);

static PyObject* __Pyx_PyDict_GetItemDefault(PyObject* d, PyObject* key, PyObject* default_value);

#define __Pyx_CallCFunction(cfunc, self, args)\
    ((PyCFunction)(void(*)(void))(cfunc)->func)(self, args)
#define __Pyx_CallCFunctionWithKeywords(cfunc, self, args, kwargs)\
    ((PyCFunctionWithKeywords)(void(*)(void))(cfunc)->func)(self, args, kwargs)
#define __Pyx_CallCFunctionFast(cfunc, self, args, nargs)\
    ((__Pyx_PyCFunctionFast)(void(*)(void))(PyCFunction)(cfunc)->func)(self, args, nargs)
#define __Pyx_CallCFunctionFastWithKeywords(cfunc, self, args, nargs, kwnames)\
    ((__Pyx_PyCFunctionFastWithKeywords)(void(*)(void))(PyCFunction)(cfunc)->func)(self, args, nargs, kwnames)

#if CYTHON_COMPILING_IN_CPYTHON
static CYTHON_INLINE PyObject* __Pyx_PyObject_Call(PyObject *func, PyObject *arg, PyObject *kw);
#else
#define __Pyx_PyObject_Call(func, arg, kw) PyObject_Call(func, arg, kw)
#endif

#if CYTHON_COMPILING_IN_CPYTHON
static CYTHON_INLINE PyObject* __Pyx_PyObject_CallMethO(PyObject *func, PyObject *arg);
#endif

#define __Pyx_PyObject_FastCall(func, args, nargs)  __Pyx_PyObject_FastCallDict(func, args, (size_t)(nargs), NULL)
static CYTHON_INLINE PyObject* __Pyx_PyObject_FastCallDict(PyObject *func, PyObject * const*args, size_t nargs, PyObject *kwargs);

static CYTHON_INLINE PyObject* __Pyx_PyObject_Call2Args(PyObject* function, PyObject* arg1, PyObject* arg2);

typedef struct {
    PyObject *type;
    PyObject **method_name;
#if CYTHON_COMPILING_IN_CPYTHON_FREETHREADING && CYTHON_ATOMICS
    __pyx_atomic_int_type initialized;
#endif
    PyCFunction func;
    PyObject *method;
    int flag;
} __Pyx_CachedCFunction;
#if CYTHON_COMPILING_IN_CPYTHON_FREETHREADING
static CYTHON_INLINE int __Pyx_CachedCFunction_GetAndSetInitializing(__Pyx_CachedCFunction *cfunc) {
#if !CYTHON_ATOMICS
    return 1;
#else
    __pyx_nonatomic_int_type expected = 0;
    if (__pyx_atomic_int_cmp_exchange(&cfunc->initialized, &expected, 1)) {
        return 0;
    }
    return expected;
#endif
}
static CYTHON_INLINE void __Pyx_CachedCFunction_SetFinishedInitializing(__Pyx_CachedCFunction *cfunc) {
#if CYTHON_ATOMICS
    __pyx_atomic_store(&cfunc->initialized, 2);
#endif
}
#else
#define __Pyx_CachedCFunction_GetAndSetInitializing(cfunc) 2
#define __Pyx_CachedCFunction_SetFinishedInitializing(cfunc)
#endif

CYTHON_UNUSED
static PyObject* __Pyx__CallUnboundCMethod1(__Pyx_CachedCFunction* cfunc, PyObject* self, PyObject* arg);
#if CYTHON_COMPILING_IN_CPYTHON
static CYTHON_INLINE PyObject* __Pyx_CallUnboundCMethod1(__Pyx_CachedCFunction* cfunc, PyObject* self, PyObject* arg);
#else
#define __Pyx_CallUnboundCMethod1(cfunc, self, arg)  __Pyx__CallUnboundCMethod1(cfunc, self, arg)
#endif

CYTHON_UNUSED
static PyObject* __Pyx__CallUnboundCMethod2(__Pyx_CachedCFunction* cfunc, PyObject* self, PyObject* arg1, PyObject* arg2);
#if CYTHON_COMPILING_IN_CPYTHON
static CYTHON_INLINE PyObject *__Pyx_CallUnboundCMethod2(__Pyx_CachedCFunction *cfunc, PyObject *self, PyObject *arg1, PyObject *arg2);
#else
#define __Pyx_CallUnboundCMethod2(cfunc, self, arg1, arg2)  __Pyx__CallUnboundCMethod2(cfunc, self, arg1, arg2)
#endif

#if !CYTHON_COMPILING_IN_PYPY
static PyObject *__Pyx_PyDict_GetItem(PyObject *d, PyObject* key);
#define __Pyx_PyObject_Dict_GetItem(obj, name)\
    (likely(PyDict_CheckExact(obj)) ?\
     __Pyx_PyDict_GetItem(obj, name) : PyObject_GetItem(obj, name))
#else
#define __Pyx_PyDict_GetItem(d, key) PyObject_GetItem(d, key)
#define __Pyx_PyObject_Dict_GetItem(obj, name)  PyObject_GetItem(obj, name)
#endif

#if CYTHON_VECTORCALL && PY_VERSION_HEX >= 0x03090000
#define __Pyx_PyObject_FastCallMethod(name, args, nargsf) PyObject_VectorcallMethod(name, args, nargsf, NULL)
#else
static PyObject *__Pyx_PyObject_FastCallMethod(PyObject *name, PyObject *const *args, size_t nargsf);
#endif

static CYTHON_INLINE int __Pyx_PySequence_ContainsTF(PyObject* item, PyObject* seq, int eq) {
    int result = PySequence_Contains(seq, item);
    return unlikely(result < 0) ? result : (result == (eq == Py_EQ));
}

#if CYTHON_COMPILING_IN_CPYTHON
static CYTHON_INLINE PyObject* __Pyx_PyList_FromArray(PyObject *const *src, Py_ssize_t n);
#endif
#if CYTHON_COMPILING_IN_CPYTHON || CYTHON_METH_FASTCALL
static CYTHON_INLINE PyObject* __Pyx_PyTuple_FromArray(PyObject *const *src, Py_ssize_t n);
#endif

#include <string.h>

static CYTHON_INLINE int __Pyx_PyBytes_Equals(PyObject* s1, PyObject* s2, int equals);

static CYTHON_INLINE int __Pyx_PyUnicode_Equals(PyObject* s1, PyObject* s2, int equals);

#if CYTHON_AVOID_BORROWED_REFS
    #define __Pyx_ArgRef_VARARGS(args, i) __Pyx_PySequence_ITEM(args, i)
#elif CYTHON_ASSUME_SAFE_MACROS
    #define __Pyx_ArgRef_VARARGS(args, i) __Pyx_NewRef(__Pyx_PyTuple_GET_ITEM(args, i))
#else
    #define __Pyx_ArgRef_VARARGS(args, i) __Pyx_XNewRef(PyTuple_GetItem(args, i))
#endif
#define __Pyx_NumKwargs_VARARGS(kwds) PyDict_Size(kwds)
#define __Pyx_KwValues_VARARGS(args, nargs) NULL
#define __Pyx_GetKwValue_VARARGS(kw, kwvalues, s) __Pyx_PyDict_GetItemStrWithError(kw, s)
#define __Pyx_KwargsAsDict_VARARGS(kw, kwvalues) PyDict_Copy(kw)
#if CYTHON_METH_FASTCALL
    #define __Pyx_ArgRef_FASTCALL(args, i) __Pyx_NewRef(args[i])
    #define __Pyx_NumKwargs_FASTCALL(kwds) __Pyx_PyTuple_GET_SIZE(kwds)
    #define __Pyx_KwValues_FASTCALL(args, nargs) ((args) + (nargs))
    static CYTHON_INLINE PyObject * __Pyx_GetKwValue_FASTCALL(PyObject *kwnames, PyObject *const *kwvalues, PyObject *s);
  #if CYTHON_COMPILING_IN_CPYTHON && PY_VERSION_HEX >= 0x030d0000 || CYTHON_COMPILING_IN_LIMITED_API
    CYTHON_UNUSED static PyObject *__Pyx_KwargsAsDict_FASTCALL(PyObject *kwnames, PyObject *const *kwvalues);
  #else
    #define __Pyx_KwargsAsDict_FASTCALL(kw, kwvalues) _PyStack_AsDict(kwvalues, kw)
  #endif
#else
    #define __Pyx_ArgRef_FASTCALL __Pyx_ArgRef_VARARGS
    #define __Pyx_NumKwargs_FASTCALL __Pyx_NumKwargs_VARARGS
    #define __Pyx_KwValues_FASTCALL __Pyx_KwValues_VARARGS
    #define __Pyx_GetKwValue_FASTCALL __Pyx_GetKwValue_VARARGS
    #define __Pyx_KwargsAsDict_FASTCALL __Pyx_KwargsAsDict_VARARGS
#endif
#define __Pyx_ArgsSlice_VARARGS(args, start, stop) PyTuple_GetSlice(args, start, stop)
#if CYTHON_METH_FASTCALL || (CYTHON_COMPILING_IN_CPYTHON && CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS)
#define __Pyx_ArgsSlice_FASTCALL(args, start, stop) __Pyx_PyTuple_FromArray(args + start, stop - start)
#else
#define __Pyx_ArgsSlice_FASTCALL(args, start, stop) PyTuple_GetSlice(args, start, stop)
#endif

static CYTHON_INLINE PyObject* __Pyx_PyDict_Items(PyObject* d);

static CYTHON_INLINE PyObject* __Pyx_PyObject_CallOneArg(PyObject *func, PyObject *arg);

CYTHON_UNUSED
static PyObject* __Pyx__CallUnboundCMethod0(__Pyx_CachedCFunction* cfunc, PyObject* self);
#if CYTHON_COMPILING_IN_CPYTHON
static CYTHON_INLINE PyObject* __Pyx_CallUnboundCMethod0(__Pyx_CachedCFunction* cfunc, PyObject* self);
#else
#define __Pyx_CallUnboundCMethod0(cfunc, self)  __Pyx__CallUnboundCMethod0(cfunc, self)
#endif

static CYTHON_INLINE PyObject* __Pyx_PyDict_Values(PyObject* d);

#if CYTHON_AVOID_BORROWED_REFS
static int __Pyx_PyDict_NextRef(PyObject *p, PyObject **ppos, PyObject **pkey, PyObject **pvalue);
#else
CYTHON_INLINE
static int __Pyx_PyDict_NextRef(PyObject *p, Py_ssize_t *ppos, PyObject **pkey, PyObject **pvalue);
#endif

static void __Pyx_RaiseDoubleKeywordsError(const char* func_name, PyObject* kw_name);

static int __Pyx_ParseKeywordsTuple(
    PyObject *kwds,
    PyObject * const *kwvalues,
    PyObject ** const argnames[],
    PyObject *kwds2,
    PyObject *values[],
    Py_ssize_t num_pos_args,
    Py_ssize_t num_kwargs,
    const char* function_name,
    int ignore_unknown_kwargs
);
static int __Pyx_ParseKeywordDictToDict(
    PyObject *kwds,
    PyObject ** const argnames[],
    PyObject *kwds2,
    PyObject *values[],
    Py_ssize_t num_pos_args,
    const char* function_name
);
static int __Pyx_ParseKeywordDict(
    PyObject *kwds,
    PyObject ** const argnames[],
    PyObject *values[],
    Py_ssize_t num_pos_args,
    Py_ssize_t num_kwargs,
    const char* function_name,
    int ignore_unknown_kwargs
);

static CYTHON_INLINE int __Pyx_ParseKeywords(
    PyObject *kwds, PyObject *const *kwvalues, PyObject ** const argnames[],
    PyObject *kwds2, PyObject *values[],
    Py_ssize_t num_pos_args, Py_ssize_t num_kwargs,
    const char* function_name,
    int ignore_unknown_kwargs
);

static void __Pyx_RaiseArgtupleInvalid(const char* func_name, int exact,
    Py_ssize_t num_min, Py_ssize_t num_max, Py_ssize_t num_found);

static int __Pyx__ArgTypeTest(PyObject *obj, PyTypeObject *type, const char *name, int exact);

#define __Pyx_ArgTypeTest(obj, type, none_allowed, name, exact)\
    ((likely(__Pyx_IS_TYPE(obj, type) | (none_allowed && (obj == Py_None)))) ? 1 :\
        __Pyx__ArgTypeTest(obj, type, name, exact))

static CYTHON_INLINE void __Pyx_RaiseTooManyValuesError(Py_ssize_t expected);

static CYTHON_INLINE void __Pyx_RaiseNeedMoreValuesError(Py_ssize_t index);

static CYTHON_INLINE int __Pyx_IterFinish(void);

static int __Pyx_IternextUnpackEndCheck(PyObject *retval, Py_ssize_t expected);

static int __Pyx_RaiseUnexpectedTypeError(const char *expected, PyObject *obj);

static CYTHON_INLINE PyObject* __Pyx_PyObject_CallNoArg(PyObject *func);

#if !(CYTHON_VECTORCALL && (__PYX_LIMITED_VERSION_HEX >= 0x030C0000 || (!CYTHON_COMPILING_IN_LIMITED_API && PY_VERSION_HEX >= 0x03090000)))
static int __Pyx_PyObject_GetMethod(PyObject *obj, PyObject *name, PyObject **method);
#endif

static PyObject* __Pyx_PyObject_CallMethod0(PyObject* obj, PyObject* method_name);

static CYTHON_INLINE void __Pyx_RaiseNoneNotIterableError(void);

static void __Pyx_UnpackTupleError(PyObject *, Py_ssize_t index);

static CYTHON_INLINE int __Pyx_unpack_tuple2(
    PyObject* tuple, PyObject** value1, PyObject** value2, int is_tuple, int has_known_size, int decref_tuple);
static CYTHON_INLINE int __Pyx_unpack_tuple2_exact(
    PyObject* tuple, PyObject** value1, PyObject** value2, int decref_tuple);
static int __Pyx_unpack_tuple2_generic(
    PyObject* tuple, PyObject** value1, PyObject** value2, int has_known_size, int decref_tuple);

static CYTHON_INLINE PyObject* __Pyx_dict_iterator(PyObject* dict, int is_dict, PyObject* method_name,
                                                   Py_ssize_t* p_orig_length, int* p_is_dict);
static CYTHON_INLINE int __Pyx_dict_iter_next(PyObject* dict_or_iter, Py_ssize_t orig_length, Py_ssize_t* ppos,
                                              PyObject** pkey, PyObject** pvalue, PyObject** pitem, int is_dict);

#if CYTHON_USE_TYPE_SLOTS
static CYTHON_INLINE PyObject *__Pyx_PyObject_GetItem(PyObject *obj, PyObject *key);
#else
#define __Pyx_PyObject_GetItem(obj, key)  PyObject_GetItem(obj, key)
#endif

#if CYTHON_USE_PYLIST_INTERNALS && CYTHON_ASSUME_SAFE_MACROS
static CYTHON_INLINE int __Pyx_PyList_Append(PyObject* list, PyObject* x) {
    PyListObject* L = (PyListObject*) list;
    Py_ssize_t len = Py_SIZE(list);
    if (likely(L->allocated > len) & likely(len > (L->allocated >> 1))) {
        Py_INCREF(x);
        #if CYTHON_COMPILING_IN_CPYTHON && PY_VERSION_HEX >= 0x030d0000
        L->ob_item[len] = x;
        #else
        PyList_SET_ITEM(list, len, x);
        #endif
        __Pyx_SET_SIZE(list, len + 1);
        return 0;
    }
    return PyList_Append(list, x);
}
#else
#define __Pyx_PyList_Append(L,x) PyList_Append(L,x)
#endif

static CYTHON_INLINE int __Pyx_PyDict_ContainsTF(PyObject* item, PyObject* dict, int eq) {
    int result = PyDict_Contains(dict, item);
    return unlikely(result < 0) ? result : (result == (eq == Py_EQ));
}

#if CYTHON_USE_DICT_VERSIONS && CYTHON_USE_TYPE_SLOTS
#define __PYX_DICT_VERSION_INIT  ((PY_UINT64_T) -1)
#define __PYX_GET_DICT_VERSION(dict)  (((PyDictObject*)(dict))->ma_version_tag)
#define __PYX_UPDATE_DICT_CACHE(dict, value, cache_var, version_var)\
    (version_var) = __PYX_GET_DICT_VERSION(dict);\
    (cache_var) = (value);
#define __PYX_PY_DICT_LOOKUP_IF_MODIFIED(VAR, DICT, LOOKUP) {\
    static PY_UINT64_T __pyx_dict_version = 0;\
    static PyObject *__pyx_dict_cached_value = NULL;\
    if (likely(__PYX_GET_DICT_VERSION(DICT) == __pyx_dict_version)) {\
        (VAR) = __Pyx_XNewRef(__pyx_dict_cached_value);\
    } else {\
        (VAR) = __pyx_dict_cached_value = (LOOKUP);\
        __pyx_dict_version = __PYX_GET_DICT_VERSION(DICT);\
    }\
}
static CYTHON_INLINE PY_UINT64_T __Pyx_get_tp_dict_version(PyObject *obj);
static CYTHON_INLINE PY_UINT64_T __Pyx_get_object_dict_version(PyObject *obj);
static CYTHON_INLINE int __Pyx_object_dict_version_matches(PyObject* obj, PY_UINT64_T tp_dict_version, PY_UINT64_T obj_dict_version);
#else
#define __PYX_GET_DICT_VERSION(dict)  (0)
#define __PYX_UPDATE_DICT_CACHE(dict, value, cache_var, version_var)
#define __PYX_PY_DICT_LOOKUP_IF_MODIFIED(VAR, DICT, LOOKUP)  (VAR) = (LOOKUP);
#endif

#if CYTHON_USE_DICT_VERSIONS
#define __Pyx_GetModuleGlobalName(var, name)  do {\
    static PY_UINT64_T __pyx_dict_version = 0;\
    static PyObject *__pyx_dict_cached_value = NULL;\
    (var) = (likely(__pyx_dict_version == __PYX_GET_DICT_VERSION(__pyx_mstate_global->__pyx_d))) ?\
        (likely(__pyx_dict_cached_value) ? __Pyx_NewRef(__pyx_dict_cached_value) : __Pyx_GetBuiltinName(name)) :\
        __Pyx__GetModuleGlobalName(name, &__pyx_dict_version, &__pyx_dict_cached_value);\
} while(0)
#define __Pyx_GetModuleGlobalNameUncached(var, name)  do {\
    PY_UINT64_T __pyx_dict_version;\
    PyObject *__pyx_dict_cached_value;\
    (var) = __Pyx__GetModuleGlobalName(name, &__pyx_dict_version, &__pyx_dict_cached_value);\
} while(0)
static PyObject *__Pyx__GetModuleGlobalName(PyObject *name, PY_UINT64_T *dict_version, PyObject **dict_cached_value);
#else
#define __Pyx_GetModuleGlobalName(var, name)  (var) = __Pyx__GetModuleGlobalName(name)
#define __Pyx_GetModuleGlobalNameUncached(var, name)  (var) = __Pyx__GetModuleGlobalName(name)
static CYTHON_INLINE PyObject *__Pyx__GetModuleGlobalName(PyObject *name);
#endif

#if CYTHON_USE_EXC_INFO_STACK && CYTHON_FAST_THREAD_STATE
static _PyErr_StackItem * __Pyx_PyErr_GetTopmostException(PyThreadState *tstate);
#endif

#if CYTHON_FAST_THREAD_STATE
#define __Pyx_ExceptionSave(type, value, tb)  __Pyx__ExceptionSave(__pyx_tstate, type, value, tb)
static CYTHON_INLINE void __Pyx__ExceptionSave(PyThreadState *tstate, PyObject **type, PyObject **value, PyObject **tb);
#define __Pyx_ExceptionReset(type, value, tb)  __Pyx__ExceptionReset(__pyx_tstate, type, value, tb)
static CYTHON_INLINE void __Pyx__ExceptionReset(PyThreadState *tstate, PyObject *type, PyObject *value, PyObject *tb);
#else
#define __Pyx_ExceptionSave(type, value, tb)   PyErr_GetExcInfo(type, value, tb)
#define __Pyx_ExceptionReset(type, value, tb)  PyErr_SetExcInfo(type, value, tb)
#endif

#define __Pyx_PyExc_TypeError_Check(obj)  __Pyx_TypeCheck(obj, PyExc_TypeError)

#define __Pyx_PyExc_ValueError_Check(obj)  __Pyx_TypeCheck(obj, PyExc_ValueError)

#if CYTHON_COMPILING_IN_CPYTHON
#define __Pyx_TypeCheck(obj, type) __Pyx_IsSubtype(Py_TYPE(obj), (PyTypeObject *)type)
#define __Pyx_TypeCheck2(obj, type1, type2) __Pyx_IsAnySubtype2(Py_TYPE(obj), (PyTypeObject *)type1, (PyTypeObject *)type2)
static CYTHON_INLINE int __Pyx_IsSubtype(PyTypeObject *a, PyTypeObject *b);
static CYTHON_INLINE int __Pyx_IsAnySubtype2(PyTypeObject *cls, PyTypeObject *a, PyTypeObject *b);
static CYTHON_INLINE int __Pyx_PyErr_GivenExceptionMatches(PyObject *err, PyObject *type);
static CYTHON_INLINE int __Pyx_PyErr_GivenExceptionMatches2(PyObject *err, PyObject *type1, PyObject *type2);
#else
#define __Pyx_TypeCheck(obj, type) PyObject_TypeCheck(obj, (PyTypeObject *)type)
#define __Pyx_TypeCheck2(obj, type1, type2) (PyObject_TypeCheck(obj, (PyTypeObject *)type1) || PyObject_TypeCheck(obj, (PyTypeObject *)type2))
#define __Pyx_PyErr_GivenExceptionMatches(err, type) PyErr_GivenExceptionMatches(err, type)
static CYTHON_INLINE int __Pyx_PyErr_GivenExceptionMatches2(PyObject *err, PyObject *type1, PyObject *type2) {
    return PyErr_GivenExceptionMatches(err, type1) || PyErr_GivenExceptionMatches(err, type2);
}
#endif
#define __Pyx_PyErr_ExceptionMatches2(err1, err2)  __Pyx_PyErr_GivenExceptionMatches2(__Pyx_PyErr_CurrentExceptionType(), err1, err2)
#define __Pyx_PyException_Check(obj) __Pyx_TypeCheck(obj, PyExc_Exception)
#ifdef PyExceptionInstance_Check
  #define __Pyx_PyBaseException_Check(obj) PyExceptionInstance_Check(obj)
#else
  #define __Pyx_PyBaseException_Check(obj) __Pyx_TypeCheck(obj, PyExc_BaseException)
#endif

#if CYTHON_FAST_THREAD_STATE
#define __Pyx_GetException(type, value, tb)  __Pyx__GetException(__pyx_tstate, type, value, tb)
static int __Pyx__GetException(PyThreadState *tstate, PyObject **type, PyObject **value, PyObject **tb);
#else
static int __Pyx_GetException(PyObject **type, PyObject **value, PyObject **tb);
#endif

#if !CYTHON_COMPILING_IN_PYPY
static CYTHON_INLINE PyObject* __Pyx_PyLong_AddObjC(PyObject *op1, PyObject *op2, long intval, int inplace, int zerodivision_check);
#else
#define __Pyx_PyLong_AddObjC(op1, op2, intval, inplace, zerodivision_check)\
    (inplace ? PyNumber_InPlaceAdd(op1, op2) : PyNumber_Add(op1, op2))
#endif

#define __Pyx_SetItemInt(o, i, v, type, is_signed, to_py_func, is_list, wraparound, boundscheck, has_gil, unsafe_shared)\
    (__Pyx_fits_Py_ssize_t(i, type, is_signed) ?\
    __Pyx_SetItemInt_Fast(o, (Py_ssize_t)i, v, is_list, wraparound, boundscheck, unsafe_shared) :\
    (is_list ? (PyErr_SetString(PyExc_IndexError, "list assignment index out of range"), -1) :\
               __Pyx_SetItemInt_Generic(o, to_py_func(i), v)))
static int __Pyx_SetItemInt_Generic(PyObject *o, PyObject *j, PyObject *v);
static CYTHON_INLINE int __Pyx_SetItemInt_Fast(PyObject *o, Py_ssize_t i, PyObject *v,
                                               int is_list, int wraparound, int boundscheck, int unsafe_shared);

static CYTHON_INLINE int __Pyx_PyDict_Pop_ignore(PyObject *d, PyObject *key, PyObject *default_value);

#define __Pyx_PyObject_DelSlice(obj, cstart, cstop, py_start, py_stop, py_slice, has_cstart, has_cstop, wraparound)\
    __Pyx_PyObject_SetSlice(obj, (PyObject*)NULL, cstart, cstop, py_start, py_stop, py_slice, has_cstart, has_cstop, wraparound)
static CYTHON_INLINE int __Pyx_PyObject_SetSlice(
        PyObject* obj, PyObject* value, Py_ssize_t cstart, Py_ssize_t cstop,
        PyObject** py_start, PyObject** py_stop, PyObject** py_slice,
        int has_cstart, int has_cstop, int wraparound);

#if __PYX_LIMITED_VERSION_HEX >= 0x030d0000
#define __Pyx_HasAttr(o, n)  PyObject_HasAttrWithError(o, n)
#else
static CYTHON_INLINE int __Pyx_HasAttr(PyObject *, PyObject *);
#endif

static PyObject *__Pyx__Import(PyObject *name, PyObject *const *imported_names, Py_ssize_t len_imported_names, PyObject *qualname, PyObject *moddict, int level);

static CYTHON_INLINE PyObject *__Pyx_Import(PyObject *name, PyObject *const *imported_names, Py_ssize_t len_imported_names, PyObject *qualname, int level);

static PyObject* __Pyx_ImportFrom(PyObject* module, PyObject* name);

static CYTHON_INLINE PyObject *__Pyx_PyDict_SetDefault(PyObject *d, PyObject *key, PyObject *default_value);

#if CYTHON_COMPILING_IN_LIMITED_API
static PyObject *__Pyx_GetTypeDict(PyTypeObject *tp);
#endif

static int __Pyx__SetItemOnTypeDict(PyTypeObject *tp, PyObject *k, PyObject *v);
#define __Pyx_SetItemOnTypeDict(tp, k, v) __Pyx__SetItemOnTypeDict((PyTypeObject*)tp, k, v)

static CYTHON_INLINE int __Pyx_fix_up_extension_type_from_spec(PyType_Spec *spec, PyTypeObject *type);

#if ((CYTHON_COMPILING_IN_CPYTHON_FREETHREADING ) ||\
     __PYX_LIMITED_VERSION_HEX < 0x030d0000)
  static PyObject *__Pyx_PyImport_AddModuleRef(const char *name);
#else
  #define __Pyx_PyImport_AddModuleRef(name) PyImport_AddModuleRef(name)
#endif

static PyObject *__Pyx_FetchSharedCythonABIModule(void);

static PyTypeObject* __Pyx_FetchCommonTypeFromSpec(PyTypeObject *metaclass, PyObject *module, PyType_Spec *spec, PyObject *bases);

static int __pyx_CommonTypesMetaclass_init(PyObject *module);
#define __Pyx_CommonTypesMetaclass_USED

#if !CYTHON_USE_TYPE_SPECS || (!CYTHON_COMPILING_IN_LIMITED_API && PY_VERSION_HEX < 0x03090000)
#define __Pyx_call_type_traverse(o, always_call, visit, arg) 0
#else
static int __Pyx_call_type_traverse(PyObject *o, int always_call, visitproc visit, void *arg);
#endif

static PyObject *__Pyx_PyMethod_New(PyObject *func, PyObject *self, PyObject *typ);

#if CYTHON_METH_FASTCALL && CYTHON_VECTORCALL
static CYTHON_INLINE PyObject *__Pyx_PyVectorcall_FastCallDict(PyObject *func, __pyx_vectorcallfunc vc, PyObject *const *args, size_t nargs, PyObject *kw);
#endif

#define __Pyx_CyFunction_USED
#define __Pyx_CYFUNCTION_STATICMETHOD  0x01
#define __Pyx_CYFUNCTION_CLASSMETHOD   0x02
#define __Pyx_CYFUNCTION_CCLASS        0x04
#define __Pyx_CYFUNCTION_COROUTINE     0x08
#define __Pyx_CyFunction_GetClosure(f)\
    (((__pyx_CyFunctionObject *) (f))->func_closure)
#if PY_VERSION_HEX < 0x030900B1 || CYTHON_COMPILING_IN_LIMITED_API
  #define __Pyx_CyFunction_GetClassObj(f)\
      (((__pyx_CyFunctionObject *) (f))->func_classobj)
#else
  #define __Pyx_CyFunction_GetClassObj(f)\
      ((PyObject*) ((PyCMethodObject *) (f))->mm_class)
#endif
#define __Pyx_CyFunction_SetClassObj(f, classobj)\
    __Pyx__CyFunction_SetClassObj((__pyx_CyFunctionObject *) (f), (classobj))
#define __Pyx_CyFunction_Defaults(type, f)\
    ((type *)(((__pyx_CyFunctionObject *) (f))->defaults))
#define __Pyx_CyFunction_SetDefaultsGetter(f, g)\
    ((__pyx_CyFunctionObject *) (f))->defaults_getter = (g)
typedef struct {
#if CYTHON_COMPILING_IN_LIMITED_API
    PyObject_HEAD
    PyObject *func;
#elif PY_VERSION_HEX < 0x030900B1
    PyCFunctionObject func;
#else
    PyCMethodObject func;
#endif
#if CYTHON_COMPILING_IN_LIMITED_API && CYTHON_METH_FASTCALL
    __pyx_vectorcallfunc func_vectorcall;
#endif
#if CYTHON_COMPILING_IN_LIMITED_API
    PyObject *func_weakreflist;
#endif
#if PY_VERSION_HEX < 0x030C0000 || CYTHON_COMPILING_IN_LIMITED_API
    PyObject *func_dict;
#endif
    PyObject *func_name;
    PyObject *func_qualname;
    PyObject *func_doc;
    PyObject *func_globals;
    PyObject *func_code;
    PyObject *func_closure;
#if PY_VERSION_HEX < 0x030900B1 || CYTHON_COMPILING_IN_LIMITED_API
    PyObject *func_classobj;
#endif
    PyObject *defaults;
    int flags;
    PyObject *defaults_tuple;
    PyObject *defaults_kwdict;
    PyObject *(*defaults_getter)(PyObject *);
    PyObject *func_annotations;
    PyObject *func_is_coroutine;
} __pyx_CyFunctionObject;
#undef __Pyx_CyOrPyCFunction_Check
#define __Pyx_CyFunction_Check(obj)  __Pyx_TypeCheck(obj, __pyx_mstate_global->__pyx_CyFunctionType)
#define __Pyx_CyOrPyCFunction_Check(obj)  __Pyx_TypeCheck2(obj, __pyx_mstate_global->__pyx_CyFunctionType, &PyCFunction_Type)
#define __Pyx_CyFunction_CheckExact(obj)  __Pyx_IS_TYPE(obj, __pyx_mstate_global->__pyx_CyFunctionType)
static CYTHON_INLINE int __Pyx__IsSameCyOrCFunction(PyObject *func, void (*cfunc)(void));
#undef __Pyx_IsSameCFunction
#define __Pyx_IsSameCFunction(func, cfunc)   __Pyx__IsSameCyOrCFunction(func, cfunc)
static PyObject *__Pyx_CyFunction_Init(__pyx_CyFunctionObject* op, PyMethodDef *ml,
                                      int flags, PyObject* qualname,
                                      PyObject *closure,
                                      PyObject *module, PyObject *globals,
                                      PyObject* code);
static CYTHON_INLINE void __Pyx__CyFunction_SetClassObj(__pyx_CyFunctionObject* f, PyObject* classobj);
static CYTHON_INLINE PyObject *__Pyx_CyFunction_InitDefaults(PyObject *func,
                                                         PyTypeObject *defaults_type);
static CYTHON_INLINE void __Pyx_CyFunction_SetDefaultsTuple(PyObject *m,
                                                            PyObject *tuple);
static CYTHON_INLINE void __Pyx_CyFunction_SetDefaultsKwDict(PyObject *m,
                                                             PyObject *dict);
static CYTHON_INLINE void __Pyx_CyFunction_SetAnnotationsDict(PyObject *m,
                                                              PyObject *dict);
static int __pyx_CyFunction_init(PyObject *module);
#if CYTHON_METH_FASTCALL
static PyObject * __Pyx_CyFunction_Vectorcall_NOARGS(PyObject *func, PyObject *const *args, size_t nargsf, PyObject *kwnames);
static PyObject * __Pyx_CyFunction_Vectorcall_O(PyObject *func, PyObject *const *args, size_t nargsf, PyObject *kwnames);
static PyObject * __Pyx_CyFunction_Vectorcall_FASTCALL_KEYWORDS(PyObject *func, PyObject *const *args, size_t nargsf, PyObject *kwnames);
static PyObject * __Pyx_CyFunction_Vectorcall_FASTCALL_KEYWORDS_METHOD(PyObject *func, PyObject *const *args, size_t nargsf, PyObject *kwnames);
#if CYTHON_COMPILING_IN_LIMITED_API
#define __Pyx_CyFunction_func_vectorcall(f) (((__pyx_CyFunctionObject*)f)->func_vectorcall)
#else
#define __Pyx_CyFunction_func_vectorcall(f) (((PyCFunctionObject*)f)->vectorcall)
#endif
#endif

static PyObject *__Pyx_CyFunction_New(PyMethodDef *ml,
                                      int flags, PyObject* qualname,
                                      PyObject *closure,
                                      PyObject *module, PyObject *globals,
                                      PyObject* code);

#if CYTHON_CLINE_IN_TRACEBACK && CYTHON_CLINE_IN_TRACEBACK_RUNTIME
static int __Pyx_CLineForTraceback(PyThreadState *tstate, int c_line);
#else
#define __Pyx_CLineForTraceback(tstate, c_line)  (((CYTHON_CLINE_IN_TRACEBACK)) ? c_line : 0)
#endif

#if CYTHON_COMPILING_IN_LIMITED_API
typedef PyObject __Pyx_CachedCodeObjectType;
#else
typedef PyCodeObject __Pyx_CachedCodeObjectType;
#endif
typedef struct {
    __Pyx_CachedCodeObjectType* code_object;
    int code_line;
} __Pyx_CodeObjectCacheEntry;
struct __Pyx_CodeObjectCache {
    int count;
    int max_count;
    __Pyx_CodeObjectCacheEntry* entries;
  #if CYTHON_COMPILING_IN_CPYTHON_FREETHREADING
    __pyx_atomic_int_type accessor_count;
  #endif
};
static int __pyx_bisect_code_objects(__Pyx_CodeObjectCacheEntry* entries, int count, int code_line);
static __Pyx_CachedCodeObjectType *__pyx_find_code_object(int code_line);
static void __pyx_insert_code_object(int code_line, __Pyx_CachedCodeObjectType* code_object);

static void __Pyx_AddTraceback(const char *funcname, int c_line,
                               int py_line, const char *filename);

#if !defined(__INTEL_COMPILER) && defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
#define __Pyx_HAS_GCC_DIAGNOSTIC
#endif

static CYTHON_INLINE int __Pyx_PyLong_As_int(PyObject *);

CYTHON_UNUSED static int __Pyx_VectorcallBuilder_AddArg_Check(PyObject *key, PyObject *value, PyObject *builder, PyObject **args, int n);
#if CYTHON_VECTORCALL
#if PY_VERSION_HEX >= 0x03090000
#define __Pyx_Object_Vectorcall_CallFromBuilder PyObject_Vectorcall
#else
#define __Pyx_Object_Vectorcall_CallFromBuilder _PyObject_Vectorcall
#endif
#define __Pyx_MakeVectorcallBuilderKwds(n) PyTuple_New(n)
static int __Pyx_VectorcallBuilder_AddArg(PyObject *key, PyObject *value, PyObject *builder, PyObject **args, int n);
static int __Pyx_VectorcallBuilder_AddArgStr(const char *key, PyObject *value, PyObject *builder, PyObject **args, int n);
#else
#define __Pyx_Object_Vectorcall_CallFromBuilder __Pyx_PyObject_FastCallDict
#define __Pyx_MakeVectorcallBuilderKwds(n) __Pyx_PyDict_NewPresized(n)
#define __Pyx_VectorcallBuilder_AddArg(key, value, builder, args, n) PyDict_SetItem(builder, key, value)
#define __Pyx_VectorcallBuilder_AddArgStr(key, value, builder, args, n) PyDict_SetItemString(builder, key, value)
#endif

static CYTHON_INLINE PyObject* __Pyx_PyLong_From_long(long value);

static CYTHON_INLINE PyObject* __Pyx_PyLong_From_int(int value);

#if CYTHON_COMPILING_IN_LIMITED_API
typedef PyObject *__Pyx_TypeName;
#define __Pyx_FMT_TYPENAME "%U"
#define __Pyx_DECREF_TypeName(obj) Py_XDECREF(obj)
#if __PYX_LIMITED_VERSION_HEX >= 0x030d0000
#define __Pyx_PyType_GetFullyQualifiedName PyType_GetFullyQualifiedName
#else
static __Pyx_TypeName __Pyx_PyType_GetFullyQualifiedName(PyTypeObject* tp);
#endif
#else
typedef const char *__Pyx_TypeName;
#define __Pyx_FMT_TYPENAME "%.200s"
#define __Pyx_PyType_GetFullyQualifiedName(tp) ((tp)->tp_name)
#define __Pyx_DECREF_TypeName(obj)
#endif

static CYTHON_INLINE long __Pyx_PyLong_As_long(PyObject *);

#if __PYX_LIMITED_VERSION_HEX < 0x030b0000
static unsigned long __Pyx_cached_runtime_version = 0;
static void __Pyx_init_runtime_version(void);
#else
#define __Pyx_init_runtime_version()
#endif
static unsigned long __Pyx_get_runtime_version(void);

static int __Pyx_check_binary_version(unsigned long ct_version, unsigned long rt_version, int allow_newer);

static PyObject *__Pyx_DecompressString(const char *s, Py_ssize_t length, int algo);

#if CYTHON_PEP489_MULTI_PHASE_INIT && CYTHON_USE_MODULE_STATE
static PyObject *__Pyx_State_FindModule(void*);
static int __Pyx_State_AddModule(PyObject* module, void*);
static int __Pyx_State_RemoveModule(void*);
#elif CYTHON_USE_MODULE_STATE
#define __Pyx_State_FindModule PyState_FindModule
#define __Pyx_State_AddModule PyState_AddModule
#define __Pyx_State_RemoveModule PyState_RemoveModule
#endif

#if CYTHON_COMPILING_IN_LIMITED_API
    #if CYTHON_METH_FASTCALL
        #define __PYX_FASTCALL_ABI_SUFFIX  "_fastcall"
    #else
        #define __PYX_FASTCALL_ABI_SUFFIX
    #endif
    #define __PYX_LIMITED_ABI_SUFFIX "limited" __PYX_FASTCALL_ABI_SUFFIX __PYX_AM_SEND_ABI_SUFFIX
#else
    #define __PYX_LIMITED_ABI_SUFFIX
#endif
#if __PYX_HAS_PY_AM_SEND == 1
    #define __PYX_AM_SEND_ABI_SUFFIX
#elif __PYX_HAS_PY_AM_SEND == 2
    #define __PYX_AM_SEND_ABI_SUFFIX "amsendbackport"
#else
    #define __PYX_AM_SEND_ABI_SUFFIX "noamsend"
#endif
#ifndef __PYX_MONITORING_ABI_SUFFIX
    #define __PYX_MONITORING_ABI_SUFFIX
#endif
#if CYTHON_USE_TP_FINALIZE
    #define __PYX_TP_FINALIZE_ABI_SUFFIX
#else
    #define __PYX_TP_FINALIZE_ABI_SUFFIX "nofinalize"
#endif
#if CYTHON_USE_FREELISTS || !defined(__Pyx_AsyncGen_USED)
    #define __PYX_FREELISTS_ABI_SUFFIX
#else
    #define __PYX_FREELISTS_ABI_SUFFIX "nofreelists"
#endif
#define CYTHON_ABI  __PYX_ABI_VERSION __PYX_LIMITED_ABI_SUFFIX __PYX_MONITORING_ABI_SUFFIX __PYX_TP_FINALIZE_ABI_SUFFIX __PYX_FREELISTS_ABI_SUFFIX __PYX_AM_SEND_ABI_SUFFIX
#define __PYX_ABI_MODULE_NAME "_cython_" CYTHON_ABI
#define __PYX_TYPE_MODULE_PREFIX __PYX_ABI_MODULE_NAME "."

static PyObject *__pyx_f_11_fast_match__match_path_compact_impl(PyObject *, PyObject *, int, PyObject *, PyObject *, PyObject *, PyObject *, struct __pyx_opt_args_11_fast_match__match_path_compact_impl *__pyx_optional_args);

#define __Pyx_MODULE_NAME "_fast_match"
extern int __pyx_module_is_main__fast_match;
int __pyx_module_is_main__fast_match = 0;

static PyObject *__pyx_builtin_max;
static PyObject *__pyx_builtin_min;

static const char __pyx_k_Cython_accelerated_path_matchin[] = "\nCython-accelerated path matching inner loop.\n\n\346\233\277\344\273\243 match_path_compact \347\232\204\351\202\273\345\261\205\351\201\215\345\216\206\357\274\214\344\275\277\347\224\250 CompactGraph \347\232\204 int \346\225\260\347\273\204\343\200\202\n";

static PyObject *__pyx_pf_11_fast_match_match_path_compact_fast(CYTHON_UNUSED PyObject *__pyx_self, PyObject *__pyx_v_compact_graph, PyObject *__pyx_v_path_sequence, int __pyx_v_pivot_int, PyObject *__pyx_v_edge_lookup, PyObject *__pyx_v_node_label_map, PyObject *__pyx_v_skip_label_checks, PyObject *__pyx_v_prefilter_sets, PyObject *__pyx_v_pw_eval_fn);
static PyObject *__pyx_pf_11_fast_match_2match_star_paths_batch_fast(CYTHON_UNUSED PyObject *__pyx_self, PyObject *__pyx_v_compact_graph, PyObject *__pyx_v_fast_path_plans, int __pyx_v_pivot_int, PyObject *__pyx_v_int_prefilter, PyObject *__pyx_v_pw_eval_fn);
static PyObject *__pyx_pf_11_fast_match_4match_bridge_batch(CYTHON_UNUSED PyObject *__pyx_self, PyObject *__pyx_v_compact_graph, PyObject *__pyx_v_path_seq, PyObject *__pyx_v_pivot_ints, PyObject *__pyx_v_edge_lookup, PyObject *__pyx_v_node_label_map, PyObject *__pyx_v_skip_label_checks, PyObject *__pyx_v_int_prefilter, PyObject *__pyx_v_pw_eval_fn, PyObject *__pyx_v_w_candidate_ints);
static PyObject *__pyx_pf_11_fast_match_6derive_bridge_vw_pairs_fast(CYTHON_UNUSED PyObject *__pyx_self, PyObject *__pyx_v_y0_cands, PyObject *__pyx_v_w0_cands, PyObject *__pyx_v_bridge_specs, PyObject *__pyx_v_alt_matches_per_w, PyObject *__pyx_v_F_phi);
static PyObject *__pyx_pf_11_fast_match_8propagate_candidates_dag_fast(CYTHON_UNUSED PyObject *__pyx_self, PyObject *__pyx_v_compact_graph, PyObject *__pyx_v_forward_steps, PyObject *__pyx_v_backward_steps, PyObject *__pyx_v_F_int);
static PyObject *__pyx_pf_11_fast_match_10prune_pairwise_values_fast(CYTHON_UNUSED PyObject *__pyx_self, PyObject *__pyx_v_vals1, PyObject *__pyx_v_vals2, PyObject *__pyx_v_operator);
static PyObject *__pyx_pf_11_fast_match_12filter_dis_items_fast(CYTHON_UNUSED PyObject *__pyx_self, PyObject *__pyx_v_dis_cache, PyObject *__pyx_v_anchor_id, PyObject *__pyx_v_operator, double __pyx_v_threshold, PyObject *__pyx_v_candidate_set);
static PyObject *__pyx_pf_11_fast_match_14verify_pivot_pairwise_binding_fast(CYTHON_UNUSED PyObject *__pyx_self, PyObject *__pyx_v_user_attrs, PyObject *__pyx_v_item_attrs, PyObject *__pyx_v_pairwise_plans);
static PyObject *__pyx_pf_11_fast_match_16cleanup_lazy_heap_fast(CYTHON_UNUSED PyObject *__pyx_self, PyObject *__pyx_v_heap, PyObject *__pyx_v_active_scores);
static PyObject *__pyx_pf_11_fast_match_18update_unique_topk_heap_fast(CYTHON_UNUSED PyObject *__pyx_self, PyObject *__pyx_v_item, double __pyx_v_score, PyObject *__pyx_v_heap, PyObject *__pyx_v_active_scores, int __pyx_v_k, PyObject *__pyx_v_counter_state);
static PyObject *__pyx_pf_11_fast_match_20update_heap_and_tau_fast(CYTHON_UNUSED PyObject *__pyx_self, PyObject *__pyx_v_anchor, PyObject *__pyx_v_item, double __pyx_v_score, PyObject *__pyx_v_heap, PyObject *__pyx_v_tau_dict, int __pyx_v_k, PyObject *__pyx_v_active_scores, PyObject *__pyx_v_counter_state);
static PyObject *__pyx_pf_11_fast_match_22update_global_best_and_tau_fast(CYTHON_UNUSED PyObject *__pyx_self, PyObject *__pyx_v_item, double __pyx_v_score, PyObject *__pyx_v_best_scores, PyObject *__pyx_v_global_heap, PyObject *__pyx_v_global_heap_scores, int __pyx_v_alt_k, PyObject *__pyx_v_counter_state);

#ifndef CYTHON_SMALL_CODE
#if defined(__clang__)
    #define CYTHON_SMALL_CODE
#elif defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3))
    #define CYTHON_SMALL_CODE __attribute__((cold))
#else
    #define CYTHON_SMALL_CODE
#endif
#endif

typedef struct {
  PyObject *__pyx_d;
  PyObject *__pyx_b;
  PyObject *__pyx_cython_runtime;
  PyObject *__pyx_empty_tuple;
  PyObject *__pyx_empty_bytes;
  PyObject *__pyx_empty_unicode;
  __Pyx_CachedCFunction __pyx_umethod_PyDict_Type_get;
  __Pyx_CachedCFunction __pyx_umethod_PyDict_Type_items;
  __Pyx_CachedCFunction __pyx_umethod_PyDict_Type_pop;
  __Pyx_CachedCFunction __pyx_umethod_PyDict_Type_values;
  PyObject *__pyx_tuple[2];
  PyObject *__pyx_codeobj_tab[12];
  PyObject *__pyx_string_tab[209];
  PyObject *__pyx_number_tab[2];

PyTypeObject *__pyx_CommonTypesMetaclassType;

#if CYTHON_COMPILING_IN_LIMITED_API
PyObject *__Pyx_CachedMethodType;
#endif

PyTypeObject *__pyx_CyFunctionType;

struct __Pyx_CodeObjectCache __pyx_code_cache;

} __pyx_mstatetype;

#if CYTHON_USE_MODULE_STATE
#ifdef __cplusplus
namespace {
extern struct PyModuleDef __pyx_moduledef;
}
#else
static struct PyModuleDef __pyx_moduledef;
#endif

#define __pyx_mstate_global (__Pyx_PyModule_GetState(__Pyx_State_FindModule(&__pyx_moduledef)))

#define __pyx_m (__Pyx_State_FindModule(&__pyx_moduledef))
#else
static __pyx_mstatetype __pyx_mstate_global_static =
#ifdef __cplusplus
    {};
#else
    {0};
#endif
static __pyx_mstatetype * const __pyx_mstate_global = &__pyx_mstate_global_static;
#endif

#define __pyx_kp_u_ __pyx_string_tab[0]
#define __pyx_kp_u_Note_that_Cython_is_deliberately __pyx_string_tab[1]
#define __pyx_kp_u__2 __pyx_string_tab[2]
#define __pyx_kp_u__3 __pyx_string_tab[3]
#define __pyx_kp_u__4 __pyx_string_tab[4]
#define __pyx_kp_u__5 __pyx_string_tab[5]
#define __pyx_kp_u__6 __pyx_string_tab[6]
#define __pyx_kp_u_add_note __pyx_string_tab[7]
#define __pyx_kp_u_fast_match_pyx __pyx_string_tab[8]
#define __pyx_n_u_ComparisonOp __pyx_string_tab[9]
#define __pyx_n_u_EQ __pyx_string_tab[10]
#define __pyx_n_u_F_int __pyx_string_tab[11]
#define __pyx_n_u_F_phi __pyx_string_tab[12]
#define __pyx_n_u_GE __pyx_string_tab[13]
#define __pyx_n_u_GT __pyx_string_tab[14]
#define __pyx_n_u_LE __pyx_string_tab[15]
#define __pyx_n_u_LT __pyx_string_tab[16]
#define __pyx_n_u_NE __pyx_string_tab[17]
#define __pyx_n_u_Pyx_PyDict_NextRef __pyx_string_tab[18]
#define __pyx_n_u_active_item __pyx_string_tab[19]
#define __pyx_n_u_active_score __pyx_string_tab[20]
#define __pyx_n_u_active_scores __pyx_string_tab[21]
#define __pyx_n_u_add __pyx_string_tab[22]
#define __pyx_n_u_alt_cands __pyx_string_tab[23]
#define __pyx_n_u_alt_k __pyx_string_tab[24]
#define __pyx_n_u_alt_matches_per_w __pyx_string_tab[25]
#define __pyx_n_u_anchor __pyx_string_tab[26]
#define __pyx_n_u_anchor_id __pyx_string_tab[27]
#define __pyx_n_u_asyncio_coroutines __pyx_string_tab[28]
#define __pyx_n_u_attr1 __pyx_string_tab[29]
#define __pyx_n_u_attr2 __pyx_string_tab[30]
#define __pyx_n_u_backward_steps __pyx_string_tab[31]
#define __pyx_n_u_best_scores __pyx_string_tab[32]
#define __pyx_n_u_bridge_cands __pyx_string_tab[33]
#define __pyx_n_u_bridge_specs __pyx_string_tab[34]
#define __pyx_n_u_bridge_vw __pyx_string_tab[35]
#define __pyx_n_u_bucket __pyx_string_tab[36]
#define __pyx_n_u_c1 __pyx_string_tab[37]
#define __pyx_n_u_c2 __pyx_string_tab[38]
#define __pyx_n_u_c_int __pyx_string_tab[39]
#define __pyx_n_u_candidate_set __pyx_string_tab[40]
#define __pyx_n_u_cg_in __pyx_string_tab[41]
#define __pyx_n_u_cg_labels __pyx_string_tab[42]
#define __pyx_n_u_cg_out __pyx_string_tab[43]
#define __pyx_n_u_child_ints __pyx_string_tab[44]
#define __pyx_n_u_child_var __pyx_string_tab[45]
#define __pyx_n_u_class_getitem __pyx_string_tab[46]
#define __pyx_n_u_cleanup_lazy_heap_fast __pyx_string_tab[47]
#define __pyx_n_u_cline_in_traceback __pyx_string_tab[48]
#define __pyx_n_u_compact_graph __pyx_string_tab[49]
#define __pyx_n_u_counter_state __pyx_string_tab[50]
#define __pyx_n_u_current __pyx_string_tab[51]
#define __pyx_n_u_current_int __pyx_string_tab[52]
#define __pyx_n_u_current_set __pyx_string_tab[53]
#define __pyx_n_u_current_var __pyx_string_tab[54]
#define __pyx_n_u_data_structure __pyx_string_tab[55]
#define __pyx_n_u_derive_bridge_vw_pairs_fast __pyx_string_tab[56]
#define __pyx_n_u_dis_cache __pyx_string_tab[57]
#define __pyx_n_u_do_label_check __pyx_string_tab[58]
#define __pyx_n_u_edge_info __pyx_string_tab[59]
#define __pyx_n_u_edge_label __pyx_string_tab[60]
#define __pyx_n_u_edge_lookup __pyx_string_tab[61]
#define __pyx_n_u_edges_info __pyx_string_tab[62]
#define __pyx_n_u_el __pyx_string_tab[63]
#define __pyx_n_u_expected_label __pyx_string_tab[64]
#define __pyx_n_u_failed __pyx_string_tab[65]
#define __pyx_n_u_fast_match __pyx_string_tab[66]
#define __pyx_n_u_fast_path_plans __pyx_string_tab[67]
#define __pyx_n_u_filter_dis_items_fast __pyx_string_tab[68]
#define __pyx_n_u_filtered __pyx_string_tab[69]
#define __pyx_n_u_forward_neighbor_map __pyx_string_tab[70]
#define __pyx_n_u_forward_steps __pyx_string_tab[71]
#define __pyx_n_u_func __pyx_string_tab[72]
#define __pyx_n_u_get __pyx_string_tab[73]
#define __pyx_n_u_global_heap __pyx_string_tab[74]
#define __pyx_n_u_global_heap_scores __pyx_string_tab[75]
#define __pyx_n_u_has_prefilter __pyx_string_tab[76]
#define __pyx_n_u_has_pw_eval __pyx_string_tab[77]
#define __pyx_n_u_has_w_filter __pyx_string_tab[78]
#define __pyx_n_u_heap __pyx_string_tab[79]
#define __pyx_n_u_heapify __pyx_string_tab[80]
#define __pyx_n_u_heappop __pyx_string_tab[81]
#define __pyx_n_u_heappush __pyx_string_tab[82]
#define __pyx_n_u_heapq __pyx_string_tab[83]
#define __pyx_n_u_hop_meta __pyx_string_tab[84]
#define __pyx_n_u_i __pyx_string_tab[85]
#define __pyx_n_u_in_neighbors __pyx_string_tab[86]
#define __pyx_n_u_int_prefilter __pyx_string_tab[87]
#define __pyx_n_u_int_set __pyx_string_tab[88]
#define __pyx_n_u_is_coroutine __pyx_string_tab[89]
#define __pyx_n_u_is_forward __pyx_string_tab[90]
#define __pyx_n_u_is_fwd __pyx_string_tab[91]
#define __pyx_n_u_item __pyx_string_tab[92]
#define __pyx_n_u_item_attrs __pyx_string_tab[93]
#define __pyx_n_u_items __pyx_string_tab[94]
#define __pyx_n_u_join_ok __pyx_string_tab[95]
#define __pyx_n_u_k __pyx_string_tab[96]
#define __pyx_n_u_last_var __pyx_string_tab[97]
#define __pyx_n_u_lhs_is_user __pyx_string_tab[98]
#define __pyx_n_u_main __pyx_string_tab[99]
#define __pyx_n_u_match_bridge_batch __pyx_string_tab[100]
#define __pyx_n_u_match_path_compact_fast __pyx_string_tab[101]
#define __pyx_n_u_match_star_paths_batch_fast __pyx_string_tab[102]
#define __pyx_n_u_matching_c2s __pyx_string_tab[103]
#define __pyx_n_u_max __pyx_string_tab[104]
#define __pyx_n_u_max_v1 __pyx_string_tab[105]
#define __pyx_n_u_max_v2 __pyx_string_tab[106]
#define __pyx_n_u_merged __pyx_string_tab[107]
#define __pyx_n_u_min __pyx_string_tab[108]
#define __pyx_n_u_min_v1 __pyx_string_tab[109]
#define __pyx_n_u_min_v2 __pyx_string_tab[110]
#define __pyx_n_u_module __pyx_string_tab[111]
#define __pyx_n_u_n_child __pyx_string_tab[112]
#define __pyx_n_u_n_id __pyx_string_tab[113]
#define __pyx_n_u_n_int __pyx_string_tab[114]
#define __pyx_n_u_n_parent __pyx_string_tab[115]
#define __pyx_n_u_name __pyx_string_tab[116]
#define __pyx_n_u_needs_pw_eval __pyx_string_tab[117]
#define __pyx_n_u_neighbor_map __pyx_string_tab[118]
#define __pyx_n_u_neighbors __pyx_string_tab[119]
#define __pyx_n_u_next_counter __pyx_string_tab[120]
#define __pyx_n_u_next_set __pyx_string_tab[121]
#define __pyx_n_u_next_var __pyx_string_tab[122]
#define __pyx_n_u_node_label_map __pyx_string_tab[123]
#define __pyx_n_u_node_labels __pyx_string_tab[124]
#define __pyx_n_u_operator __pyx_string_tab[125]
#define __pyx_n_u_out __pyx_string_tab[126]
#define __pyx_n_u_out_neighbors __pyx_string_tab[127]
#define __pyx_n_u_p_idx __pyx_string_tab[128]
#define __pyx_n_u_p_int __pyx_string_tab[129]
#define __pyx_n_u_pairwise_plans __pyx_string_tab[130]
#define __pyx_n_u_parent_ints __pyx_string_tab[131]
#define __pyx_n_u_parent_var __pyx_string_tab[132]
#define __pyx_n_u_path_result __pyx_string_tab[133]
#define __pyx_n_u_path_seq __pyx_string_tab[134]
#define __pyx_n_u_path_sequence __pyx_string_tab[135]
#define __pyx_n_u_path_ws __pyx_string_tab[136]
#define __pyx_n_u_per_v_matches __pyx_string_tab[137]
#define __pyx_n_u_pivot_int __pyx_string_tab[138]
#define __pyx_n_u_pivot_ints __pyx_string_tab[139]
#define __pyx_n_u_pivot_match __pyx_string_tab[140]
#define __pyx_n_u_plan __pyx_string_tab[141]
#define __pyx_n_u_pop __pyx_string_tab[142]
#define __pyx_n_u_prefilter __pyx_string_tab[143]
#define __pyx_n_u_prefilter_sets __pyx_string_tab[144]
#define __pyx_n_u_propagate_candidates_dag_fast __pyx_string_tab[145]
#define __pyx_n_u_prune_pairwise_values_fast __pyx_string_tab[146]
#define __pyx_n_u_pw_eval_fn __pyx_string_tab[147]
#define __pyx_n_u_qualname __pyx_string_tab[148]
#define __pyx_n_u_reachable_child_ints __pyx_string_tab[149]
#define __pyx_n_u_reachable_parent_ints __pyx_string_tab[150]
#define __pyx_n_u_rebuilt __pyx_string_tab[151]
#define __pyx_n_u_result __pyx_string_tab[152]
#define __pyx_n_u_reverse_neighbor_map __pyx_string_tab[153]
#define __pyx_n_u_rhs_is_user __pyx_string_tab[154]
#define __pyx_n_u_root_item __pyx_string_tab[155]
#define __pyx_n_u_root_score __pyx_string_tab[156]
#define __pyx_n_u_safe_compare_predicate_values __pyx_string_tab[157]
#define __pyx_n_u_score __pyx_string_tab[158]
#define __pyx_n_u_seq __pyx_string_tab[159]
#define __pyx_n_u_set_name __pyx_string_tab[160]
#define __pyx_n_u_setdefault __pyx_string_tab[161]
#define __pyx_n_u_shared_var __pyx_string_tab[162]
#define __pyx_n_u_shared_vars __pyx_string_tab[163]
#define __pyx_n_u_skip_checks __pyx_string_tab[164]
#define __pyx_n_u_skip_label_checks __pyx_string_tab[165]
#define __pyx_n_u_step_count __pyx_string_tab[166]
#define __pyx_n_u_tau_dict __pyx_string_tab[167]
#define __pyx_n_u_terminal_var __pyx_string_tab[168]
#define __pyx_n_u_test __pyx_string_tab[169]
#define __pyx_n_u_threshold __pyx_string_tab[170]
#define __pyx_n_u_unique_vals1 __pyx_string_tab[171]
#define __pyx_n_u_unique_vals2 __pyx_string_tab[172]
#define __pyx_n_u_update_global_best_and_tau_fast __pyx_string_tab[173]
#define __pyx_n_u_update_heap_and_tau_fast __pyx_string_tab[174]
#define __pyx_n_u_update_unique_topk_heap_fast __pyx_string_tab[175]
#define __pyx_n_u_user_attrs __pyx_string_tab[176]
#define __pyx_n_u_v __pyx_string_tab[177]
#define __pyx_n_u_v1 __pyx_string_tab[178]
#define __pyx_n_u_v2 __pyx_string_tab[179]
#define __pyx_n_u_val_to_c2 __pyx_string_tab[180]
#define __pyx_n_u_valid_ws __pyx_string_tab[181]
#define __pyx_n_u_valid_z1 __pyx_string_tab[182]
#define __pyx_n_u_valid_z2 __pyx_string_tab[183]
#define __pyx_n_u_vals1 __pyx_string_tab[184]
#define __pyx_n_u_vals2 __pyx_string_tab[185]
#define __pyx_n_u_value1 __pyx_string_tab[186]
#define __pyx_n_u_value2 __pyx_string_tab[187]
#define __pyx_n_u_values __pyx_string_tab[188]
#define __pyx_n_u_var __pyx_string_tab[189]
#define __pyx_n_u_verify_pivot_pairwise_binding_fa __pyx_string_tab[190]
#define __pyx_n_u_w __pyx_string_tab[191]
#define __pyx_n_u_w0_cands __pyx_string_tab[192]
#define __pyx_n_u_w0_var __pyx_string_tab[193]
#define __pyx_n_u_w_candidate_ints __pyx_string_tab[194]
#define __pyx_n_u_w_match __pyx_string_tab[195]
#define __pyx_n_u_y0_cands __pyx_string_tab[196]
#define __pyx_kp_b_iso88591_1_k_1 __pyx_string_tab[197]
#define __pyx_kp_b_iso88591_A_t1_q_t9Ct1_q_c_U_Q_1_N_j_t1A __pyx_string_tab[198]
#define __pyx_kp_b_iso88591_A_y_1_E_IT_a_vS_S_c_d_1_q_y_1_E __pyx_string_tab[199]
#define __pyx_kp_b_iso88591_T_q_a_Q_s_1_E_KvQ_t3a_awfAU_A_4 __pyx_string_tab[200]
#define __pyx_kp_b_iso88591_a_a_t6_D_z_y_q_A_D_e6_Yd_1_wc_A __pyx_string_tab[201]
#define __pyx_kp_b_iso88591_d_1_AQ_d_1_AQ_4q_4q_T_4q_T_7_U __pyx_string_tab[202]
#define __pyx_kp_b_iso88591_r_1_A_m4q_xwa_6_A_QfA_AQ_Qha_Ya __pyx_string_tab[203]
#define __pyx_kp_b_iso88591_t9Ct1_q_AZr_Q_a_IU_3aq_q_hauJaq __pyx_string_tab[204]
#define __pyx_kp_b_iso88591_t_AT_AT_D_1A_XQa __pyx_string_tab[205]
#define __pyx_kp_b_iso88591_t_T_q_M_K_Q_e4q_U_aq_4_3d_1_Qa __pyx_string_tab[206]
#define __pyx_kp_b_iso88591_vRq_k_Qa_xwe4vS_a_A_3a_3fD_ar_q __pyx_string_tab[207]
#define __pyx_kp_b_iso88591_wfO3a_6_s_e2Rwc_Q_4q_1_1 __pyx_string_tab[208]
#define __pyx_float_0_0 __pyx_number_tab[0]
#define __pyx_int_1 __pyx_number_tab[1]

#if CYTHON_USE_MODULE_STATE
static CYTHON_SMALL_CODE int __pyx_m_clear(PyObject *m) {
  __pyx_mstatetype *clear_module_state = __Pyx_PyModule_GetState(m);
  if (!clear_module_state) return 0;
  Py_CLEAR(clear_module_state->__pyx_d);
  Py_CLEAR(clear_module_state->__pyx_b);
  Py_CLEAR(clear_module_state->__pyx_cython_runtime);
  Py_CLEAR(clear_module_state->__pyx_empty_tuple);
  Py_CLEAR(clear_module_state->__pyx_empty_bytes);
  Py_CLEAR(clear_module_state->__pyx_empty_unicode);
  #if CYTHON_PEP489_MULTI_PHASE_INIT
  __Pyx_State_RemoveModule(NULL);
  #endif
  for (int i=0; i<2; ++i) { Py_CLEAR(clear_module_state->__pyx_tuple[i]); }
  for (int i=0; i<12; ++i) { Py_CLEAR(clear_module_state->__pyx_codeobj_tab[i]); }
  for (int i=0; i<209; ++i) { Py_CLEAR(clear_module_state->__pyx_string_tab[i]); }
  for (int i=0; i<2; ++i) { Py_CLEAR(clear_module_state->__pyx_number_tab[i]); }

Py_CLEAR(clear_module_state->__pyx_CommonTypesMetaclassType);

Py_CLEAR(clear_module_state->__pyx_CyFunctionType);

return 0;
}
#endif

#if CYTHON_USE_MODULE_STATE
static CYTHON_SMALL_CODE int __pyx_m_traverse(PyObject *m, visitproc visit, void *arg) {
  __pyx_mstatetype *traverse_module_state = __Pyx_PyModule_GetState(m);
  if (!traverse_module_state) return 0;
  Py_VISIT(traverse_module_state->__pyx_d);
  Py_VISIT(traverse_module_state->__pyx_b);
  Py_VISIT(traverse_module_state->__pyx_cython_runtime);
  __Pyx_VISIT_CONST(traverse_module_state->__pyx_empty_tuple);
  __Pyx_VISIT_CONST(traverse_module_state->__pyx_empty_bytes);
  __Pyx_VISIT_CONST(traverse_module_state->__pyx_empty_unicode);
  for (int i=0; i<2; ++i) { __Pyx_VISIT_CONST(traverse_module_state->__pyx_tuple[i]); }
  for (int i=0; i<12; ++i) { __Pyx_VISIT_CONST(traverse_module_state->__pyx_codeobj_tab[i]); }
  for (int i=0; i<209; ++i) { __Pyx_VISIT_CONST(traverse_module_state->__pyx_string_tab[i]); }
  for (int i=0; i<2; ++i) { __Pyx_VISIT_CONST(traverse_module_state->__pyx_number_tab[i]); }

Py_VISIT(traverse_module_state->__pyx_CommonTypesMetaclassType);

Py_VISIT(traverse_module_state->__pyx_CyFunctionType);

return 0;
}
#endif

static PyObject *__pyx_f_11_fast_match__match_path_compact_impl(PyObject *__pyx_v_compact_graph, PyObject *__pyx_v_path_sequence, int __pyx_v_pivot_int, PyObject *__pyx_v_edge_lookup, PyObject *__pyx_v_node_label_map, PyObject *__pyx_v_skip_label_checks, PyObject *__pyx_v_prefilter_sets, struct __pyx_opt_args_11_fast_match__match_path_compact_impl *__pyx_optional_args) {

  PyObject *__pyx_v_pw_eval_fn = ((PyObject *)Py_None);
  int __pyx_v_i;
  int __pyx_v_step_count;
  int __pyx_v_n_id;
  int __pyx_v_is_forward;
  int __pyx_v_do_label_check;
  int __pyx_v_has_prefilter;
  int __pyx_v_has_pw_eval;
  PyObject *__pyx_v_result = NULL;
  PyObject *__pyx_v_cg_out = NULL;
  PyObject *__pyx_v_cg_in = NULL;
  PyObject *__pyx_v_cg_labels = NULL;
  PyObject *__pyx_v_current_var = NULL;
  PyObject *__pyx_v_next_var = NULL;
  PyObject *__pyx_v_edge_info = NULL;
  PyObject *__pyx_v_edge_label = NULL;
  PyObject *__pyx_v_expected_label = NULL;
  PyObject *__pyx_v_current_set = NULL;
  PyObject *__pyx_v_next_set = NULL;
  PyObject *__pyx_v_prefilter = NULL;
  PyObject *__pyx_v_current_int = NULL;
  PyObject *__pyx_v_neighbors = NULL;
  PyObject *__pyx_r = NULL;
  __Pyx_RefNannyDeclarations
  int __pyx_t_1;
  int __pyx_t_2;
  PyObject *__pyx_t_3 = NULL;
  Py_ssize_t __pyx_t_4;
  PyObject *__pyx_t_5 = NULL;
  PyObject *__pyx_t_6 = NULL;
  PyObject *__pyx_t_7 = NULL;
  int __pyx_t_8;
  int __pyx_t_9;
  int __pyx_t_10;
  long __pyx_t_11;
  PyObject *(*__pyx_t_12)(PyObject *);
  size_t __pyx_t_13;
  Py_ssize_t __pyx_t_14;
  PyObject *(*__pyx_t_15)(PyObject *);
  int __pyx_t_16;
  PyObject *__pyx_t_17 = NULL;
  PyObject *__pyx_t_18 = NULL;
  int __pyx_t_19;
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  __Pyx_RefNannySetupContext("_match_path_compact_impl", 0);
  if (__pyx_optional_args) {
    if (__pyx_optional_args->__pyx_n > 0) {
      __pyx_v_pw_eval_fn = __pyx_optional_args->pw_eval_fn;
    }
  }

  __pyx_t_1 = __Pyx_PyObject_IsTrue(__pyx_v_path_sequence); if (unlikely((__pyx_t_1 < 0))) __PYX_ERR(0, 26, __pyx_L1_error)
  __pyx_t_2 = (!__pyx_t_1);
  if (__pyx_t_2) {

    __Pyx_XDECREF(__pyx_r);
    __pyx_t_3 = __Pyx_PyDict_NewPresized(0); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 27, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_3);
    __pyx_r = __pyx_t_3;
    __pyx_t_3 = 0;
    goto __pyx_L0;

  }

  __pyx_t_4 = PyObject_Length(__pyx_v_path_sequence); if (unlikely(__pyx_t_4 == ((Py_ssize_t)-1))) __PYX_ERR(0, 29, __pyx_L1_error)
  __pyx_t_2 = (__pyx_t_4 == 1);
  if (__pyx_t_2) {

    __Pyx_XDECREF(__pyx_r);
    __pyx_t_3 = __Pyx_PyDict_NewPresized(1); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 30, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_3);
    __pyx_t_5 = __Pyx_GetItemInt(__pyx_v_path_sequence, 0, long, 1, __Pyx_PyLong_From_long, 0, 0, 0, 1, __Pyx_ReferenceSharing_FunctionArgument); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 30, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_5);
    __pyx_t_6 = __Pyx_PyLong_From_int(__pyx_v_pivot_int); if (unlikely(!__pyx_t_6)) __PYX_ERR(0, 30, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_6);
    __pyx_t_7 = PySet_New(0); if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 30, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_7);
    if (PySet_Add(__pyx_t_7, __pyx_t_6) < (0)) __PYX_ERR(0, 30, __pyx_L1_error)
    __Pyx_DECREF(__pyx_t_6); __pyx_t_6 = 0;
    if (PyDict_SetItem(__pyx_t_3, __pyx_t_5, __pyx_t_7) < (0)) __PYX_ERR(0, 30, __pyx_L1_error)
    __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
    __Pyx_DECREF(__pyx_t_7); __pyx_t_7 = 0;
    __pyx_r = __pyx_t_3;
    __pyx_t_3 = 0;
    goto __pyx_L0;

  }

  __pyx_t_3 = __Pyx_PyDict_NewPresized(1); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 32, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_3);
  __pyx_t_7 = __Pyx_GetItemInt(__pyx_v_path_sequence, 0, long, 1, __Pyx_PyLong_From_long, 0, 0, 0, 1, __Pyx_ReferenceSharing_FunctionArgument); if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 32, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_7);
  __pyx_t_5 = __Pyx_PyLong_From_int(__pyx_v_pivot_int); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 32, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_5);
  __pyx_t_6 = PySet_New(0); if (unlikely(!__pyx_t_6)) __PYX_ERR(0, 32, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_6);
  if (PySet_Add(__pyx_t_6, __pyx_t_5) < (0)) __PYX_ERR(0, 32, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
  if (PyDict_SetItem(__pyx_t_3, __pyx_t_7, __pyx_t_6) < (0)) __PYX_ERR(0, 32, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_7); __pyx_t_7 = 0;
  __Pyx_DECREF(__pyx_t_6); __pyx_t_6 = 0;
  __pyx_v_result = ((PyObject*)__pyx_t_3);
  __pyx_t_3 = 0;

  __pyx_t_4 = PyObject_Length(__pyx_v_path_sequence); if (unlikely(__pyx_t_4 == ((Py_ssize_t)-1))) __PYX_ERR(0, 33, __pyx_L1_error)
  __pyx_v_step_count = (__pyx_t_4 - 1);

  __pyx_t_3 = __Pyx_PyObject_GetAttrStr(__pyx_v_compact_graph, __pyx_mstate_global->__pyx_n_u_out_neighbors); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 36, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_3);
  __pyx_v_cg_out = __pyx_t_3;
  __pyx_t_3 = 0;

  __pyx_t_3 = __Pyx_PyObject_GetAttrStr(__pyx_v_compact_graph, __pyx_mstate_global->__pyx_n_u_in_neighbors); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 37, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_3);
  __pyx_v_cg_in = __pyx_t_3;
  __pyx_t_3 = 0;

  __pyx_t_3 = __Pyx_PyObject_GetAttrStr(__pyx_v_compact_graph, __pyx_mstate_global->__pyx_n_u_node_labels); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 38, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_3);
  __pyx_v_cg_labels = __pyx_t_3;
  __pyx_t_3 = 0;

  __pyx_t_8 = __pyx_v_step_count;
  __pyx_t_9 = __pyx_t_8;
  for (__pyx_t_10 = 0; __pyx_t_10 < __pyx_t_9; __pyx_t_10+=1) {
    __pyx_v_i = __pyx_t_10;

    __pyx_t_3 = __Pyx_GetItemInt(__pyx_v_path_sequence, __pyx_v_i, int, 1, __Pyx_PyLong_From_int, 0, 0, 0, 1, __Pyx_ReferenceSharing_FunctionArgument); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 41, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_3);
    __Pyx_XDECREF_SET(__pyx_v_current_var, __pyx_t_3);
    __pyx_t_3 = 0;

    __pyx_t_11 = (__pyx_v_i + 1);
    __pyx_t_3 = __Pyx_GetItemInt(__pyx_v_path_sequence, __pyx_t_11, long, 1, __Pyx_PyLong_From_long, 0, 0, 0, 1, __Pyx_ReferenceSharing_FunctionArgument); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 42, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_3);
    __Pyx_XDECREF_SET(__pyx_v_next_var, __pyx_t_3);
    __pyx_t_3 = 0;

    if (unlikely(__pyx_v_edge_lookup == Py_None)) {
      PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "get");
      __PYX_ERR(0, 44, __pyx_L1_error)
    }
    __pyx_t_3 = PyTuple_New(2); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 44, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_3);
    __Pyx_INCREF(__pyx_v_current_var);
    __Pyx_GIVEREF(__pyx_v_current_var);
    if (__Pyx_PyTuple_SET_ITEM(__pyx_t_3, 0, __pyx_v_current_var) != (0)) __PYX_ERR(0, 44, __pyx_L1_error);
    __Pyx_INCREF(__pyx_v_next_var);
    __Pyx_GIVEREF(__pyx_v_next_var);
    if (__Pyx_PyTuple_SET_ITEM(__pyx_t_3, 1, __pyx_v_next_var) != (0)) __PYX_ERR(0, 44, __pyx_L1_error);
    __pyx_t_6 = __Pyx_PyDict_GetItemDefault(__pyx_v_edge_lookup, __pyx_t_3, Py_None); if (unlikely(!__pyx_t_6)) __PYX_ERR(0, 44, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_6);
    __Pyx_DECREF(__pyx_t_3); __pyx_t_3 = 0;
    __Pyx_XDECREF_SET(__pyx_v_edge_info, __pyx_t_6);
    __pyx_t_6 = 0;

    __pyx_t_2 = (__pyx_v_edge_info == Py_None);
    if (__pyx_t_2) {

      __Pyx_XDECREF(__pyx_r);
      __pyx_r = Py_None; __Pyx_INCREF(Py_None);
      goto __pyx_L0;

    }

    __pyx_t_6 = __Pyx_GetItemInt(__pyx_v_edge_info, 0, long, 1, __Pyx_PyLong_From_long, 0, 0, 0, 1, __Pyx_ReferenceSharing_OwnStrongReference); if (unlikely(!__pyx_t_6)) __PYX_ERR(0, 48, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_6);
    __Pyx_XDECREF_SET(__pyx_v_edge_label, __pyx_t_6);
    __pyx_t_6 = 0;

    __pyx_t_6 = __Pyx_GetItemInt(__pyx_v_edge_info, 1, long, 1, __Pyx_PyLong_From_long, 0, 0, 0, 1, __Pyx_ReferenceSharing_OwnStrongReference); if (unlikely(!__pyx_t_6)) __PYX_ERR(0, 49, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_6);
    __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_t_6); if (unlikely((__pyx_t_2 == (int)-1) && PyErr_Occurred())) __PYX_ERR(0, 49, __pyx_L1_error)
    __Pyx_DECREF(__pyx_t_6); __pyx_t_6 = 0;
    __pyx_v_is_forward = __pyx_t_2;

    if (unlikely(__pyx_v_node_label_map == Py_None)) {
      PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "get");
      __PYX_ERR(0, 51, __pyx_L1_error)
    }
    __pyx_t_6 = __Pyx_PyDict_GetItemDefault(__pyx_v_node_label_map, __pyx_v_next_var, Py_None); if (unlikely(!__pyx_t_6)) __PYX_ERR(0, 51, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_6);
    __Pyx_XDECREF_SET(__pyx_v_expected_label, __pyx_t_6);
    __pyx_t_6 = 0;

    if (unlikely(__pyx_v_skip_label_checks == Py_None)) {
      PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "get");
      __PYX_ERR(0, 52, __pyx_L1_error)
    }
    __pyx_t_6 = __Pyx_PyLong_From_int(__pyx_v_i); if (unlikely(!__pyx_t_6)) __PYX_ERR(0, 52, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_6);
    __pyx_t_3 = __Pyx_PyDict_GetItemDefault(__pyx_v_skip_label_checks, __pyx_t_6, Py_False); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 52, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_3);
    __Pyx_DECREF(__pyx_t_6); __pyx_t_6 = 0;
    __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_t_3); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 52, __pyx_L1_error)
    __Pyx_DECREF(__pyx_t_3); __pyx_t_3 = 0;
    __pyx_v_do_label_check = (!__pyx_t_2);

    __pyx_t_3 = __Pyx_PyDict_GetItem(__pyx_v_result, __pyx_v_current_var); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 54, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_3);
    __Pyx_XDECREF_SET(__pyx_v_current_set, __pyx_t_3);
    __pyx_t_3 = 0;

    __pyx_t_3 = PySet_New(0); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 55, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_3);
    __Pyx_XDECREF_SET(__pyx_v_next_set, ((PyObject*)__pyx_t_3));
    __pyx_t_3 = 0;

    if (unlikely(__pyx_v_prefilter_sets == Py_None)) {
      PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "get");
      __PYX_ERR(0, 58, __pyx_L1_error)
    }
    __pyx_t_3 = __Pyx_PyDict_GetItemDefault(__pyx_v_prefilter_sets, __pyx_v_next_var, Py_None); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 58, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_3);
    __Pyx_XDECREF_SET(__pyx_v_prefilter, __pyx_t_3);
    __pyx_t_3 = 0;

    __pyx_t_2 = (__pyx_v_prefilter != Py_None);
    __pyx_v_has_prefilter = __pyx_t_2;

    __pyx_t_2 = (__pyx_v_pw_eval_fn != Py_None);
    __pyx_v_has_pw_eval = __pyx_t_2;

    if (likely(PyList_CheckExact(__pyx_v_current_set)) || PyTuple_CheckExact(__pyx_v_current_set)) {
      __pyx_t_3 = __pyx_v_current_set; __Pyx_INCREF(__pyx_t_3);
      __pyx_t_4 = 0;
      __pyx_t_12 = NULL;
    } else {
      __pyx_t_4 = -1; __pyx_t_3 = PyObject_GetIter(__pyx_v_current_set); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 63, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_3);
      __pyx_t_12 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_3); if (unlikely(!__pyx_t_12)) __PYX_ERR(0, 63, __pyx_L1_error)
    }
    for (;;) {
      if (likely(!__pyx_t_12)) {
        if (likely(PyList_CheckExact(__pyx_t_3))) {
          {
            Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_t_3);
            #if !CYTHON_ASSUME_SAFE_SIZE
            if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 63, __pyx_L1_error)
            #endif
            if (__pyx_t_4 >= __pyx_temp) break;
          }
          __pyx_t_6 = __Pyx_PyList_GetItemRefFast(__pyx_t_3, __pyx_t_4, __Pyx_ReferenceSharing_OwnStrongReference);
          ++__pyx_t_4;
        } else {
          {
            Py_ssize_t __pyx_temp = __Pyx_PyTuple_GET_SIZE(__pyx_t_3);
            #if !CYTHON_ASSUME_SAFE_SIZE
            if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 63, __pyx_L1_error)
            #endif
            if (__pyx_t_4 >= __pyx_temp) break;
          }
          #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
          __pyx_t_6 = __Pyx_NewRef(PyTuple_GET_ITEM(__pyx_t_3, __pyx_t_4));
          #else
          __pyx_t_6 = __Pyx_PySequence_ITEM(__pyx_t_3, __pyx_t_4);
          #endif
          ++__pyx_t_4;
        }
        if (unlikely(!__pyx_t_6)) __PYX_ERR(0, 63, __pyx_L1_error)
      } else {
        __pyx_t_6 = __pyx_t_12(__pyx_t_3);
        if (unlikely(!__pyx_t_6)) {
          PyObject* exc_type = PyErr_Occurred();
          if (exc_type) {
            if (unlikely(!__Pyx_PyErr_GivenExceptionMatches(exc_type, PyExc_StopIteration))) __PYX_ERR(0, 63, __pyx_L1_error)
            PyErr_Clear();
          }
          break;
        }
      }
      __Pyx_GOTREF(__pyx_t_6);
      __Pyx_XDECREF_SET(__pyx_v_current_int, __pyx_t_6);
      __pyx_t_6 = 0;

      if (__pyx_v_is_forward) {

        __pyx_t_7 = __pyx_v_cg_out;
        __Pyx_INCREF(__pyx_t_7);
        __pyx_t_5 = PyTuple_New(2); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 65, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_5);
        __Pyx_INCREF(__pyx_v_current_int);
        __Pyx_GIVEREF(__pyx_v_current_int);
        if (__Pyx_PyTuple_SET_ITEM(__pyx_t_5, 0, __pyx_v_current_int) != (0)) __PYX_ERR(0, 65, __pyx_L1_error);
        __Pyx_INCREF(__pyx_v_edge_label);
        __Pyx_GIVEREF(__pyx_v_edge_label);
        if (__Pyx_PyTuple_SET_ITEM(__pyx_t_5, 1, __pyx_v_edge_label) != (0)) __PYX_ERR(0, 65, __pyx_L1_error);
        __pyx_t_13 = 0;
        {
          PyObject *__pyx_callargs[2] = {__pyx_t_7, __pyx_t_5};
          __pyx_t_6 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_get, __pyx_callargs+__pyx_t_13, (2-__pyx_t_13) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
          __Pyx_XDECREF(__pyx_t_7); __pyx_t_7 = 0;
          __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
          if (unlikely(!__pyx_t_6)) __PYX_ERR(0, 65, __pyx_L1_error)
          __Pyx_GOTREF(__pyx_t_6);
        }
        __Pyx_XDECREF_SET(__pyx_v_neighbors, __pyx_t_6);
        __pyx_t_6 = 0;

        goto __pyx_L10;
      }

       {
        __pyx_t_5 = __pyx_v_cg_in;
        __Pyx_INCREF(__pyx_t_5);
        __pyx_t_7 = PyTuple_New(2); if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 67, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_7);
        __Pyx_INCREF(__pyx_v_current_int);
        __Pyx_GIVEREF(__pyx_v_current_int);
        if (__Pyx_PyTuple_SET_ITEM(__pyx_t_7, 0, __pyx_v_current_int) != (0)) __PYX_ERR(0, 67, __pyx_L1_error);
        __Pyx_INCREF(__pyx_v_edge_label);
        __Pyx_GIVEREF(__pyx_v_edge_label);
        if (__Pyx_PyTuple_SET_ITEM(__pyx_t_7, 1, __pyx_v_edge_label) != (0)) __PYX_ERR(0, 67, __pyx_L1_error);
        __pyx_t_13 = 0;
        {
          PyObject *__pyx_callargs[2] = {__pyx_t_5, __pyx_t_7};
          __pyx_t_6 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_get, __pyx_callargs+__pyx_t_13, (2-__pyx_t_13) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
          __Pyx_XDECREF(__pyx_t_5); __pyx_t_5 = 0;
          __Pyx_DECREF(__pyx_t_7); __pyx_t_7 = 0;
          if (unlikely(!__pyx_t_6)) __PYX_ERR(0, 67, __pyx_L1_error)
          __Pyx_GOTREF(__pyx_t_6);
        }
        __Pyx_XDECREF_SET(__pyx_v_neighbors, __pyx_t_6);
        __pyx_t_6 = 0;
      }
      __pyx_L10:;

      __pyx_t_2 = (__pyx_v_neighbors == Py_None);
      if (__pyx_t_2) {

        goto __pyx_L8_continue;

      }

      if (likely(PyList_CheckExact(__pyx_v_neighbors)) || PyTuple_CheckExact(__pyx_v_neighbors)) {
        __pyx_t_6 = __pyx_v_neighbors; __Pyx_INCREF(__pyx_t_6);
        __pyx_t_14 = 0;
        __pyx_t_15 = NULL;
      } else {
        __pyx_t_14 = -1; __pyx_t_6 = PyObject_GetIter(__pyx_v_neighbors); if (unlikely(!__pyx_t_6)) __PYX_ERR(0, 72, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_6);
        __pyx_t_15 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_6); if (unlikely(!__pyx_t_15)) __PYX_ERR(0, 72, __pyx_L1_error)
      }
      for (;;) {
        if (likely(!__pyx_t_15)) {
          if (likely(PyList_CheckExact(__pyx_t_6))) {
            {
              Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_t_6);
              #if !CYTHON_ASSUME_SAFE_SIZE
              if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 72, __pyx_L1_error)
              #endif
              if (__pyx_t_14 >= __pyx_temp) break;
            }
            __pyx_t_7 = __Pyx_PyList_GetItemRefFast(__pyx_t_6, __pyx_t_14, __Pyx_ReferenceSharing_OwnStrongReference);
            ++__pyx_t_14;
          } else {
            {
              Py_ssize_t __pyx_temp = __Pyx_PyTuple_GET_SIZE(__pyx_t_6);
              #if !CYTHON_ASSUME_SAFE_SIZE
              if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 72, __pyx_L1_error)
              #endif
              if (__pyx_t_14 >= __pyx_temp) break;
            }
            #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
            __pyx_t_7 = __Pyx_NewRef(PyTuple_GET_ITEM(__pyx_t_6, __pyx_t_14));
            #else
            __pyx_t_7 = __Pyx_PySequence_ITEM(__pyx_t_6, __pyx_t_14);
            #endif
            ++__pyx_t_14;
          }
          if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 72, __pyx_L1_error)
        } else {
          __pyx_t_7 = __pyx_t_15(__pyx_t_6);
          if (unlikely(!__pyx_t_7)) {
            PyObject* exc_type = PyErr_Occurred();
            if (exc_type) {
              if (unlikely(!__Pyx_PyErr_GivenExceptionMatches(exc_type, PyExc_StopIteration))) __PYX_ERR(0, 72, __pyx_L1_error)
              PyErr_Clear();
            }
            break;
          }
        }
        __Pyx_GOTREF(__pyx_t_7);
        __pyx_t_16 = __Pyx_PyLong_As_int(__pyx_t_7); if (unlikely((__pyx_t_16 == (int)-1) && PyErr_Occurred())) __PYX_ERR(0, 72, __pyx_L1_error)
        __Pyx_DECREF(__pyx_t_7); __pyx_t_7 = 0;
        __pyx_v_n_id = __pyx_t_16;

        if (__pyx_v_do_label_check) {
        } else {
          __pyx_t_2 = __pyx_v_do_label_check;
          goto __pyx_L15_bool_binop_done;
        }
        __pyx_t_7 = __Pyx_GetItemInt(__pyx_v_cg_labels, __pyx_v_n_id, int, 1, __Pyx_PyLong_From_int, 0, 0, 0, 1, __Pyx_ReferenceSharing_OwnStrongReference); if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 74, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_7);
        __pyx_t_5 = PyObject_RichCompare(__pyx_t_7, __pyx_v_expected_label, Py_NE); __Pyx_XGOTREF(__pyx_t_5); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 74, __pyx_L1_error)
        __Pyx_DECREF(__pyx_t_7); __pyx_t_7 = 0;
        __pyx_t_1 = __Pyx_PyObject_IsTrue(__pyx_t_5); if (unlikely((__pyx_t_1 < 0))) __PYX_ERR(0, 74, __pyx_L1_error)
        __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
        __pyx_t_2 = __pyx_t_1;
        __pyx_L15_bool_binop_done:;
        if (__pyx_t_2) {

          goto __pyx_L12_continue;

        }

        if (__pyx_v_has_prefilter) {

          __pyx_t_5 = __Pyx_PyLong_From_int(__pyx_v_n_id); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 79, __pyx_L1_error)
          __Pyx_GOTREF(__pyx_t_5);
          __pyx_t_2 = (__Pyx_PySequence_ContainsTF(__pyx_t_5, __pyx_v_prefilter, Py_NE)); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 79, __pyx_L1_error)
          __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
          if (__pyx_t_2) {

            goto __pyx_L12_continue;

          }

          goto __pyx_L17;
        }

        if (__pyx_v_has_pw_eval) {

          __pyx_t_7 = NULL;
          __Pyx_INCREF(__pyx_v_pw_eval_fn);
          __pyx_t_17 = __pyx_v_pw_eval_fn;
          __pyx_t_18 = __Pyx_PyLong_From_int(__pyx_v_n_id); if (unlikely(!__pyx_t_18)) __PYX_ERR(0, 82, __pyx_L1_error)
          __Pyx_GOTREF(__pyx_t_18);
          __pyx_t_13 = 1;
          #if CYTHON_UNPACK_METHODS
          if (unlikely(PyMethod_Check(__pyx_t_17))) {
            __pyx_t_7 = PyMethod_GET_SELF(__pyx_t_17);
            assert(__pyx_t_7);
            PyObject* __pyx__function = PyMethod_GET_FUNCTION(__pyx_t_17);
            __Pyx_INCREF(__pyx_t_7);
            __Pyx_INCREF(__pyx__function);
            __Pyx_DECREF_SET(__pyx_t_17, __pyx__function);
            __pyx_t_13 = 0;
          }
          #endif
          {
            PyObject *__pyx_callargs[3] = {__pyx_t_7, __pyx_v_next_var, __pyx_t_18};
            __pyx_t_5 = __Pyx_PyObject_FastCall((PyObject*)__pyx_t_17, __pyx_callargs+__pyx_t_13, (3-__pyx_t_13) | (__pyx_t_13*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
            __Pyx_XDECREF(__pyx_t_7); __pyx_t_7 = 0;
            __Pyx_DECREF(__pyx_t_18); __pyx_t_18 = 0;
            __Pyx_DECREF(__pyx_t_17); __pyx_t_17 = 0;
            if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 82, __pyx_L1_error)
            __Pyx_GOTREF(__pyx_t_5);
          }
          __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_t_5); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 82, __pyx_L1_error)
          __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
          __pyx_t_1 = (!__pyx_t_2);
          if (__pyx_t_1) {

            goto __pyx_L12_continue;

          }

        }
        __pyx_L17:;

        __pyx_t_5 = __Pyx_PyLong_From_int(__pyx_v_n_id); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 85, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_5);
        __pyx_t_19 = PySet_Add(__pyx_v_next_set, __pyx_t_5); if (unlikely(__pyx_t_19 == ((int)-1))) __PYX_ERR(0, 85, __pyx_L1_error)
        __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;

        __pyx_L12_continue:;
      }
      __Pyx_DECREF(__pyx_t_6); __pyx_t_6 = 0;

      __pyx_L8_continue:;
    }
    __Pyx_DECREF(__pyx_t_3); __pyx_t_3 = 0;

    {
      Py_ssize_t __pyx_temp = __Pyx_PySet_GET_SIZE(__pyx_v_next_set);
      if (unlikely(((!CYTHON_ASSUME_SAFE_SIZE) && __pyx_temp < 0))) __PYX_ERR(0, 87, __pyx_L1_error)
      __pyx_t_1 = (__pyx_temp != 0);
    }

    __pyx_t_2 = (!__pyx_t_1);
    if (__pyx_t_2) {

      __Pyx_XDECREF(__pyx_r);
      __pyx_r = Py_None; __Pyx_INCREF(Py_None);
      goto __pyx_L0;

    }

    if (unlikely((PyDict_SetItem(__pyx_v_result, __pyx_v_next_var, __pyx_v_next_set) < 0))) __PYX_ERR(0, 90, __pyx_L1_error)
  }

  __Pyx_XDECREF(__pyx_r);
  __Pyx_INCREF(__pyx_v_result);
  __pyx_r = __pyx_v_result;
  goto __pyx_L0;

  __pyx_L1_error:;
  __Pyx_XDECREF(__pyx_t_3);
  __Pyx_XDECREF(__pyx_t_5);
  __Pyx_XDECREF(__pyx_t_6);
  __Pyx_XDECREF(__pyx_t_7);
  __Pyx_XDECREF(__pyx_t_17);
  __Pyx_XDECREF(__pyx_t_18);
  __Pyx_AddTraceback("_fast_match._match_path_compact_impl", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __pyx_r = 0;
  __pyx_L0:;
  __Pyx_XDECREF(__pyx_v_result);
  __Pyx_XDECREF(__pyx_v_cg_out);
  __Pyx_XDECREF(__pyx_v_cg_in);
  __Pyx_XDECREF(__pyx_v_cg_labels);
  __Pyx_XDECREF(__pyx_v_current_var);
  __Pyx_XDECREF(__pyx_v_next_var);
  __Pyx_XDECREF(__pyx_v_edge_info);
  __Pyx_XDECREF(__pyx_v_edge_label);
  __Pyx_XDECREF(__pyx_v_expected_label);
  __Pyx_XDECREF(__pyx_v_current_set);
  __Pyx_XDECREF(__pyx_v_next_set);
  __Pyx_XDECREF(__pyx_v_prefilter);
  __Pyx_XDECREF(__pyx_v_current_int);
  __Pyx_XDECREF(__pyx_v_neighbors);
  __Pyx_XGIVEREF(__pyx_r);
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}

static PyObject *__pyx_pw_11_fast_match_1match_path_compact_fast(PyObject *__pyx_self,
#if CYTHON_METH_FASTCALL
PyObject *const *__pyx_args, Py_ssize_t __pyx_nargs, PyObject *__pyx_kwds
#else
PyObject *__pyx_args, PyObject *__pyx_kwds
#endif
);
PyDoc_STRVAR(__pyx_doc_11_fast_match_match_path_compact_fast, "\n    \347\264\247\345\207\221\350\267\257\345\276\204\345\214\271\351\205\215\342\200\224\342\200\224\346\240\270\345\277\203\345\206\205\345\276\252\347\216\257\347\224\250 C \347\272\247\346\223\215\344\275\234\343\200\202\n\n    Returns:\n        {var_name: set_of_int_ids} or None if match fails.\n    ");
static PyMethodDef __pyx_mdef_11_fast_match_1match_path_compact_fast = {"match_path_compact_fast", (PyCFunction)(void(*)(void))(__Pyx_PyCFunction_FastCallWithKeywords)__pyx_pw_11_fast_match_1match_path_compact_fast, __Pyx_METH_FASTCALL|METH_KEYWORDS, __pyx_doc_11_fast_match_match_path_compact_fast};
static PyObject *__pyx_pw_11_fast_match_1match_path_compact_fast(PyObject *__pyx_self,
#if CYTHON_METH_FASTCALL
PyObject *const *__pyx_args, Py_ssize_t __pyx_nargs, PyObject *__pyx_kwds
#else
PyObject *__pyx_args, PyObject *__pyx_kwds
#endif
) {
  PyObject *__pyx_v_compact_graph = 0;
  PyObject *__pyx_v_path_sequence = 0;
  int __pyx_v_pivot_int;
  PyObject *__pyx_v_edge_lookup = 0;
  PyObject *__pyx_v_node_label_map = 0;
  PyObject *__pyx_v_skip_label_checks = 0;
  PyObject *__pyx_v_prefilter_sets = 0;
  PyObject *__pyx_v_pw_eval_fn = 0;
  #if !CYTHON_METH_FASTCALL
  CYTHON_UNUSED Py_ssize_t __pyx_nargs;
  #endif
  CYTHON_UNUSED PyObject *const *__pyx_kwvalues;
  PyObject* values[8] = {0,0,0,0,0,0,0,0};
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  PyObject *__pyx_r = 0;
  __Pyx_RefNannyDeclarations
  __Pyx_RefNannySetupContext("match_path_compact_fast (wrapper)", 0);
  #if !CYTHON_METH_FASTCALL
  #if CYTHON_ASSUME_SAFE_SIZE
  __pyx_nargs = PyTuple_GET_SIZE(__pyx_args);
  #else
  __pyx_nargs = PyTuple_Size(__pyx_args); if (unlikely(__pyx_nargs < 0)) return NULL;
  #endif
  #endif
  __pyx_kwvalues = __Pyx_KwValues_FASTCALL(__pyx_args, __pyx_nargs);
  {
    PyObject ** const __pyx_pyargnames[] = {&__pyx_mstate_global->__pyx_n_u_compact_graph,&__pyx_mstate_global->__pyx_n_u_path_sequence,&__pyx_mstate_global->__pyx_n_u_pivot_int,&__pyx_mstate_global->__pyx_n_u_edge_lookup,&__pyx_mstate_global->__pyx_n_u_node_label_map,&__pyx_mstate_global->__pyx_n_u_skip_label_checks,&__pyx_mstate_global->__pyx_n_u_prefilter_sets,&__pyx_mstate_global->__pyx_n_u_pw_eval_fn,0};
    const Py_ssize_t __pyx_kwds_len = (__pyx_kwds) ? __Pyx_NumKwargs_FASTCALL(__pyx_kwds) : 0;
    if (unlikely(__pyx_kwds_len) < 0) __PYX_ERR(0, 95, __pyx_L3_error)
    if (__pyx_kwds_len > 0) {
      switch (__pyx_nargs) {
        case  8:
        values[7] = __Pyx_ArgRef_FASTCALL(__pyx_args, 7);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[7])) __PYX_ERR(0, 95, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  7:
        values[6] = __Pyx_ArgRef_FASTCALL(__pyx_args, 6);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[6])) __PYX_ERR(0, 95, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  6:
        values[5] = __Pyx_ArgRef_FASTCALL(__pyx_args, 5);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[5])) __PYX_ERR(0, 95, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  5:
        values[4] = __Pyx_ArgRef_FASTCALL(__pyx_args, 4);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[4])) __PYX_ERR(0, 95, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  4:
        values[3] = __Pyx_ArgRef_FASTCALL(__pyx_args, 3);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[3])) __PYX_ERR(0, 95, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  3:
        values[2] = __Pyx_ArgRef_FASTCALL(__pyx_args, 2);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[2])) __PYX_ERR(0, 95, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  2:
        values[1] = __Pyx_ArgRef_FASTCALL(__pyx_args, 1);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[1])) __PYX_ERR(0, 95, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  1:
        values[0] = __Pyx_ArgRef_FASTCALL(__pyx_args, 0);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[0])) __PYX_ERR(0, 95, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  0: break;
        default: goto __pyx_L5_argtuple_error;
      }
      const Py_ssize_t kwd_pos_args = __pyx_nargs;
      if (__Pyx_ParseKeywords(__pyx_kwds, __pyx_kwvalues, __pyx_pyargnames, 0, values, kwd_pos_args, __pyx_kwds_len, "match_path_compact_fast", 0) < (0)) __PYX_ERR(0, 95, __pyx_L3_error)

      if (!values[7]) values[7] = __Pyx_NewRef(((PyObject *)Py_None));
      for (Py_ssize_t i = __pyx_nargs; i < 7; i++) {
        if (unlikely(!values[i])) { __Pyx_RaiseArgtupleInvalid("match_path_compact_fast", 0, 7, 8, i); __PYX_ERR(0, 95, __pyx_L3_error) }
      }
    } else {
      switch (__pyx_nargs) {
        case  8:
        values[7] = __Pyx_ArgRef_FASTCALL(__pyx_args, 7);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[7])) __PYX_ERR(0, 95, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  7:
        values[6] = __Pyx_ArgRef_FASTCALL(__pyx_args, 6);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[6])) __PYX_ERR(0, 95, __pyx_L3_error)
        values[5] = __Pyx_ArgRef_FASTCALL(__pyx_args, 5);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[5])) __PYX_ERR(0, 95, __pyx_L3_error)
        values[4] = __Pyx_ArgRef_FASTCALL(__pyx_args, 4);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[4])) __PYX_ERR(0, 95, __pyx_L3_error)
        values[3] = __Pyx_ArgRef_FASTCALL(__pyx_args, 3);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[3])) __PYX_ERR(0, 95, __pyx_L3_error)
        values[2] = __Pyx_ArgRef_FASTCALL(__pyx_args, 2);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[2])) __PYX_ERR(0, 95, __pyx_L3_error)
        values[1] = __Pyx_ArgRef_FASTCALL(__pyx_args, 1);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[1])) __PYX_ERR(0, 95, __pyx_L3_error)
        values[0] = __Pyx_ArgRef_FASTCALL(__pyx_args, 0);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[0])) __PYX_ERR(0, 95, __pyx_L3_error)
        break;
        default: goto __pyx_L5_argtuple_error;
      }
      if (!values[7]) values[7] = __Pyx_NewRef(((PyObject *)Py_None));
    }
    __pyx_v_compact_graph = values[0];
    __pyx_v_path_sequence = values[1];
    __pyx_v_pivot_int = __Pyx_PyLong_As_int(values[2]); if (unlikely((__pyx_v_pivot_int == (int)-1) && PyErr_Occurred())) __PYX_ERR(0, 98, __pyx_L3_error)
    __pyx_v_edge_lookup = ((PyObject*)values[3]);
    __pyx_v_node_label_map = ((PyObject*)values[4]);
    __pyx_v_skip_label_checks = ((PyObject*)values[5]);
    __pyx_v_prefilter_sets = ((PyObject*)values[6]);
    __pyx_v_pw_eval_fn = values[7];
  }
  goto __pyx_L6_skip;
  __pyx_L5_argtuple_error:;
  __Pyx_RaiseArgtupleInvalid("match_path_compact_fast", 0, 7, 8, __pyx_nargs); __PYX_ERR(0, 95, __pyx_L3_error)
  __pyx_L6_skip:;
  goto __pyx_L4_argument_unpacking_done;
  __pyx_L3_error:;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  __Pyx_AddTraceback("_fast_match.match_path_compact_fast", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __Pyx_RefNannyFinishContext();
  return NULL;
  __pyx_L4_argument_unpacking_done:;
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_edge_lookup), (&PyDict_Type), 1, "edge_lookup", 1))) __PYX_ERR(0, 99, __pyx_L1_error)
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_node_label_map), (&PyDict_Type), 1, "node_label_map", 1))) __PYX_ERR(0, 100, __pyx_L1_error)
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_skip_label_checks), (&PyDict_Type), 1, "skip_label_checks", 1))) __PYX_ERR(0, 101, __pyx_L1_error)
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_prefilter_sets), (&PyDict_Type), 1, "prefilter_sets", 1))) __PYX_ERR(0, 102, __pyx_L1_error)
  __pyx_r = __pyx_pf_11_fast_match_match_path_compact_fast(__pyx_self, __pyx_v_compact_graph, __pyx_v_path_sequence, __pyx_v_pivot_int, __pyx_v_edge_lookup, __pyx_v_node_label_map, __pyx_v_skip_label_checks, __pyx_v_prefilter_sets, __pyx_v_pw_eval_fn);

  goto __pyx_L0;
  __pyx_L1_error:;
  __pyx_r = NULL;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  goto __pyx_L7_cleaned_up;
  __pyx_L0:;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  __pyx_L7_cleaned_up:;
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}

static PyObject *__pyx_pf_11_fast_match_match_path_compact_fast(CYTHON_UNUSED PyObject *__pyx_self, PyObject *__pyx_v_compact_graph, PyObject *__pyx_v_path_sequence, int __pyx_v_pivot_int, PyObject *__pyx_v_edge_lookup, PyObject *__pyx_v_node_label_map, PyObject *__pyx_v_skip_label_checks, PyObject *__pyx_v_prefilter_sets, PyObject *__pyx_v_pw_eval_fn) {
  PyObject *__pyx_r = NULL;
  __Pyx_RefNannyDeclarations
  PyObject *__pyx_t_1 = NULL;
  struct __pyx_opt_args_11_fast_match__match_path_compact_impl __pyx_t_2;
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  __Pyx_RefNannySetupContext("match_path_compact_fast", 0);

  __Pyx_XDECREF(__pyx_r);

  __pyx_t_2.__pyx_n = 1;
  __pyx_t_2.pw_eval_fn = __pyx_v_pw_eval_fn;
  __pyx_t_1 = __pyx_f_11_fast_match__match_path_compact_impl(__pyx_v_compact_graph, __pyx_v_path_sequence, __pyx_v_pivot_int, __pyx_v_edge_lookup, __pyx_v_node_label_map, __pyx_v_skip_label_checks, __pyx_v_prefilter_sets, &__pyx_t_2); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 111, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_1);
  __pyx_r = __pyx_t_1;
  __pyx_t_1 = 0;
  goto __pyx_L0;

  __pyx_L1_error:;
  __Pyx_XDECREF(__pyx_t_1);
  __Pyx_AddTraceback("_fast_match.match_path_compact_fast", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __pyx_r = NULL;
  __pyx_L0:;
  __Pyx_XGIVEREF(__pyx_r);
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}

static PyObject *__pyx_pw_11_fast_match_3match_star_paths_batch_fast(PyObject *__pyx_self,
#if CYTHON_METH_FASTCALL
PyObject *const *__pyx_args, Py_ssize_t __pyx_nargs, PyObject *__pyx_kwds
#else
PyObject *__pyx_args, PyObject *__pyx_kwds
#endif
);
PyDoc_STRVAR(__pyx_doc_11_fast_match_2match_star_paths_batch_fast, "\n    Match and merge all paths of one star from a single pivot in int space.\n\n    fast_path_plans entries are:\n      (seq, edge_lookup, node_label_map, skip_checks, needs_pw_eval)\n    ");
static PyMethodDef __pyx_mdef_11_fast_match_3match_star_paths_batch_fast = {"match_star_paths_batch_fast", (PyCFunction)(void(*)(void))(__Pyx_PyCFunction_FastCallWithKeywords)__pyx_pw_11_fast_match_3match_star_paths_batch_fast, __Pyx_METH_FASTCALL|METH_KEYWORDS, __pyx_doc_11_fast_match_2match_star_paths_batch_fast};
static PyObject *__pyx_pw_11_fast_match_3match_star_paths_batch_fast(PyObject *__pyx_self,
#if CYTHON_METH_FASTCALL
PyObject *const *__pyx_args, Py_ssize_t __pyx_nargs, PyObject *__pyx_kwds
#else
PyObject *__pyx_args, PyObject *__pyx_kwds
#endif
) {
  PyObject *__pyx_v_compact_graph = 0;
  PyObject *__pyx_v_fast_path_plans = 0;
  int __pyx_v_pivot_int;
  PyObject *__pyx_v_int_prefilter = 0;
  PyObject *__pyx_v_pw_eval_fn = 0;
  #if !CYTHON_METH_FASTCALL
  CYTHON_UNUSED Py_ssize_t __pyx_nargs;
  #endif
  CYTHON_UNUSED PyObject *const *__pyx_kwvalues;
  PyObject* values[5] = {0,0,0,0,0};
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  PyObject *__pyx_r = 0;
  __Pyx_RefNannyDeclarations
  __Pyx_RefNannySetupContext("match_star_paths_batch_fast (wrapper)", 0);
  #if !CYTHON_METH_FASTCALL
  #if CYTHON_ASSUME_SAFE_SIZE
  __pyx_nargs = PyTuple_GET_SIZE(__pyx_args);
  #else
  __pyx_nargs = PyTuple_Size(__pyx_args); if (unlikely(__pyx_nargs < 0)) return NULL;
  #endif
  #endif
  __pyx_kwvalues = __Pyx_KwValues_FASTCALL(__pyx_args, __pyx_nargs);
  {
    PyObject ** const __pyx_pyargnames[] = {&__pyx_mstate_global->__pyx_n_u_compact_graph,&__pyx_mstate_global->__pyx_n_u_fast_path_plans,&__pyx_mstate_global->__pyx_n_u_pivot_int,&__pyx_mstate_global->__pyx_n_u_int_prefilter,&__pyx_mstate_global->__pyx_n_u_pw_eval_fn,0};
    const Py_ssize_t __pyx_kwds_len = (__pyx_kwds) ? __Pyx_NumKwargs_FASTCALL(__pyx_kwds) : 0;
    if (unlikely(__pyx_kwds_len) < 0) __PYX_ERR(0, 117, __pyx_L3_error)
    if (__pyx_kwds_len > 0) {
      switch (__pyx_nargs) {
        case  5:
        values[4] = __Pyx_ArgRef_FASTCALL(__pyx_args, 4);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[4])) __PYX_ERR(0, 117, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  4:
        values[3] = __Pyx_ArgRef_FASTCALL(__pyx_args, 3);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[3])) __PYX_ERR(0, 117, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  3:
        values[2] = __Pyx_ArgRef_FASTCALL(__pyx_args, 2);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[2])) __PYX_ERR(0, 117, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  2:
        values[1] = __Pyx_ArgRef_FASTCALL(__pyx_args, 1);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[1])) __PYX_ERR(0, 117, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  1:
        values[0] = __Pyx_ArgRef_FASTCALL(__pyx_args, 0);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[0])) __PYX_ERR(0, 117, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  0: break;
        default: goto __pyx_L5_argtuple_error;
      }
      const Py_ssize_t kwd_pos_args = __pyx_nargs;
      if (__Pyx_ParseKeywords(__pyx_kwds, __pyx_kwvalues, __pyx_pyargnames, 0, values, kwd_pos_args, __pyx_kwds_len, "match_star_paths_batch_fast", 0) < (0)) __PYX_ERR(0, 117, __pyx_L3_error)

      if (!values[4]) values[4] = __Pyx_NewRef(((PyObject *)Py_None));
      for (Py_ssize_t i = __pyx_nargs; i < 4; i++) {
        if (unlikely(!values[i])) { __Pyx_RaiseArgtupleInvalid("match_star_paths_batch_fast", 0, 4, 5, i); __PYX_ERR(0, 117, __pyx_L3_error) }
      }
    } else {
      switch (__pyx_nargs) {
        case  5:
        values[4] = __Pyx_ArgRef_FASTCALL(__pyx_args, 4);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[4])) __PYX_ERR(0, 117, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  4:
        values[3] = __Pyx_ArgRef_FASTCALL(__pyx_args, 3);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[3])) __PYX_ERR(0, 117, __pyx_L3_error)
        values[2] = __Pyx_ArgRef_FASTCALL(__pyx_args, 2);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[2])) __PYX_ERR(0, 117, __pyx_L3_error)
        values[1] = __Pyx_ArgRef_FASTCALL(__pyx_args, 1);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[1])) __PYX_ERR(0, 117, __pyx_L3_error)
        values[0] = __Pyx_ArgRef_FASTCALL(__pyx_args, 0);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[0])) __PYX_ERR(0, 117, __pyx_L3_error)
        break;
        default: goto __pyx_L5_argtuple_error;
      }
      if (!values[4]) values[4] = __Pyx_NewRef(((PyObject *)Py_None));
    }
    __pyx_v_compact_graph = values[0];
    __pyx_v_fast_path_plans = values[1];
    __pyx_v_pivot_int = __Pyx_PyLong_As_int(values[2]); if (unlikely((__pyx_v_pivot_int == (int)-1) && PyErr_Occurred())) __PYX_ERR(0, 120, __pyx_L3_error)
    __pyx_v_int_prefilter = ((PyObject*)values[3]);
    __pyx_v_pw_eval_fn = values[4];
  }
  goto __pyx_L6_skip;
  __pyx_L5_argtuple_error:;
  __Pyx_RaiseArgtupleInvalid("match_star_paths_batch_fast", 0, 4, 5, __pyx_nargs); __PYX_ERR(0, 117, __pyx_L3_error)
  __pyx_L6_skip:;
  goto __pyx_L4_argument_unpacking_done;
  __pyx_L3_error:;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  __Pyx_AddTraceback("_fast_match.match_star_paths_batch_fast", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __Pyx_RefNannyFinishContext();
  return NULL;
  __pyx_L4_argument_unpacking_done:;
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_int_prefilter), (&PyDict_Type), 1, "int_prefilter", 1))) __PYX_ERR(0, 121, __pyx_L1_error)
  __pyx_r = __pyx_pf_11_fast_match_2match_star_paths_batch_fast(__pyx_self, __pyx_v_compact_graph, __pyx_v_fast_path_plans, __pyx_v_pivot_int, __pyx_v_int_prefilter, __pyx_v_pw_eval_fn);

  goto __pyx_L0;
  __pyx_L1_error:;
  __pyx_r = NULL;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  goto __pyx_L7_cleaned_up;
  __pyx_L0:;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  __pyx_L7_cleaned_up:;
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}

static PyObject *__pyx_pf_11_fast_match_2match_star_paths_batch_fast(CYTHON_UNUSED PyObject *__pyx_self, PyObject *__pyx_v_compact_graph, PyObject *__pyx_v_fast_path_plans, int __pyx_v_pivot_int, PyObject *__pyx_v_int_prefilter, PyObject *__pyx_v_pw_eval_fn) {
  PyObject *__pyx_v_merged = 0;
  PyObject *__pyx_v_seq = 0;
  PyObject *__pyx_v_el = 0;
  PyObject *__pyx_v_node_label_map = 0;
  PyObject *__pyx_v_skip_checks = 0;
  PyObject *__pyx_v_path_result = 0;
  int __pyx_v_needs_pw_eval;
  PyObject *__pyx_v_var = NULL;
  PyObject *__pyx_v_int_set = NULL;
  PyObject *__pyx_r = NULL;
  __Pyx_RefNannyDeclarations
  PyObject *__pyx_t_1 = NULL;
  Py_ssize_t __pyx_t_2;
  PyObject *(*__pyx_t_3)(PyObject *);
  PyObject *__pyx_t_4 = NULL;
  PyObject *__pyx_t_5 = NULL;
  PyObject *__pyx_t_6 = NULL;
  PyObject *__pyx_t_7 = NULL;
  PyObject *__pyx_t_8 = NULL;
  PyObject *__pyx_t_9 = NULL;
  PyObject *__pyx_t_10 = NULL;
  PyObject *(*__pyx_t_11)(PyObject *);
  int __pyx_t_12;
  struct __pyx_opt_args_11_fast_match__match_path_compact_impl __pyx_t_13;
  Py_ssize_t __pyx_t_14;
  Py_ssize_t __pyx_t_15;
  int __pyx_t_16;
  int __pyx_t_17;
  int __pyx_t_18;
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  __Pyx_RefNannySetupContext("match_star_paths_batch_fast", 0);

  __pyx_t_1 = __Pyx_PyDict_NewPresized(0); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 130, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_1);
  __pyx_v_merged = __pyx_t_1;
  __pyx_t_1 = 0;

  if (likely(PyList_CheckExact(__pyx_v_fast_path_plans)) || PyTuple_CheckExact(__pyx_v_fast_path_plans)) {
    __pyx_t_1 = __pyx_v_fast_path_plans; __Pyx_INCREF(__pyx_t_1);
    __pyx_t_2 = 0;
    __pyx_t_3 = NULL;
  } else {
    __pyx_t_2 = -1; __pyx_t_1 = PyObject_GetIter(__pyx_v_fast_path_plans); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 134, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_1);
    __pyx_t_3 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_1); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 134, __pyx_L1_error)
  }
  for (;;) {
    if (likely(!__pyx_t_3)) {
      if (likely(PyList_CheckExact(__pyx_t_1))) {
        {
          Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_t_1);
          #if !CYTHON_ASSUME_SAFE_SIZE
          if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 134, __pyx_L1_error)
          #endif
          if (__pyx_t_2 >= __pyx_temp) break;
        }
        __pyx_t_4 = __Pyx_PyList_GetItemRefFast(__pyx_t_1, __pyx_t_2, __Pyx_ReferenceSharing_OwnStrongReference);
        ++__pyx_t_2;
      } else {
        {
          Py_ssize_t __pyx_temp = __Pyx_PyTuple_GET_SIZE(__pyx_t_1);
          #if !CYTHON_ASSUME_SAFE_SIZE
          if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 134, __pyx_L1_error)
          #endif
          if (__pyx_t_2 >= __pyx_temp) break;
        }
        #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
        __pyx_t_4 = __Pyx_NewRef(PyTuple_GET_ITEM(__pyx_t_1, __pyx_t_2));
        #else
        __pyx_t_4 = __Pyx_PySequence_ITEM(__pyx_t_1, __pyx_t_2);
        #endif
        ++__pyx_t_2;
      }
      if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 134, __pyx_L1_error)
    } else {
      __pyx_t_4 = __pyx_t_3(__pyx_t_1);
      if (unlikely(!__pyx_t_4)) {
        PyObject* exc_type = PyErr_Occurred();
        if (exc_type) {
          if (unlikely(!__Pyx_PyErr_GivenExceptionMatches(exc_type, PyExc_StopIteration))) __PYX_ERR(0, 134, __pyx_L1_error)
          PyErr_Clear();
        }
        break;
      }
    }
    __Pyx_GOTREF(__pyx_t_4);
    if ((likely(PyTuple_CheckExact(__pyx_t_4))) || (PyList_CheckExact(__pyx_t_4))) {
      PyObject* sequence = __pyx_t_4;
      Py_ssize_t size = __Pyx_PySequence_SIZE(sequence);
      if (unlikely(size != 5)) {
        if (size > 5) __Pyx_RaiseTooManyValuesError(5);
        else if (size >= 0) __Pyx_RaiseNeedMoreValuesError(size);
        __PYX_ERR(0, 134, __pyx_L1_error)
      }
      #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
      if (likely(PyTuple_CheckExact(sequence))) {
        __pyx_t_5 = PyTuple_GET_ITEM(sequence, 0);
        __Pyx_INCREF(__pyx_t_5);
        __pyx_t_6 = PyTuple_GET_ITEM(sequence, 1);
        __Pyx_INCREF(__pyx_t_6);
        __pyx_t_7 = PyTuple_GET_ITEM(sequence, 2);
        __Pyx_INCREF(__pyx_t_7);
        __pyx_t_8 = PyTuple_GET_ITEM(sequence, 3);
        __Pyx_INCREF(__pyx_t_8);
        __pyx_t_9 = PyTuple_GET_ITEM(sequence, 4);
        __Pyx_INCREF(__pyx_t_9);
      } else {
        __pyx_t_5 = __Pyx_PyList_GetItemRefFast(sequence, 0, __Pyx_ReferenceSharing_SharedReference);
        if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 134, __pyx_L1_error)
        __Pyx_XGOTREF(__pyx_t_5);
        __pyx_t_6 = __Pyx_PyList_GetItemRefFast(sequence, 1, __Pyx_ReferenceSharing_SharedReference);
        if (unlikely(!__pyx_t_6)) __PYX_ERR(0, 134, __pyx_L1_error)
        __Pyx_XGOTREF(__pyx_t_6);
        __pyx_t_7 = __Pyx_PyList_GetItemRefFast(sequence, 2, __Pyx_ReferenceSharing_SharedReference);
        if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 134, __pyx_L1_error)
        __Pyx_XGOTREF(__pyx_t_7);
        __pyx_t_8 = __Pyx_PyList_GetItemRefFast(sequence, 3, __Pyx_ReferenceSharing_SharedReference);
        if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 134, __pyx_L1_error)
        __Pyx_XGOTREF(__pyx_t_8);
        __pyx_t_9 = __Pyx_PyList_GetItemRefFast(sequence, 4, __Pyx_ReferenceSharing_SharedReference);
        if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 134, __pyx_L1_error)
        __Pyx_XGOTREF(__pyx_t_9);
      }
      #else
      {
        Py_ssize_t i;
        PyObject** temps[5] = {&__pyx_t_5,&__pyx_t_6,&__pyx_t_7,&__pyx_t_8,&__pyx_t_9};
        for (i=0; i < 5; i++) {
          PyObject* item = __Pyx_PySequence_ITEM(sequence, i); if (unlikely(!item)) __PYX_ERR(0, 134, __pyx_L1_error)
          __Pyx_GOTREF(item);
          *(temps[i]) = item;
        }
      }
      #endif
      __Pyx_DECREF(__pyx_t_4); __pyx_t_4 = 0;
    } else {
      Py_ssize_t index = -1;
      PyObject** temps[5] = {&__pyx_t_5,&__pyx_t_6,&__pyx_t_7,&__pyx_t_8,&__pyx_t_9};
      __pyx_t_10 = PyObject_GetIter(__pyx_t_4); if (unlikely(!__pyx_t_10)) __PYX_ERR(0, 134, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_10);
      __Pyx_DECREF(__pyx_t_4); __pyx_t_4 = 0;
      __pyx_t_11 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_10);
      for (index=0; index < 5; index++) {
        PyObject* item = __pyx_t_11(__pyx_t_10); if (unlikely(!item)) goto __pyx_L5_unpacking_failed;
        __Pyx_GOTREF(item);
        *(temps[index]) = item;
      }
      if (__Pyx_IternextUnpackEndCheck(__pyx_t_11(__pyx_t_10), 5) < (0)) __PYX_ERR(0, 134, __pyx_L1_error)
      __pyx_t_11 = NULL;
      __Pyx_DECREF(__pyx_t_10); __pyx_t_10 = 0;
      goto __pyx_L6_unpacking_done;
      __pyx_L5_unpacking_failed:;
      __Pyx_DECREF(__pyx_t_10); __pyx_t_10 = 0;
      __pyx_t_11 = NULL;
      if (__Pyx_IterFinish() == 0) __Pyx_RaiseNeedMoreValuesError(index);
      __PYX_ERR(0, 134, __pyx_L1_error)
      __pyx_L6_unpacking_done:;
    }
    __pyx_t_12 = __Pyx_PyObject_IsTrue(__pyx_t_9); if (unlikely((__pyx_t_12 == (int)-1) && PyErr_Occurred())) __PYX_ERR(0, 134, __pyx_L1_error)
    __Pyx_DECREF(__pyx_t_9); __pyx_t_9 = 0;
    __Pyx_XDECREF_SET(__pyx_v_seq, __pyx_t_5);
    __pyx_t_5 = 0;
    __Pyx_XDECREF_SET(__pyx_v_el, __pyx_t_6);
    __pyx_t_6 = 0;
    __Pyx_XDECREF_SET(__pyx_v_node_label_map, __pyx_t_7);
    __pyx_t_7 = 0;
    __Pyx_XDECREF_SET(__pyx_v_skip_checks, __pyx_t_8);
    __pyx_t_8 = 0;
    __pyx_v_needs_pw_eval = __pyx_t_12;

    __pyx_t_4 = __pyx_v_el;
    __Pyx_INCREF(__pyx_t_4);
    if (!(likely(PyDict_CheckExact(__pyx_t_4))||((__pyx_t_4) == Py_None) || __Pyx_RaiseUnexpectedTypeError("dict", __pyx_t_4))) __PYX_ERR(0, 139, __pyx_L1_error)

    __pyx_t_9 = __pyx_v_node_label_map;
    __Pyx_INCREF(__pyx_t_9);
    if (!(likely(PyDict_CheckExact(__pyx_t_9))||((__pyx_t_9) == Py_None) || __Pyx_RaiseUnexpectedTypeError("dict", __pyx_t_9))) __PYX_ERR(0, 140, __pyx_L1_error)

    __pyx_t_8 = __pyx_v_skip_checks;
    __Pyx_INCREF(__pyx_t_8);
    if (!(likely(PyDict_CheckExact(__pyx_t_8))||((__pyx_t_8) == Py_None) || __Pyx_RaiseUnexpectedTypeError("dict", __pyx_t_8))) __PYX_ERR(0, 141, __pyx_L1_error)

    if (__pyx_v_needs_pw_eval) {
      __Pyx_INCREF(__pyx_v_pw_eval_fn);
      __pyx_t_7 = __pyx_v_pw_eval_fn;
    } else {
      __Pyx_INCREF(Py_None);
      __pyx_t_7 = Py_None;
    }

    __pyx_t_13.__pyx_n = 1;
    __pyx_t_13.pw_eval_fn = __pyx_t_7;
    __pyx_t_6 = __pyx_f_11_fast_match__match_path_compact_impl(__pyx_v_compact_graph, __pyx_v_seq, __pyx_v_pivot_int, ((PyObject*)__pyx_t_4), ((PyObject*)__pyx_t_9), ((PyObject*)__pyx_t_8), __pyx_v_int_prefilter, &__pyx_t_13); if (unlikely(!__pyx_t_6)) __PYX_ERR(0, 135, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_6);
    __Pyx_DECREF(__pyx_t_4); __pyx_t_4 = 0;
    __Pyx_DECREF(__pyx_t_9); __pyx_t_9 = 0;
    __Pyx_DECREF(__pyx_t_8); __pyx_t_8 = 0;
    __Pyx_DECREF(__pyx_t_7); __pyx_t_7 = 0;
    __Pyx_XDECREF_SET(__pyx_v_path_result, __pyx_t_6);
    __pyx_t_6 = 0;

    __pyx_t_12 = (__pyx_v_path_result == Py_None);
    if (__pyx_t_12) {

      __Pyx_XDECREF(__pyx_r);
      __pyx_r = Py_None; __Pyx_INCREF(Py_None);
      __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;
      goto __pyx_L0;

    }

    __pyx_t_14 = 0;
    if (unlikely(__pyx_v_path_result == Py_None)) {
      PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "items");
      __PYX_ERR(0, 147, __pyx_L1_error)
    }
    __pyx_t_7 = __Pyx_dict_iterator(__pyx_v_path_result, 0, __pyx_mstate_global->__pyx_n_u_items, (&__pyx_t_15), (&__pyx_t_16)); if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 147, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_7);
    __Pyx_XDECREF(__pyx_t_6);
    __pyx_t_6 = __pyx_t_7;
    __pyx_t_7 = 0;
    while (1) {
      __pyx_t_17 = __Pyx_dict_iter_next(__pyx_t_6, __pyx_t_15, &__pyx_t_14, &__pyx_t_7, &__pyx_t_8, NULL, __pyx_t_16);
      if (unlikely(__pyx_t_17 == 0)) break;
      if (unlikely(__pyx_t_17 == -1)) __PYX_ERR(0, 147, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_7);
      __Pyx_GOTREF(__pyx_t_8);
      __Pyx_XDECREF_SET(__pyx_v_var, __pyx_t_7);
      __pyx_t_7 = 0;
      __Pyx_XDECREF_SET(__pyx_v_int_set, __pyx_t_8);
      __pyx_t_8 = 0;

      __pyx_t_12 = (__Pyx_PySequence_ContainsTF(__pyx_v_var, __pyx_v_merged, Py_EQ)); if (unlikely((__pyx_t_12 < 0))) __PYX_ERR(0, 148, __pyx_L1_error)
      if (__pyx_t_12) {

        __pyx_t_8 = __Pyx_PyObject_GetItem(__pyx_v_merged, __pyx_v_var); if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 149, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_8);
        __pyx_t_7 = PyNumber_And(__pyx_t_8, __pyx_v_int_set); if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 149, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_7);
        __Pyx_DECREF(__pyx_t_8); __pyx_t_8 = 0;
        if (unlikely((PyObject_SetItem(__pyx_v_merged, __pyx_v_var, __pyx_t_7) < 0))) __PYX_ERR(0, 149, __pyx_L1_error)
        __Pyx_DECREF(__pyx_t_7); __pyx_t_7 = 0;

        __pyx_t_7 = __Pyx_PyObject_GetItem(__pyx_v_merged, __pyx_v_var); if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 150, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_7);
        __pyx_t_12 = __Pyx_PyObject_IsTrue(__pyx_t_7); if (unlikely((__pyx_t_12 < 0))) __PYX_ERR(0, 150, __pyx_L1_error)
        __Pyx_DECREF(__pyx_t_7); __pyx_t_7 = 0;
        __pyx_t_18 = (!__pyx_t_12);
        if (__pyx_t_18) {

          __Pyx_XDECREF(__pyx_r);
          __pyx_r = Py_None; __Pyx_INCREF(Py_None);
          __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;
          __Pyx_DECREF(__pyx_t_6); __pyx_t_6 = 0;
          goto __pyx_L0;

        }

        goto __pyx_L10;
      }

       {
        __pyx_t_7 = PySet_New(__pyx_v_int_set); if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 153, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_7);
        if (unlikely((PyObject_SetItem(__pyx_v_merged, __pyx_v_var, __pyx_t_7) < 0))) __PYX_ERR(0, 153, __pyx_L1_error)
        __Pyx_DECREF(__pyx_t_7); __pyx_t_7 = 0;
      }
      __pyx_L10:;
    }
    __Pyx_DECREF(__pyx_t_6); __pyx_t_6 = 0;

  }
  __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;

  __Pyx_XDECREF(__pyx_r);
  __Pyx_INCREF(__pyx_v_merged);
  __pyx_r = __pyx_v_merged;
  goto __pyx_L0;

  __pyx_L1_error:;
  __Pyx_XDECREF(__pyx_t_1);
  __Pyx_XDECREF(__pyx_t_4);
  __Pyx_XDECREF(__pyx_t_5);
  __Pyx_XDECREF(__pyx_t_6);
  __Pyx_XDECREF(__pyx_t_7);
  __Pyx_XDECREF(__pyx_t_8);
  __Pyx_XDECREF(__pyx_t_9);
  __Pyx_XDECREF(__pyx_t_10);
  __Pyx_AddTraceback("_fast_match.match_star_paths_batch_fast", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __pyx_r = NULL;
  __pyx_L0:;
  __Pyx_XDECREF(__pyx_v_merged);
  __Pyx_XDECREF(__pyx_v_seq);
  __Pyx_XDECREF(__pyx_v_el);
  __Pyx_XDECREF(__pyx_v_node_label_map);
  __Pyx_XDECREF(__pyx_v_skip_checks);
  __Pyx_XDECREF(__pyx_v_path_result);
  __Pyx_XDECREF(__pyx_v_var);
  __Pyx_XDECREF(__pyx_v_int_set);
  __Pyx_XGIVEREF(__pyx_r);
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}

static PyObject *__pyx_pw_11_fast_match_5match_bridge_batch(PyObject *__pyx_self,
#if CYTHON_METH_FASTCALL
PyObject *const *__pyx_args, Py_ssize_t __pyx_nargs, PyObject *__pyx_kwds
#else
PyObject *__pyx_args, PyObject *__pyx_kwds
#endif
);
PyDoc_STRVAR(__pyx_doc_11_fast_match_4match_bridge_batch, "\n    Batch bridge matching: match one path template from MULTIPLE pivots.\n\n    Returns:\n        Dict[int, Optional[Dict[str, Set[int]]]]\n        Maps each pivot_int to per-variable candidate sets (or None).\n    ");
static PyMethodDef __pyx_mdef_11_fast_match_5match_bridge_batch = {"match_bridge_batch", (PyCFunction)(void(*)(void))(__Pyx_PyCFunction_FastCallWithKeywords)__pyx_pw_11_fast_match_5match_bridge_batch, __Pyx_METH_FASTCALL|METH_KEYWORDS, __pyx_doc_11_fast_match_4match_bridge_batch};
static PyObject *__pyx_pw_11_fast_match_5match_bridge_batch(PyObject *__pyx_self,
#if CYTHON_METH_FASTCALL
PyObject *const *__pyx_args, Py_ssize_t __pyx_nargs, PyObject *__pyx_kwds
#else
PyObject *__pyx_args, PyObject *__pyx_kwds
#endif
) {
  PyObject *__pyx_v_compact_graph = 0;
  PyObject *__pyx_v_path_seq = 0;
  PyObject *__pyx_v_pivot_ints = 0;
  PyObject *__pyx_v_edge_lookup = 0;
  PyObject *__pyx_v_node_label_map = 0;
  PyObject *__pyx_v_skip_label_checks = 0;
  PyObject *__pyx_v_int_prefilter = 0;
  PyObject *__pyx_v_pw_eval_fn = 0;
  PyObject *__pyx_v_w_candidate_ints = 0;
  #if !CYTHON_METH_FASTCALL
  CYTHON_UNUSED Py_ssize_t __pyx_nargs;
  #endif
  CYTHON_UNUSED PyObject *const *__pyx_kwvalues;
  PyObject* values[9] = {0,0,0,0,0,0,0,0,0};
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  PyObject *__pyx_r = 0;
  __Pyx_RefNannyDeclarations
  __Pyx_RefNannySetupContext("match_bridge_batch (wrapper)", 0);
  #if !CYTHON_METH_FASTCALL
  #if CYTHON_ASSUME_SAFE_SIZE
  __pyx_nargs = PyTuple_GET_SIZE(__pyx_args);
  #else
  __pyx_nargs = PyTuple_Size(__pyx_args); if (unlikely(__pyx_nargs < 0)) return NULL;
  #endif
  #endif
  __pyx_kwvalues = __Pyx_KwValues_FASTCALL(__pyx_args, __pyx_nargs);
  {
    PyObject ** const __pyx_pyargnames[] = {&__pyx_mstate_global->__pyx_n_u_compact_graph,&__pyx_mstate_global->__pyx_n_u_path_seq,&__pyx_mstate_global->__pyx_n_u_pivot_ints,&__pyx_mstate_global->__pyx_n_u_edge_lookup,&__pyx_mstate_global->__pyx_n_u_node_label_map,&__pyx_mstate_global->__pyx_n_u_skip_label_checks,&__pyx_mstate_global->__pyx_n_u_int_prefilter,&__pyx_mstate_global->__pyx_n_u_pw_eval_fn,&__pyx_mstate_global->__pyx_n_u_w_candidate_ints,0};
    const Py_ssize_t __pyx_kwds_len = (__pyx_kwds) ? __Pyx_NumKwargs_FASTCALL(__pyx_kwds) : 0;
    if (unlikely(__pyx_kwds_len) < 0) __PYX_ERR(0, 157, __pyx_L3_error)
    if (__pyx_kwds_len > 0) {
      switch (__pyx_nargs) {
        case  9:
        values[8] = __Pyx_ArgRef_FASTCALL(__pyx_args, 8);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[8])) __PYX_ERR(0, 157, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  8:
        values[7] = __Pyx_ArgRef_FASTCALL(__pyx_args, 7);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[7])) __PYX_ERR(0, 157, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  7:
        values[6] = __Pyx_ArgRef_FASTCALL(__pyx_args, 6);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[6])) __PYX_ERR(0, 157, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  6:
        values[5] = __Pyx_ArgRef_FASTCALL(__pyx_args, 5);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[5])) __PYX_ERR(0, 157, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  5:
        values[4] = __Pyx_ArgRef_FASTCALL(__pyx_args, 4);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[4])) __PYX_ERR(0, 157, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  4:
        values[3] = __Pyx_ArgRef_FASTCALL(__pyx_args, 3);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[3])) __PYX_ERR(0, 157, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  3:
        values[2] = __Pyx_ArgRef_FASTCALL(__pyx_args, 2);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[2])) __PYX_ERR(0, 157, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  2:
        values[1] = __Pyx_ArgRef_FASTCALL(__pyx_args, 1);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[1])) __PYX_ERR(0, 157, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  1:
        values[0] = __Pyx_ArgRef_FASTCALL(__pyx_args, 0);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[0])) __PYX_ERR(0, 157, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  0: break;
        default: goto __pyx_L5_argtuple_error;
      }
      const Py_ssize_t kwd_pos_args = __pyx_nargs;
      if (__Pyx_ParseKeywords(__pyx_kwds, __pyx_kwvalues, __pyx_pyargnames, 0, values, kwd_pos_args, __pyx_kwds_len, "match_bridge_batch", 0) < (0)) __PYX_ERR(0, 157, __pyx_L3_error)

      if (!values[7]) values[7] = __Pyx_NewRef(((PyObject *)Py_None));

      if (!values[8]) values[8] = __Pyx_NewRef(((PyObject *)Py_None));
      for (Py_ssize_t i = __pyx_nargs; i < 7; i++) {
        if (unlikely(!values[i])) { __Pyx_RaiseArgtupleInvalid("match_bridge_batch", 0, 7, 9, i); __PYX_ERR(0, 157, __pyx_L3_error) }
      }
    } else {
      switch (__pyx_nargs) {
        case  9:
        values[8] = __Pyx_ArgRef_FASTCALL(__pyx_args, 8);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[8])) __PYX_ERR(0, 157, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  8:
        values[7] = __Pyx_ArgRef_FASTCALL(__pyx_args, 7);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[7])) __PYX_ERR(0, 157, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  7:
        values[6] = __Pyx_ArgRef_FASTCALL(__pyx_args, 6);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[6])) __PYX_ERR(0, 157, __pyx_L3_error)
        values[5] = __Pyx_ArgRef_FASTCALL(__pyx_args, 5);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[5])) __PYX_ERR(0, 157, __pyx_L3_error)
        values[4] = __Pyx_ArgRef_FASTCALL(__pyx_args, 4);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[4])) __PYX_ERR(0, 157, __pyx_L3_error)
        values[3] = __Pyx_ArgRef_FASTCALL(__pyx_args, 3);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[3])) __PYX_ERR(0, 157, __pyx_L3_error)
        values[2] = __Pyx_ArgRef_FASTCALL(__pyx_args, 2);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[2])) __PYX_ERR(0, 157, __pyx_L3_error)
        values[1] = __Pyx_ArgRef_FASTCALL(__pyx_args, 1);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[1])) __PYX_ERR(0, 157, __pyx_L3_error)
        values[0] = __Pyx_ArgRef_FASTCALL(__pyx_args, 0);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[0])) __PYX_ERR(0, 157, __pyx_L3_error)
        break;
        default: goto __pyx_L5_argtuple_error;
      }

      if (!values[7]) values[7] = __Pyx_NewRef(((PyObject *)Py_None));

      if (!values[8]) values[8] = __Pyx_NewRef(((PyObject *)Py_None));
    }
    __pyx_v_compact_graph = values[0];
    __pyx_v_path_seq = values[1];
    __pyx_v_pivot_ints = ((PyObject*)values[2]);
    __pyx_v_edge_lookup = ((PyObject*)values[3]);
    __pyx_v_node_label_map = ((PyObject*)values[4]);
    __pyx_v_skip_label_checks = ((PyObject*)values[5]);
    __pyx_v_int_prefilter = ((PyObject*)values[6]);
    __pyx_v_pw_eval_fn = values[7];
    __pyx_v_w_candidate_ints = values[8];
  }
  goto __pyx_L6_skip;
  __pyx_L5_argtuple_error:;
  __Pyx_RaiseArgtupleInvalid("match_bridge_batch", 0, 7, 9, __pyx_nargs); __PYX_ERR(0, 157, __pyx_L3_error)
  __pyx_L6_skip:;
  goto __pyx_L4_argument_unpacking_done;
  __pyx_L3_error:;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  __Pyx_AddTraceback("_fast_match.match_bridge_batch", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __Pyx_RefNannyFinishContext();
  return NULL;
  __pyx_L4_argument_unpacking_done:;
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_pivot_ints), (&PyList_Type), 1, "pivot_ints", 1))) __PYX_ERR(0, 160, __pyx_L1_error)
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_edge_lookup), (&PyDict_Type), 1, "edge_lookup", 1))) __PYX_ERR(0, 161, __pyx_L1_error)
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_node_label_map), (&PyDict_Type), 1, "node_label_map", 1))) __PYX_ERR(0, 162, __pyx_L1_error)
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_skip_label_checks), (&PyDict_Type), 1, "skip_label_checks", 1))) __PYX_ERR(0, 163, __pyx_L1_error)
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_int_prefilter), (&PyDict_Type), 1, "int_prefilter", 1))) __PYX_ERR(0, 164, __pyx_L1_error)
  __pyx_r = __pyx_pf_11_fast_match_4match_bridge_batch(__pyx_self, __pyx_v_compact_graph, __pyx_v_path_seq, __pyx_v_pivot_ints, __pyx_v_edge_lookup, __pyx_v_node_label_map, __pyx_v_skip_label_checks, __pyx_v_int_prefilter, __pyx_v_pw_eval_fn, __pyx_v_w_candidate_ints);

  goto __pyx_L0;
  __pyx_L1_error:;
  __pyx_r = NULL;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  goto __pyx_L7_cleaned_up;
  __pyx_L0:;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  __pyx_L7_cleaned_up:;
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}

static PyObject *__pyx_pf_11_fast_match_4match_bridge_batch(CYTHON_UNUSED PyObject *__pyx_self, PyObject *__pyx_v_compact_graph, PyObject *__pyx_v_path_seq, PyObject *__pyx_v_pivot_ints, PyObject *__pyx_v_edge_lookup, PyObject *__pyx_v_node_label_map, PyObject *__pyx_v_skip_label_checks, PyObject *__pyx_v_int_prefilter, PyObject *__pyx_v_pw_eval_fn, PyObject *__pyx_v_w_candidate_ints) {
  int __pyx_v_i;
  int __pyx_v_step_count;
  int __pyx_v_n_id;
  int __pyx_v_p_idx;
  int __pyx_v_is_forward;
  int __pyx_v_do_label_check;
  int __pyx_v_has_prefilter;
  int __pyx_v_has_pw_eval;
  PyObject *__pyx_v_out = NULL;
  PyObject *__pyx_v_cg_out = NULL;
  PyObject *__pyx_v_cg_in = NULL;
  PyObject *__pyx_v_cg_labels = NULL;
  PyObject *__pyx_v_hop_meta = NULL;
  PyObject *__pyx_v_current_var = NULL;
  PyObject *__pyx_v_next_var = NULL;
  PyObject *__pyx_v_edge_info = NULL;
  PyObject *__pyx_v_last_var = NULL;
  PyObject *__pyx_v_has_w_filter = NULL;
  PyObject *__pyx_v_pivot_int = NULL;
  PyObject *__pyx_v_result = NULL;
  int __pyx_v_failed;
  PyObject *__pyx_v_edge_label = NULL;
  PyObject *__pyx_v_expected_label = NULL;
  PyObject *__pyx_v_prefilter = NULL;
  PyObject *__pyx_v_current_set = NULL;
  PyObject *__pyx_v_next_set = NULL;
  PyObject *__pyx_v_current_int = NULL;
  PyObject *__pyx_v_neighbors = NULL;
  PyObject *__pyx_v_filtered = NULL;
  PyObject *__pyx_r = NULL;
  __Pyx_RefNannyDeclarations
  int __pyx_t_1;
  int __pyx_t_2;
  int __pyx_t_3;
  PyObject *__pyx_t_4 = NULL;
  Py_ssize_t __pyx_t_5;
  Py_ssize_t __pyx_t_6;
  int __pyx_t_7;
  PyObject *__pyx_t_8 = NULL;
  PyObject *__pyx_t_9 = NULL;
  int __pyx_t_10;
  int __pyx_t_11;
  long __pyx_t_12;
  int __pyx_t_13;
  PyObject *__pyx_t_14 = NULL;
  PyObject *__pyx_t_15 = NULL;
  PyObject *__pyx_t_16 = NULL;
  int __pyx_t_17;
  Py_ssize_t __pyx_t_18;
  PyObject *(*__pyx_t_19)(PyObject *);
  size_t __pyx_t_20;
  Py_ssize_t __pyx_t_21;
  PyObject *(*__pyx_t_22)(PyObject *);
  int __pyx_t_23;
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  __Pyx_RefNannySetupContext("match_bridge_batch", 0);

  __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_v_path_seq); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 178, __pyx_L1_error)
  __pyx_t_3 = (!__pyx_t_2);
  if (!__pyx_t_3) {
  } else {
    __pyx_t_1 = __pyx_t_3;
    goto __pyx_L4_bool_binop_done;
  }
  if (__pyx_v_pivot_ints == Py_None) __pyx_t_3 = 0;
  else
  {
    Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_v_pivot_ints);
    if (unlikely(((!CYTHON_ASSUME_SAFE_SIZE) && __pyx_temp < 0))) __PYX_ERR(0, 178, __pyx_L1_error)
    __pyx_t_3 = (__pyx_temp != 0);
  }

  __pyx_t_2 = (!__pyx_t_3);
  __pyx_t_1 = __pyx_t_2;
  __pyx_L4_bool_binop_done:;
  if (__pyx_t_1) {

    __Pyx_XDECREF(__pyx_r);
    __pyx_t_4 = __Pyx_PyDict_NewPresized(0); if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 179, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_4);
    __pyx_r = __pyx_t_4;
    __pyx_t_4 = 0;
    goto __pyx_L0;

  }

  __pyx_t_5 = PyObject_Length(__pyx_v_path_seq); if (unlikely(__pyx_t_5 == ((Py_ssize_t)-1))) __PYX_ERR(0, 181, __pyx_L1_error)
  __pyx_v_step_count = (__pyx_t_5 - 1);

  __pyx_t_1 = (__pyx_v_step_count == 0);
  if (__pyx_t_1) {

    __pyx_t_4 = __Pyx_PyDict_NewPresized(0); if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 183, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_4);
    __pyx_v_out = ((PyObject*)__pyx_t_4);
    __pyx_t_4 = 0;

    if (unlikely(__pyx_v_pivot_ints == Py_None)) {
      PyErr_SetString(PyExc_TypeError, "object of type 'NoneType' has no len()");
      __PYX_ERR(0, 184, __pyx_L1_error)
    }
    __pyx_t_5 = __Pyx_PyList_GET_SIZE(__pyx_v_pivot_ints); if (unlikely(__pyx_t_5 == ((Py_ssize_t)-1))) __PYX_ERR(0, 184, __pyx_L1_error)
    __pyx_t_6 = __pyx_t_5;
    for (__pyx_t_7 = 0; __pyx_t_7 < __pyx_t_6; __pyx_t_7+=1) {
      __pyx_v_p_idx = __pyx_t_7;

      __pyx_t_4 = __Pyx_PyDict_NewPresized(1); if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 185, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_4);
      __pyx_t_8 = __Pyx_GetItemInt(__pyx_v_path_seq, 0, long, 1, __Pyx_PyLong_From_long, 0, 0, 0, 1, __Pyx_ReferenceSharing_FunctionArgument); if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 185, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_8);
      if (unlikely(__pyx_v_pivot_ints == Py_None)) {
        PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
        __PYX_ERR(0, 185, __pyx_L1_error)
      }
      __pyx_t_9 = PySet_New(0); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 185, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_9);
      if (PySet_Add(__pyx_t_9, __Pyx_PyList_GET_ITEM(__pyx_v_pivot_ints, __pyx_v_p_idx)) < (0)) __PYX_ERR(0, 185, __pyx_L1_error)
      if (PyDict_SetItem(__pyx_t_4, __pyx_t_8, __pyx_t_9) < (0)) __PYX_ERR(0, 185, __pyx_L1_error)
      __Pyx_DECREF(__pyx_t_8); __pyx_t_8 = 0;
      __Pyx_DECREF(__pyx_t_9); __pyx_t_9 = 0;
      if (unlikely(__pyx_v_pivot_ints == Py_None)) {
        PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
        __PYX_ERR(0, 185, __pyx_L1_error)
      }
      if (unlikely((PyDict_SetItem(__pyx_v_out, __Pyx_PyList_GET_ITEM(__pyx_v_pivot_ints, __pyx_v_p_idx), __pyx_t_4) < 0))) __PYX_ERR(0, 185, __pyx_L1_error)
      __Pyx_DECREF(__pyx_t_4); __pyx_t_4 = 0;
    }

    __Pyx_XDECREF(__pyx_r);
    __Pyx_INCREF(__pyx_v_out);
    __pyx_r = __pyx_v_out;
    goto __pyx_L0;

  }

  __pyx_t_4 = __Pyx_PyObject_GetAttrStr(__pyx_v_compact_graph, __pyx_mstate_global->__pyx_n_u_out_neighbors); if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 189, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_4);
  __pyx_v_cg_out = __pyx_t_4;
  __pyx_t_4 = 0;

  __pyx_t_4 = __Pyx_PyObject_GetAttrStr(__pyx_v_compact_graph, __pyx_mstate_global->__pyx_n_u_in_neighbors); if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 190, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_4);
  __pyx_v_cg_in = __pyx_t_4;
  __pyx_t_4 = 0;

  __pyx_t_4 = __Pyx_PyObject_GetAttrStr(__pyx_v_compact_graph, __pyx_mstate_global->__pyx_n_u_node_labels); if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 191, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_4);
  __pyx_v_cg_labels = __pyx_t_4;
  __pyx_t_4 = 0;

  __pyx_t_4 = PyList_New(0); if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 194, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_4);
  __pyx_v_hop_meta = ((PyObject*)__pyx_t_4);
  __pyx_t_4 = 0;

  __pyx_t_7 = __pyx_v_step_count;
  __pyx_t_10 = __pyx_t_7;
  for (__pyx_t_11 = 0; __pyx_t_11 < __pyx_t_10; __pyx_t_11+=1) {
    __pyx_v_i = __pyx_t_11;

    __pyx_t_4 = __Pyx_GetItemInt(__pyx_v_path_seq, __pyx_v_i, int, 1, __Pyx_PyLong_From_int, 0, 0, 0, 1, __Pyx_ReferenceSharing_FunctionArgument); if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 196, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_4);
    __Pyx_XDECREF_SET(__pyx_v_current_var, __pyx_t_4);
    __pyx_t_4 = 0;

    __pyx_t_12 = (__pyx_v_i + 1);
    __pyx_t_4 = __Pyx_GetItemInt(__pyx_v_path_seq, __pyx_t_12, long, 1, __Pyx_PyLong_From_long, 0, 0, 0, 1, __Pyx_ReferenceSharing_FunctionArgument); if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 197, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_4);
    __Pyx_XDECREF_SET(__pyx_v_next_var, __pyx_t_4);
    __pyx_t_4 = 0;

    if (unlikely(__pyx_v_edge_lookup == Py_None)) {
      PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "get");
      __PYX_ERR(0, 198, __pyx_L1_error)
    }
    __pyx_t_4 = PyTuple_New(2); if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 198, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_4);
    __Pyx_INCREF(__pyx_v_current_var);
    __Pyx_GIVEREF(__pyx_v_current_var);
    if (__Pyx_PyTuple_SET_ITEM(__pyx_t_4, 0, __pyx_v_current_var) != (0)) __PYX_ERR(0, 198, __pyx_L1_error);
    __Pyx_INCREF(__pyx_v_next_var);
    __Pyx_GIVEREF(__pyx_v_next_var);
    if (__Pyx_PyTuple_SET_ITEM(__pyx_t_4, 1, __pyx_v_next_var) != (0)) __PYX_ERR(0, 198, __pyx_L1_error);
    __pyx_t_9 = __Pyx_PyDict_GetItemDefault(__pyx_v_edge_lookup, __pyx_t_4, Py_None); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 198, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_9);
    __Pyx_DECREF(__pyx_t_4); __pyx_t_4 = 0;
    __Pyx_XDECREF_SET(__pyx_v_edge_info, __pyx_t_9);
    __pyx_t_9 = 0;

    __pyx_t_1 = (__pyx_v_edge_info == Py_None);
    if (__pyx_t_1) {

      __pyx_t_9 = __Pyx_PyDict_NewPresized(0); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 201, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_9);
      __pyx_v_out = ((PyObject*)__pyx_t_9);
      __pyx_t_9 = 0;

      if (unlikely(__pyx_v_pivot_ints == Py_None)) {
        PyErr_SetString(PyExc_TypeError, "object of type 'NoneType' has no len()");
        __PYX_ERR(0, 202, __pyx_L1_error)
      }
      __pyx_t_5 = __Pyx_PyList_GET_SIZE(__pyx_v_pivot_ints); if (unlikely(__pyx_t_5 == ((Py_ssize_t)-1))) __PYX_ERR(0, 202, __pyx_L1_error)
      __pyx_t_6 = __pyx_t_5;
      for (__pyx_t_13 = 0; __pyx_t_13 < __pyx_t_6; __pyx_t_13+=1) {
        __pyx_v_p_idx = __pyx_t_13;

        if (unlikely(__pyx_v_pivot_ints == Py_None)) {
          PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
          __PYX_ERR(0, 203, __pyx_L1_error)
        }
        if (unlikely((PyDict_SetItem(__pyx_v_out, __Pyx_PyList_GET_ITEM(__pyx_v_pivot_ints, __pyx_v_p_idx), Py_None) < 0))) __PYX_ERR(0, 203, __pyx_L1_error)
      }

      __Pyx_XDECREF(__pyx_r);
      __Pyx_INCREF(__pyx_v_out);
      __pyx_r = __pyx_v_out;
      goto __pyx_L0;

    }

    __pyx_t_9 = __Pyx_GetItemInt(__pyx_v_edge_info, 0, long, 1, __Pyx_PyLong_From_long, 0, 0, 0, 1, __Pyx_ReferenceSharing_OwnStrongReference); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 208, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_9);

    __pyx_t_4 = __Pyx_GetItemInt(__pyx_v_edge_info, 1, long, 1, __Pyx_PyLong_From_long, 0, 0, 0, 1, __Pyx_ReferenceSharing_OwnStrongReference); if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 209, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_4);

    if (unlikely(__pyx_v_node_label_map == Py_None)) {
      PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "get");
      __PYX_ERR(0, 210, __pyx_L1_error)
    }
    __pyx_t_8 = __Pyx_PyDict_GetItemDefault(__pyx_v_node_label_map, __pyx_v_next_var, Py_None); if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 210, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_8);

    if (unlikely(__pyx_v_skip_label_checks == Py_None)) {
      PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "get");
      __PYX_ERR(0, 211, __pyx_L1_error)
    }
    __pyx_t_14 = __Pyx_PyLong_From_int(__pyx_v_i); if (unlikely(!__pyx_t_14)) __PYX_ERR(0, 211, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_14);
    __pyx_t_15 = __Pyx_PyDict_GetItemDefault(__pyx_v_skip_label_checks, __pyx_t_14, Py_False); if (unlikely(!__pyx_t_15)) __PYX_ERR(0, 211, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_15);
    __Pyx_DECREF(__pyx_t_14); __pyx_t_14 = 0;
    __pyx_t_1 = __Pyx_PyObject_IsTrue(__pyx_t_15); if (unlikely((__pyx_t_1 < 0))) __PYX_ERR(0, 211, __pyx_L1_error)
    __Pyx_DECREF(__pyx_t_15); __pyx_t_15 = 0;
    __pyx_t_15 = __Pyx_PyBool_FromLong((!__pyx_t_1)); if (unlikely(!__pyx_t_15)) __PYX_ERR(0, 211, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_15);

    if (unlikely(__pyx_v_int_prefilter == Py_None)) {
      PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "get");
      __PYX_ERR(0, 212, __pyx_L1_error)
    }
    __pyx_t_14 = __Pyx_PyDict_GetItemDefault(__pyx_v_int_prefilter, __pyx_v_next_var, Py_None); if (unlikely(!__pyx_t_14)) __PYX_ERR(0, 212, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_14);

    __pyx_t_16 = PyTuple_New(7); if (unlikely(!__pyx_t_16)) __PYX_ERR(0, 206, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_16);
    __Pyx_INCREF(__pyx_v_current_var);
    __Pyx_GIVEREF(__pyx_v_current_var);
    if (__Pyx_PyTuple_SET_ITEM(__pyx_t_16, 0, __pyx_v_current_var) != (0)) __PYX_ERR(0, 206, __pyx_L1_error);
    __Pyx_INCREF(__pyx_v_next_var);
    __Pyx_GIVEREF(__pyx_v_next_var);
    if (__Pyx_PyTuple_SET_ITEM(__pyx_t_16, 1, __pyx_v_next_var) != (0)) __PYX_ERR(0, 206, __pyx_L1_error);
    __Pyx_GIVEREF(__pyx_t_9);
    if (__Pyx_PyTuple_SET_ITEM(__pyx_t_16, 2, __pyx_t_9) != (0)) __PYX_ERR(0, 206, __pyx_L1_error);
    __Pyx_GIVEREF(__pyx_t_4);
    if (__Pyx_PyTuple_SET_ITEM(__pyx_t_16, 3, __pyx_t_4) != (0)) __PYX_ERR(0, 206, __pyx_L1_error);
    __Pyx_GIVEREF(__pyx_t_8);
    if (__Pyx_PyTuple_SET_ITEM(__pyx_t_16, 4, __pyx_t_8) != (0)) __PYX_ERR(0, 206, __pyx_L1_error);
    __Pyx_GIVEREF(__pyx_t_15);
    if (__Pyx_PyTuple_SET_ITEM(__pyx_t_16, 5, __pyx_t_15) != (0)) __PYX_ERR(0, 206, __pyx_L1_error);
    __Pyx_GIVEREF(__pyx_t_14);
    if (__Pyx_PyTuple_SET_ITEM(__pyx_t_16, 6, __pyx_t_14) != (0)) __PYX_ERR(0, 206, __pyx_L1_error);
    __pyx_t_9 = 0;
    __pyx_t_4 = 0;
    __pyx_t_8 = 0;
    __pyx_t_15 = 0;
    __pyx_t_14 = 0;

    __pyx_t_17 = __Pyx_PyList_Append(__pyx_v_hop_meta, __pyx_t_16); if (unlikely(__pyx_t_17 == ((int)-1))) __PYX_ERR(0, 205, __pyx_L1_error)
    __Pyx_DECREF(__pyx_t_16); __pyx_t_16 = 0;
  }

  __pyx_t_1 = (__pyx_v_pw_eval_fn != Py_None);
  __pyx_v_has_pw_eval = __pyx_t_1;

  __pyx_t_16 = __Pyx_GetItemInt(__pyx_v_path_seq, __pyx_v_step_count, int, 1, __Pyx_PyLong_From_int, 0, 0, 0, 1, __Pyx_ReferenceSharing_FunctionArgument); if (unlikely(!__pyx_t_16)) __PYX_ERR(0, 216, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_16);
  __pyx_v_last_var = __pyx_t_16;
  __pyx_t_16 = 0;

  __pyx_t_1 = (__pyx_v_w_candidate_ints != Py_None);
  __pyx_t_16 = __Pyx_PyBool_FromLong(__pyx_t_1); if (unlikely(!__pyx_t_16)) __PYX_ERR(0, 217, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_16);
  __pyx_v_has_w_filter = __pyx_t_16;
  __pyx_t_16 = 0;

  __pyx_t_16 = __Pyx_PyDict_NewPresized(0); if (unlikely(!__pyx_t_16)) __PYX_ERR(0, 219, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_16);
  __pyx_v_out = ((PyObject*)__pyx_t_16);
  __pyx_t_16 = 0;

  if (unlikely(__pyx_v_pivot_ints == Py_None)) {
    PyErr_SetString(PyExc_TypeError, "object of type 'NoneType' has no len()");
    __PYX_ERR(0, 220, __pyx_L1_error)
  }
  __pyx_t_5 = __Pyx_PyList_GET_SIZE(__pyx_v_pivot_ints); if (unlikely(__pyx_t_5 == ((Py_ssize_t)-1))) __PYX_ERR(0, 220, __pyx_L1_error)
  __pyx_t_6 = __pyx_t_5;
  for (__pyx_t_7 = 0; __pyx_t_7 < __pyx_t_6; __pyx_t_7+=1) {
    __pyx_v_p_idx = __pyx_t_7;

    if (unlikely(__pyx_v_pivot_ints == Py_None)) {
      PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
      __PYX_ERR(0, 221, __pyx_L1_error)
    }
    __pyx_t_16 = __Pyx_PyList_GET_ITEM(__pyx_v_pivot_ints, __pyx_v_p_idx);
    __Pyx_INCREF(__pyx_t_16);
    __Pyx_XDECREF_SET(__pyx_v_pivot_int, __pyx_t_16);
    __pyx_t_16 = 0;

    __pyx_t_16 = __Pyx_PyDict_NewPresized(1); if (unlikely(!__pyx_t_16)) __PYX_ERR(0, 222, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_16);
    __pyx_t_14 = __Pyx_GetItemInt(__pyx_v_path_seq, 0, long, 1, __Pyx_PyLong_From_long, 0, 0, 0, 1, __Pyx_ReferenceSharing_FunctionArgument); if (unlikely(!__pyx_t_14)) __PYX_ERR(0, 222, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_14);
    __pyx_t_15 = PySet_New(0); if (unlikely(!__pyx_t_15)) __PYX_ERR(0, 222, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_15);
    if (PySet_Add(__pyx_t_15, __pyx_v_pivot_int) < (0)) __PYX_ERR(0, 222, __pyx_L1_error)
    if (PyDict_SetItem(__pyx_t_16, __pyx_t_14, __pyx_t_15) < (0)) __PYX_ERR(0, 222, __pyx_L1_error)
    __Pyx_DECREF(__pyx_t_14); __pyx_t_14 = 0;
    __Pyx_DECREF(__pyx_t_15); __pyx_t_15 = 0;
    __Pyx_XDECREF_SET(__pyx_v_result, ((PyObject*)__pyx_t_16));
    __pyx_t_16 = 0;

    __pyx_v_failed = 0;

    __pyx_t_10 = __pyx_v_step_count;
    __pyx_t_11 = __pyx_t_10;
    for (__pyx_t_13 = 0; __pyx_t_13 < __pyx_t_11; __pyx_t_13+=1) {
      __pyx_v_i = __pyx_t_13;

      __pyx_t_16 = __Pyx_GetItemInt(__Pyx_PyList_GET_ITEM(__pyx_v_hop_meta, __pyx_v_i), 0, long, 1, __Pyx_PyLong_From_long, 0, 0, 0, 1, __Pyx_ReferenceSharing_SharedReference); if (unlikely(!__pyx_t_16)) __PYX_ERR(0, 226, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_16);
      __Pyx_XDECREF_SET(__pyx_v_current_var, __pyx_t_16);
      __pyx_t_16 = 0;

      __pyx_t_16 = __Pyx_GetItemInt(__Pyx_PyList_GET_ITEM(__pyx_v_hop_meta, __pyx_v_i), 1, long, 1, __Pyx_PyLong_From_long, 0, 0, 0, 1, __Pyx_ReferenceSharing_SharedReference); if (unlikely(!__pyx_t_16)) __PYX_ERR(0, 227, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_16);
      __Pyx_XDECREF_SET(__pyx_v_next_var, __pyx_t_16);
      __pyx_t_16 = 0;

      __pyx_t_16 = __Pyx_GetItemInt(__Pyx_PyList_GET_ITEM(__pyx_v_hop_meta, __pyx_v_i), 2, long, 1, __Pyx_PyLong_From_long, 0, 0, 0, 1, __Pyx_ReferenceSharing_SharedReference); if (unlikely(!__pyx_t_16)) __PYX_ERR(0, 228, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_16);
      __Pyx_XDECREF_SET(__pyx_v_edge_label, __pyx_t_16);
      __pyx_t_16 = 0;

      __pyx_t_16 = __Pyx_GetItemInt(__Pyx_PyList_GET_ITEM(__pyx_v_hop_meta, __pyx_v_i), 3, long, 1, __Pyx_PyLong_From_long, 0, 0, 0, 1, __Pyx_ReferenceSharing_SharedReference); if (unlikely(!__pyx_t_16)) __PYX_ERR(0, 229, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_16);
      __pyx_t_1 = __Pyx_PyObject_IsTrue(__pyx_t_16); if (unlikely((__pyx_t_1 == (int)-1) && PyErr_Occurred())) __PYX_ERR(0, 229, __pyx_L1_error)
      __Pyx_DECREF(__pyx_t_16); __pyx_t_16 = 0;
      __pyx_v_is_forward = __pyx_t_1;

      __pyx_t_16 = __Pyx_GetItemInt(__Pyx_PyList_GET_ITEM(__pyx_v_hop_meta, __pyx_v_i), 4, long, 1, __Pyx_PyLong_From_long, 0, 0, 0, 1, __Pyx_ReferenceSharing_SharedReference); if (unlikely(!__pyx_t_16)) __PYX_ERR(0, 230, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_16);
      __Pyx_XDECREF_SET(__pyx_v_expected_label, __pyx_t_16);
      __pyx_t_16 = 0;

      __pyx_t_16 = __Pyx_GetItemInt(__Pyx_PyList_GET_ITEM(__pyx_v_hop_meta, __pyx_v_i), 5, long, 1, __Pyx_PyLong_From_long, 0, 0, 0, 1, __Pyx_ReferenceSharing_SharedReference); if (unlikely(!__pyx_t_16)) __PYX_ERR(0, 231, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_16);
      __pyx_t_1 = __Pyx_PyObject_IsTrue(__pyx_t_16); if (unlikely((__pyx_t_1 == (int)-1) && PyErr_Occurred())) __PYX_ERR(0, 231, __pyx_L1_error)
      __Pyx_DECREF(__pyx_t_16); __pyx_t_16 = 0;
      __pyx_v_do_label_check = __pyx_t_1;

      __pyx_t_16 = __Pyx_GetItemInt(__Pyx_PyList_GET_ITEM(__pyx_v_hop_meta, __pyx_v_i), 6, long, 1, __Pyx_PyLong_From_long, 0, 0, 0, 1, __Pyx_ReferenceSharing_SharedReference); if (unlikely(!__pyx_t_16)) __PYX_ERR(0, 232, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_16);
      __Pyx_XDECREF_SET(__pyx_v_prefilter, __pyx_t_16);
      __pyx_t_16 = 0;

      __pyx_t_1 = (__pyx_v_prefilter != Py_None);
      __pyx_v_has_prefilter = __pyx_t_1;

      __pyx_t_16 = __Pyx_PyDict_GetItem(__pyx_v_result, __pyx_v_current_var); if (unlikely(!__pyx_t_16)) __PYX_ERR(0, 235, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_16);
      __Pyx_XDECREF_SET(__pyx_v_current_set, __pyx_t_16);
      __pyx_t_16 = 0;

      __pyx_t_16 = PySet_New(0); if (unlikely(!__pyx_t_16)) __PYX_ERR(0, 236, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_16);
      __Pyx_XDECREF_SET(__pyx_v_next_set, ((PyObject*)__pyx_t_16));
      __pyx_t_16 = 0;

      if (likely(PyList_CheckExact(__pyx_v_current_set)) || PyTuple_CheckExact(__pyx_v_current_set)) {
        __pyx_t_16 = __pyx_v_current_set; __Pyx_INCREF(__pyx_t_16);
        __pyx_t_18 = 0;
        __pyx_t_19 = NULL;
      } else {
        __pyx_t_18 = -1; __pyx_t_16 = PyObject_GetIter(__pyx_v_current_set); if (unlikely(!__pyx_t_16)) __PYX_ERR(0, 238, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_16);
        __pyx_t_19 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_16); if (unlikely(!__pyx_t_19)) __PYX_ERR(0, 238, __pyx_L1_error)
      }
      for (;;) {
        if (likely(!__pyx_t_19)) {
          if (likely(PyList_CheckExact(__pyx_t_16))) {
            {
              Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_t_16);
              #if !CYTHON_ASSUME_SAFE_SIZE
              if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 238, __pyx_L1_error)
              #endif
              if (__pyx_t_18 >= __pyx_temp) break;
            }
            __pyx_t_15 = __Pyx_PyList_GetItemRefFast(__pyx_t_16, __pyx_t_18, __Pyx_ReferenceSharing_OwnStrongReference);
            ++__pyx_t_18;
          } else {
            {
              Py_ssize_t __pyx_temp = __Pyx_PyTuple_GET_SIZE(__pyx_t_16);
              #if !CYTHON_ASSUME_SAFE_SIZE
              if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 238, __pyx_L1_error)
              #endif
              if (__pyx_t_18 >= __pyx_temp) break;
            }
            #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
            __pyx_t_15 = __Pyx_NewRef(PyTuple_GET_ITEM(__pyx_t_16, __pyx_t_18));
            #else
            __pyx_t_15 = __Pyx_PySequence_ITEM(__pyx_t_16, __pyx_t_18);
            #endif
            ++__pyx_t_18;
          }
          if (unlikely(!__pyx_t_15)) __PYX_ERR(0, 238, __pyx_L1_error)
        } else {
          __pyx_t_15 = __pyx_t_19(__pyx_t_16);
          if (unlikely(!__pyx_t_15)) {
            PyObject* exc_type = PyErr_Occurred();
            if (exc_type) {
              if (unlikely(!__Pyx_PyErr_GivenExceptionMatches(exc_type, PyExc_StopIteration))) __PYX_ERR(0, 238, __pyx_L1_error)
              PyErr_Clear();
            }
            break;
          }
        }
        __Pyx_GOTREF(__pyx_t_15);
        __Pyx_XDECREF_SET(__pyx_v_current_int, __pyx_t_15);
        __pyx_t_15 = 0;

        if (__pyx_v_is_forward) {

          __pyx_t_14 = __pyx_v_cg_out;
          __Pyx_INCREF(__pyx_t_14);
          __pyx_t_8 = PyTuple_New(2); if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 240, __pyx_L1_error)
          __Pyx_GOTREF(__pyx_t_8);
          __Pyx_INCREF(__pyx_v_current_int);
          __Pyx_GIVEREF(__pyx_v_current_int);
          if (__Pyx_PyTuple_SET_ITEM(__pyx_t_8, 0, __pyx_v_current_int) != (0)) __PYX_ERR(0, 240, __pyx_L1_error);
          __Pyx_INCREF(__pyx_v_edge_label);
          __Pyx_GIVEREF(__pyx_v_edge_label);
          if (__Pyx_PyTuple_SET_ITEM(__pyx_t_8, 1, __pyx_v_edge_label) != (0)) __PYX_ERR(0, 240, __pyx_L1_error);
          __pyx_t_20 = 0;
          {
            PyObject *__pyx_callargs[2] = {__pyx_t_14, __pyx_t_8};
            __pyx_t_15 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_get, __pyx_callargs+__pyx_t_20, (2-__pyx_t_20) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
            __Pyx_XDECREF(__pyx_t_14); __pyx_t_14 = 0;
            __Pyx_DECREF(__pyx_t_8); __pyx_t_8 = 0;
            if (unlikely(!__pyx_t_15)) __PYX_ERR(0, 240, __pyx_L1_error)
            __Pyx_GOTREF(__pyx_t_15);
          }
          __Pyx_XDECREF_SET(__pyx_v_neighbors, __pyx_t_15);
          __pyx_t_15 = 0;

          goto __pyx_L20;
        }

         {
          __pyx_t_8 = __pyx_v_cg_in;
          __Pyx_INCREF(__pyx_t_8);
          __pyx_t_14 = PyTuple_New(2); if (unlikely(!__pyx_t_14)) __PYX_ERR(0, 242, __pyx_L1_error)
          __Pyx_GOTREF(__pyx_t_14);
          __Pyx_INCREF(__pyx_v_current_int);
          __Pyx_GIVEREF(__pyx_v_current_int);
          if (__Pyx_PyTuple_SET_ITEM(__pyx_t_14, 0, __pyx_v_current_int) != (0)) __PYX_ERR(0, 242, __pyx_L1_error);
          __Pyx_INCREF(__pyx_v_edge_label);
          __Pyx_GIVEREF(__pyx_v_edge_label);
          if (__Pyx_PyTuple_SET_ITEM(__pyx_t_14, 1, __pyx_v_edge_label) != (0)) __PYX_ERR(0, 242, __pyx_L1_error);
          __pyx_t_20 = 0;
          {
            PyObject *__pyx_callargs[2] = {__pyx_t_8, __pyx_t_14};
            __pyx_t_15 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_get, __pyx_callargs+__pyx_t_20, (2-__pyx_t_20) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
            __Pyx_XDECREF(__pyx_t_8); __pyx_t_8 = 0;
            __Pyx_DECREF(__pyx_t_14); __pyx_t_14 = 0;
            if (unlikely(!__pyx_t_15)) __PYX_ERR(0, 242, __pyx_L1_error)
            __Pyx_GOTREF(__pyx_t_15);
          }
          __Pyx_XDECREF_SET(__pyx_v_neighbors, __pyx_t_15);
          __pyx_t_15 = 0;
        }
        __pyx_L20:;

        __pyx_t_1 = (__pyx_v_neighbors == Py_None);
        if (__pyx_t_1) {

          goto __pyx_L18_continue;

        }

        if (likely(PyList_CheckExact(__pyx_v_neighbors)) || PyTuple_CheckExact(__pyx_v_neighbors)) {
          __pyx_t_15 = __pyx_v_neighbors; __Pyx_INCREF(__pyx_t_15);
          __pyx_t_21 = 0;
          __pyx_t_22 = NULL;
        } else {
          __pyx_t_21 = -1; __pyx_t_15 = PyObject_GetIter(__pyx_v_neighbors); if (unlikely(!__pyx_t_15)) __PYX_ERR(0, 245, __pyx_L1_error)
          __Pyx_GOTREF(__pyx_t_15);
          __pyx_t_22 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_15); if (unlikely(!__pyx_t_22)) __PYX_ERR(0, 245, __pyx_L1_error)
        }
        for (;;) {
          if (likely(!__pyx_t_22)) {
            if (likely(PyList_CheckExact(__pyx_t_15))) {
              {
                Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_t_15);
                #if !CYTHON_ASSUME_SAFE_SIZE
                if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 245, __pyx_L1_error)
                #endif
                if (__pyx_t_21 >= __pyx_temp) break;
              }
              __pyx_t_14 = __Pyx_PyList_GetItemRefFast(__pyx_t_15, __pyx_t_21, __Pyx_ReferenceSharing_OwnStrongReference);
              ++__pyx_t_21;
            } else {
              {
                Py_ssize_t __pyx_temp = __Pyx_PyTuple_GET_SIZE(__pyx_t_15);
                #if !CYTHON_ASSUME_SAFE_SIZE
                if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 245, __pyx_L1_error)
                #endif
                if (__pyx_t_21 >= __pyx_temp) break;
              }
              #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
              __pyx_t_14 = __Pyx_NewRef(PyTuple_GET_ITEM(__pyx_t_15, __pyx_t_21));
              #else
              __pyx_t_14 = __Pyx_PySequence_ITEM(__pyx_t_15, __pyx_t_21);
              #endif
              ++__pyx_t_21;
            }
            if (unlikely(!__pyx_t_14)) __PYX_ERR(0, 245, __pyx_L1_error)
          } else {
            __pyx_t_14 = __pyx_t_22(__pyx_t_15);
            if (unlikely(!__pyx_t_14)) {
              PyObject* exc_type = PyErr_Occurred();
              if (exc_type) {
                if (unlikely(!__Pyx_PyErr_GivenExceptionMatches(exc_type, PyExc_StopIteration))) __PYX_ERR(0, 245, __pyx_L1_error)
                PyErr_Clear();
              }
              break;
            }
          }
          __Pyx_GOTREF(__pyx_t_14);
          __pyx_t_23 = __Pyx_PyLong_As_int(__pyx_t_14); if (unlikely((__pyx_t_23 == (int)-1) && PyErr_Occurred())) __PYX_ERR(0, 245, __pyx_L1_error)
          __Pyx_DECREF(__pyx_t_14); __pyx_t_14 = 0;
          __pyx_v_n_id = __pyx_t_23;

          if (__pyx_v_do_label_check) {
          } else {
            __pyx_t_1 = __pyx_v_do_label_check;
            goto __pyx_L25_bool_binop_done;
          }
          __pyx_t_14 = __Pyx_GetItemInt(__pyx_v_cg_labels, __pyx_v_n_id, int, 1, __Pyx_PyLong_From_int, 0, 0, 0, 1, __Pyx_ReferenceSharing_OwnStrongReference); if (unlikely(!__pyx_t_14)) __PYX_ERR(0, 246, __pyx_L1_error)
          __Pyx_GOTREF(__pyx_t_14);
          __pyx_t_8 = PyObject_RichCompare(__pyx_t_14, __pyx_v_expected_label, Py_NE); __Pyx_XGOTREF(__pyx_t_8); if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 246, __pyx_L1_error)
          __Pyx_DECREF(__pyx_t_14); __pyx_t_14 = 0;
          __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_t_8); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 246, __pyx_L1_error)
          __Pyx_DECREF(__pyx_t_8); __pyx_t_8 = 0;
          __pyx_t_1 = __pyx_t_2;
          __pyx_L25_bool_binop_done:;
          if (__pyx_t_1) {

            goto __pyx_L22_continue;

          }

          if (__pyx_v_has_prefilter) {

            __pyx_t_8 = __Pyx_PyLong_From_int(__pyx_v_n_id); if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 249, __pyx_L1_error)
            __Pyx_GOTREF(__pyx_t_8);
            __pyx_t_1 = (__Pyx_PySequence_ContainsTF(__pyx_t_8, __pyx_v_prefilter, Py_NE)); if (unlikely((__pyx_t_1 < 0))) __PYX_ERR(0, 249, __pyx_L1_error)
            __Pyx_DECREF(__pyx_t_8); __pyx_t_8 = 0;
            if (__pyx_t_1) {

              goto __pyx_L22_continue;

            }

            goto __pyx_L27;
          }

          if (__pyx_v_has_pw_eval) {

            __pyx_t_14 = NULL;
            __Pyx_INCREF(__pyx_v_pw_eval_fn);
            __pyx_t_4 = __pyx_v_pw_eval_fn;
            __pyx_t_9 = __Pyx_PyLong_From_int(__pyx_v_n_id); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 252, __pyx_L1_error)
            __Pyx_GOTREF(__pyx_t_9);
            __pyx_t_20 = 1;
            #if CYTHON_UNPACK_METHODS
            if (unlikely(PyMethod_Check(__pyx_t_4))) {
              __pyx_t_14 = PyMethod_GET_SELF(__pyx_t_4);
              assert(__pyx_t_14);
              PyObject* __pyx__function = PyMethod_GET_FUNCTION(__pyx_t_4);
              __Pyx_INCREF(__pyx_t_14);
              __Pyx_INCREF(__pyx__function);
              __Pyx_DECREF_SET(__pyx_t_4, __pyx__function);
              __pyx_t_20 = 0;
            }
            #endif
            {
              PyObject *__pyx_callargs[3] = {__pyx_t_14, __pyx_v_next_var, __pyx_t_9};
              __pyx_t_8 = __Pyx_PyObject_FastCall((PyObject*)__pyx_t_4, __pyx_callargs+__pyx_t_20, (3-__pyx_t_20) | (__pyx_t_20*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
              __Pyx_XDECREF(__pyx_t_14); __pyx_t_14 = 0;
              __Pyx_DECREF(__pyx_t_9); __pyx_t_9 = 0;
              __Pyx_DECREF(__pyx_t_4); __pyx_t_4 = 0;
              if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 252, __pyx_L1_error)
              __Pyx_GOTREF(__pyx_t_8);
            }
            __pyx_t_1 = __Pyx_PyObject_IsTrue(__pyx_t_8); if (unlikely((__pyx_t_1 < 0))) __PYX_ERR(0, 252, __pyx_L1_error)
            __Pyx_DECREF(__pyx_t_8); __pyx_t_8 = 0;
            __pyx_t_2 = (!__pyx_t_1);
            if (__pyx_t_2) {

              goto __pyx_L22_continue;

            }

          }
          __pyx_L27:;

          __pyx_t_8 = __Pyx_PyLong_From_int(__pyx_v_n_id); if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 254, __pyx_L1_error)
          __Pyx_GOTREF(__pyx_t_8);
          __pyx_t_17 = PySet_Add(__pyx_v_next_set, __pyx_t_8); if (unlikely(__pyx_t_17 == ((int)-1))) __PYX_ERR(0, 254, __pyx_L1_error)
          __Pyx_DECREF(__pyx_t_8); __pyx_t_8 = 0;

          __pyx_L22_continue:;
        }
        __Pyx_DECREF(__pyx_t_15); __pyx_t_15 = 0;

        __pyx_L18_continue:;
      }
      __Pyx_DECREF(__pyx_t_16); __pyx_t_16 = 0;

      {
        Py_ssize_t __pyx_temp = __Pyx_PySet_GET_SIZE(__pyx_v_next_set);
        if (unlikely(((!CYTHON_ASSUME_SAFE_SIZE) && __pyx_temp < 0))) __PYX_ERR(0, 256, __pyx_L1_error)
        __pyx_t_2 = (__pyx_temp != 0);
      }

      __pyx_t_1 = (!__pyx_t_2);
      if (__pyx_t_1) {

        __pyx_v_failed = 1;

        goto __pyx_L17_break;

      }

      if (unlikely((PyDict_SetItem(__pyx_v_result, __pyx_v_next_var, __pyx_v_next_set) < 0))) __PYX_ERR(0, 259, __pyx_L1_error)
    }
    __pyx_L17_break:;

    if (__pyx_v_failed) {

      if (unlikely((PyDict_SetItem(__pyx_v_out, __pyx_v_pivot_int, Py_None) < 0))) __PYX_ERR(0, 262, __pyx_L1_error)

      goto __pyx_L14_continue;

    }

    __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_v_has_w_filter); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 266, __pyx_L1_error)
    if (__pyx_t_2) {
    } else {
      __pyx_t_1 = __pyx_t_2;
      goto __pyx_L35_bool_binop_done;
    }
    __pyx_t_2 = (__Pyx_PyDict_ContainsTF(__pyx_v_last_var, __pyx_v_result, Py_EQ)); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 266, __pyx_L1_error)
    __pyx_t_1 = __pyx_t_2;
    __pyx_L35_bool_binop_done:;
    if (__pyx_t_1) {

      __pyx_t_16 = __Pyx_PyDict_GetItem(__pyx_v_result, __pyx_v_last_var); if (unlikely(!__pyx_t_16)) __PYX_ERR(0, 267, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_16);
      __pyx_t_15 = PyNumber_And(__pyx_t_16, __pyx_v_w_candidate_ints); if (unlikely(!__pyx_t_15)) __PYX_ERR(0, 267, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_15);
      __Pyx_DECREF(__pyx_t_16); __pyx_t_16 = 0;
      __Pyx_XDECREF_SET(__pyx_v_filtered, __pyx_t_15);
      __pyx_t_15 = 0;

      __pyx_t_1 = __Pyx_PyObject_IsTrue(__pyx_v_filtered); if (unlikely((__pyx_t_1 < 0))) __PYX_ERR(0, 268, __pyx_L1_error)
      __pyx_t_2 = (!__pyx_t_1);
      if (__pyx_t_2) {

        if (unlikely((PyDict_SetItem(__pyx_v_out, __pyx_v_pivot_int, Py_None) < 0))) __PYX_ERR(0, 269, __pyx_L1_error)

        goto __pyx_L14_continue;

      }

      if (unlikely((PyDict_SetItem(__pyx_v_result, __pyx_v_last_var, __pyx_v_filtered) < 0))) __PYX_ERR(0, 271, __pyx_L1_error)

    }

    if (unlikely((PyDict_SetItem(__pyx_v_out, __pyx_v_pivot_int, __pyx_v_result) < 0))) __PYX_ERR(0, 273, __pyx_L1_error)
    __pyx_L14_continue:;
  }

  __Pyx_XDECREF(__pyx_r);
  __Pyx_INCREF(__pyx_v_out);
  __pyx_r = __pyx_v_out;
  goto __pyx_L0;

  __pyx_L1_error:;
  __Pyx_XDECREF(__pyx_t_4);
  __Pyx_XDECREF(__pyx_t_8);
  __Pyx_XDECREF(__pyx_t_9);
  __Pyx_XDECREF(__pyx_t_14);
  __Pyx_XDECREF(__pyx_t_15);
  __Pyx_XDECREF(__pyx_t_16);
  __Pyx_AddTraceback("_fast_match.match_bridge_batch", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __pyx_r = NULL;
  __pyx_L0:;
  __Pyx_XDECREF(__pyx_v_out);
  __Pyx_XDECREF(__pyx_v_cg_out);
  __Pyx_XDECREF(__pyx_v_cg_in);
  __Pyx_XDECREF(__pyx_v_cg_labels);
  __Pyx_XDECREF(__pyx_v_hop_meta);
  __Pyx_XDECREF(__pyx_v_current_var);
  __Pyx_XDECREF(__pyx_v_next_var);
  __Pyx_XDECREF(__pyx_v_edge_info);
  __Pyx_XDECREF(__pyx_v_last_var);
  __Pyx_XDECREF(__pyx_v_has_w_filter);
  __Pyx_XDECREF(__pyx_v_pivot_int);
  __Pyx_XDECREF(__pyx_v_result);
  __Pyx_XDECREF(__pyx_v_edge_label);
  __Pyx_XDECREF(__pyx_v_expected_label);
  __Pyx_XDECREF(__pyx_v_prefilter);
  __Pyx_XDECREF(__pyx_v_current_set);
  __Pyx_XDECREF(__pyx_v_next_set);
  __Pyx_XDECREF(__pyx_v_current_int);
  __Pyx_XDECREF(__pyx_v_neighbors);
  __Pyx_XDECREF(__pyx_v_filtered);
  __Pyx_XGIVEREF(__pyx_r);
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}

static PyObject *__pyx_pw_11_fast_match_7derive_bridge_vw_pairs_fast(PyObject *__pyx_self,
#if CYTHON_METH_FASTCALL
PyObject *const *__pyx_args, Py_ssize_t __pyx_nargs, PyObject *__pyx_kwds
#else
PyObject *__pyx_args, PyObject *__pyx_kwds
#endif
);
PyDoc_STRVAR(__pyx_doc_11_fast_match_6derive_bridge_vw_pairs_fast, "\n    Recover per-anchor -> alternative reachability from stored bridge and\n    alt-star matches.\n\n    bridge_specs entries are:\n      (terminal_var, shared_vars, per_v_matches, w0_var)\n    ");
static PyMethodDef __pyx_mdef_11_fast_match_7derive_bridge_vw_pairs_fast = {"derive_bridge_vw_pairs_fast", (PyCFunction)(void(*)(void))(__Pyx_PyCFunction_FastCallWithKeywords)__pyx_pw_11_fast_match_7derive_bridge_vw_pairs_fast, __Pyx_METH_FASTCALL|METH_KEYWORDS, __pyx_doc_11_fast_match_6derive_bridge_vw_pairs_fast};
static PyObject *__pyx_pw_11_fast_match_7derive_bridge_vw_pairs_fast(PyObject *__pyx_self,
#if CYTHON_METH_FASTCALL
PyObject *const *__pyx_args, Py_ssize_t __pyx_nargs, PyObject *__pyx_kwds
#else
PyObject *__pyx_args, PyObject *__pyx_kwds
#endif
) {
  PyObject *__pyx_v_y0_cands = 0;
  PyObject *__pyx_v_w0_cands = 0;
  PyObject *__pyx_v_bridge_specs = 0;
  PyObject *__pyx_v_alt_matches_per_w = 0;
  PyObject *__pyx_v_F_phi = 0;
  #if !CYTHON_METH_FASTCALL
  CYTHON_UNUSED Py_ssize_t __pyx_nargs;
  #endif
  CYTHON_UNUSED PyObject *const *__pyx_kwvalues;
  PyObject* values[5] = {0,0,0,0,0};
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  PyObject *__pyx_r = 0;
  __Pyx_RefNannyDeclarations
  __Pyx_RefNannySetupContext("derive_bridge_vw_pairs_fast (wrapper)", 0);
  #if !CYTHON_METH_FASTCALL
  #if CYTHON_ASSUME_SAFE_SIZE
  __pyx_nargs = PyTuple_GET_SIZE(__pyx_args);
  #else
  __pyx_nargs = PyTuple_Size(__pyx_args); if (unlikely(__pyx_nargs < 0)) return NULL;
  #endif
  #endif
  __pyx_kwvalues = __Pyx_KwValues_FASTCALL(__pyx_args, __pyx_nargs);
  {
    PyObject ** const __pyx_pyargnames[] = {&__pyx_mstate_global->__pyx_n_u_y0_cands,&__pyx_mstate_global->__pyx_n_u_w0_cands,&__pyx_mstate_global->__pyx_n_u_bridge_specs,&__pyx_mstate_global->__pyx_n_u_alt_matches_per_w,&__pyx_mstate_global->__pyx_n_u_F_phi,0};
    const Py_ssize_t __pyx_kwds_len = (__pyx_kwds) ? __Pyx_NumKwargs_FASTCALL(__pyx_kwds) : 0;
    if (unlikely(__pyx_kwds_len) < 0) __PYX_ERR(0, 278, __pyx_L3_error)
    if (__pyx_kwds_len > 0) {
      switch (__pyx_nargs) {
        case  5:
        values[4] = __Pyx_ArgRef_FASTCALL(__pyx_args, 4);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[4])) __PYX_ERR(0, 278, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  4:
        values[3] = __Pyx_ArgRef_FASTCALL(__pyx_args, 3);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[3])) __PYX_ERR(0, 278, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  3:
        values[2] = __Pyx_ArgRef_FASTCALL(__pyx_args, 2);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[2])) __PYX_ERR(0, 278, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  2:
        values[1] = __Pyx_ArgRef_FASTCALL(__pyx_args, 1);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[1])) __PYX_ERR(0, 278, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  1:
        values[0] = __Pyx_ArgRef_FASTCALL(__pyx_args, 0);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[0])) __PYX_ERR(0, 278, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  0: break;
        default: goto __pyx_L5_argtuple_error;
      }
      const Py_ssize_t kwd_pos_args = __pyx_nargs;
      if (__Pyx_ParseKeywords(__pyx_kwds, __pyx_kwvalues, __pyx_pyargnames, 0, values, kwd_pos_args, __pyx_kwds_len, "derive_bridge_vw_pairs_fast", 0) < (0)) __PYX_ERR(0, 278, __pyx_L3_error)
      for (Py_ssize_t i = __pyx_nargs; i < 5; i++) {
        if (unlikely(!values[i])) { __Pyx_RaiseArgtupleInvalid("derive_bridge_vw_pairs_fast", 1, 5, 5, i); __PYX_ERR(0, 278, __pyx_L3_error) }
      }
    } else if (unlikely(__pyx_nargs != 5)) {
      goto __pyx_L5_argtuple_error;
    } else {
      values[0] = __Pyx_ArgRef_FASTCALL(__pyx_args, 0);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[0])) __PYX_ERR(0, 278, __pyx_L3_error)
      values[1] = __Pyx_ArgRef_FASTCALL(__pyx_args, 1);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[1])) __PYX_ERR(0, 278, __pyx_L3_error)
      values[2] = __Pyx_ArgRef_FASTCALL(__pyx_args, 2);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[2])) __PYX_ERR(0, 278, __pyx_L3_error)
      values[3] = __Pyx_ArgRef_FASTCALL(__pyx_args, 3);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[3])) __PYX_ERR(0, 278, __pyx_L3_error)
      values[4] = __Pyx_ArgRef_FASTCALL(__pyx_args, 4);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[4])) __PYX_ERR(0, 278, __pyx_L3_error)
    }
    __pyx_v_y0_cands = values[0];
    __pyx_v_w0_cands = values[1];
    __pyx_v_bridge_specs = values[2];
    __pyx_v_alt_matches_per_w = ((PyObject*)values[3]);
    __pyx_v_F_phi = ((PyObject*)values[4]);
  }
  goto __pyx_L6_skip;
  __pyx_L5_argtuple_error:;
  __Pyx_RaiseArgtupleInvalid("derive_bridge_vw_pairs_fast", 1, 5, 5, __pyx_nargs); __PYX_ERR(0, 278, __pyx_L3_error)
  __pyx_L6_skip:;
  goto __pyx_L4_argument_unpacking_done;
  __pyx_L3_error:;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  __Pyx_AddTraceback("_fast_match.derive_bridge_vw_pairs_fast", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __Pyx_RefNannyFinishContext();
  return NULL;
  __pyx_L4_argument_unpacking_done:;
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_alt_matches_per_w), (&PyDict_Type), 1, "alt_matches_per_w", 1))) __PYX_ERR(0, 278, __pyx_L1_error)
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_F_phi), (&PyDict_Type), 1, "F_phi", 1))) __PYX_ERR(0, 278, __pyx_L1_error)
  __pyx_r = __pyx_pf_11_fast_match_6derive_bridge_vw_pairs_fast(__pyx_self, __pyx_v_y0_cands, __pyx_v_w0_cands, __pyx_v_bridge_specs, __pyx_v_alt_matches_per_w, __pyx_v_F_phi);

  goto __pyx_L0;
  __pyx_L1_error:;
  __pyx_r = NULL;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  goto __pyx_L7_cleaned_up;
  __pyx_L0:;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  __pyx_L7_cleaned_up:;
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}

static PyObject *__pyx_pf_11_fast_match_6derive_bridge_vw_pairs_fast(CYTHON_UNUSED PyObject *__pyx_self, PyObject *__pyx_v_y0_cands, PyObject *__pyx_v_w0_cands, PyObject *__pyx_v_bridge_specs, PyObject *__pyx_v_alt_matches_per_w, PyObject *__pyx_v_F_phi) {
  PyObject *__pyx_v_bridge_vw = 0;
  PyObject *__pyx_v_valid_ws = 0;
  PyObject *__pyx_v_path_ws = 0;
  PyObject *__pyx_v_pivot_match = 0;
  PyObject *__pyx_v_w_match = 0;
  PyObject *__pyx_v_terminal_var = 0;
  PyObject *__pyx_v_shared_vars = 0;
  PyObject *__pyx_v_per_v_matches = 0;
  PyObject *__pyx_v_w0_var = 0;
  PyObject *__pyx_v_shared_var = 0;
  PyObject *__pyx_v_bridge_cands = 0;
  PyObject *__pyx_v_alt_cands = 0;
  PyObject *__pyx_v_v = NULL;
  PyObject *__pyx_v_w = NULL;
  int __pyx_v_join_ok;
  PyObject *__pyx_7genexpr__pyx_v_v = NULL;
  PyObject *__pyx_r = NULL;
  __Pyx_RefNannyDeclarations
  PyObject *__pyx_t_1 = NULL;
  int __pyx_t_2;
  int __pyx_t_3;
  int __pyx_t_4;
  PyObject *__pyx_t_5 = NULL;
  Py_ssize_t __pyx_t_6;
  PyObject *(*__pyx_t_7)(PyObject *);
  PyObject *__pyx_t_8 = NULL;
  Py_ssize_t __pyx_t_9;
  PyObject *(*__pyx_t_10)(PyObject *);
  PyObject *__pyx_t_11 = NULL;
  PyObject *__pyx_t_12 = NULL;
  PyObject *__pyx_t_13 = NULL;
  PyObject *__pyx_t_14 = NULL;
  PyObject *__pyx_t_15 = NULL;
  PyObject *(*__pyx_t_16)(PyObject *);
  size_t __pyx_t_17;
  Py_ssize_t __pyx_t_18;
  PyObject *(*__pyx_t_19)(PyObject *);
  Py_ssize_t __pyx_t_20;
  PyObject *(*__pyx_t_21)(PyObject *);
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  __Pyx_RefNannySetupContext("derive_bridge_vw_pairs_fast", 0);

  __pyx_t_1 = __Pyx_PyDict_NewPresized(0); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 286, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_1);
  __pyx_v_bridge_vw = __pyx_t_1;
  __pyx_t_1 = 0;

  __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_v_bridge_specs); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 291, __pyx_L1_error)
  __pyx_t_3 = (!__pyx_t_2);
  if (__pyx_t_3) {

    __Pyx_XDECREF(__pyx_r);
    __pyx_r = Py_None; __Pyx_INCREF(Py_None);
    goto __pyx_L0;

  }

  __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_v_y0_cands); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 294, __pyx_L1_error)
  __pyx_t_4 = (!__pyx_t_2);
  if (!__pyx_t_4) {
  } else {
    __pyx_t_3 = __pyx_t_4;
    goto __pyx_L5_bool_binop_done;
  }
  __pyx_t_4 = __Pyx_PyObject_IsTrue(__pyx_v_w0_cands); if (unlikely((__pyx_t_4 < 0))) __PYX_ERR(0, 294, __pyx_L1_error)
  __pyx_t_2 = (!__pyx_t_4);
  __pyx_t_3 = __pyx_t_2;
  __pyx_L5_bool_binop_done:;
  if (__pyx_t_3) {

    __Pyx_XDECREF(__pyx_r);
    {
      __pyx_t_1 = PyDict_New(); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 295, __pyx_L9_error)
      __Pyx_GOTREF(__pyx_t_1);
      if (likely(PyList_CheckExact(__pyx_v_y0_cands)) || PyTuple_CheckExact(__pyx_v_y0_cands)) {
        __pyx_t_5 = __pyx_v_y0_cands; __Pyx_INCREF(__pyx_t_5);
        __pyx_t_6 = 0;
        __pyx_t_7 = NULL;
      } else {
        __pyx_t_6 = -1; __pyx_t_5 = PyObject_GetIter(__pyx_v_y0_cands); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 295, __pyx_L9_error)
        __Pyx_GOTREF(__pyx_t_5);
        __pyx_t_7 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_5); if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 295, __pyx_L9_error)
      }
      for (;;) {
        if (likely(!__pyx_t_7)) {
          if (likely(PyList_CheckExact(__pyx_t_5))) {
            {
              Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_t_5);
              #if !CYTHON_ASSUME_SAFE_SIZE
              if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 295, __pyx_L9_error)
              #endif
              if (__pyx_t_6 >= __pyx_temp) break;
            }
            __pyx_t_8 = __Pyx_PyList_GetItemRefFast(__pyx_t_5, __pyx_t_6, __Pyx_ReferenceSharing_OwnStrongReference);
            ++__pyx_t_6;
          } else {
            {
              Py_ssize_t __pyx_temp = __Pyx_PyTuple_GET_SIZE(__pyx_t_5);
              #if !CYTHON_ASSUME_SAFE_SIZE
              if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 295, __pyx_L9_error)
              #endif
              if (__pyx_t_6 >= __pyx_temp) break;
            }
            #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
            __pyx_t_8 = __Pyx_NewRef(PyTuple_GET_ITEM(__pyx_t_5, __pyx_t_6));
            #else
            __pyx_t_8 = __Pyx_PySequence_ITEM(__pyx_t_5, __pyx_t_6);
            #endif
            ++__pyx_t_6;
          }
          if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 295, __pyx_L9_error)
        } else {
          __pyx_t_8 = __pyx_t_7(__pyx_t_5);
          if (unlikely(!__pyx_t_8)) {
            PyObject* exc_type = PyErr_Occurred();
            if (exc_type) {
              if (unlikely(!__Pyx_PyErr_GivenExceptionMatches(exc_type, PyExc_StopIteration))) __PYX_ERR(0, 295, __pyx_L9_error)
              PyErr_Clear();
            }
            break;
          }
        }
        __Pyx_GOTREF(__pyx_t_8);
        __Pyx_XDECREF_SET(__pyx_7genexpr__pyx_v_v, __pyx_t_8);
        __pyx_t_8 = 0;
        __pyx_t_8 = PySet_New(0); if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 295, __pyx_L9_error)
        __Pyx_GOTREF(__pyx_t_8);
        if (unlikely(PyDict_SetItem(__pyx_t_1, (PyObject*)__pyx_7genexpr__pyx_v_v, (PyObject*)__pyx_t_8))) __PYX_ERR(0, 295, __pyx_L9_error)
        __Pyx_DECREF(__pyx_t_8); __pyx_t_8 = 0;
      }
      __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
      __Pyx_XDECREF(__pyx_7genexpr__pyx_v_v); __pyx_7genexpr__pyx_v_v = 0;
      goto __pyx_L13_exit_scope;
      __pyx_L9_error:;
      __Pyx_XDECREF(__pyx_7genexpr__pyx_v_v); __pyx_7genexpr__pyx_v_v = 0;
      goto __pyx_L1_error;
      __pyx_L13_exit_scope:;
    }
    __pyx_r = __pyx_t_1;
    __pyx_t_1 = 0;
    goto __pyx_L0;

  }

  if (likely(PyList_CheckExact(__pyx_v_y0_cands)) || PyTuple_CheckExact(__pyx_v_y0_cands)) {
    __pyx_t_1 = __pyx_v_y0_cands; __Pyx_INCREF(__pyx_t_1);
    __pyx_t_6 = 0;
    __pyx_t_7 = NULL;
  } else {
    __pyx_t_6 = -1; __pyx_t_1 = PyObject_GetIter(__pyx_v_y0_cands); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 297, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_1);
    __pyx_t_7 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_1); if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 297, __pyx_L1_error)
  }
  for (;;) {
    if (likely(!__pyx_t_7)) {
      if (likely(PyList_CheckExact(__pyx_t_1))) {
        {
          Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_t_1);
          #if !CYTHON_ASSUME_SAFE_SIZE
          if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 297, __pyx_L1_error)
          #endif
          if (__pyx_t_6 >= __pyx_temp) break;
        }
        __pyx_t_5 = __Pyx_PyList_GetItemRefFast(__pyx_t_1, __pyx_t_6, __Pyx_ReferenceSharing_OwnStrongReference);
        ++__pyx_t_6;
      } else {
        {
          Py_ssize_t __pyx_temp = __Pyx_PyTuple_GET_SIZE(__pyx_t_1);
          #if !CYTHON_ASSUME_SAFE_SIZE
          if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 297, __pyx_L1_error)
          #endif
          if (__pyx_t_6 >= __pyx_temp) break;
        }
        #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
        __pyx_t_5 = __Pyx_NewRef(PyTuple_GET_ITEM(__pyx_t_1, __pyx_t_6));
        #else
        __pyx_t_5 = __Pyx_PySequence_ITEM(__pyx_t_1, __pyx_t_6);
        #endif
        ++__pyx_t_6;
      }
      if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 297, __pyx_L1_error)
    } else {
      __pyx_t_5 = __pyx_t_7(__pyx_t_1);
      if (unlikely(!__pyx_t_5)) {
        PyObject* exc_type = PyErr_Occurred();
        if (exc_type) {
          if (unlikely(!__Pyx_PyErr_GivenExceptionMatches(exc_type, PyExc_StopIteration))) __PYX_ERR(0, 297, __pyx_L1_error)
          PyErr_Clear();
        }
        break;
      }
    }
    __Pyx_GOTREF(__pyx_t_5);
    __Pyx_XDECREF_SET(__pyx_v_v, __pyx_t_5);
    __pyx_t_5 = 0;

    __Pyx_INCREF(Py_None);
    __Pyx_XDECREF_SET(__pyx_v_valid_ws, Py_None);

    if (likely(PyList_CheckExact(__pyx_v_bridge_specs)) || PyTuple_CheckExact(__pyx_v_bridge_specs)) {
      __pyx_t_5 = __pyx_v_bridge_specs; __Pyx_INCREF(__pyx_t_5);
      __pyx_t_9 = 0;
      __pyx_t_10 = NULL;
    } else {
      __pyx_t_9 = -1; __pyx_t_5 = PyObject_GetIter(__pyx_v_bridge_specs); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 299, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_5);
      __pyx_t_10 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_5); if (unlikely(!__pyx_t_10)) __PYX_ERR(0, 299, __pyx_L1_error)
    }
    for (;;) {
      if (likely(!__pyx_t_10)) {
        if (likely(PyList_CheckExact(__pyx_t_5))) {
          {
            Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_t_5);
            #if !CYTHON_ASSUME_SAFE_SIZE
            if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 299, __pyx_L1_error)
            #endif
            if (__pyx_t_9 >= __pyx_temp) break;
          }
          __pyx_t_8 = __Pyx_PyList_GetItemRefFast(__pyx_t_5, __pyx_t_9, __Pyx_ReferenceSharing_OwnStrongReference);
          ++__pyx_t_9;
        } else {
          {
            Py_ssize_t __pyx_temp = __Pyx_PyTuple_GET_SIZE(__pyx_t_5);
            #if !CYTHON_ASSUME_SAFE_SIZE
            if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 299, __pyx_L1_error)
            #endif
            if (__pyx_t_9 >= __pyx_temp) break;
          }
          #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
          __pyx_t_8 = __Pyx_NewRef(PyTuple_GET_ITEM(__pyx_t_5, __pyx_t_9));
          #else
          __pyx_t_8 = __Pyx_PySequence_ITEM(__pyx_t_5, __pyx_t_9);
          #endif
          ++__pyx_t_9;
        }
        if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 299, __pyx_L1_error)
      } else {
        __pyx_t_8 = __pyx_t_10(__pyx_t_5);
        if (unlikely(!__pyx_t_8)) {
          PyObject* exc_type = PyErr_Occurred();
          if (exc_type) {
            if (unlikely(!__Pyx_PyErr_GivenExceptionMatches(exc_type, PyExc_StopIteration))) __PYX_ERR(0, 299, __pyx_L1_error)
            PyErr_Clear();
          }
          break;
        }
      }
      __Pyx_GOTREF(__pyx_t_8);
      if ((likely(PyTuple_CheckExact(__pyx_t_8))) || (PyList_CheckExact(__pyx_t_8))) {
        PyObject* sequence = __pyx_t_8;
        Py_ssize_t size = __Pyx_PySequence_SIZE(sequence);
        if (unlikely(size != 4)) {
          if (size > 4) __Pyx_RaiseTooManyValuesError(4);
          else if (size >= 0) __Pyx_RaiseNeedMoreValuesError(size);
          __PYX_ERR(0, 299, __pyx_L1_error)
        }
        #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
        if (likely(PyTuple_CheckExact(sequence))) {
          __pyx_t_11 = PyTuple_GET_ITEM(sequence, 0);
          __Pyx_INCREF(__pyx_t_11);
          __pyx_t_12 = PyTuple_GET_ITEM(sequence, 1);
          __Pyx_INCREF(__pyx_t_12);
          __pyx_t_13 = PyTuple_GET_ITEM(sequence, 2);
          __Pyx_INCREF(__pyx_t_13);
          __pyx_t_14 = PyTuple_GET_ITEM(sequence, 3);
          __Pyx_INCREF(__pyx_t_14);
        } else {
          __pyx_t_11 = __Pyx_PyList_GetItemRefFast(sequence, 0, __Pyx_ReferenceSharing_SharedReference);
          if (unlikely(!__pyx_t_11)) __PYX_ERR(0, 299, __pyx_L1_error)
          __Pyx_XGOTREF(__pyx_t_11);
          __pyx_t_12 = __Pyx_PyList_GetItemRefFast(sequence, 1, __Pyx_ReferenceSharing_SharedReference);
          if (unlikely(!__pyx_t_12)) __PYX_ERR(0, 299, __pyx_L1_error)
          __Pyx_XGOTREF(__pyx_t_12);
          __pyx_t_13 = __Pyx_PyList_GetItemRefFast(sequence, 2, __Pyx_ReferenceSharing_SharedReference);
          if (unlikely(!__pyx_t_13)) __PYX_ERR(0, 299, __pyx_L1_error)
          __Pyx_XGOTREF(__pyx_t_13);
          __pyx_t_14 = __Pyx_PyList_GetItemRefFast(sequence, 3, __Pyx_ReferenceSharing_SharedReference);
          if (unlikely(!__pyx_t_14)) __PYX_ERR(0, 299, __pyx_L1_error)
          __Pyx_XGOTREF(__pyx_t_14);
        }
        #else
        {
          Py_ssize_t i;
          PyObject** temps[4] = {&__pyx_t_11,&__pyx_t_12,&__pyx_t_13,&__pyx_t_14};
          for (i=0; i < 4; i++) {
            PyObject* item = __Pyx_PySequence_ITEM(sequence, i); if (unlikely(!item)) __PYX_ERR(0, 299, __pyx_L1_error)
            __Pyx_GOTREF(item);
            *(temps[i]) = item;
          }
        }
        #endif
        __Pyx_DECREF(__pyx_t_8); __pyx_t_8 = 0;
      } else {
        Py_ssize_t index = -1;
        PyObject** temps[4] = {&__pyx_t_11,&__pyx_t_12,&__pyx_t_13,&__pyx_t_14};
        __pyx_t_15 = PyObject_GetIter(__pyx_t_8); if (unlikely(!__pyx_t_15)) __PYX_ERR(0, 299, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_15);
        __Pyx_DECREF(__pyx_t_8); __pyx_t_8 = 0;
        __pyx_t_16 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_15);
        for (index=0; index < 4; index++) {
          PyObject* item = __pyx_t_16(__pyx_t_15); if (unlikely(!item)) goto __pyx_L18_unpacking_failed;
          __Pyx_GOTREF(item);
          *(temps[index]) = item;
        }
        if (__Pyx_IternextUnpackEndCheck(__pyx_t_16(__pyx_t_15), 4) < (0)) __PYX_ERR(0, 299, __pyx_L1_error)
        __pyx_t_16 = NULL;
        __Pyx_DECREF(__pyx_t_15); __pyx_t_15 = 0;
        goto __pyx_L19_unpacking_done;
        __pyx_L18_unpacking_failed:;
        __Pyx_DECREF(__pyx_t_15); __pyx_t_15 = 0;
        __pyx_t_16 = NULL;
        if (__Pyx_IterFinish() == 0) __Pyx_RaiseNeedMoreValuesError(index);
        __PYX_ERR(0, 299, __pyx_L1_error)
        __pyx_L19_unpacking_done:;
      }
      __Pyx_XDECREF_SET(__pyx_v_terminal_var, __pyx_t_11);
      __pyx_t_11 = 0;
      __Pyx_XDECREF_SET(__pyx_v_shared_vars, __pyx_t_12);
      __pyx_t_12 = 0;
      __Pyx_XDECREF_SET(__pyx_v_per_v_matches, __pyx_t_13);
      __pyx_t_13 = 0;
      __Pyx_XDECREF_SET(__pyx_v_w0_var, __pyx_t_14);
      __pyx_t_14 = 0;

      __pyx_t_14 = __pyx_v_per_v_matches;
      __Pyx_INCREF(__pyx_t_14);
      __pyx_t_17 = 0;
      {
        PyObject *__pyx_callargs[2] = {__pyx_t_14, __pyx_v_v};
        __pyx_t_8 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_get, __pyx_callargs+__pyx_t_17, (2-__pyx_t_17) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
        __Pyx_XDECREF(__pyx_t_14); __pyx_t_14 = 0;
        if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 300, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_8);
      }
      __Pyx_XDECREF_SET(__pyx_v_pivot_match, __pyx_t_8);
      __pyx_t_8 = 0;

      __pyx_t_3 = __Pyx_PyObject_IsTrue(__pyx_v_pivot_match); if (unlikely((__pyx_t_3 < 0))) __PYX_ERR(0, 301, __pyx_L1_error)
      __pyx_t_2 = (!__pyx_t_3);
      if (__pyx_t_2) {

        __pyx_t_8 = PySet_New(0); if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 302, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_8);
        __Pyx_XDECREF_SET(__pyx_v_path_ws, __pyx_t_8);
        __pyx_t_8 = 0;

        goto __pyx_L20;
      }

      __pyx_t_8 = PyObject_RichCompare(__pyx_v_terminal_var, __pyx_v_w0_var, Py_EQ); __Pyx_XGOTREF(__pyx_t_8); if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 303, __pyx_L1_error)
      __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_t_8); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 303, __pyx_L1_error)
      __Pyx_DECREF(__pyx_t_8); __pyx_t_8 = 0;
      if (__pyx_t_2) {

        __pyx_t_14 = __pyx_v_pivot_match;
        __Pyx_INCREF(__pyx_t_14);
        __pyx_t_13 = PySet_New(0); if (unlikely(!__pyx_t_13)) __PYX_ERR(0, 304, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_13);
        __pyx_t_17 = 0;
        {
          PyObject *__pyx_callargs[3] = {__pyx_t_14, __pyx_v_w0_var, __pyx_t_13};
          __pyx_t_8 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_get, __pyx_callargs+__pyx_t_17, (3-__pyx_t_17) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
          __Pyx_XDECREF(__pyx_t_14); __pyx_t_14 = 0;
          __Pyx_DECREF(__pyx_t_13); __pyx_t_13 = 0;
          if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 304, __pyx_L1_error)
          __Pyx_GOTREF(__pyx_t_8);
        }
        __pyx_t_13 = PyNumber_And(__pyx_t_8, __pyx_v_w0_cands); if (unlikely(!__pyx_t_13)) __PYX_ERR(0, 304, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_13);
        __Pyx_DECREF(__pyx_t_8); __pyx_t_8 = 0;
        __Pyx_XDECREF_SET(__pyx_v_path_ws, __pyx_t_13);
        __pyx_t_13 = 0;

        goto __pyx_L20;
      }

      __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_v_shared_vars); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 305, __pyx_L1_error)
      __pyx_t_3 = (!__pyx_t_2);
      if (__pyx_t_3) {

        __pyx_t_13 = PySet_New(0); if (unlikely(!__pyx_t_13)) __PYX_ERR(0, 306, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_13);
        __Pyx_XDECREF_SET(__pyx_v_path_ws, __pyx_t_13);
        __pyx_t_13 = 0;

        goto __pyx_L20;
      }

       {
        __pyx_t_13 = PySet_New(0); if (unlikely(!__pyx_t_13)) __PYX_ERR(0, 308, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_13);
        __Pyx_XDECREF_SET(__pyx_v_path_ws, __pyx_t_13);
        __pyx_t_13 = 0;

        if (likely(PyList_CheckExact(__pyx_v_w0_cands)) || PyTuple_CheckExact(__pyx_v_w0_cands)) {
          __pyx_t_13 = __pyx_v_w0_cands; __Pyx_INCREF(__pyx_t_13);
          __pyx_t_18 = 0;
          __pyx_t_19 = NULL;
        } else {
          __pyx_t_18 = -1; __pyx_t_13 = PyObject_GetIter(__pyx_v_w0_cands); if (unlikely(!__pyx_t_13)) __PYX_ERR(0, 309, __pyx_L1_error)
          __Pyx_GOTREF(__pyx_t_13);
          __pyx_t_19 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_13); if (unlikely(!__pyx_t_19)) __PYX_ERR(0, 309, __pyx_L1_error)
        }
        for (;;) {
          if (likely(!__pyx_t_19)) {
            if (likely(PyList_CheckExact(__pyx_t_13))) {
              {
                Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_t_13);
                #if !CYTHON_ASSUME_SAFE_SIZE
                if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 309, __pyx_L1_error)
                #endif
                if (__pyx_t_18 >= __pyx_temp) break;
              }
              __pyx_t_8 = __Pyx_PyList_GetItemRefFast(__pyx_t_13, __pyx_t_18, __Pyx_ReferenceSharing_OwnStrongReference);
              ++__pyx_t_18;
            } else {
              {
                Py_ssize_t __pyx_temp = __Pyx_PyTuple_GET_SIZE(__pyx_t_13);
                #if !CYTHON_ASSUME_SAFE_SIZE
                if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 309, __pyx_L1_error)
                #endif
                if (__pyx_t_18 >= __pyx_temp) break;
              }
              #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
              __pyx_t_8 = __Pyx_NewRef(PyTuple_GET_ITEM(__pyx_t_13, __pyx_t_18));
              #else
              __pyx_t_8 = __Pyx_PySequence_ITEM(__pyx_t_13, __pyx_t_18);
              #endif
              ++__pyx_t_18;
            }
            if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 309, __pyx_L1_error)
          } else {
            __pyx_t_8 = __pyx_t_19(__pyx_t_13);
            if (unlikely(!__pyx_t_8)) {
              PyObject* exc_type = PyErr_Occurred();
              if (exc_type) {
                if (unlikely(!__Pyx_PyErr_GivenExceptionMatches(exc_type, PyExc_StopIteration))) __PYX_ERR(0, 309, __pyx_L1_error)
                PyErr_Clear();
              }
              break;
            }
          }
          __Pyx_GOTREF(__pyx_t_8);
          __Pyx_XDECREF_SET(__pyx_v_w, __pyx_t_8);
          __pyx_t_8 = 0;

          if (unlikely(__pyx_v_alt_matches_per_w == Py_None)) {
            PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "get");
            __PYX_ERR(0, 310, __pyx_L1_error)
          }
          __pyx_t_8 = __Pyx_PyDict_GetItemDefault(__pyx_v_alt_matches_per_w, __pyx_v_w, Py_None); if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 310, __pyx_L1_error)
          __Pyx_GOTREF(__pyx_t_8);
          __Pyx_XDECREF_SET(__pyx_v_w_match, __pyx_t_8);
          __pyx_t_8 = 0;

          __pyx_t_3 = (__pyx_v_w_match == Py_None);
          if (__pyx_t_3) {

            goto __pyx_L21_continue;

          }

          __pyx_v_join_ok = 1;

          if (likely(PyList_CheckExact(__pyx_v_shared_vars)) || PyTuple_CheckExact(__pyx_v_shared_vars)) {
            __pyx_t_8 = __pyx_v_shared_vars; __Pyx_INCREF(__pyx_t_8);
            __pyx_t_20 = 0;
            __pyx_t_21 = NULL;
          } else {
            __pyx_t_20 = -1; __pyx_t_8 = PyObject_GetIter(__pyx_v_shared_vars); if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 314, __pyx_L1_error)
            __Pyx_GOTREF(__pyx_t_8);
            __pyx_t_21 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_8); if (unlikely(!__pyx_t_21)) __PYX_ERR(0, 314, __pyx_L1_error)
          }
          for (;;) {
            if (likely(!__pyx_t_21)) {
              if (likely(PyList_CheckExact(__pyx_t_8))) {
                {
                  Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_t_8);
                  #if !CYTHON_ASSUME_SAFE_SIZE
                  if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 314, __pyx_L1_error)
                  #endif
                  if (__pyx_t_20 >= __pyx_temp) break;
                }
                __pyx_t_14 = __Pyx_PyList_GetItemRefFast(__pyx_t_8, __pyx_t_20, __Pyx_ReferenceSharing_OwnStrongReference);
                ++__pyx_t_20;
              } else {
                {
                  Py_ssize_t __pyx_temp = __Pyx_PyTuple_GET_SIZE(__pyx_t_8);
                  #if !CYTHON_ASSUME_SAFE_SIZE
                  if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 314, __pyx_L1_error)
                  #endif
                  if (__pyx_t_20 >= __pyx_temp) break;
                }
                #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
                __pyx_t_14 = __Pyx_NewRef(PyTuple_GET_ITEM(__pyx_t_8, __pyx_t_20));
                #else
                __pyx_t_14 = __Pyx_PySequence_ITEM(__pyx_t_8, __pyx_t_20);
                #endif
                ++__pyx_t_20;
              }
              if (unlikely(!__pyx_t_14)) __PYX_ERR(0, 314, __pyx_L1_error)
            } else {
              __pyx_t_14 = __pyx_t_21(__pyx_t_8);
              if (unlikely(!__pyx_t_14)) {
                PyObject* exc_type = PyErr_Occurred();
                if (exc_type) {
                  if (unlikely(!__Pyx_PyErr_GivenExceptionMatches(exc_type, PyExc_StopIteration))) __PYX_ERR(0, 314, __pyx_L1_error)
                  PyErr_Clear();
                }
                break;
              }
            }
            __Pyx_GOTREF(__pyx_t_14);
            __Pyx_XDECREF_SET(__pyx_v_shared_var, __pyx_t_14);
            __pyx_t_14 = 0;

            __pyx_t_12 = __pyx_v_pivot_match;
            __Pyx_INCREF(__pyx_t_12);
            __pyx_t_11 = PySet_New(0); if (unlikely(!__pyx_t_11)) __PYX_ERR(0, 315, __pyx_L1_error)
            __Pyx_GOTREF(__pyx_t_11);
            __pyx_t_17 = 0;
            {
              PyObject *__pyx_callargs[3] = {__pyx_t_12, __pyx_v_shared_var, __pyx_t_11};
              __pyx_t_14 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_get, __pyx_callargs+__pyx_t_17, (3-__pyx_t_17) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
              __Pyx_XDECREF(__pyx_t_12); __pyx_t_12 = 0;
              __Pyx_DECREF(__pyx_t_11); __pyx_t_11 = 0;
              if (unlikely(!__pyx_t_14)) __PYX_ERR(0, 315, __pyx_L1_error)
              __Pyx_GOTREF(__pyx_t_14);
            }
            __pyx_t_11 = PySet_New(__pyx_t_14); if (unlikely(!__pyx_t_11)) __PYX_ERR(0, 315, __pyx_L1_error)
            __Pyx_GOTREF(__pyx_t_11);
            __Pyx_DECREF(__pyx_t_14); __pyx_t_14 = 0;
            __Pyx_XDECREF_SET(__pyx_v_bridge_cands, __pyx_t_11);
            __pyx_t_11 = 0;

            __pyx_t_14 = __pyx_v_w_match;
            __Pyx_INCREF(__pyx_t_14);
            __pyx_t_12 = PySet_New(0); if (unlikely(!__pyx_t_12)) __PYX_ERR(0, 316, __pyx_L1_error)
            __Pyx_GOTREF(__pyx_t_12);
            __pyx_t_17 = 0;
            {
              PyObject *__pyx_callargs[3] = {__pyx_t_14, __pyx_v_shared_var, __pyx_t_12};
              __pyx_t_11 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_get, __pyx_callargs+__pyx_t_17, (3-__pyx_t_17) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
              __Pyx_XDECREF(__pyx_t_14); __pyx_t_14 = 0;
              __Pyx_DECREF(__pyx_t_12); __pyx_t_12 = 0;
              if (unlikely(!__pyx_t_11)) __PYX_ERR(0, 316, __pyx_L1_error)
              __Pyx_GOTREF(__pyx_t_11);
            }
            __pyx_t_12 = PySet_New(__pyx_t_11); if (unlikely(!__pyx_t_12)) __PYX_ERR(0, 316, __pyx_L1_error)
            __Pyx_GOTREF(__pyx_t_12);
            __Pyx_DECREF(__pyx_t_11); __pyx_t_11 = 0;
            __Pyx_XDECREF_SET(__pyx_v_alt_cands, __pyx_t_12);
            __pyx_t_12 = 0;

            if (unlikely(__pyx_v_F_phi == Py_None)) {
              PyErr_SetString(PyExc_TypeError, "'NoneType' object is not iterable");
              __PYX_ERR(0, 317, __pyx_L1_error)
            }
            __pyx_t_3 = (__Pyx_PyDict_ContainsTF(__pyx_v_shared_var, __pyx_v_F_phi, Py_EQ)); if (unlikely((__pyx_t_3 < 0))) __PYX_ERR(0, 317, __pyx_L1_error)
            if (__pyx_t_3) {

              if (unlikely(__pyx_v_F_phi == Py_None)) {
                PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
                __PYX_ERR(0, 318, __pyx_L1_error)
              }
              __pyx_t_12 = __Pyx_PyDict_GetItem(__pyx_v_F_phi, __pyx_v_shared_var); if (unlikely(!__pyx_t_12)) __PYX_ERR(0, 318, __pyx_L1_error)
              __Pyx_GOTREF(__pyx_t_12);
              __pyx_t_11 = PyNumber_InPlaceAnd(__pyx_v_bridge_cands, __pyx_t_12); if (unlikely(!__pyx_t_11)) __PYX_ERR(0, 318, __pyx_L1_error)
              __Pyx_GOTREF(__pyx_t_11);
              __Pyx_DECREF(__pyx_t_12); __pyx_t_12 = 0;
              __Pyx_DECREF_SET(__pyx_v_bridge_cands, __pyx_t_11);
              __pyx_t_11 = 0;

              if (unlikely(__pyx_v_F_phi == Py_None)) {
                PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
                __PYX_ERR(0, 319, __pyx_L1_error)
              }
              __pyx_t_11 = __Pyx_PyDict_GetItem(__pyx_v_F_phi, __pyx_v_shared_var); if (unlikely(!__pyx_t_11)) __PYX_ERR(0, 319, __pyx_L1_error)
              __Pyx_GOTREF(__pyx_t_11);
              __pyx_t_12 = PyNumber_InPlaceAnd(__pyx_v_alt_cands, __pyx_t_11); if (unlikely(!__pyx_t_12)) __PYX_ERR(0, 319, __pyx_L1_error)
              __Pyx_GOTREF(__pyx_t_12);
              __Pyx_DECREF(__pyx_t_11); __pyx_t_11 = 0;
              __Pyx_DECREF_SET(__pyx_v_alt_cands, __pyx_t_12);
              __pyx_t_12 = 0;

            }

            __pyx_t_12 = PyNumber_And(__pyx_v_bridge_cands, __pyx_v_alt_cands); if (unlikely(!__pyx_t_12)) __PYX_ERR(0, 320, __pyx_L1_error)
            __Pyx_GOTREF(__pyx_t_12);
            __pyx_t_3 = __Pyx_PyObject_IsTrue(__pyx_t_12); if (unlikely((__pyx_t_3 < 0))) __PYX_ERR(0, 320, __pyx_L1_error)
            __Pyx_DECREF(__pyx_t_12); __pyx_t_12 = 0;
            __pyx_t_2 = (!__pyx_t_3);
            if (__pyx_t_2) {

              __pyx_v_join_ok = 0;

              goto __pyx_L25_break;

            }

          }
          __Pyx_DECREF(__pyx_t_8); __pyx_t_8 = 0;
          goto __pyx_L28_for_end;
          __pyx_L25_break:;
          __Pyx_DECREF(__pyx_t_8); __pyx_t_8 = 0;
          goto __pyx_L28_for_end;
          __pyx_L28_for_end:;

          if (__pyx_v_join_ok) {

            __pyx_t_12 = __pyx_v_path_ws;
            __Pyx_INCREF(__pyx_t_12);
            __pyx_t_17 = 0;
            {
              PyObject *__pyx_callargs[2] = {__pyx_t_12, __pyx_v_w};
              __pyx_t_8 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_add, __pyx_callargs+__pyx_t_17, (2-__pyx_t_17) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
              __Pyx_XDECREF(__pyx_t_12); __pyx_t_12 = 0;
              if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 324, __pyx_L1_error)
              __Pyx_GOTREF(__pyx_t_8);
            }
            __Pyx_DECREF(__pyx_t_8); __pyx_t_8 = 0;

          }

          __pyx_L21_continue:;
        }
        __Pyx_DECREF(__pyx_t_13); __pyx_t_13 = 0;
      }
      __pyx_L20:;

      __pyx_t_2 = (__pyx_v_valid_ws == Py_None);
      if (__pyx_t_2) {
        __Pyx_INCREF(__pyx_v_path_ws);
        __pyx_t_13 = __pyx_v_path_ws;
      } else {
        __pyx_t_8 = PyNumber_And(__pyx_v_valid_ws, __pyx_v_path_ws); if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 326, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_8);
        __pyx_t_13 = __pyx_t_8;
        __pyx_t_8 = 0;
      }
      __Pyx_DECREF_SET(__pyx_v_valid_ws, __pyx_t_13);
      __pyx_t_13 = 0;

      __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_v_valid_ws); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 327, __pyx_L1_error)
      __pyx_t_3 = (!__pyx_t_2);
      if (__pyx_t_3) {

        goto __pyx_L17_break;

      }

    }
    __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
    goto __pyx_L32_for_end;
    __pyx_L17_break:;
    __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
    goto __pyx_L32_for_end;
    __pyx_L32_for_end:;

    __pyx_t_3 = (__pyx_v_valid_ws != Py_None);
    if (__pyx_t_3) {
      __Pyx_INCREF(__pyx_v_valid_ws);
      __pyx_t_5 = __pyx_v_valid_ws;
    } else {
      __pyx_t_13 = PySet_New(0); if (unlikely(!__pyx_t_13)) __PYX_ERR(0, 330, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_13);
      __pyx_t_5 = __pyx_t_13;
      __pyx_t_13 = 0;
    }
    if (unlikely((PyObject_SetItem(__pyx_v_bridge_vw, __pyx_v_v, __pyx_t_5) < 0))) __PYX_ERR(0, 330, __pyx_L1_error)
    __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;

  }
  __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;

  __Pyx_XDECREF(__pyx_r);
  __Pyx_INCREF(__pyx_v_bridge_vw);
  __pyx_r = __pyx_v_bridge_vw;
  goto __pyx_L0;

  __pyx_L1_error:;
  __Pyx_XDECREF(__pyx_t_1);
  __Pyx_XDECREF(__pyx_t_5);
  __Pyx_XDECREF(__pyx_t_8);
  __Pyx_XDECREF(__pyx_t_11);
  __Pyx_XDECREF(__pyx_t_12);
  __Pyx_XDECREF(__pyx_t_13);
  __Pyx_XDECREF(__pyx_t_14);
  __Pyx_XDECREF(__pyx_t_15);
  __Pyx_AddTraceback("_fast_match.derive_bridge_vw_pairs_fast", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __pyx_r = NULL;
  __pyx_L0:;
  __Pyx_XDECREF(__pyx_v_bridge_vw);
  __Pyx_XDECREF(__pyx_v_valid_ws);
  __Pyx_XDECREF(__pyx_v_path_ws);
  __Pyx_XDECREF(__pyx_v_pivot_match);
  __Pyx_XDECREF(__pyx_v_w_match);
  __Pyx_XDECREF(__pyx_v_terminal_var);
  __Pyx_XDECREF(__pyx_v_shared_vars);
  __Pyx_XDECREF(__pyx_v_per_v_matches);
  __Pyx_XDECREF(__pyx_v_w0_var);
  __Pyx_XDECREF(__pyx_v_shared_var);
  __Pyx_XDECREF(__pyx_v_bridge_cands);
  __Pyx_XDECREF(__pyx_v_alt_cands);
  __Pyx_XDECREF(__pyx_v_v);
  __Pyx_XDECREF(__pyx_v_w);
  __Pyx_XDECREF(__pyx_7genexpr__pyx_v_v);
  __Pyx_XGIVEREF(__pyx_r);
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}

static PyObject *__pyx_pw_11_fast_match_9propagate_candidates_dag_fast(PyObject *__pyx_self,
#if CYTHON_METH_FASTCALL
PyObject *const *__pyx_args, Py_ssize_t __pyx_nargs, PyObject *__pyx_kwds
#else
PyObject *__pyx_args, PyObject *__pyx_kwds
#endif
);
PyDoc_STRVAR(__pyx_doc_11_fast_match_8propagate_candidates_dag_fast, "\n    CompactGraph DAG propagation using the same semantics as the Python path.\n\n    Mutates and returns F_int, or returns None when propagation empties a\n    required variable domain.\n    ");
static PyMethodDef __pyx_mdef_11_fast_match_9propagate_candidates_dag_fast = {"propagate_candidates_dag_fast", (PyCFunction)(void(*)(void))(__Pyx_PyCFunction_FastCallWithKeywords)__pyx_pw_11_fast_match_9propagate_candidates_dag_fast, __Pyx_METH_FASTCALL|METH_KEYWORDS, __pyx_doc_11_fast_match_8propagate_candidates_dag_fast};
static PyObject *__pyx_pw_11_fast_match_9propagate_candidates_dag_fast(PyObject *__pyx_self,
#if CYTHON_METH_FASTCALL
PyObject *const *__pyx_args, Py_ssize_t __pyx_nargs, PyObject *__pyx_kwds
#else
PyObject *__pyx_args, PyObject *__pyx_kwds
#endif
) {
  PyObject *__pyx_v_compact_graph = 0;
  PyObject *__pyx_v_forward_steps = 0;
  PyObject *__pyx_v_backward_steps = 0;
  PyObject *__pyx_v_F_int = 0;
  #if !CYTHON_METH_FASTCALL
  CYTHON_UNUSED Py_ssize_t __pyx_nargs;
  #endif
  CYTHON_UNUSED PyObject *const *__pyx_kwvalues;
  PyObject* values[4] = {0,0,0,0};
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  PyObject *__pyx_r = 0;
  __Pyx_RefNannyDeclarations
  __Pyx_RefNannySetupContext("propagate_candidates_dag_fast (wrapper)", 0);
  #if !CYTHON_METH_FASTCALL
  #if CYTHON_ASSUME_SAFE_SIZE
  __pyx_nargs = PyTuple_GET_SIZE(__pyx_args);
  #else
  __pyx_nargs = PyTuple_Size(__pyx_args); if (unlikely(__pyx_nargs < 0)) return NULL;
  #endif
  #endif
  __pyx_kwvalues = __Pyx_KwValues_FASTCALL(__pyx_args, __pyx_nargs);
  {
    PyObject ** const __pyx_pyargnames[] = {&__pyx_mstate_global->__pyx_n_u_compact_graph,&__pyx_mstate_global->__pyx_n_u_forward_steps,&__pyx_mstate_global->__pyx_n_u_backward_steps,&__pyx_mstate_global->__pyx_n_u_F_int,0};
    const Py_ssize_t __pyx_kwds_len = (__pyx_kwds) ? __Pyx_NumKwargs_FASTCALL(__pyx_kwds) : 0;
    if (unlikely(__pyx_kwds_len) < 0) __PYX_ERR(0, 334, __pyx_L3_error)
    if (__pyx_kwds_len > 0) {
      switch (__pyx_nargs) {
        case  4:
        values[3] = __Pyx_ArgRef_FASTCALL(__pyx_args, 3);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[3])) __PYX_ERR(0, 334, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  3:
        values[2] = __Pyx_ArgRef_FASTCALL(__pyx_args, 2);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[2])) __PYX_ERR(0, 334, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  2:
        values[1] = __Pyx_ArgRef_FASTCALL(__pyx_args, 1);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[1])) __PYX_ERR(0, 334, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  1:
        values[0] = __Pyx_ArgRef_FASTCALL(__pyx_args, 0);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[0])) __PYX_ERR(0, 334, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  0: break;
        default: goto __pyx_L5_argtuple_error;
      }
      const Py_ssize_t kwd_pos_args = __pyx_nargs;
      if (__Pyx_ParseKeywords(__pyx_kwds, __pyx_kwvalues, __pyx_pyargnames, 0, values, kwd_pos_args, __pyx_kwds_len, "propagate_candidates_dag_fast", 0) < (0)) __PYX_ERR(0, 334, __pyx_L3_error)
      for (Py_ssize_t i = __pyx_nargs; i < 4; i++) {
        if (unlikely(!values[i])) { __Pyx_RaiseArgtupleInvalid("propagate_candidates_dag_fast", 1, 4, 4, i); __PYX_ERR(0, 334, __pyx_L3_error) }
      }
    } else if (unlikely(__pyx_nargs != 4)) {
      goto __pyx_L5_argtuple_error;
    } else {
      values[0] = __Pyx_ArgRef_FASTCALL(__pyx_args, 0);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[0])) __PYX_ERR(0, 334, __pyx_L3_error)
      values[1] = __Pyx_ArgRef_FASTCALL(__pyx_args, 1);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[1])) __PYX_ERR(0, 334, __pyx_L3_error)
      values[2] = __Pyx_ArgRef_FASTCALL(__pyx_args, 2);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[2])) __PYX_ERR(0, 334, __pyx_L3_error)
      values[3] = __Pyx_ArgRef_FASTCALL(__pyx_args, 3);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[3])) __PYX_ERR(0, 334, __pyx_L3_error)
    }
    __pyx_v_compact_graph = values[0];
    __pyx_v_forward_steps = values[1];
    __pyx_v_backward_steps = values[2];
    __pyx_v_F_int = values[3];
  }
  goto __pyx_L6_skip;
  __pyx_L5_argtuple_error:;
  __Pyx_RaiseArgtupleInvalid("propagate_candidates_dag_fast", 1, 4, 4, __pyx_nargs); __PYX_ERR(0, 334, __pyx_L3_error)
  __pyx_L6_skip:;
  goto __pyx_L4_argument_unpacking_done;
  __pyx_L3_error:;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  __Pyx_AddTraceback("_fast_match.propagate_candidates_dag_fast", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __Pyx_RefNannyFinishContext();
  return NULL;
  __pyx_L4_argument_unpacking_done:;
  __pyx_r = __pyx_pf_11_fast_match_8propagate_candidates_dag_fast(__pyx_self, __pyx_v_compact_graph, __pyx_v_forward_steps, __pyx_v_backward_steps, __pyx_v_F_int);

  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}

static PyObject *__pyx_pf_11_fast_match_8propagate_candidates_dag_fast(CYTHON_UNUSED PyObject *__pyx_self, PyObject *__pyx_v_compact_graph, PyObject *__pyx_v_forward_steps, PyObject *__pyx_v_backward_steps, PyObject *__pyx_v_F_int) {
  int __pyx_v_n_child;
  int __pyx_v_n_parent;
  PyObject *__pyx_v_parent_var = 0;
  PyObject *__pyx_v_child_var = 0;
  PyObject *__pyx_v_edges_info = 0;
  PyObject *__pyx_v_edge_label = 0;
  PyObject *__pyx_v_parent_ints = 0;
  PyObject *__pyx_v_child_ints = 0;
  PyObject *__pyx_v_reachable_child_ints = 0;
  PyObject *__pyx_v_reachable_parent_ints = 0;
  int __pyx_v_is_fwd;
  PyObject *__pyx_v_cg_out = NULL;
  PyObject *__pyx_v_cg_in = NULL;
  PyObject *__pyx_v_neighbor_map = NULL;
  PyObject *__pyx_v_p_int = NULL;
  PyObject *__pyx_v_neighbors = NULL;
  PyObject *__pyx_v_n_int = NULL;
  PyObject *__pyx_v_reverse_neighbor_map = NULL;
  PyObject *__pyx_v_c_int = NULL;
  PyObject *__pyx_v_forward_neighbor_map = NULL;
  PyObject *__pyx_r = NULL;
  __Pyx_RefNannyDeclarations
  int __pyx_t_1;
  int __pyx_t_2;
  int __pyx_t_3;
  PyObject *__pyx_t_4 = NULL;
  Py_ssize_t __pyx_t_5;
  PyObject *(*__pyx_t_6)(PyObject *);
  PyObject *__pyx_t_7 = NULL;
  PyObject *__pyx_t_8 = NULL;
  PyObject *__pyx_t_9 = NULL;
  PyObject *__pyx_t_10 = NULL;
  PyObject *__pyx_t_11 = NULL;
  PyObject *(*__pyx_t_12)(PyObject *);
  size_t __pyx_t_13;
  Py_ssize_t __pyx_t_14;
  PyObject *(*__pyx_t_15)(PyObject *);
  Py_ssize_t __pyx_t_16;
  Py_ssize_t __pyx_t_17;
  PyObject *(*__pyx_t_18)(PyObject *);
  PyObject *(*__pyx_t_19)(PyObject *);
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  __Pyx_RefNannySetupContext("propagate_candidates_dag_fast", 0);

  __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_v_forward_steps); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 347, __pyx_L1_error)
  __pyx_t_3 = (!__pyx_t_2);
  if (__pyx_t_3) {
  } else {
    __pyx_t_1 = __pyx_t_3;
    goto __pyx_L4_bool_binop_done;
  }
  __pyx_t_3 = __Pyx_PyObject_IsTrue(__pyx_v_backward_steps); if (unlikely((__pyx_t_3 < 0))) __PYX_ERR(0, 347, __pyx_L1_error)
  __pyx_t_2 = (!__pyx_t_3);
  __pyx_t_1 = __pyx_t_2;
  __pyx_L4_bool_binop_done:;
  if (__pyx_t_1) {

    __Pyx_XDECREF(__pyx_r);
    __Pyx_INCREF(__pyx_v_F_int);
    __pyx_r = __pyx_v_F_int;
    goto __pyx_L0;

  }

  __pyx_t_4 = __Pyx_PyObject_GetAttrStr(__pyx_v_compact_graph, __pyx_mstate_global->__pyx_n_u_out_neighbors); if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 350, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_4);
  __pyx_v_cg_out = __pyx_t_4;
  __pyx_t_4 = 0;

  __pyx_t_4 = __Pyx_PyObject_GetAttrStr(__pyx_v_compact_graph, __pyx_mstate_global->__pyx_n_u_in_neighbors); if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 351, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_4);
  __pyx_v_cg_in = __pyx_t_4;
  __pyx_t_4 = 0;

  if (likely(PyList_CheckExact(__pyx_v_forward_steps)) || PyTuple_CheckExact(__pyx_v_forward_steps)) {
    __pyx_t_4 = __pyx_v_forward_steps; __Pyx_INCREF(__pyx_t_4);
    __pyx_t_5 = 0;
    __pyx_t_6 = NULL;
  } else {
    __pyx_t_5 = -1; __pyx_t_4 = PyObject_GetIter(__pyx_v_forward_steps); if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 353, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_4);
    __pyx_t_6 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_4); if (unlikely(!__pyx_t_6)) __PYX_ERR(0, 353, __pyx_L1_error)
  }
  for (;;) {
    if (likely(!__pyx_t_6)) {
      if (likely(PyList_CheckExact(__pyx_t_4))) {
        {
          Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_t_4);
          #if !CYTHON_ASSUME_SAFE_SIZE
          if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 353, __pyx_L1_error)
          #endif
          if (__pyx_t_5 >= __pyx_temp) break;
        }
        __pyx_t_7 = __Pyx_PyList_GetItemRefFast(__pyx_t_4, __pyx_t_5, __Pyx_ReferenceSharing_OwnStrongReference);
        ++__pyx_t_5;
      } else {
        {
          Py_ssize_t __pyx_temp = __Pyx_PyTuple_GET_SIZE(__pyx_t_4);
          #if !CYTHON_ASSUME_SAFE_SIZE
          if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 353, __pyx_L1_error)
          #endif
          if (__pyx_t_5 >= __pyx_temp) break;
        }
        #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
        __pyx_t_7 = __Pyx_NewRef(PyTuple_GET_ITEM(__pyx_t_4, __pyx_t_5));
        #else
        __pyx_t_7 = __Pyx_PySequence_ITEM(__pyx_t_4, __pyx_t_5);
        #endif
        ++__pyx_t_5;
      }
      if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 353, __pyx_L1_error)
    } else {
      __pyx_t_7 = __pyx_t_6(__pyx_t_4);
      if (unlikely(!__pyx_t_7)) {
        PyObject* exc_type = PyErr_Occurred();
        if (exc_type) {
          if (unlikely(!__Pyx_PyErr_GivenExceptionMatches(exc_type, PyExc_StopIteration))) __PYX_ERR(0, 353, __pyx_L1_error)
          PyErr_Clear();
        }
        break;
      }
    }
    __Pyx_GOTREF(__pyx_t_7);
    if ((likely(PyTuple_CheckExact(__pyx_t_7))) || (PyList_CheckExact(__pyx_t_7))) {
      PyObject* sequence = __pyx_t_7;
      Py_ssize_t size = __Pyx_PySequence_SIZE(sequence);
      if (unlikely(size != 3)) {
        if (size > 3) __Pyx_RaiseTooManyValuesError(3);
        else if (size >= 0) __Pyx_RaiseNeedMoreValuesError(size);
        __PYX_ERR(0, 353, __pyx_L1_error)
      }
      #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
      if (likely(PyTuple_CheckExact(sequence))) {
        __pyx_t_8 = PyTuple_GET_ITEM(sequence, 0);
        __Pyx_INCREF(__pyx_t_8);
        __pyx_t_9 = PyTuple_GET_ITEM(sequence, 1);
        __Pyx_INCREF(__pyx_t_9);
        __pyx_t_10 = PyTuple_GET_ITEM(sequence, 2);
        __Pyx_INCREF(__pyx_t_10);
      } else {
        __pyx_t_8 = __Pyx_PyList_GetItemRefFast(sequence, 0, __Pyx_ReferenceSharing_SharedReference);
        if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 353, __pyx_L1_error)
        __Pyx_XGOTREF(__pyx_t_8);
        __pyx_t_9 = __Pyx_PyList_GetItemRefFast(sequence, 1, __Pyx_ReferenceSharing_SharedReference);
        if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 353, __pyx_L1_error)
        __Pyx_XGOTREF(__pyx_t_9);
        __pyx_t_10 = __Pyx_PyList_GetItemRefFast(sequence, 2, __Pyx_ReferenceSharing_SharedReference);
        if (unlikely(!__pyx_t_10)) __PYX_ERR(0, 353, __pyx_L1_error)
        __Pyx_XGOTREF(__pyx_t_10);
      }
      #else
      __pyx_t_8 = __Pyx_PySequence_ITEM(sequence, 0); if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 353, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_8);
      __pyx_t_9 = __Pyx_PySequence_ITEM(sequence, 1); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 353, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_9);
      __pyx_t_10 = __Pyx_PySequence_ITEM(sequence, 2); if (unlikely(!__pyx_t_10)) __PYX_ERR(0, 353, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_10);
      #endif
      __Pyx_DECREF(__pyx_t_7); __pyx_t_7 = 0;
    } else {
      Py_ssize_t index = -1;
      __pyx_t_11 = PyObject_GetIter(__pyx_t_7); if (unlikely(!__pyx_t_11)) __PYX_ERR(0, 353, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_11);
      __Pyx_DECREF(__pyx_t_7); __pyx_t_7 = 0;
      __pyx_t_12 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_11);
      index = 0; __pyx_t_8 = __pyx_t_12(__pyx_t_11); if (unlikely(!__pyx_t_8)) goto __pyx_L8_unpacking_failed;
      __Pyx_GOTREF(__pyx_t_8);
      index = 1; __pyx_t_9 = __pyx_t_12(__pyx_t_11); if (unlikely(!__pyx_t_9)) goto __pyx_L8_unpacking_failed;
      __Pyx_GOTREF(__pyx_t_9);
      index = 2; __pyx_t_10 = __pyx_t_12(__pyx_t_11); if (unlikely(!__pyx_t_10)) goto __pyx_L8_unpacking_failed;
      __Pyx_GOTREF(__pyx_t_10);
      if (__Pyx_IternextUnpackEndCheck(__pyx_t_12(__pyx_t_11), 3) < (0)) __PYX_ERR(0, 353, __pyx_L1_error)
      __pyx_t_12 = NULL;
      __Pyx_DECREF(__pyx_t_11); __pyx_t_11 = 0;
      goto __pyx_L9_unpacking_done;
      __pyx_L8_unpacking_failed:;
      __Pyx_DECREF(__pyx_t_11); __pyx_t_11 = 0;
      __pyx_t_12 = NULL;
      if (__Pyx_IterFinish() == 0) __Pyx_RaiseNeedMoreValuesError(index);
      __PYX_ERR(0, 353, __pyx_L1_error)
      __pyx_L9_unpacking_done:;
    }
    __Pyx_XDECREF_SET(__pyx_v_parent_var, __pyx_t_8);
    __pyx_t_8 = 0;
    __Pyx_XDECREF_SET(__pyx_v_child_var, __pyx_t_9);
    __pyx_t_9 = 0;
    __Pyx_XDECREF_SET(__pyx_v_edges_info, __pyx_t_10);
    __pyx_t_10 = 0;

    __pyx_t_10 = __pyx_v_F_int;
    __Pyx_INCREF(__pyx_t_10);
    __pyx_t_13 = 0;
    {
      PyObject *__pyx_callargs[2] = {__pyx_t_10, __pyx_v_parent_var};
      __pyx_t_7 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_get, __pyx_callargs+__pyx_t_13, (2-__pyx_t_13) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
      __Pyx_XDECREF(__pyx_t_10); __pyx_t_10 = 0;
      if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 354, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_7);
    }
    __Pyx_XDECREF_SET(__pyx_v_parent_ints, __pyx_t_7);
    __pyx_t_7 = 0;

    __pyx_t_10 = __pyx_v_F_int;
    __Pyx_INCREF(__pyx_t_10);
    __pyx_t_13 = 0;
    {
      PyObject *__pyx_callargs[2] = {__pyx_t_10, __pyx_v_child_var};
      __pyx_t_7 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_get, __pyx_callargs+__pyx_t_13, (2-__pyx_t_13) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
      __Pyx_XDECREF(__pyx_t_10); __pyx_t_10 = 0;
      if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 355, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_7);
    }
    __Pyx_XDECREF_SET(__pyx_v_child_ints, __pyx_t_7);
    __pyx_t_7 = 0;

    __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_v_parent_ints); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 356, __pyx_L1_error)
    __pyx_t_3 = (!__pyx_t_2);
    if (!__pyx_t_3) {
    } else {
      __pyx_t_1 = __pyx_t_3;
      goto __pyx_L11_bool_binop_done;
    }
    __pyx_t_3 = __Pyx_PyObject_IsTrue(__pyx_v_child_ints); if (unlikely((__pyx_t_3 < 0))) __PYX_ERR(0, 356, __pyx_L1_error)
    __pyx_t_2 = (!__pyx_t_3);
    __pyx_t_1 = __pyx_t_2;
    __pyx_L11_bool_binop_done:;
    if (__pyx_t_1) {

      __Pyx_XDECREF(__pyx_r);
      __pyx_r = Py_None; __Pyx_INCREF(Py_None);
      __Pyx_DECREF(__pyx_t_4); __pyx_t_4 = 0;
      goto __pyx_L0;

    }

    __pyx_t_7 = PySet_New(0); if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 359, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_7);
    __Pyx_XDECREF_SET(__pyx_v_reachable_child_ints, __pyx_t_7);
    __pyx_t_7 = 0;

    __pyx_t_14 = PyObject_Length(__pyx_v_child_ints); if (unlikely(__pyx_t_14 == ((Py_ssize_t)-1))) __PYX_ERR(0, 360, __pyx_L1_error)
    __pyx_v_n_child = __pyx_t_14;

    if (likely(PyList_CheckExact(__pyx_v_edges_info)) || PyTuple_CheckExact(__pyx_v_edges_info)) {
      __pyx_t_7 = __pyx_v_edges_info; __Pyx_INCREF(__pyx_t_7);
      __pyx_t_14 = 0;
      __pyx_t_15 = NULL;
    } else {
      __pyx_t_14 = -1; __pyx_t_7 = PyObject_GetIter(__pyx_v_edges_info); if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 361, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_7);
      __pyx_t_15 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_7); if (unlikely(!__pyx_t_15)) __PYX_ERR(0, 361, __pyx_L1_error)
    }
    for (;;) {
      if (likely(!__pyx_t_15)) {
        if (likely(PyList_CheckExact(__pyx_t_7))) {
          {
            Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_t_7);
            #if !CYTHON_ASSUME_SAFE_SIZE
            if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 361, __pyx_L1_error)
            #endif
            if (__pyx_t_14 >= __pyx_temp) break;
          }
          __pyx_t_10 = __Pyx_PyList_GetItemRefFast(__pyx_t_7, __pyx_t_14, __Pyx_ReferenceSharing_OwnStrongReference);
          ++__pyx_t_14;
        } else {
          {
            Py_ssize_t __pyx_temp = __Pyx_PyTuple_GET_SIZE(__pyx_t_7);
            #if !CYTHON_ASSUME_SAFE_SIZE
            if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 361, __pyx_L1_error)
            #endif
            if (__pyx_t_14 >= __pyx_temp) break;
          }
          #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
          __pyx_t_10 = __Pyx_NewRef(PyTuple_GET_ITEM(__pyx_t_7, __pyx_t_14));
          #else
          __pyx_t_10 = __Pyx_PySequence_ITEM(__pyx_t_7, __pyx_t_14);
          #endif
          ++__pyx_t_14;
        }
        if (unlikely(!__pyx_t_10)) __PYX_ERR(0, 361, __pyx_L1_error)
      } else {
        __pyx_t_10 = __pyx_t_15(__pyx_t_7);
        if (unlikely(!__pyx_t_10)) {
          PyObject* exc_type = PyErr_Occurred();
          if (exc_type) {
            if (unlikely(!__Pyx_PyErr_GivenExceptionMatches(exc_type, PyExc_StopIteration))) __PYX_ERR(0, 361, __pyx_L1_error)
            PyErr_Clear();
          }
          break;
        }
      }
      __Pyx_GOTREF(__pyx_t_10);
      if ((likely(PyTuple_CheckExact(__pyx_t_10))) || (PyList_CheckExact(__pyx_t_10))) {
        PyObject* sequence = __pyx_t_10;
        Py_ssize_t size = __Pyx_PySequence_SIZE(sequence);
        if (unlikely(size != 2)) {
          if (size > 2) __Pyx_RaiseTooManyValuesError(2);
          else if (size >= 0) __Pyx_RaiseNeedMoreValuesError(size);
          __PYX_ERR(0, 361, __pyx_L1_error)
        }
        #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
        if (likely(PyTuple_CheckExact(sequence))) {
          __pyx_t_9 = PyTuple_GET_ITEM(sequence, 0);
          __Pyx_INCREF(__pyx_t_9);
          __pyx_t_8 = PyTuple_GET_ITEM(sequence, 1);
          __Pyx_INCREF(__pyx_t_8);
        } else {
          __pyx_t_9 = __Pyx_PyList_GetItemRefFast(sequence, 0, __Pyx_ReferenceSharing_SharedReference);
          if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 361, __pyx_L1_error)
          __Pyx_XGOTREF(__pyx_t_9);
          __pyx_t_8 = __Pyx_PyList_GetItemRefFast(sequence, 1, __Pyx_ReferenceSharing_SharedReference);
          if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 361, __pyx_L1_error)
          __Pyx_XGOTREF(__pyx_t_8);
        }
        #else
        __pyx_t_9 = __Pyx_PySequence_ITEM(sequence, 0); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 361, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_9);
        __pyx_t_8 = __Pyx_PySequence_ITEM(sequence, 1); if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 361, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_8);
        #endif
        __Pyx_DECREF(__pyx_t_10); __pyx_t_10 = 0;
      } else {
        Py_ssize_t index = -1;
        __pyx_t_11 = PyObject_GetIter(__pyx_t_10); if (unlikely(!__pyx_t_11)) __PYX_ERR(0, 361, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_11);
        __Pyx_DECREF(__pyx_t_10); __pyx_t_10 = 0;
        __pyx_t_12 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_11);
        index = 0; __pyx_t_9 = __pyx_t_12(__pyx_t_11); if (unlikely(!__pyx_t_9)) goto __pyx_L15_unpacking_failed;
        __Pyx_GOTREF(__pyx_t_9);
        index = 1; __pyx_t_8 = __pyx_t_12(__pyx_t_11); if (unlikely(!__pyx_t_8)) goto __pyx_L15_unpacking_failed;
        __Pyx_GOTREF(__pyx_t_8);
        if (__Pyx_IternextUnpackEndCheck(__pyx_t_12(__pyx_t_11), 2) < (0)) __PYX_ERR(0, 361, __pyx_L1_error)
        __pyx_t_12 = NULL;
        __Pyx_DECREF(__pyx_t_11); __pyx_t_11 = 0;
        goto __pyx_L16_unpacking_done;
        __pyx_L15_unpacking_failed:;
        __Pyx_DECREF(__pyx_t_11); __pyx_t_11 = 0;
        __pyx_t_12 = NULL;
        if (__Pyx_IterFinish() == 0) __Pyx_RaiseNeedMoreValuesError(index);
        __PYX_ERR(0, 361, __pyx_L1_error)
        __pyx_L16_unpacking_done:;
      }
      __pyx_t_1 = __Pyx_PyObject_IsTrue(__pyx_t_8); if (unlikely((__pyx_t_1 == (int)-1) && PyErr_Occurred())) __PYX_ERR(0, 361, __pyx_L1_error)
      __Pyx_DECREF(__pyx_t_8); __pyx_t_8 = 0;
      __Pyx_XDECREF_SET(__pyx_v_edge_label, __pyx_t_9);
      __pyx_t_9 = 0;
      __pyx_v_is_fwd = __pyx_t_1;

      __pyx_t_16 = PyObject_Length(__pyx_v_parent_ints); if (unlikely(__pyx_t_16 == ((Py_ssize_t)-1))) __PYX_ERR(0, 362, __pyx_L1_error)
      __pyx_t_17 = PyObject_Length(__pyx_v_child_ints); if (unlikely(__pyx_t_17 == ((Py_ssize_t)-1))) __PYX_ERR(0, 362, __pyx_L1_error)
      __pyx_t_1 = (__pyx_t_16 <= __pyx_t_17);
      if (__pyx_t_1) {

        if (__pyx_v_is_fwd) {
          __Pyx_INCREF(__pyx_v_cg_out);
          __pyx_t_10 = __pyx_v_cg_out;
        } else {
          __Pyx_INCREF(__pyx_v_cg_in);
          __pyx_t_10 = __pyx_v_cg_in;
        }
        __Pyx_XDECREF_SET(__pyx_v_neighbor_map, __pyx_t_10);
        __pyx_t_10 = 0;

        if (likely(PyList_CheckExact(__pyx_v_parent_ints)) || PyTuple_CheckExact(__pyx_v_parent_ints)) {
          __pyx_t_10 = __pyx_v_parent_ints; __Pyx_INCREF(__pyx_t_10);
          __pyx_t_17 = 0;
          __pyx_t_18 = NULL;
        } else {
          __pyx_t_17 = -1; __pyx_t_10 = PyObject_GetIter(__pyx_v_parent_ints); if (unlikely(!__pyx_t_10)) __PYX_ERR(0, 364, __pyx_L1_error)
          __Pyx_GOTREF(__pyx_t_10);
          __pyx_t_18 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_10); if (unlikely(!__pyx_t_18)) __PYX_ERR(0, 364, __pyx_L1_error)
        }
        for (;;) {
          if (likely(!__pyx_t_18)) {
            if (likely(PyList_CheckExact(__pyx_t_10))) {
              {
                Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_t_10);
                #if !CYTHON_ASSUME_SAFE_SIZE
                if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 364, __pyx_L1_error)
                #endif
                if (__pyx_t_17 >= __pyx_temp) break;
              }
              __pyx_t_8 = __Pyx_PyList_GetItemRefFast(__pyx_t_10, __pyx_t_17, __Pyx_ReferenceSharing_OwnStrongReference);
              ++__pyx_t_17;
            } else {
              {
                Py_ssize_t __pyx_temp = __Pyx_PyTuple_GET_SIZE(__pyx_t_10);
                #if !CYTHON_ASSUME_SAFE_SIZE
                if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 364, __pyx_L1_error)
                #endif
                if (__pyx_t_17 >= __pyx_temp) break;
              }
              #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
              __pyx_t_8 = __Pyx_NewRef(PyTuple_GET_ITEM(__pyx_t_10, __pyx_t_17));
              #else
              __pyx_t_8 = __Pyx_PySequence_ITEM(__pyx_t_10, __pyx_t_17);
              #endif
              ++__pyx_t_17;
            }
            if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 364, __pyx_L1_error)
          } else {
            __pyx_t_8 = __pyx_t_18(__pyx_t_10);
            if (unlikely(!__pyx_t_8)) {
              PyObject* exc_type = PyErr_Occurred();
              if (exc_type) {
                if (unlikely(!__Pyx_PyErr_GivenExceptionMatches(exc_type, PyExc_StopIteration))) __PYX_ERR(0, 364, __pyx_L1_error)
                PyErr_Clear();
              }
              break;
            }
          }
          __Pyx_GOTREF(__pyx_t_8);
          __Pyx_XDECREF_SET(__pyx_v_p_int, __pyx_t_8);
          __pyx_t_8 = 0;

          __pyx_t_9 = __pyx_v_neighbor_map;
          __Pyx_INCREF(__pyx_t_9);
          __pyx_t_11 = PyTuple_New(2); if (unlikely(!__pyx_t_11)) __PYX_ERR(0, 365, __pyx_L1_error)
          __Pyx_GOTREF(__pyx_t_11);
          __Pyx_INCREF(__pyx_v_p_int);
          __Pyx_GIVEREF(__pyx_v_p_int);
          if (__Pyx_PyTuple_SET_ITEM(__pyx_t_11, 0, __pyx_v_p_int) != (0)) __PYX_ERR(0, 365, __pyx_L1_error);
          __Pyx_INCREF(__pyx_v_edge_label);
          __Pyx_GIVEREF(__pyx_v_edge_label);
          if (__Pyx_PyTuple_SET_ITEM(__pyx_t_11, 1, __pyx_v_edge_label) != (0)) __PYX_ERR(0, 365, __pyx_L1_error);
          __pyx_t_13 = 0;
          {
            PyObject *__pyx_callargs[2] = {__pyx_t_9, __pyx_t_11};
            __pyx_t_8 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_get, __pyx_callargs+__pyx_t_13, (2-__pyx_t_13) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
            __Pyx_XDECREF(__pyx_t_9); __pyx_t_9 = 0;
            __Pyx_DECREF(__pyx_t_11); __pyx_t_11 = 0;
            if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 365, __pyx_L1_error)
            __Pyx_GOTREF(__pyx_t_8);
          }
          __Pyx_XDECREF_SET(__pyx_v_neighbors, __pyx_t_8);
          __pyx_t_8 = 0;

          __pyx_t_1 = (__pyx_v_neighbors == Py_None);
          if (__pyx_t_1) {

            goto __pyx_L18_continue;

          }

          if (likely(PyList_CheckExact(__pyx_v_neighbors)) || PyTuple_CheckExact(__pyx_v_neighbors)) {
            __pyx_t_8 = __pyx_v_neighbors; __Pyx_INCREF(__pyx_t_8);
            __pyx_t_16 = 0;
            __pyx_t_19 = NULL;
          } else {
            __pyx_t_16 = -1; __pyx_t_8 = PyObject_GetIter(__pyx_v_neighbors); if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 368, __pyx_L1_error)
            __Pyx_GOTREF(__pyx_t_8);
            __pyx_t_19 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_8); if (unlikely(!__pyx_t_19)) __PYX_ERR(0, 368, __pyx_L1_error)
          }
          for (;;) {
            if (likely(!__pyx_t_19)) {
              if (likely(PyList_CheckExact(__pyx_t_8))) {
                {
                  Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_t_8);
                  #if !CYTHON_ASSUME_SAFE_SIZE
                  if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 368, __pyx_L1_error)
                  #endif
                  if (__pyx_t_16 >= __pyx_temp) break;
                }
                __pyx_t_11 = __Pyx_PyList_GetItemRefFast(__pyx_t_8, __pyx_t_16, __Pyx_ReferenceSharing_OwnStrongReference);
                ++__pyx_t_16;
              } else {
                {
                  Py_ssize_t __pyx_temp = __Pyx_PyTuple_GET_SIZE(__pyx_t_8);
                  #if !CYTHON_ASSUME_SAFE_SIZE
                  if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 368, __pyx_L1_error)
                  #endif
                  if (__pyx_t_16 >= __pyx_temp) break;
                }
                #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
                __pyx_t_11 = __Pyx_NewRef(PyTuple_GET_ITEM(__pyx_t_8, __pyx_t_16));
                #else
                __pyx_t_11 = __Pyx_PySequence_ITEM(__pyx_t_8, __pyx_t_16);
                #endif
                ++__pyx_t_16;
              }
              if (unlikely(!__pyx_t_11)) __PYX_ERR(0, 368, __pyx_L1_error)
            } else {
              __pyx_t_11 = __pyx_t_19(__pyx_t_8);
              if (unlikely(!__pyx_t_11)) {
                PyObject* exc_type = PyErr_Occurred();
                if (exc_type) {
                  if (unlikely(!__Pyx_PyErr_GivenExceptionMatches(exc_type, PyExc_StopIteration))) __PYX_ERR(0, 368, __pyx_L1_error)
                  PyErr_Clear();
                }
                break;
              }
            }
            __Pyx_GOTREF(__pyx_t_11);
            __Pyx_XDECREF_SET(__pyx_v_n_int, __pyx_t_11);
            __pyx_t_11 = 0;

            __pyx_t_1 = (__Pyx_PySequence_ContainsTF(__pyx_v_n_int, __pyx_v_child_ints, Py_EQ)); if (unlikely((__pyx_t_1 < 0))) __PYX_ERR(0, 369, __pyx_L1_error)
            if (__pyx_t_1) {

              __pyx_t_9 = __pyx_v_reachable_child_ints;
              __Pyx_INCREF(__pyx_t_9);
              __pyx_t_13 = 0;
              {
                PyObject *__pyx_callargs[2] = {__pyx_t_9, __pyx_v_n_int};
                __pyx_t_11 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_add, __pyx_callargs+__pyx_t_13, (2-__pyx_t_13) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
                __Pyx_XDECREF(__pyx_t_9); __pyx_t_9 = 0;
                if (unlikely(!__pyx_t_11)) __PYX_ERR(0, 370, __pyx_L1_error)
                __Pyx_GOTREF(__pyx_t_11);
              }
              __Pyx_DECREF(__pyx_t_11); __pyx_t_11 = 0;

            }

          }
          __Pyx_DECREF(__pyx_t_8); __pyx_t_8 = 0;

          __pyx_t_16 = PyObject_Length(__pyx_v_reachable_child_ints); if (unlikely(__pyx_t_16 == ((Py_ssize_t)-1))) __PYX_ERR(0, 371, __pyx_L1_error)
          __pyx_t_1 = (__pyx_t_16 == __pyx_v_n_child);
          if (__pyx_t_1) {

            goto __pyx_L19_break;

          }

          __pyx_L18_continue:;
        }
        __Pyx_DECREF(__pyx_t_10); __pyx_t_10 = 0;
        goto __pyx_L26_for_end;
        __pyx_L19_break:;
        __Pyx_DECREF(__pyx_t_10); __pyx_t_10 = 0;
        goto __pyx_L26_for_end;
        __pyx_L26_for_end:;

        goto __pyx_L17;
      }

       {
        if (__pyx_v_is_fwd) {
          __Pyx_INCREF(__pyx_v_cg_in);
          __pyx_t_10 = __pyx_v_cg_in;
        } else {
          __Pyx_INCREF(__pyx_v_cg_out);
          __pyx_t_10 = __pyx_v_cg_out;
        }
        __Pyx_XDECREF_SET(__pyx_v_reverse_neighbor_map, __pyx_t_10);
        __pyx_t_10 = 0;

        if (likely(PyList_CheckExact(__pyx_v_child_ints)) || PyTuple_CheckExact(__pyx_v_child_ints)) {
          __pyx_t_10 = __pyx_v_child_ints; __Pyx_INCREF(__pyx_t_10);
          __pyx_t_17 = 0;
          __pyx_t_18 = NULL;
        } else {
          __pyx_t_17 = -1; __pyx_t_10 = PyObject_GetIter(__pyx_v_child_ints); if (unlikely(!__pyx_t_10)) __PYX_ERR(0, 375, __pyx_L1_error)
          __Pyx_GOTREF(__pyx_t_10);
          __pyx_t_18 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_10); if (unlikely(!__pyx_t_18)) __PYX_ERR(0, 375, __pyx_L1_error)
        }
        for (;;) {
          if (likely(!__pyx_t_18)) {
            if (likely(PyList_CheckExact(__pyx_t_10))) {
              {
                Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_t_10);
                #if !CYTHON_ASSUME_SAFE_SIZE
                if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 375, __pyx_L1_error)
                #endif
                if (__pyx_t_17 >= __pyx_temp) break;
              }
              __pyx_t_8 = __Pyx_PyList_GetItemRefFast(__pyx_t_10, __pyx_t_17, __Pyx_ReferenceSharing_OwnStrongReference);
              ++__pyx_t_17;
            } else {
              {
                Py_ssize_t __pyx_temp = __Pyx_PyTuple_GET_SIZE(__pyx_t_10);
                #if !CYTHON_ASSUME_SAFE_SIZE
                if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 375, __pyx_L1_error)
                #endif
                if (__pyx_t_17 >= __pyx_temp) break;
              }
              #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
              __pyx_t_8 = __Pyx_NewRef(PyTuple_GET_ITEM(__pyx_t_10, __pyx_t_17));
              #else
              __pyx_t_8 = __Pyx_PySequence_ITEM(__pyx_t_10, __pyx_t_17);
              #endif
              ++__pyx_t_17;
            }
            if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 375, __pyx_L1_error)
          } else {
            __pyx_t_8 = __pyx_t_18(__pyx_t_10);
            if (unlikely(!__pyx_t_8)) {
              PyObject* exc_type = PyErr_Occurred();
              if (exc_type) {
                if (unlikely(!__Pyx_PyErr_GivenExceptionMatches(exc_type, PyExc_StopIteration))) __PYX_ERR(0, 375, __pyx_L1_error)
                PyErr_Clear();
              }
              break;
            }
          }
          __Pyx_GOTREF(__pyx_t_8);
          __Pyx_XDECREF_SET(__pyx_v_c_int, __pyx_t_8);
          __pyx_t_8 = 0;

          __pyx_t_11 = __pyx_v_reverse_neighbor_map;
          __Pyx_INCREF(__pyx_t_11);
          __pyx_t_9 = PyTuple_New(2); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 376, __pyx_L1_error)
          __Pyx_GOTREF(__pyx_t_9);
          __Pyx_INCREF(__pyx_v_c_int);
          __Pyx_GIVEREF(__pyx_v_c_int);
          if (__Pyx_PyTuple_SET_ITEM(__pyx_t_9, 0, __pyx_v_c_int) != (0)) __PYX_ERR(0, 376, __pyx_L1_error);
          __Pyx_INCREF(__pyx_v_edge_label);
          __Pyx_GIVEREF(__pyx_v_edge_label);
          if (__Pyx_PyTuple_SET_ITEM(__pyx_t_9, 1, __pyx_v_edge_label) != (0)) __PYX_ERR(0, 376, __pyx_L1_error);
          __pyx_t_13 = 0;
          {
            PyObject *__pyx_callargs[2] = {__pyx_t_11, __pyx_t_9};
            __pyx_t_8 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_get, __pyx_callargs+__pyx_t_13, (2-__pyx_t_13) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
            __Pyx_XDECREF(__pyx_t_11); __pyx_t_11 = 0;
            __Pyx_DECREF(__pyx_t_9); __pyx_t_9 = 0;
            if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 376, __pyx_L1_error)
            __Pyx_GOTREF(__pyx_t_8);
          }
          __Pyx_XDECREF_SET(__pyx_v_neighbors, __pyx_t_8);
          __pyx_t_8 = 0;

          __pyx_t_1 = (__pyx_v_neighbors == Py_None);
          if (__pyx_t_1) {

            goto __pyx_L27_continue;

          }

          if (likely(PyList_CheckExact(__pyx_v_neighbors)) || PyTuple_CheckExact(__pyx_v_neighbors)) {
            __pyx_t_8 = __pyx_v_neighbors; __Pyx_INCREF(__pyx_t_8);
            __pyx_t_16 = 0;
            __pyx_t_19 = NULL;
          } else {
            __pyx_t_16 = -1; __pyx_t_8 = PyObject_GetIter(__pyx_v_neighbors); if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 379, __pyx_L1_error)
            __Pyx_GOTREF(__pyx_t_8);
            __pyx_t_19 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_8); if (unlikely(!__pyx_t_19)) __PYX_ERR(0, 379, __pyx_L1_error)
          }
          for (;;) {
            if (likely(!__pyx_t_19)) {
              if (likely(PyList_CheckExact(__pyx_t_8))) {
                {
                  Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_t_8);
                  #if !CYTHON_ASSUME_SAFE_SIZE
                  if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 379, __pyx_L1_error)
                  #endif
                  if (__pyx_t_16 >= __pyx_temp) break;
                }
                __pyx_t_9 = __Pyx_PyList_GetItemRefFast(__pyx_t_8, __pyx_t_16, __Pyx_ReferenceSharing_OwnStrongReference);
                ++__pyx_t_16;
              } else {
                {
                  Py_ssize_t __pyx_temp = __Pyx_PyTuple_GET_SIZE(__pyx_t_8);
                  #if !CYTHON_ASSUME_SAFE_SIZE
                  if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 379, __pyx_L1_error)
                  #endif
                  if (__pyx_t_16 >= __pyx_temp) break;
                }
                #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
                __pyx_t_9 = __Pyx_NewRef(PyTuple_GET_ITEM(__pyx_t_8, __pyx_t_16));
                #else
                __pyx_t_9 = __Pyx_PySequence_ITEM(__pyx_t_8, __pyx_t_16);
                #endif
                ++__pyx_t_16;
              }
              if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 379, __pyx_L1_error)
            } else {
              __pyx_t_9 = __pyx_t_19(__pyx_t_8);
              if (unlikely(!__pyx_t_9)) {
                PyObject* exc_type = PyErr_Occurred();
                if (exc_type) {
                  if (unlikely(!__Pyx_PyErr_GivenExceptionMatches(exc_type, PyExc_StopIteration))) __PYX_ERR(0, 379, __pyx_L1_error)
                  PyErr_Clear();
                }
                break;
              }
            }
            __Pyx_GOTREF(__pyx_t_9);
            __Pyx_XDECREF_SET(__pyx_v_n_int, __pyx_t_9);
            __pyx_t_9 = 0;

            __pyx_t_1 = (__Pyx_PySequence_ContainsTF(__pyx_v_n_int, __pyx_v_parent_ints, Py_EQ)); if (unlikely((__pyx_t_1 < 0))) __PYX_ERR(0, 380, __pyx_L1_error)
            if (__pyx_t_1) {

              __pyx_t_11 = __pyx_v_reachable_child_ints;
              __Pyx_INCREF(__pyx_t_11);
              __pyx_t_13 = 0;
              {
                PyObject *__pyx_callargs[2] = {__pyx_t_11, __pyx_v_c_int};
                __pyx_t_9 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_add, __pyx_callargs+__pyx_t_13, (2-__pyx_t_13) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
                __Pyx_XDECREF(__pyx_t_11); __pyx_t_11 = 0;
                if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 381, __pyx_L1_error)
                __Pyx_GOTREF(__pyx_t_9);
              }
              __Pyx_DECREF(__pyx_t_9); __pyx_t_9 = 0;

              goto __pyx_L31_break;

            }

          }
          __Pyx_DECREF(__pyx_t_8); __pyx_t_8 = 0;
          goto __pyx_L33_for_end;
          __pyx_L31_break:;
          __Pyx_DECREF(__pyx_t_8); __pyx_t_8 = 0;
          goto __pyx_L33_for_end;
          __pyx_L33_for_end:;

          __pyx_t_16 = PyObject_Length(__pyx_v_reachable_child_ints); if (unlikely(__pyx_t_16 == ((Py_ssize_t)-1))) __PYX_ERR(0, 383, __pyx_L1_error)
          __pyx_t_1 = (__pyx_t_16 == __pyx_v_n_child);
          if (__pyx_t_1) {

            goto __pyx_L28_break;

          }

          __pyx_L27_continue:;
        }
        __Pyx_DECREF(__pyx_t_10); __pyx_t_10 = 0;
        goto __pyx_L35_for_end;
        __pyx_L28_break:;
        __Pyx_DECREF(__pyx_t_10); __pyx_t_10 = 0;
        goto __pyx_L35_for_end;
        __pyx_L35_for_end:;
      }
      __pyx_L17:;

      __pyx_t_17 = PyObject_Length(__pyx_v_reachable_child_ints); if (unlikely(__pyx_t_17 == ((Py_ssize_t)-1))) __PYX_ERR(0, 385, __pyx_L1_error)
      __pyx_t_1 = (__pyx_t_17 == __pyx_v_n_child);
      if (__pyx_t_1) {

        goto __pyx_L14_break;

      }

    }
    __Pyx_DECREF(__pyx_t_7); __pyx_t_7 = 0;
    goto __pyx_L37_for_end;
    __pyx_L14_break:;
    __Pyx_DECREF(__pyx_t_7); __pyx_t_7 = 0;
    goto __pyx_L37_for_end;
    __pyx_L37_for_end:;

    __pyx_t_1 = __Pyx_PyObject_IsTrue(__pyx_v_reachable_child_ints); if (unlikely((__pyx_t_1 < 0))) __PYX_ERR(0, 388, __pyx_L1_error)
    __pyx_t_2 = (!__pyx_t_1);
    if (__pyx_t_2) {

      __Pyx_XDECREF(__pyx_r);
      __pyx_r = Py_None; __Pyx_INCREF(Py_None);
      __Pyx_DECREF(__pyx_t_4); __pyx_t_4 = 0;
      goto __pyx_L0;

    }

    if (unlikely((PyObject_SetItem(__pyx_v_F_int, __pyx_v_child_var, __pyx_v_reachable_child_ints) < 0))) __PYX_ERR(0, 390, __pyx_L1_error)

  }
  __Pyx_DECREF(__pyx_t_4); __pyx_t_4 = 0;

  if (likely(PyList_CheckExact(__pyx_v_backward_steps)) || PyTuple_CheckExact(__pyx_v_backward_steps)) {
    __pyx_t_4 = __pyx_v_backward_steps; __Pyx_INCREF(__pyx_t_4);
    __pyx_t_5 = 0;
    __pyx_t_6 = NULL;
  } else {
    __pyx_t_5 = -1; __pyx_t_4 = PyObject_GetIter(__pyx_v_backward_steps); if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 392, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_4);
    __pyx_t_6 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_4); if (unlikely(!__pyx_t_6)) __PYX_ERR(0, 392, __pyx_L1_error)
  }
  for (;;) {
    if (likely(!__pyx_t_6)) {
      if (likely(PyList_CheckExact(__pyx_t_4))) {
        {
          Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_t_4);
          #if !CYTHON_ASSUME_SAFE_SIZE
          if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 392, __pyx_L1_error)
          #endif
          if (__pyx_t_5 >= __pyx_temp) break;
        }
        __pyx_t_7 = __Pyx_PyList_GetItemRefFast(__pyx_t_4, __pyx_t_5, __Pyx_ReferenceSharing_OwnStrongReference);
        ++__pyx_t_5;
      } else {
        {
          Py_ssize_t __pyx_temp = __Pyx_PyTuple_GET_SIZE(__pyx_t_4);
          #if !CYTHON_ASSUME_SAFE_SIZE
          if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 392, __pyx_L1_error)
          #endif
          if (__pyx_t_5 >= __pyx_temp) break;
        }
        #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
        __pyx_t_7 = __Pyx_NewRef(PyTuple_GET_ITEM(__pyx_t_4, __pyx_t_5));
        #else
        __pyx_t_7 = __Pyx_PySequence_ITEM(__pyx_t_4, __pyx_t_5);
        #endif
        ++__pyx_t_5;
      }
      if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 392, __pyx_L1_error)
    } else {
      __pyx_t_7 = __pyx_t_6(__pyx_t_4);
      if (unlikely(!__pyx_t_7)) {
        PyObject* exc_type = PyErr_Occurred();
        if (exc_type) {
          if (unlikely(!__Pyx_PyErr_GivenExceptionMatches(exc_type, PyExc_StopIteration))) __PYX_ERR(0, 392, __pyx_L1_error)
          PyErr_Clear();
        }
        break;
      }
    }
    __Pyx_GOTREF(__pyx_t_7);
    if ((likely(PyTuple_CheckExact(__pyx_t_7))) || (PyList_CheckExact(__pyx_t_7))) {
      PyObject* sequence = __pyx_t_7;
      Py_ssize_t size = __Pyx_PySequence_SIZE(sequence);
      if (unlikely(size != 3)) {
        if (size > 3) __Pyx_RaiseTooManyValuesError(3);
        else if (size >= 0) __Pyx_RaiseNeedMoreValuesError(size);
        __PYX_ERR(0, 392, __pyx_L1_error)
      }
      #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
      if (likely(PyTuple_CheckExact(sequence))) {
        __pyx_t_10 = PyTuple_GET_ITEM(sequence, 0);
        __Pyx_INCREF(__pyx_t_10);
        __pyx_t_8 = PyTuple_GET_ITEM(sequence, 1);
        __Pyx_INCREF(__pyx_t_8);
        __pyx_t_9 = PyTuple_GET_ITEM(sequence, 2);
        __Pyx_INCREF(__pyx_t_9);
      } else {
        __pyx_t_10 = __Pyx_PyList_GetItemRefFast(sequence, 0, __Pyx_ReferenceSharing_SharedReference);
        if (unlikely(!__pyx_t_10)) __PYX_ERR(0, 392, __pyx_L1_error)
        __Pyx_XGOTREF(__pyx_t_10);
        __pyx_t_8 = __Pyx_PyList_GetItemRefFast(sequence, 1, __Pyx_ReferenceSharing_SharedReference);
        if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 392, __pyx_L1_error)
        __Pyx_XGOTREF(__pyx_t_8);
        __pyx_t_9 = __Pyx_PyList_GetItemRefFast(sequence, 2, __Pyx_ReferenceSharing_SharedReference);
        if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 392, __pyx_L1_error)
        __Pyx_XGOTREF(__pyx_t_9);
      }
      #else
      __pyx_t_10 = __Pyx_PySequence_ITEM(sequence, 0); if (unlikely(!__pyx_t_10)) __PYX_ERR(0, 392, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_10);
      __pyx_t_8 = __Pyx_PySequence_ITEM(sequence, 1); if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 392, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_8);
      __pyx_t_9 = __Pyx_PySequence_ITEM(sequence, 2); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 392, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_9);
      #endif
      __Pyx_DECREF(__pyx_t_7); __pyx_t_7 = 0;
    } else {
      Py_ssize_t index = -1;
      __pyx_t_11 = PyObject_GetIter(__pyx_t_7); if (unlikely(!__pyx_t_11)) __PYX_ERR(0, 392, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_11);
      __Pyx_DECREF(__pyx_t_7); __pyx_t_7 = 0;
      __pyx_t_12 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_11);
      index = 0; __pyx_t_10 = __pyx_t_12(__pyx_t_11); if (unlikely(!__pyx_t_10)) goto __pyx_L42_unpacking_failed;
      __Pyx_GOTREF(__pyx_t_10);
      index = 1; __pyx_t_8 = __pyx_t_12(__pyx_t_11); if (unlikely(!__pyx_t_8)) goto __pyx_L42_unpacking_failed;
      __Pyx_GOTREF(__pyx_t_8);
      index = 2; __pyx_t_9 = __pyx_t_12(__pyx_t_11); if (unlikely(!__pyx_t_9)) goto __pyx_L42_unpacking_failed;
      __Pyx_GOTREF(__pyx_t_9);
      if (__Pyx_IternextUnpackEndCheck(__pyx_t_12(__pyx_t_11), 3) < (0)) __PYX_ERR(0, 392, __pyx_L1_error)
      __pyx_t_12 = NULL;
      __Pyx_DECREF(__pyx_t_11); __pyx_t_11 = 0;
      goto __pyx_L43_unpacking_done;
      __pyx_L42_unpacking_failed:;
      __Pyx_DECREF(__pyx_t_11); __pyx_t_11 = 0;
      __pyx_t_12 = NULL;
      if (__Pyx_IterFinish() == 0) __Pyx_RaiseNeedMoreValuesError(index);
      __PYX_ERR(0, 392, __pyx_L1_error)
      __pyx_L43_unpacking_done:;
    }
    __Pyx_XDECREF_SET(__pyx_v_parent_var, __pyx_t_10);
    __pyx_t_10 = 0;
    __Pyx_XDECREF_SET(__pyx_v_child_var, __pyx_t_8);
    __pyx_t_8 = 0;
    __Pyx_XDECREF_SET(__pyx_v_edges_info, __pyx_t_9);
    __pyx_t_9 = 0;

    __pyx_t_9 = __pyx_v_F_int;
    __Pyx_INCREF(__pyx_t_9);
    __pyx_t_13 = 0;
    {
      PyObject *__pyx_callargs[2] = {__pyx_t_9, __pyx_v_child_var};
      __pyx_t_7 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_get, __pyx_callargs+__pyx_t_13, (2-__pyx_t_13) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
      __Pyx_XDECREF(__pyx_t_9); __pyx_t_9 = 0;
      if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 393, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_7);
    }
    __Pyx_XDECREF_SET(__pyx_v_child_ints, __pyx_t_7);
    __pyx_t_7 = 0;

    __pyx_t_9 = __pyx_v_F_int;
    __Pyx_INCREF(__pyx_t_9);
    __pyx_t_13 = 0;
    {
      PyObject *__pyx_callargs[2] = {__pyx_t_9, __pyx_v_parent_var};
      __pyx_t_7 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_get, __pyx_callargs+__pyx_t_13, (2-__pyx_t_13) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
      __Pyx_XDECREF(__pyx_t_9); __pyx_t_9 = 0;
      if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 394, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_7);
    }
    __Pyx_XDECREF_SET(__pyx_v_parent_ints, __pyx_t_7);
    __pyx_t_7 = 0;

    __pyx_t_1 = __Pyx_PyObject_IsTrue(__pyx_v_child_ints); if (unlikely((__pyx_t_1 < 0))) __PYX_ERR(0, 395, __pyx_L1_error)
    __pyx_t_3 = (!__pyx_t_1);
    if (!__pyx_t_3) {
    } else {
      __pyx_t_2 = __pyx_t_3;
      goto __pyx_L45_bool_binop_done;
    }
    __pyx_t_3 = __Pyx_PyObject_IsTrue(__pyx_v_parent_ints); if (unlikely((__pyx_t_3 < 0))) __PYX_ERR(0, 395, __pyx_L1_error)
    __pyx_t_1 = (!__pyx_t_3);
    __pyx_t_2 = __pyx_t_1;
    __pyx_L45_bool_binop_done:;
    if (__pyx_t_2) {

      __Pyx_XDECREF(__pyx_r);
      __pyx_r = Py_None; __Pyx_INCREF(Py_None);
      __Pyx_DECREF(__pyx_t_4); __pyx_t_4 = 0;
      goto __pyx_L0;

    }

    __pyx_t_7 = PySet_New(0); if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 398, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_7);
    __Pyx_XDECREF_SET(__pyx_v_reachable_parent_ints, __pyx_t_7);
    __pyx_t_7 = 0;

    __pyx_t_14 = PyObject_Length(__pyx_v_parent_ints); if (unlikely(__pyx_t_14 == ((Py_ssize_t)-1))) __PYX_ERR(0, 399, __pyx_L1_error)
    __pyx_v_n_parent = __pyx_t_14;

    if (likely(PyList_CheckExact(__pyx_v_edges_info)) || PyTuple_CheckExact(__pyx_v_edges_info)) {
      __pyx_t_7 = __pyx_v_edges_info; __Pyx_INCREF(__pyx_t_7);
      __pyx_t_14 = 0;
      __pyx_t_15 = NULL;
    } else {
      __pyx_t_14 = -1; __pyx_t_7 = PyObject_GetIter(__pyx_v_edges_info); if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 400, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_7);
      __pyx_t_15 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_7); if (unlikely(!__pyx_t_15)) __PYX_ERR(0, 400, __pyx_L1_error)
    }
    for (;;) {
      if (likely(!__pyx_t_15)) {
        if (likely(PyList_CheckExact(__pyx_t_7))) {
          {
            Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_t_7);
            #if !CYTHON_ASSUME_SAFE_SIZE
            if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 400, __pyx_L1_error)
            #endif
            if (__pyx_t_14 >= __pyx_temp) break;
          }
          __pyx_t_9 = __Pyx_PyList_GetItemRefFast(__pyx_t_7, __pyx_t_14, __Pyx_ReferenceSharing_OwnStrongReference);
          ++__pyx_t_14;
        } else {
          {
            Py_ssize_t __pyx_temp = __Pyx_PyTuple_GET_SIZE(__pyx_t_7);
            #if !CYTHON_ASSUME_SAFE_SIZE
            if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 400, __pyx_L1_error)
            #endif
            if (__pyx_t_14 >= __pyx_temp) break;
          }
          #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
          __pyx_t_9 = __Pyx_NewRef(PyTuple_GET_ITEM(__pyx_t_7, __pyx_t_14));
          #else
          __pyx_t_9 = __Pyx_PySequence_ITEM(__pyx_t_7, __pyx_t_14);
          #endif
          ++__pyx_t_14;
        }
        if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 400, __pyx_L1_error)
      } else {
        __pyx_t_9 = __pyx_t_15(__pyx_t_7);
        if (unlikely(!__pyx_t_9)) {
          PyObject* exc_type = PyErr_Occurred();
          if (exc_type) {
            if (unlikely(!__Pyx_PyErr_GivenExceptionMatches(exc_type, PyExc_StopIteration))) __PYX_ERR(0, 400, __pyx_L1_error)
            PyErr_Clear();
          }
          break;
        }
      }
      __Pyx_GOTREF(__pyx_t_9);
      if ((likely(PyTuple_CheckExact(__pyx_t_9))) || (PyList_CheckExact(__pyx_t_9))) {
        PyObject* sequence = __pyx_t_9;
        Py_ssize_t size = __Pyx_PySequence_SIZE(sequence);
        if (unlikely(size != 2)) {
          if (size > 2) __Pyx_RaiseTooManyValuesError(2);
          else if (size >= 0) __Pyx_RaiseNeedMoreValuesError(size);
          __PYX_ERR(0, 400, __pyx_L1_error)
        }
        #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
        if (likely(PyTuple_CheckExact(sequence))) {
          __pyx_t_8 = PyTuple_GET_ITEM(sequence, 0);
          __Pyx_INCREF(__pyx_t_8);
          __pyx_t_10 = PyTuple_GET_ITEM(sequence, 1);
          __Pyx_INCREF(__pyx_t_10);
        } else {
          __pyx_t_8 = __Pyx_PyList_GetItemRefFast(sequence, 0, __Pyx_ReferenceSharing_SharedReference);
          if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 400, __pyx_L1_error)
          __Pyx_XGOTREF(__pyx_t_8);
          __pyx_t_10 = __Pyx_PyList_GetItemRefFast(sequence, 1, __Pyx_ReferenceSharing_SharedReference);
          if (unlikely(!__pyx_t_10)) __PYX_ERR(0, 400, __pyx_L1_error)
          __Pyx_XGOTREF(__pyx_t_10);
        }
        #else
        __pyx_t_8 = __Pyx_PySequence_ITEM(sequence, 0); if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 400, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_8);
        __pyx_t_10 = __Pyx_PySequence_ITEM(sequence, 1); if (unlikely(!__pyx_t_10)) __PYX_ERR(0, 400, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_10);
        #endif
        __Pyx_DECREF(__pyx_t_9); __pyx_t_9 = 0;
      } else {
        Py_ssize_t index = -1;
        __pyx_t_11 = PyObject_GetIter(__pyx_t_9); if (unlikely(!__pyx_t_11)) __PYX_ERR(0, 400, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_11);
        __Pyx_DECREF(__pyx_t_9); __pyx_t_9 = 0;
        __pyx_t_12 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_11);
        index = 0; __pyx_t_8 = __pyx_t_12(__pyx_t_11); if (unlikely(!__pyx_t_8)) goto __pyx_L49_unpacking_failed;
        __Pyx_GOTREF(__pyx_t_8);
        index = 1; __pyx_t_10 = __pyx_t_12(__pyx_t_11); if (unlikely(!__pyx_t_10)) goto __pyx_L49_unpacking_failed;
        __Pyx_GOTREF(__pyx_t_10);
        if (__Pyx_IternextUnpackEndCheck(__pyx_t_12(__pyx_t_11), 2) < (0)) __PYX_ERR(0, 400, __pyx_L1_error)
        __pyx_t_12 = NULL;
        __Pyx_DECREF(__pyx_t_11); __pyx_t_11 = 0;
        goto __pyx_L50_unpacking_done;
        __pyx_L49_unpacking_failed:;
        __Pyx_DECREF(__pyx_t_11); __pyx_t_11 = 0;
        __pyx_t_12 = NULL;
        if (__Pyx_IterFinish() == 0) __Pyx_RaiseNeedMoreValuesError(index);
        __PYX_ERR(0, 400, __pyx_L1_error)
        __pyx_L50_unpacking_done:;
      }
      __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_t_10); if (unlikely((__pyx_t_2 == (int)-1) && PyErr_Occurred())) __PYX_ERR(0, 400, __pyx_L1_error)
      __Pyx_DECREF(__pyx_t_10); __pyx_t_10 = 0;
      __Pyx_XDECREF_SET(__pyx_v_edge_label, __pyx_t_8);
      __pyx_t_8 = 0;
      __pyx_v_is_fwd = __pyx_t_2;

      __pyx_t_17 = PyObject_Length(__pyx_v_child_ints); if (unlikely(__pyx_t_17 == ((Py_ssize_t)-1))) __PYX_ERR(0, 401, __pyx_L1_error)
      __pyx_t_16 = PyObject_Length(__pyx_v_parent_ints); if (unlikely(__pyx_t_16 == ((Py_ssize_t)-1))) __PYX_ERR(0, 401, __pyx_L1_error)
      __pyx_t_2 = (__pyx_t_17 <= __pyx_t_16);
      if (__pyx_t_2) {

        if (__pyx_v_is_fwd) {
          __Pyx_INCREF(__pyx_v_cg_in);
          __pyx_t_9 = __pyx_v_cg_in;
        } else {
          __Pyx_INCREF(__pyx_v_cg_out);
          __pyx_t_9 = __pyx_v_cg_out;
        }
        __Pyx_XDECREF_SET(__pyx_v_reverse_neighbor_map, __pyx_t_9);
        __pyx_t_9 = 0;

        if (likely(PyList_CheckExact(__pyx_v_child_ints)) || PyTuple_CheckExact(__pyx_v_child_ints)) {
          __pyx_t_9 = __pyx_v_child_ints; __Pyx_INCREF(__pyx_t_9);
          __pyx_t_16 = 0;
          __pyx_t_18 = NULL;
        } else {
          __pyx_t_16 = -1; __pyx_t_9 = PyObject_GetIter(__pyx_v_child_ints); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 403, __pyx_L1_error)
          __Pyx_GOTREF(__pyx_t_9);
          __pyx_t_18 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_9); if (unlikely(!__pyx_t_18)) __PYX_ERR(0, 403, __pyx_L1_error)
        }
        for (;;) {
          if (likely(!__pyx_t_18)) {
            if (likely(PyList_CheckExact(__pyx_t_9))) {
              {
                Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_t_9);
                #if !CYTHON_ASSUME_SAFE_SIZE
                if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 403, __pyx_L1_error)
                #endif
                if (__pyx_t_16 >= __pyx_temp) break;
              }
              __pyx_t_10 = __Pyx_PyList_GetItemRefFast(__pyx_t_9, __pyx_t_16, __Pyx_ReferenceSharing_OwnStrongReference);
              ++__pyx_t_16;
            } else {
              {
                Py_ssize_t __pyx_temp = __Pyx_PyTuple_GET_SIZE(__pyx_t_9);
                #if !CYTHON_ASSUME_SAFE_SIZE
                if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 403, __pyx_L1_error)
                #endif
                if (__pyx_t_16 >= __pyx_temp) break;
              }
              #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
              __pyx_t_10 = __Pyx_NewRef(PyTuple_GET_ITEM(__pyx_t_9, __pyx_t_16));
              #else
              __pyx_t_10 = __Pyx_PySequence_ITEM(__pyx_t_9, __pyx_t_16);
              #endif
              ++__pyx_t_16;
            }
            if (unlikely(!__pyx_t_10)) __PYX_ERR(0, 403, __pyx_L1_error)
          } else {
            __pyx_t_10 = __pyx_t_18(__pyx_t_9);
            if (unlikely(!__pyx_t_10)) {
              PyObject* exc_type = PyErr_Occurred();
              if (exc_type) {
                if (unlikely(!__Pyx_PyErr_GivenExceptionMatches(exc_type, PyExc_StopIteration))) __PYX_ERR(0, 403, __pyx_L1_error)
                PyErr_Clear();
              }
              break;
            }
          }
          __Pyx_GOTREF(__pyx_t_10);
          __Pyx_XDECREF_SET(__pyx_v_c_int, __pyx_t_10);
          __pyx_t_10 = 0;

          __pyx_t_8 = __pyx_v_reverse_neighbor_map;
          __Pyx_INCREF(__pyx_t_8);
          __pyx_t_11 = PyTuple_New(2); if (unlikely(!__pyx_t_11)) __PYX_ERR(0, 404, __pyx_L1_error)
          __Pyx_GOTREF(__pyx_t_11);
          __Pyx_INCREF(__pyx_v_c_int);
          __Pyx_GIVEREF(__pyx_v_c_int);
          if (__Pyx_PyTuple_SET_ITEM(__pyx_t_11, 0, __pyx_v_c_int) != (0)) __PYX_ERR(0, 404, __pyx_L1_error);
          __Pyx_INCREF(__pyx_v_edge_label);
          __Pyx_GIVEREF(__pyx_v_edge_label);
          if (__Pyx_PyTuple_SET_ITEM(__pyx_t_11, 1, __pyx_v_edge_label) != (0)) __PYX_ERR(0, 404, __pyx_L1_error);
          __pyx_t_13 = 0;
          {
            PyObject *__pyx_callargs[2] = {__pyx_t_8, __pyx_t_11};
            __pyx_t_10 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_get, __pyx_callargs+__pyx_t_13, (2-__pyx_t_13) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
            __Pyx_XDECREF(__pyx_t_8); __pyx_t_8 = 0;
            __Pyx_DECREF(__pyx_t_11); __pyx_t_11 = 0;
            if (unlikely(!__pyx_t_10)) __PYX_ERR(0, 404, __pyx_L1_error)
            __Pyx_GOTREF(__pyx_t_10);
          }
          __Pyx_XDECREF_SET(__pyx_v_neighbors, __pyx_t_10);
          __pyx_t_10 = 0;

          __pyx_t_2 = (__pyx_v_neighbors == Py_None);
          if (__pyx_t_2) {

            goto __pyx_L52_continue;

          }

          if (likely(PyList_CheckExact(__pyx_v_neighbors)) || PyTuple_CheckExact(__pyx_v_neighbors)) {
            __pyx_t_10 = __pyx_v_neighbors; __Pyx_INCREF(__pyx_t_10);
            __pyx_t_17 = 0;
            __pyx_t_19 = NULL;
          } else {
            __pyx_t_17 = -1; __pyx_t_10 = PyObject_GetIter(__pyx_v_neighbors); if (unlikely(!__pyx_t_10)) __PYX_ERR(0, 407, __pyx_L1_error)
            __Pyx_GOTREF(__pyx_t_10);
            __pyx_t_19 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_10); if (unlikely(!__pyx_t_19)) __PYX_ERR(0, 407, __pyx_L1_error)
          }
          for (;;) {
            if (likely(!__pyx_t_19)) {
              if (likely(PyList_CheckExact(__pyx_t_10))) {
                {
                  Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_t_10);
                  #if !CYTHON_ASSUME_SAFE_SIZE
                  if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 407, __pyx_L1_error)
                  #endif
                  if (__pyx_t_17 >= __pyx_temp) break;
                }
                __pyx_t_11 = __Pyx_PyList_GetItemRefFast(__pyx_t_10, __pyx_t_17, __Pyx_ReferenceSharing_OwnStrongReference);
                ++__pyx_t_17;
              } else {
                {
                  Py_ssize_t __pyx_temp = __Pyx_PyTuple_GET_SIZE(__pyx_t_10);
                  #if !CYTHON_ASSUME_SAFE_SIZE
                  if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 407, __pyx_L1_error)
                  #endif
                  if (__pyx_t_17 >= __pyx_temp) break;
                }
                #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
                __pyx_t_11 = __Pyx_NewRef(PyTuple_GET_ITEM(__pyx_t_10, __pyx_t_17));
                #else
                __pyx_t_11 = __Pyx_PySequence_ITEM(__pyx_t_10, __pyx_t_17);
                #endif
                ++__pyx_t_17;
              }
              if (unlikely(!__pyx_t_11)) __PYX_ERR(0, 407, __pyx_L1_error)
            } else {
              __pyx_t_11 = __pyx_t_19(__pyx_t_10);
              if (unlikely(!__pyx_t_11)) {
                PyObject* exc_type = PyErr_Occurred();
                if (exc_type) {
                  if (unlikely(!__Pyx_PyErr_GivenExceptionMatches(exc_type, PyExc_StopIteration))) __PYX_ERR(0, 407, __pyx_L1_error)
                  PyErr_Clear();
                }
                break;
              }
            }
            __Pyx_GOTREF(__pyx_t_11);
            __Pyx_XDECREF_SET(__pyx_v_n_int, __pyx_t_11);
            __pyx_t_11 = 0;

            __pyx_t_2 = (__Pyx_PySequence_ContainsTF(__pyx_v_n_int, __pyx_v_parent_ints, Py_EQ)); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 408, __pyx_L1_error)
            if (__pyx_t_2) {

              __pyx_t_8 = __pyx_v_reachable_parent_ints;
              __Pyx_INCREF(__pyx_t_8);
              __pyx_t_13 = 0;
              {
                PyObject *__pyx_callargs[2] = {__pyx_t_8, __pyx_v_n_int};
                __pyx_t_11 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_add, __pyx_callargs+__pyx_t_13, (2-__pyx_t_13) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
                __Pyx_XDECREF(__pyx_t_8); __pyx_t_8 = 0;
                if (unlikely(!__pyx_t_11)) __PYX_ERR(0, 409, __pyx_L1_error)
                __Pyx_GOTREF(__pyx_t_11);
              }
              __Pyx_DECREF(__pyx_t_11); __pyx_t_11 = 0;

            }

          }
          __Pyx_DECREF(__pyx_t_10); __pyx_t_10 = 0;

          __pyx_t_17 = PyObject_Length(__pyx_v_reachable_parent_ints); if (unlikely(__pyx_t_17 == ((Py_ssize_t)-1))) __PYX_ERR(0, 410, __pyx_L1_error)
          __pyx_t_2 = (__pyx_t_17 == __pyx_v_n_parent);
          if (__pyx_t_2) {

            goto __pyx_L53_break;

          }

          __pyx_L52_continue:;
        }
        __Pyx_DECREF(__pyx_t_9); __pyx_t_9 = 0;
        goto __pyx_L60_for_end;
        __pyx_L53_break:;
        __Pyx_DECREF(__pyx_t_9); __pyx_t_9 = 0;
        goto __pyx_L60_for_end;
        __pyx_L60_for_end:;

        goto __pyx_L51;
      }

       {
        if (__pyx_v_is_fwd) {
          __Pyx_INCREF(__pyx_v_cg_out);
          __pyx_t_9 = __pyx_v_cg_out;
        } else {
          __Pyx_INCREF(__pyx_v_cg_in);
          __pyx_t_9 = __pyx_v_cg_in;
        }
        __Pyx_XDECREF_SET(__pyx_v_forward_neighbor_map, __pyx_t_9);
        __pyx_t_9 = 0;

        if (likely(PyList_CheckExact(__pyx_v_parent_ints)) || PyTuple_CheckExact(__pyx_v_parent_ints)) {
          __pyx_t_9 = __pyx_v_parent_ints; __Pyx_INCREF(__pyx_t_9);
          __pyx_t_16 = 0;
          __pyx_t_18 = NULL;
        } else {
          __pyx_t_16 = -1; __pyx_t_9 = PyObject_GetIter(__pyx_v_parent_ints); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 414, __pyx_L1_error)
          __Pyx_GOTREF(__pyx_t_9);
          __pyx_t_18 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_9); if (unlikely(!__pyx_t_18)) __PYX_ERR(0, 414, __pyx_L1_error)
        }
        for (;;) {
          if (likely(!__pyx_t_18)) {
            if (likely(PyList_CheckExact(__pyx_t_9))) {
              {
                Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_t_9);
                #if !CYTHON_ASSUME_SAFE_SIZE
                if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 414, __pyx_L1_error)
                #endif
                if (__pyx_t_16 >= __pyx_temp) break;
              }
              __pyx_t_10 = __Pyx_PyList_GetItemRefFast(__pyx_t_9, __pyx_t_16, __Pyx_ReferenceSharing_OwnStrongReference);
              ++__pyx_t_16;
            } else {
              {
                Py_ssize_t __pyx_temp = __Pyx_PyTuple_GET_SIZE(__pyx_t_9);
                #if !CYTHON_ASSUME_SAFE_SIZE
                if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 414, __pyx_L1_error)
                #endif
                if (__pyx_t_16 >= __pyx_temp) break;
              }
              #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
              __pyx_t_10 = __Pyx_NewRef(PyTuple_GET_ITEM(__pyx_t_9, __pyx_t_16));
              #else
              __pyx_t_10 = __Pyx_PySequence_ITEM(__pyx_t_9, __pyx_t_16);
              #endif
              ++__pyx_t_16;
            }
            if (unlikely(!__pyx_t_10)) __PYX_ERR(0, 414, __pyx_L1_error)
          } else {
            __pyx_t_10 = __pyx_t_18(__pyx_t_9);
            if (unlikely(!__pyx_t_10)) {
              PyObject* exc_type = PyErr_Occurred();
              if (exc_type) {
                if (unlikely(!__Pyx_PyErr_GivenExceptionMatches(exc_type, PyExc_StopIteration))) __PYX_ERR(0, 414, __pyx_L1_error)
                PyErr_Clear();
              }
              break;
            }
          }
          __Pyx_GOTREF(__pyx_t_10);
          __Pyx_XDECREF_SET(__pyx_v_p_int, __pyx_t_10);
          __pyx_t_10 = 0;

          __pyx_t_11 = __pyx_v_forward_neighbor_map;
          __Pyx_INCREF(__pyx_t_11);
          __pyx_t_8 = PyTuple_New(2); if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 415, __pyx_L1_error)
          __Pyx_GOTREF(__pyx_t_8);
          __Pyx_INCREF(__pyx_v_p_int);
          __Pyx_GIVEREF(__pyx_v_p_int);
          if (__Pyx_PyTuple_SET_ITEM(__pyx_t_8, 0, __pyx_v_p_int) != (0)) __PYX_ERR(0, 415, __pyx_L1_error);
          __Pyx_INCREF(__pyx_v_edge_label);
          __Pyx_GIVEREF(__pyx_v_edge_label);
          if (__Pyx_PyTuple_SET_ITEM(__pyx_t_8, 1, __pyx_v_edge_label) != (0)) __PYX_ERR(0, 415, __pyx_L1_error);
          __pyx_t_13 = 0;
          {
            PyObject *__pyx_callargs[2] = {__pyx_t_11, __pyx_t_8};
            __pyx_t_10 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_get, __pyx_callargs+__pyx_t_13, (2-__pyx_t_13) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
            __Pyx_XDECREF(__pyx_t_11); __pyx_t_11 = 0;
            __Pyx_DECREF(__pyx_t_8); __pyx_t_8 = 0;
            if (unlikely(!__pyx_t_10)) __PYX_ERR(0, 415, __pyx_L1_error)
            __Pyx_GOTREF(__pyx_t_10);
          }
          __Pyx_XDECREF_SET(__pyx_v_neighbors, __pyx_t_10);
          __pyx_t_10 = 0;

          __pyx_t_2 = (__pyx_v_neighbors == Py_None);
          if (__pyx_t_2) {

            goto __pyx_L61_continue;

          }

          if (likely(PyList_CheckExact(__pyx_v_neighbors)) || PyTuple_CheckExact(__pyx_v_neighbors)) {
            __pyx_t_10 = __pyx_v_neighbors; __Pyx_INCREF(__pyx_t_10);
            __pyx_t_17 = 0;
            __pyx_t_19 = NULL;
          } else {
            __pyx_t_17 = -1; __pyx_t_10 = PyObject_GetIter(__pyx_v_neighbors); if (unlikely(!__pyx_t_10)) __PYX_ERR(0, 418, __pyx_L1_error)
            __Pyx_GOTREF(__pyx_t_10);
            __pyx_t_19 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_10); if (unlikely(!__pyx_t_19)) __PYX_ERR(0, 418, __pyx_L1_error)
          }
          for (;;) {
            if (likely(!__pyx_t_19)) {
              if (likely(PyList_CheckExact(__pyx_t_10))) {
                {
                  Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_t_10);
                  #if !CYTHON_ASSUME_SAFE_SIZE
                  if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 418, __pyx_L1_error)
                  #endif
                  if (__pyx_t_17 >= __pyx_temp) break;
                }
                __pyx_t_8 = __Pyx_PyList_GetItemRefFast(__pyx_t_10, __pyx_t_17, __Pyx_ReferenceSharing_OwnStrongReference);
                ++__pyx_t_17;
              } else {
                {
                  Py_ssize_t __pyx_temp = __Pyx_PyTuple_GET_SIZE(__pyx_t_10);
                  #if !CYTHON_ASSUME_SAFE_SIZE
                  if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 418, __pyx_L1_error)
                  #endif
                  if (__pyx_t_17 >= __pyx_temp) break;
                }
                #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
                __pyx_t_8 = __Pyx_NewRef(PyTuple_GET_ITEM(__pyx_t_10, __pyx_t_17));
                #else
                __pyx_t_8 = __Pyx_PySequence_ITEM(__pyx_t_10, __pyx_t_17);
                #endif
                ++__pyx_t_17;
              }
              if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 418, __pyx_L1_error)
            } else {
              __pyx_t_8 = __pyx_t_19(__pyx_t_10);
              if (unlikely(!__pyx_t_8)) {
                PyObject* exc_type = PyErr_Occurred();
                if (exc_type) {
                  if (unlikely(!__Pyx_PyErr_GivenExceptionMatches(exc_type, PyExc_StopIteration))) __PYX_ERR(0, 418, __pyx_L1_error)
                  PyErr_Clear();
                }
                break;
              }
            }
            __Pyx_GOTREF(__pyx_t_8);
            __Pyx_XDECREF_SET(__pyx_v_n_int, __pyx_t_8);
            __pyx_t_8 = 0;

            __pyx_t_2 = (__Pyx_PySequence_ContainsTF(__pyx_v_n_int, __pyx_v_child_ints, Py_EQ)); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 419, __pyx_L1_error)
            if (__pyx_t_2) {

              __pyx_t_11 = __pyx_v_reachable_parent_ints;
              __Pyx_INCREF(__pyx_t_11);
              __pyx_t_13 = 0;
              {
                PyObject *__pyx_callargs[2] = {__pyx_t_11, __pyx_v_p_int};
                __pyx_t_8 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_add, __pyx_callargs+__pyx_t_13, (2-__pyx_t_13) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
                __Pyx_XDECREF(__pyx_t_11); __pyx_t_11 = 0;
                if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 420, __pyx_L1_error)
                __Pyx_GOTREF(__pyx_t_8);
              }
              __Pyx_DECREF(__pyx_t_8); __pyx_t_8 = 0;

              goto __pyx_L65_break;

            }

          }
          __Pyx_DECREF(__pyx_t_10); __pyx_t_10 = 0;
          goto __pyx_L67_for_end;
          __pyx_L65_break:;
          __Pyx_DECREF(__pyx_t_10); __pyx_t_10 = 0;
          goto __pyx_L67_for_end;
          __pyx_L67_for_end:;

          __pyx_t_17 = PyObject_Length(__pyx_v_reachable_parent_ints); if (unlikely(__pyx_t_17 == ((Py_ssize_t)-1))) __PYX_ERR(0, 422, __pyx_L1_error)
          __pyx_t_2 = (__pyx_t_17 == __pyx_v_n_parent);
          if (__pyx_t_2) {

            goto __pyx_L62_break;

          }

          __pyx_L61_continue:;
        }
        __Pyx_DECREF(__pyx_t_9); __pyx_t_9 = 0;
        goto __pyx_L69_for_end;
        __pyx_L62_break:;
        __Pyx_DECREF(__pyx_t_9); __pyx_t_9 = 0;
        goto __pyx_L69_for_end;
        __pyx_L69_for_end:;
      }
      __pyx_L51:;

      __pyx_t_16 = PyObject_Length(__pyx_v_reachable_parent_ints); if (unlikely(__pyx_t_16 == ((Py_ssize_t)-1))) __PYX_ERR(0, 424, __pyx_L1_error)
      __pyx_t_2 = (__pyx_t_16 == __pyx_v_n_parent);
      if (__pyx_t_2) {

        goto __pyx_L48_break;

      }

    }
    __Pyx_DECREF(__pyx_t_7); __pyx_t_7 = 0;
    goto __pyx_L71_for_end;
    __pyx_L48_break:;
    __Pyx_DECREF(__pyx_t_7); __pyx_t_7 = 0;
    goto __pyx_L71_for_end;
    __pyx_L71_for_end:;

    __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_v_reachable_parent_ints); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 427, __pyx_L1_error)
    __pyx_t_1 = (!__pyx_t_2);
    if (__pyx_t_1) {

      __Pyx_XDECREF(__pyx_r);
      __pyx_r = Py_None; __Pyx_INCREF(Py_None);
      __Pyx_DECREF(__pyx_t_4); __pyx_t_4 = 0;
      goto __pyx_L0;

    }

    if (unlikely((PyObject_SetItem(__pyx_v_F_int, __pyx_v_parent_var, __pyx_v_reachable_parent_ints) < 0))) __PYX_ERR(0, 429, __pyx_L1_error)

  }
  __Pyx_DECREF(__pyx_t_4); __pyx_t_4 = 0;

  __Pyx_XDECREF(__pyx_r);
  __Pyx_INCREF(__pyx_v_F_int);
  __pyx_r = __pyx_v_F_int;
  goto __pyx_L0;

  __pyx_L1_error:;
  __Pyx_XDECREF(__pyx_t_4);
  __Pyx_XDECREF(__pyx_t_7);
  __Pyx_XDECREF(__pyx_t_8);
  __Pyx_XDECREF(__pyx_t_9);
  __Pyx_XDECREF(__pyx_t_10);
  __Pyx_XDECREF(__pyx_t_11);
  __Pyx_AddTraceback("_fast_match.propagate_candidates_dag_fast", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __pyx_r = NULL;
  __pyx_L0:;
  __Pyx_XDECREF(__pyx_v_parent_var);
  __Pyx_XDECREF(__pyx_v_child_var);
  __Pyx_XDECREF(__pyx_v_edges_info);
  __Pyx_XDECREF(__pyx_v_edge_label);
  __Pyx_XDECREF(__pyx_v_parent_ints);
  __Pyx_XDECREF(__pyx_v_child_ints);
  __Pyx_XDECREF(__pyx_v_reachable_child_ints);
  __Pyx_XDECREF(__pyx_v_reachable_parent_ints);
  __Pyx_XDECREF(__pyx_v_cg_out);
  __Pyx_XDECREF(__pyx_v_cg_in);
  __Pyx_XDECREF(__pyx_v_neighbor_map);
  __Pyx_XDECREF(__pyx_v_p_int);
  __Pyx_XDECREF(__pyx_v_neighbors);
  __Pyx_XDECREF(__pyx_v_n_int);
  __Pyx_XDECREF(__pyx_v_reverse_neighbor_map);
  __Pyx_XDECREF(__pyx_v_c_int);
  __Pyx_XDECREF(__pyx_v_forward_neighbor_map);
  __Pyx_XGIVEREF(__pyx_r);
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}

static PyObject *__pyx_pw_11_fast_match_11prune_pairwise_values_fast(PyObject *__pyx_self,
#if CYTHON_METH_FASTCALL
PyObject *const *__pyx_args, Py_ssize_t __pyx_nargs, PyObject *__pyx_kwds
#else
PyObject *__pyx_args, PyObject *__pyx_kwds
#endif
);
PyDoc_STRVAR(__pyx_doc_11_fast_match_10prune_pairwise_values_fast, "\n    Compute pairwise-pruning survivors from already extracted attribute values.\n\n    Returns:\n        (valid_z1, valid_z2)\n    ");
static PyMethodDef __pyx_mdef_11_fast_match_11prune_pairwise_values_fast = {"prune_pairwise_values_fast", (PyCFunction)(void(*)(void))(__Pyx_PyCFunction_FastCallWithKeywords)__pyx_pw_11_fast_match_11prune_pairwise_values_fast, __Pyx_METH_FASTCALL|METH_KEYWORDS, __pyx_doc_11_fast_match_10prune_pairwise_values_fast};
static PyObject *__pyx_pw_11_fast_match_11prune_pairwise_values_fast(PyObject *__pyx_self,
#if CYTHON_METH_FASTCALL
PyObject *const *__pyx_args, Py_ssize_t __pyx_nargs, PyObject *__pyx_kwds
#else
PyObject *__pyx_args, PyObject *__pyx_kwds
#endif
) {
  PyObject *__pyx_v_vals1 = 0;
  PyObject *__pyx_v_vals2 = 0;
  PyObject *__pyx_v_operator = 0;
  #if !CYTHON_METH_FASTCALL
  CYTHON_UNUSED Py_ssize_t __pyx_nargs;
  #endif
  CYTHON_UNUSED PyObject *const *__pyx_kwvalues;
  PyObject* values[3] = {0,0,0};
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  PyObject *__pyx_r = 0;
  __Pyx_RefNannyDeclarations
  __Pyx_RefNannySetupContext("prune_pairwise_values_fast (wrapper)", 0);
  #if !CYTHON_METH_FASTCALL
  #if CYTHON_ASSUME_SAFE_SIZE
  __pyx_nargs = PyTuple_GET_SIZE(__pyx_args);
  #else
  __pyx_nargs = PyTuple_Size(__pyx_args); if (unlikely(__pyx_nargs < 0)) return NULL;
  #endif
  #endif
  __pyx_kwvalues = __Pyx_KwValues_FASTCALL(__pyx_args, __pyx_nargs);
  {
    PyObject ** const __pyx_pyargnames[] = {&__pyx_mstate_global->__pyx_n_u_vals1,&__pyx_mstate_global->__pyx_n_u_vals2,&__pyx_mstate_global->__pyx_n_u_operator,0};
    const Py_ssize_t __pyx_kwds_len = (__pyx_kwds) ? __Pyx_NumKwargs_FASTCALL(__pyx_kwds) : 0;
    if (unlikely(__pyx_kwds_len) < 0) __PYX_ERR(0, 434, __pyx_L3_error)
    if (__pyx_kwds_len > 0) {
      switch (__pyx_nargs) {
        case  3:
        values[2] = __Pyx_ArgRef_FASTCALL(__pyx_args, 2);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[2])) __PYX_ERR(0, 434, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  2:
        values[1] = __Pyx_ArgRef_FASTCALL(__pyx_args, 1);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[1])) __PYX_ERR(0, 434, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  1:
        values[0] = __Pyx_ArgRef_FASTCALL(__pyx_args, 0);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[0])) __PYX_ERR(0, 434, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  0: break;
        default: goto __pyx_L5_argtuple_error;
      }
      const Py_ssize_t kwd_pos_args = __pyx_nargs;
      if (__Pyx_ParseKeywords(__pyx_kwds, __pyx_kwvalues, __pyx_pyargnames, 0, values, kwd_pos_args, __pyx_kwds_len, "prune_pairwise_values_fast", 0) < (0)) __PYX_ERR(0, 434, __pyx_L3_error)
      for (Py_ssize_t i = __pyx_nargs; i < 3; i++) {
        if (unlikely(!values[i])) { __Pyx_RaiseArgtupleInvalid("prune_pairwise_values_fast", 1, 3, 3, i); __PYX_ERR(0, 434, __pyx_L3_error) }
      }
    } else if (unlikely(__pyx_nargs != 3)) {
      goto __pyx_L5_argtuple_error;
    } else {
      values[0] = __Pyx_ArgRef_FASTCALL(__pyx_args, 0);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[0])) __PYX_ERR(0, 434, __pyx_L3_error)
      values[1] = __Pyx_ArgRef_FASTCALL(__pyx_args, 1);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[1])) __PYX_ERR(0, 434, __pyx_L3_error)
      values[2] = __Pyx_ArgRef_FASTCALL(__pyx_args, 2);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[2])) __PYX_ERR(0, 434, __pyx_L3_error)
    }
    __pyx_v_vals1 = ((PyObject*)values[0]);
    __pyx_v_vals2 = ((PyObject*)values[1]);
    __pyx_v_operator = values[2];
  }
  goto __pyx_L6_skip;
  __pyx_L5_argtuple_error:;
  __Pyx_RaiseArgtupleInvalid("prune_pairwise_values_fast", 1, 3, 3, __pyx_nargs); __PYX_ERR(0, 434, __pyx_L3_error)
  __pyx_L6_skip:;
  goto __pyx_L4_argument_unpacking_done;
  __pyx_L3_error:;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  __Pyx_AddTraceback("_fast_match.prune_pairwise_values_fast", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __Pyx_RefNannyFinishContext();
  return NULL;
  __pyx_L4_argument_unpacking_done:;
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_vals1), (&PyDict_Type), 1, "vals1", 1))) __PYX_ERR(0, 434, __pyx_L1_error)
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_vals2), (&PyDict_Type), 1, "vals2", 1))) __PYX_ERR(0, 434, __pyx_L1_error)
  __pyx_r = __pyx_pf_11_fast_match_10prune_pairwise_values_fast(__pyx_self, __pyx_v_vals1, __pyx_v_vals2, __pyx_v_operator);

  goto __pyx_L0;
  __pyx_L1_error:;
  __pyx_r = NULL;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  goto __pyx_L7_cleaned_up;
  __pyx_L0:;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  __pyx_L7_cleaned_up:;
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}

static PyObject *__pyx_pf_11_fast_match_10prune_pairwise_values_fast(CYTHON_UNUSED PyObject *__pyx_self, PyObject *__pyx_v_vals1, PyObject *__pyx_v_vals2, PyObject *__pyx_v_operator) {
  PyObject *__pyx_v_valid_z1 = 0;
  PyObject *__pyx_v_valid_z2 = 0;
  PyObject *__pyx_v_c1 = 0;
  PyObject *__pyx_v_v1 = 0;
  PyObject *__pyx_v_c2 = 0;
  PyObject *__pyx_v_v2 = 0;
  PyObject *__pyx_v_matching_c2s = 0;
  PyObject *__pyx_v_unique_vals1 = 0;
  PyObject *__pyx_v_unique_vals2 = 0;
  PyObject *__pyx_v_max_v2 = 0;
  PyObject *__pyx_v_min_v1 = 0;
  PyObject *__pyx_v_min_v2 = 0;
  PyObject *__pyx_v_max_v1 = 0;
  PyObject *__pyx_v_val_to_c2 = NULL;
  PyObject *__pyx_v_bucket = NULL;
  PyObject *__pyx_r = NULL;
  __Pyx_RefNannyDeclarations
  PyObject *__pyx_t_1 = NULL;
  int __pyx_t_2;
  int __pyx_t_3;
  int __pyx_t_4;
  PyObject *__pyx_t_5 = NULL;
  Py_ssize_t __pyx_t_6;
  Py_ssize_t __pyx_t_7;
  int __pyx_t_8;
  PyObject *__pyx_t_9 = NULL;
  int __pyx_t_10;
  size_t __pyx_t_11;
  Py_ssize_t __pyx_t_12;
  PyObject *__pyx_t_13 = NULL;
  PyObject *__pyx_t_14 = NULL;
  PyObject *__pyx_t_15 = NULL;
  Py_ssize_t __pyx_t_16;
  PyObject *__pyx_t_17 = NULL;
  int __pyx_t_18;
  PyObject *__pyx_t_19 = NULL;
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  __Pyx_RefNannySetupContext("prune_pairwise_values_fast", 0);

  __pyx_t_1 = PySet_New(0); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 441, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_1);
  __pyx_v_valid_z1 = __pyx_t_1;
  __pyx_t_1 = 0;

  __pyx_t_1 = PySet_New(0); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 442, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_1);
  __pyx_v_valid_z2 = __pyx_t_1;
  __pyx_t_1 = 0;

  __pyx_t_3 = __Pyx_PyObject_IsTrue(__pyx_v_vals1); if (unlikely((__pyx_t_3 < 0))) __PYX_ERR(0, 447, __pyx_L1_error)
  __pyx_t_4 = (!__pyx_t_3);
  if (!__pyx_t_4) {
  } else {
    __pyx_t_2 = __pyx_t_4;
    goto __pyx_L4_bool_binop_done;
  }
  __pyx_t_4 = __Pyx_PyObject_IsTrue(__pyx_v_vals2); if (unlikely((__pyx_t_4 < 0))) __PYX_ERR(0, 447, __pyx_L1_error)
  __pyx_t_3 = (!__pyx_t_4);
  __pyx_t_2 = __pyx_t_3;
  __pyx_L4_bool_binop_done:;
  if (__pyx_t_2) {

    __Pyx_XDECREF(__pyx_r);
    __pyx_t_1 = PyTuple_New(2); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 448, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_1);
    __Pyx_INCREF(__pyx_v_valid_z1);
    __Pyx_GIVEREF(__pyx_v_valid_z1);
    if (__Pyx_PyTuple_SET_ITEM(__pyx_t_1, 0, __pyx_v_valid_z1) != (0)) __PYX_ERR(0, 448, __pyx_L1_error);
    __Pyx_INCREF(__pyx_v_valid_z2);
    __Pyx_GIVEREF(__pyx_v_valid_z2);
    if (__Pyx_PyTuple_SET_ITEM(__pyx_t_1, 1, __pyx_v_valid_z2) != (0)) __PYX_ERR(0, 448, __pyx_L1_error);
    __pyx_r = __pyx_t_1;
    __pyx_t_1 = 0;
    goto __pyx_L0;

  }

  __Pyx_GetModuleGlobalName(__pyx_t_1, __pyx_mstate_global->__pyx_n_u_ComparisonOp); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 450, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_1);
  __pyx_t_5 = __Pyx_PyObject_GetAttrStr(__pyx_t_1, __pyx_mstate_global->__pyx_n_u_EQ); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 450, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_5);
  __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;
  __pyx_t_1 = PyObject_RichCompare(__pyx_v_operator, __pyx_t_5, Py_EQ); __Pyx_XGOTREF(__pyx_t_1); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 450, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
  __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_t_1); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 450, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;
  if (__pyx_t_2) {

    __pyx_t_1 = __Pyx_PyDict_NewPresized(0); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 451, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_1);
    __pyx_v_val_to_c2 = ((PyObject*)__pyx_t_1);
    __pyx_t_1 = 0;

    __pyx_t_6 = 0;
    if (unlikely(__pyx_v_vals2 == Py_None)) {
      PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "items");
      __PYX_ERR(0, 452, __pyx_L1_error)
    }
    __pyx_t_5 = __Pyx_dict_iterator(__pyx_v_vals2, 1, __pyx_mstate_global->__pyx_n_u_items, (&__pyx_t_7), (&__pyx_t_8)); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 452, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_5);
    __Pyx_XDECREF(__pyx_t_1);
    __pyx_t_1 = __pyx_t_5;
    __pyx_t_5 = 0;
    while (1) {
      __pyx_t_10 = __Pyx_dict_iter_next(__pyx_t_1, __pyx_t_7, &__pyx_t_6, &__pyx_t_5, &__pyx_t_9, NULL, __pyx_t_8);
      if (unlikely(__pyx_t_10 == 0)) break;
      if (unlikely(__pyx_t_10 == -1)) __PYX_ERR(0, 452, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_5);
      __Pyx_GOTREF(__pyx_t_9);
      __Pyx_XDECREF_SET(__pyx_v_c2, __pyx_t_5);
      __pyx_t_5 = 0;
      __Pyx_XDECREF_SET(__pyx_v_v2, __pyx_t_9);
      __pyx_t_9 = 0;

      __pyx_t_9 = __Pyx_PyDict_GetItemDefault(__pyx_v_val_to_c2, __pyx_v_v2, Py_None); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 453, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_9);
      __Pyx_XDECREF_SET(__pyx_v_bucket, __pyx_t_9);
      __pyx_t_9 = 0;

      __pyx_t_2 = (__pyx_v_bucket == Py_None);
      if (__pyx_t_2) {

        __pyx_t_9 = PySet_New(0); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 455, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_9);
        __Pyx_DECREF_SET(__pyx_v_bucket, __pyx_t_9);
        __pyx_t_9 = 0;

        if (unlikely((PyDict_SetItem(__pyx_v_val_to_c2, __pyx_v_v2, __pyx_v_bucket) < 0))) __PYX_ERR(0, 456, __pyx_L1_error)

      }

      __pyx_t_5 = __pyx_v_bucket;
      __Pyx_INCREF(__pyx_t_5);
      __pyx_t_11 = 0;
      {
        PyObject *__pyx_callargs[2] = {__pyx_t_5, __pyx_v_c2};
        __pyx_t_9 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_add, __pyx_callargs+__pyx_t_11, (2-__pyx_t_11) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
        __Pyx_XDECREF(__pyx_t_5); __pyx_t_5 = 0;
        if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 457, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_9);
      }
      __Pyx_DECREF(__pyx_t_9); __pyx_t_9 = 0;
    }
    __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;

    __pyx_t_7 = 0;
    if (unlikely(__pyx_v_vals1 == Py_None)) {
      PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "items");
      __PYX_ERR(0, 458, __pyx_L1_error)
    }
    __pyx_t_9 = __Pyx_dict_iterator(__pyx_v_vals1, 1, __pyx_mstate_global->__pyx_n_u_items, (&__pyx_t_6), (&__pyx_t_8)); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 458, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_9);
    __Pyx_XDECREF(__pyx_t_1);
    __pyx_t_1 = __pyx_t_9;
    __pyx_t_9 = 0;
    while (1) {
      __pyx_t_10 = __Pyx_dict_iter_next(__pyx_t_1, __pyx_t_6, &__pyx_t_7, &__pyx_t_9, &__pyx_t_5, NULL, __pyx_t_8);
      if (unlikely(__pyx_t_10 == 0)) break;
      if (unlikely(__pyx_t_10 == -1)) __PYX_ERR(0, 458, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_9);
      __Pyx_GOTREF(__pyx_t_5);
      __Pyx_XDECREF_SET(__pyx_v_c1, __pyx_t_9);
      __pyx_t_9 = 0;
      __Pyx_XDECREF_SET(__pyx_v_v1, __pyx_t_5);
      __pyx_t_5 = 0;

      __pyx_t_5 = __Pyx_PyDict_GetItemDefault(__pyx_v_val_to_c2, __pyx_v_v1, Py_None); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 459, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_5);
      __Pyx_XDECREF_SET(__pyx_v_matching_c2s, __pyx_t_5);
      __pyx_t_5 = 0;

      __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_v_matching_c2s); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 460, __pyx_L1_error)
      if (__pyx_t_2) {

        __pyx_t_9 = __pyx_v_valid_z1;
        __Pyx_INCREF(__pyx_t_9);
        __pyx_t_11 = 0;
        {
          PyObject *__pyx_callargs[2] = {__pyx_t_9, __pyx_v_c1};
          __pyx_t_5 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_add, __pyx_callargs+__pyx_t_11, (2-__pyx_t_11) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
          __Pyx_XDECREF(__pyx_t_9); __pyx_t_9 = 0;
          if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 461, __pyx_L1_error)
          __Pyx_GOTREF(__pyx_t_5);
        }
        __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;

        __pyx_t_5 = PyNumber_InPlaceOr(__pyx_v_valid_z2, __pyx_v_matching_c2s); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 462, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_5);
        __Pyx_DECREF_SET(__pyx_v_valid_z2, __pyx_t_5);
        __pyx_t_5 = 0;

      }
    }
    __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;

    __Pyx_XDECREF(__pyx_r);
    __pyx_t_1 = PyTuple_New(2); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 463, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_1);
    __Pyx_INCREF(__pyx_v_valid_z1);
    __Pyx_GIVEREF(__pyx_v_valid_z1);
    if (__Pyx_PyTuple_SET_ITEM(__pyx_t_1, 0, __pyx_v_valid_z1) != (0)) __PYX_ERR(0, 463, __pyx_L1_error);
    __Pyx_INCREF(__pyx_v_valid_z2);
    __Pyx_GIVEREF(__pyx_v_valid_z2);
    if (__Pyx_PyTuple_SET_ITEM(__pyx_t_1, 1, __pyx_v_valid_z2) != (0)) __PYX_ERR(0, 463, __pyx_L1_error);
    __pyx_r = __pyx_t_1;
    __pyx_t_1 = 0;
    goto __pyx_L0;

  }

  __Pyx_GetModuleGlobalName(__pyx_t_1, __pyx_mstate_global->__pyx_n_u_ComparisonOp); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 465, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_1);
  __pyx_t_5 = __Pyx_PyObject_GetAttrStr(__pyx_t_1, __pyx_mstate_global->__pyx_n_u_NE); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 465, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_5);
  __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;
  __pyx_t_1 = PyObject_RichCompare(__pyx_v_operator, __pyx_t_5, Py_EQ); __Pyx_XGOTREF(__pyx_t_1); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 465, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
  __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_t_1); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 465, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;
  if (__pyx_t_2) {

    if (unlikely(__pyx_v_vals2 == Py_None)) {
      PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "values");
      __PYX_ERR(0, 466, __pyx_L1_error)
    }
    __pyx_t_1 = __Pyx_PyDict_Values(__pyx_v_vals2); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 466, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_1);
    __pyx_t_5 = PySet_New(__pyx_t_1); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 466, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_5);
    __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;
    __pyx_v_unique_vals2 = __pyx_t_5;
    __pyx_t_5 = 0;

    if (unlikely(__pyx_v_vals1 == Py_None)) {
      PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "values");
      __PYX_ERR(0, 467, __pyx_L1_error)
    }
    __pyx_t_5 = __Pyx_PyDict_Values(__pyx_v_vals1); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 467, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_5);
    __pyx_t_1 = PySet_New(__pyx_t_5); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 467, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_1);
    __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
    __pyx_v_unique_vals1 = __pyx_t_1;
    __pyx_t_1 = 0;

    __pyx_t_6 = 0;
    if (unlikely(__pyx_v_vals1 == Py_None)) {
      PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "items");
      __PYX_ERR(0, 468, __pyx_L1_error)
    }
    __pyx_t_5 = __Pyx_dict_iterator(__pyx_v_vals1, 1, __pyx_mstate_global->__pyx_n_u_items, (&__pyx_t_7), (&__pyx_t_8)); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 468, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_5);
    __Pyx_XDECREF(__pyx_t_1);
    __pyx_t_1 = __pyx_t_5;
    __pyx_t_5 = 0;
    while (1) {
      __pyx_t_10 = __Pyx_dict_iter_next(__pyx_t_1, __pyx_t_7, &__pyx_t_6, &__pyx_t_5, &__pyx_t_9, NULL, __pyx_t_8);
      if (unlikely(__pyx_t_10 == 0)) break;
      if (unlikely(__pyx_t_10 == -1)) __PYX_ERR(0, 468, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_5);
      __Pyx_GOTREF(__pyx_t_9);
      __Pyx_XDECREF_SET(__pyx_v_c1, __pyx_t_5);
      __pyx_t_5 = 0;
      __Pyx_XDECREF_SET(__pyx_v_v1, __pyx_t_9);
      __pyx_t_9 = 0;

      __pyx_t_12 = PyObject_Length(__pyx_v_unique_vals2); if (unlikely(__pyx_t_12 == ((Py_ssize_t)-1))) __PYX_ERR(0, 469, __pyx_L1_error)
      __pyx_t_3 = (__pyx_t_12 > 1);
      if (!__pyx_t_3) {
      } else {
        __pyx_t_2 = __pyx_t_3;
        goto __pyx_L17_bool_binop_done;
      }
      __pyx_t_3 = __Pyx_PyObject_IsTrue(__pyx_v_unique_vals2); if (unlikely((__pyx_t_3 < 0))) __PYX_ERR(0, 469, __pyx_L1_error)
      if (__pyx_t_3) {
      } else {
        __pyx_t_2 = __pyx_t_3;
        goto __pyx_L17_bool_binop_done;
      }
      __pyx_t_3 = (__Pyx_PySequence_ContainsTF(__pyx_v_v1, __pyx_v_unique_vals2, Py_NE)); if (unlikely((__pyx_t_3 < 0))) __PYX_ERR(0, 469, __pyx_L1_error)
      __pyx_t_2 = __pyx_t_3;
      __pyx_L17_bool_binop_done:;
      if (__pyx_t_2) {

        __pyx_t_5 = __pyx_v_valid_z1;
        __Pyx_INCREF(__pyx_t_5);
        __pyx_t_11 = 0;
        {
          PyObject *__pyx_callargs[2] = {__pyx_t_5, __pyx_v_c1};
          __pyx_t_9 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_add, __pyx_callargs+__pyx_t_11, (2-__pyx_t_11) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
          __Pyx_XDECREF(__pyx_t_5); __pyx_t_5 = 0;
          if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 470, __pyx_L1_error)
          __Pyx_GOTREF(__pyx_t_9);
        }
        __Pyx_DECREF(__pyx_t_9); __pyx_t_9 = 0;

      }
    }
    __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;

    __pyx_t_7 = 0;
    if (unlikely(__pyx_v_vals2 == Py_None)) {
      PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "items");
      __PYX_ERR(0, 471, __pyx_L1_error)
    }
    __pyx_t_9 = __Pyx_dict_iterator(__pyx_v_vals2, 1, __pyx_mstate_global->__pyx_n_u_items, (&__pyx_t_6), (&__pyx_t_8)); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 471, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_9);
    __Pyx_XDECREF(__pyx_t_1);
    __pyx_t_1 = __pyx_t_9;
    __pyx_t_9 = 0;
    while (1) {
      __pyx_t_10 = __Pyx_dict_iter_next(__pyx_t_1, __pyx_t_6, &__pyx_t_7, &__pyx_t_9, &__pyx_t_5, NULL, __pyx_t_8);
      if (unlikely(__pyx_t_10 == 0)) break;
      if (unlikely(__pyx_t_10 == -1)) __PYX_ERR(0, 471, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_9);
      __Pyx_GOTREF(__pyx_t_5);
      __Pyx_XDECREF_SET(__pyx_v_c2, __pyx_t_9);
      __pyx_t_9 = 0;
      __Pyx_XDECREF_SET(__pyx_v_v2, __pyx_t_5);
      __pyx_t_5 = 0;

      __pyx_t_12 = PyObject_Length(__pyx_v_unique_vals1); if (unlikely(__pyx_t_12 == ((Py_ssize_t)-1))) __PYX_ERR(0, 472, __pyx_L1_error)
      __pyx_t_3 = (__pyx_t_12 > 1);
      if (!__pyx_t_3) {
      } else {
        __pyx_t_2 = __pyx_t_3;
        goto __pyx_L23_bool_binop_done;
      }
      __pyx_t_3 = __Pyx_PyObject_IsTrue(__pyx_v_unique_vals1); if (unlikely((__pyx_t_3 < 0))) __PYX_ERR(0, 472, __pyx_L1_error)
      if (__pyx_t_3) {
      } else {
        __pyx_t_2 = __pyx_t_3;
        goto __pyx_L23_bool_binop_done;
      }
      __pyx_t_3 = (__Pyx_PySequence_ContainsTF(__pyx_v_v2, __pyx_v_unique_vals1, Py_NE)); if (unlikely((__pyx_t_3 < 0))) __PYX_ERR(0, 472, __pyx_L1_error)
      __pyx_t_2 = __pyx_t_3;
      __pyx_L23_bool_binop_done:;
      if (__pyx_t_2) {

        __pyx_t_9 = __pyx_v_valid_z2;
        __Pyx_INCREF(__pyx_t_9);
        __pyx_t_11 = 0;
        {
          PyObject *__pyx_callargs[2] = {__pyx_t_9, __pyx_v_c2};
          __pyx_t_5 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_add, __pyx_callargs+__pyx_t_11, (2-__pyx_t_11) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
          __Pyx_XDECREF(__pyx_t_9); __pyx_t_9 = 0;
          if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 473, __pyx_L1_error)
          __Pyx_GOTREF(__pyx_t_5);
        }
        __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;

      }
    }
    __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;

    __Pyx_XDECREF(__pyx_r);
    __pyx_t_1 = PyTuple_New(2); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 474, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_1);
    __Pyx_INCREF(__pyx_v_valid_z1);
    __Pyx_GIVEREF(__pyx_v_valid_z1);
    if (__Pyx_PyTuple_SET_ITEM(__pyx_t_1, 0, __pyx_v_valid_z1) != (0)) __PYX_ERR(0, 474, __pyx_L1_error);
    __Pyx_INCREF(__pyx_v_valid_z2);
    __Pyx_GIVEREF(__pyx_v_valid_z2);
    if (__Pyx_PyTuple_SET_ITEM(__pyx_t_1, 1, __pyx_v_valid_z2) != (0)) __PYX_ERR(0, 474, __pyx_L1_error);
    __pyx_r = __pyx_t_1;
    __pyx_t_1 = 0;
    goto __pyx_L0;

  }

  __Pyx_GetModuleGlobalName(__pyx_t_1, __pyx_mstate_global->__pyx_n_u_ComparisonOp); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 476, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_1);
  __pyx_t_5 = __Pyx_PyObject_GetAttrStr(__pyx_t_1, __pyx_mstate_global->__pyx_n_u_LT); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 476, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_5);
  __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;
  __pyx_t_1 = PyObject_RichCompare(__pyx_v_operator, __pyx_t_5, Py_EQ); __Pyx_XGOTREF(__pyx_t_1); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 476, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
  __pyx_t_3 = __Pyx_PyObject_IsTrue(__pyx_t_1); if (unlikely((__pyx_t_3 < 0))) __PYX_ERR(0, 476, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;
  if (!__pyx_t_3) {
  } else {
    __pyx_t_2 = __pyx_t_3;
    goto __pyx_L27_bool_binop_done;
  }
  __Pyx_GetModuleGlobalName(__pyx_t_1, __pyx_mstate_global->__pyx_n_u_ComparisonOp); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 476, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_1);
  __pyx_t_5 = __Pyx_PyObject_GetAttrStr(__pyx_t_1, __pyx_mstate_global->__pyx_n_u_LE); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 476, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_5);
  __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;
  __pyx_t_1 = PyObject_RichCompare(__pyx_v_operator, __pyx_t_5, Py_EQ); __Pyx_XGOTREF(__pyx_t_1); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 476, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
  __pyx_t_3 = __Pyx_PyObject_IsTrue(__pyx_t_1); if (unlikely((__pyx_t_3 < 0))) __PYX_ERR(0, 476, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;
  __pyx_t_2 = __pyx_t_3;
  __pyx_L27_bool_binop_done:;
  if (__pyx_t_2) {

    {
      __Pyx_PyThreadState_declare
      __Pyx_PyThreadState_assign
      __Pyx_ExceptionSave(&__pyx_t_13, &__pyx_t_14, &__pyx_t_15);
      __Pyx_XGOTREF(__pyx_t_13);
      __Pyx_XGOTREF(__pyx_t_14);
      __Pyx_XGOTREF(__pyx_t_15);
       {

        __pyx_t_5 = NULL;
        if (unlikely(__pyx_v_vals2 == Py_None)) {
          PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "values");
          __PYX_ERR(0, 478, __pyx_L29_error)
        }
        __pyx_t_9 = __Pyx_PyDict_Values(__pyx_v_vals2); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 478, __pyx_L29_error)
        __Pyx_GOTREF(__pyx_t_9);
        __pyx_t_11 = 1;
        {
          PyObject *__pyx_callargs[2] = {__pyx_t_5, __pyx_t_9};
          __pyx_t_1 = __Pyx_PyObject_FastCall((PyObject*)__pyx_builtin_max, __pyx_callargs+__pyx_t_11, (2-__pyx_t_11) | (__pyx_t_11*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
          __Pyx_XDECREF(__pyx_t_5); __pyx_t_5 = 0;
          __Pyx_DECREF(__pyx_t_9); __pyx_t_9 = 0;
          if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 478, __pyx_L29_error)
          __Pyx_GOTREF(__pyx_t_1);
        }
        __pyx_v_max_v2 = __pyx_t_1;
        __pyx_t_1 = 0;

        __pyx_t_9 = NULL;
        if (unlikely(__pyx_v_vals1 == Py_None)) {
          PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "values");
          __PYX_ERR(0, 479, __pyx_L29_error)
        }
        __pyx_t_5 = __Pyx_PyDict_Values(__pyx_v_vals1); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 479, __pyx_L29_error)
        __Pyx_GOTREF(__pyx_t_5);
        __pyx_t_11 = 1;
        {
          PyObject *__pyx_callargs[2] = {__pyx_t_9, __pyx_t_5};
          __pyx_t_1 = __Pyx_PyObject_FastCall((PyObject*)__pyx_builtin_min, __pyx_callargs+__pyx_t_11, (2-__pyx_t_11) | (__pyx_t_11*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
          __Pyx_XDECREF(__pyx_t_9); __pyx_t_9 = 0;
          __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
          if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 479, __pyx_L29_error)
          __Pyx_GOTREF(__pyx_t_1);
        }
        __pyx_v_min_v1 = __pyx_t_1;
        __pyx_t_1 = 0;

        __Pyx_GetModuleGlobalName(__pyx_t_1, __pyx_mstate_global->__pyx_n_u_ComparisonOp); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 480, __pyx_L29_error)
        __Pyx_GOTREF(__pyx_t_1);
        __pyx_t_5 = __Pyx_PyObject_GetAttrStr(__pyx_t_1, __pyx_mstate_global->__pyx_n_u_LT); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 480, __pyx_L29_error)
        __Pyx_GOTREF(__pyx_t_5);
        __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;
        __pyx_t_1 = PyObject_RichCompare(__pyx_v_operator, __pyx_t_5, Py_EQ); __Pyx_XGOTREF(__pyx_t_1); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 480, __pyx_L29_error)
        __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
        __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_t_1); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 480, __pyx_L29_error)
        __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;
        if (__pyx_t_2) {

          __pyx_t_6 = 0;
          if (unlikely(__pyx_v_vals1 == Py_None)) {
            PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "items");
            __PYX_ERR(0, 481, __pyx_L29_error)
          }
          __pyx_t_5 = __Pyx_dict_iterator(__pyx_v_vals1, 1, __pyx_mstate_global->__pyx_n_u_items, (&__pyx_t_7), (&__pyx_t_8)); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 481, __pyx_L29_error)
          __Pyx_GOTREF(__pyx_t_5);
          __Pyx_XDECREF(__pyx_t_1);
          __pyx_t_1 = __pyx_t_5;
          __pyx_t_5 = 0;
          while (1) {
            __pyx_t_10 = __Pyx_dict_iter_next(__pyx_t_1, __pyx_t_7, &__pyx_t_6, &__pyx_t_5, &__pyx_t_9, NULL, __pyx_t_8);
            if (unlikely(__pyx_t_10 == 0)) break;
            if (unlikely(__pyx_t_10 == -1)) __PYX_ERR(0, 481, __pyx_L29_error)
            __Pyx_GOTREF(__pyx_t_5);
            __Pyx_GOTREF(__pyx_t_9);
            __Pyx_XDECREF_SET(__pyx_v_c1, __pyx_t_5);
            __pyx_t_5 = 0;
            __Pyx_XDECREF_SET(__pyx_v_v1, __pyx_t_9);
            __pyx_t_9 = 0;

            __pyx_t_9 = PyObject_RichCompare(__pyx_v_v1, __pyx_v_max_v2, Py_LT); __Pyx_XGOTREF(__pyx_t_9); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 482, __pyx_L29_error)
            __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_t_9); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 482, __pyx_L29_error)
            __Pyx_DECREF(__pyx_t_9); __pyx_t_9 = 0;
            if (__pyx_t_2) {

              __pyx_t_5 = __pyx_v_valid_z1;
              __Pyx_INCREF(__pyx_t_5);
              __pyx_t_11 = 0;
              {
                PyObject *__pyx_callargs[2] = {__pyx_t_5, __pyx_v_c1};
                __pyx_t_9 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_add, __pyx_callargs+__pyx_t_11, (2-__pyx_t_11) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
                __Pyx_XDECREF(__pyx_t_5); __pyx_t_5 = 0;
                if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 483, __pyx_L29_error)
                __Pyx_GOTREF(__pyx_t_9);
              }
              __Pyx_DECREF(__pyx_t_9); __pyx_t_9 = 0;

            }
          }
          __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;

          __pyx_t_7 = 0;
          if (unlikely(__pyx_v_vals2 == Py_None)) {
            PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "items");
            __PYX_ERR(0, 484, __pyx_L29_error)
          }
          __pyx_t_9 = __Pyx_dict_iterator(__pyx_v_vals2, 1, __pyx_mstate_global->__pyx_n_u_items, (&__pyx_t_6), (&__pyx_t_8)); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 484, __pyx_L29_error)
          __Pyx_GOTREF(__pyx_t_9);
          __Pyx_XDECREF(__pyx_t_1);
          __pyx_t_1 = __pyx_t_9;
          __pyx_t_9 = 0;
          while (1) {
            __pyx_t_10 = __Pyx_dict_iter_next(__pyx_t_1, __pyx_t_6, &__pyx_t_7, &__pyx_t_9, &__pyx_t_5, NULL, __pyx_t_8);
            if (unlikely(__pyx_t_10 == 0)) break;
            if (unlikely(__pyx_t_10 == -1)) __PYX_ERR(0, 484, __pyx_L29_error)
            __Pyx_GOTREF(__pyx_t_9);
            __Pyx_GOTREF(__pyx_t_5);
            __Pyx_XDECREF_SET(__pyx_v_c2, __pyx_t_9);
            __pyx_t_9 = 0;
            __Pyx_XDECREF_SET(__pyx_v_v2, __pyx_t_5);
            __pyx_t_5 = 0;

            __pyx_t_5 = PyObject_RichCompare(__pyx_v_min_v1, __pyx_v_v2, Py_LT); __Pyx_XGOTREF(__pyx_t_5); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 485, __pyx_L29_error)
            __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_t_5); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 485, __pyx_L29_error)
            __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
            if (__pyx_t_2) {

              __pyx_t_9 = __pyx_v_valid_z2;
              __Pyx_INCREF(__pyx_t_9);
              __pyx_t_11 = 0;
              {
                PyObject *__pyx_callargs[2] = {__pyx_t_9, __pyx_v_c2};
                __pyx_t_5 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_add, __pyx_callargs+__pyx_t_11, (2-__pyx_t_11) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
                __Pyx_XDECREF(__pyx_t_9); __pyx_t_9 = 0;
                if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 486, __pyx_L29_error)
                __Pyx_GOTREF(__pyx_t_5);
              }
              __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;

            }
          }
          __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;

          goto __pyx_L35;
        }

         {
          __pyx_t_6 = 0;
          if (unlikely(__pyx_v_vals1 == Py_None)) {
            PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "items");
            __PYX_ERR(0, 488, __pyx_L29_error)
          }
          __pyx_t_5 = __Pyx_dict_iterator(__pyx_v_vals1, 1, __pyx_mstate_global->__pyx_n_u_items, (&__pyx_t_7), (&__pyx_t_8)); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 488, __pyx_L29_error)
          __Pyx_GOTREF(__pyx_t_5);
          __Pyx_XDECREF(__pyx_t_1);
          __pyx_t_1 = __pyx_t_5;
          __pyx_t_5 = 0;
          while (1) {
            __pyx_t_10 = __Pyx_dict_iter_next(__pyx_t_1, __pyx_t_7, &__pyx_t_6, &__pyx_t_5, &__pyx_t_9, NULL, __pyx_t_8);
            if (unlikely(__pyx_t_10 == 0)) break;
            if (unlikely(__pyx_t_10 == -1)) __PYX_ERR(0, 488, __pyx_L29_error)
            __Pyx_GOTREF(__pyx_t_5);
            __Pyx_GOTREF(__pyx_t_9);
            __Pyx_XDECREF_SET(__pyx_v_c1, __pyx_t_5);
            __pyx_t_5 = 0;
            __Pyx_XDECREF_SET(__pyx_v_v1, __pyx_t_9);
            __pyx_t_9 = 0;

            __pyx_t_9 = PyObject_RichCompare(__pyx_v_v1, __pyx_v_max_v2, Py_LE); __Pyx_XGOTREF(__pyx_t_9); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 489, __pyx_L29_error)
            __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_t_9); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 489, __pyx_L29_error)
            __Pyx_DECREF(__pyx_t_9); __pyx_t_9 = 0;
            if (__pyx_t_2) {

              __pyx_t_5 = __pyx_v_valid_z1;
              __Pyx_INCREF(__pyx_t_5);
              __pyx_t_11 = 0;
              {
                PyObject *__pyx_callargs[2] = {__pyx_t_5, __pyx_v_c1};
                __pyx_t_9 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_add, __pyx_callargs+__pyx_t_11, (2-__pyx_t_11) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
                __Pyx_XDECREF(__pyx_t_5); __pyx_t_5 = 0;
                if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 490, __pyx_L29_error)
                __Pyx_GOTREF(__pyx_t_9);
              }
              __Pyx_DECREF(__pyx_t_9); __pyx_t_9 = 0;

            }
          }
          __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;

          __pyx_t_7 = 0;
          if (unlikely(__pyx_v_vals2 == Py_None)) {
            PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "items");
            __PYX_ERR(0, 491, __pyx_L29_error)
          }
          __pyx_t_9 = __Pyx_dict_iterator(__pyx_v_vals2, 1, __pyx_mstate_global->__pyx_n_u_items, (&__pyx_t_6), (&__pyx_t_8)); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 491, __pyx_L29_error)
          __Pyx_GOTREF(__pyx_t_9);
          __Pyx_XDECREF(__pyx_t_1);
          __pyx_t_1 = __pyx_t_9;
          __pyx_t_9 = 0;
          while (1) {
            __pyx_t_10 = __Pyx_dict_iter_next(__pyx_t_1, __pyx_t_6, &__pyx_t_7, &__pyx_t_9, &__pyx_t_5, NULL, __pyx_t_8);
            if (unlikely(__pyx_t_10 == 0)) break;
            if (unlikely(__pyx_t_10 == -1)) __PYX_ERR(0, 491, __pyx_L29_error)
            __Pyx_GOTREF(__pyx_t_9);
            __Pyx_GOTREF(__pyx_t_5);
            __Pyx_XDECREF_SET(__pyx_v_c2, __pyx_t_9);
            __pyx_t_9 = 0;
            __Pyx_XDECREF_SET(__pyx_v_v2, __pyx_t_5);
            __pyx_t_5 = 0;

            __pyx_t_5 = PyObject_RichCompare(__pyx_v_min_v1, __pyx_v_v2, Py_LE); __Pyx_XGOTREF(__pyx_t_5); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 492, __pyx_L29_error)
            __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_t_5); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 492, __pyx_L29_error)
            __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
            if (__pyx_t_2) {

              __pyx_t_9 = __pyx_v_valid_z2;
              __Pyx_INCREF(__pyx_t_9);
              __pyx_t_11 = 0;
              {
                PyObject *__pyx_callargs[2] = {__pyx_t_9, __pyx_v_c2};
                __pyx_t_5 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_add, __pyx_callargs+__pyx_t_11, (2-__pyx_t_11) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
                __Pyx_XDECREF(__pyx_t_9); __pyx_t_9 = 0;
                if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 493, __pyx_L29_error)
                __Pyx_GOTREF(__pyx_t_5);
              }
              __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;

            }
          }
          __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;
        }
        __pyx_L35:;

        __Pyx_XDECREF(__pyx_r);
        __pyx_t_1 = PyTuple_New(2); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 494, __pyx_L29_error)
        __Pyx_GOTREF(__pyx_t_1);
        __Pyx_INCREF(__pyx_v_valid_z1);
        __Pyx_GIVEREF(__pyx_v_valid_z1);
        if (__Pyx_PyTuple_SET_ITEM(__pyx_t_1, 0, __pyx_v_valid_z1) != (0)) __PYX_ERR(0, 494, __pyx_L29_error);
        __Pyx_INCREF(__pyx_v_valid_z2);
        __Pyx_GIVEREF(__pyx_v_valid_z2);
        if (__Pyx_PyTuple_SET_ITEM(__pyx_t_1, 1, __pyx_v_valid_z2) != (0)) __PYX_ERR(0, 494, __pyx_L29_error);
        __pyx_r = __pyx_t_1;
        __pyx_t_1 = 0;
        goto __pyx_L33_try_return;

      }
      __pyx_L29_error:;
      __Pyx_XDECREF(__pyx_t_1); __pyx_t_1 = 0;
      __Pyx_XDECREF(__pyx_t_5); __pyx_t_5 = 0;
      __Pyx_XDECREF(__pyx_t_9); __pyx_t_9 = 0;

      __pyx_t_8 = __Pyx_PyErr_ExceptionMatches(((PyObject *)(((PyTypeObject*)PyExc_TypeError))));
      if (__pyx_t_8) {
        __Pyx_ErrRestore(0,0,0);
        goto __pyx_L30_exception_handled;
      }
      goto __pyx_L31_except_error;

      __pyx_L31_except_error:;
      __Pyx_XGIVEREF(__pyx_t_13);
      __Pyx_XGIVEREF(__pyx_t_14);
      __Pyx_XGIVEREF(__pyx_t_15);
      __Pyx_ExceptionReset(__pyx_t_13, __pyx_t_14, __pyx_t_15);
      goto __pyx_L1_error;
      __pyx_L33_try_return:;
      __Pyx_XGIVEREF(__pyx_t_13);
      __Pyx_XGIVEREF(__pyx_t_14);
      __Pyx_XGIVEREF(__pyx_t_15);
      __Pyx_ExceptionReset(__pyx_t_13, __pyx_t_14, __pyx_t_15);
      goto __pyx_L0;
      __pyx_L30_exception_handled:;
      __Pyx_XGIVEREF(__pyx_t_13);
      __Pyx_XGIVEREF(__pyx_t_14);
      __Pyx_XGIVEREF(__pyx_t_15);
      __Pyx_ExceptionReset(__pyx_t_13, __pyx_t_14, __pyx_t_15);
    }

  }

  __Pyx_GetModuleGlobalName(__pyx_t_1, __pyx_mstate_global->__pyx_n_u_ComparisonOp); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 498, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_1);
  __pyx_t_5 = __Pyx_PyObject_GetAttrStr(__pyx_t_1, __pyx_mstate_global->__pyx_n_u_GT); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 498, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_5);
  __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;
  __pyx_t_1 = PyObject_RichCompare(__pyx_v_operator, __pyx_t_5, Py_EQ); __Pyx_XGOTREF(__pyx_t_1); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 498, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
  __pyx_t_3 = __Pyx_PyObject_IsTrue(__pyx_t_1); if (unlikely((__pyx_t_3 < 0))) __PYX_ERR(0, 498, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;
  if (!__pyx_t_3) {
  } else {
    __pyx_t_2 = __pyx_t_3;
    goto __pyx_L51_bool_binop_done;
  }
  __Pyx_GetModuleGlobalName(__pyx_t_1, __pyx_mstate_global->__pyx_n_u_ComparisonOp); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 498, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_1);
  __pyx_t_5 = __Pyx_PyObject_GetAttrStr(__pyx_t_1, __pyx_mstate_global->__pyx_n_u_GE); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 498, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_5);
  __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;
  __pyx_t_1 = PyObject_RichCompare(__pyx_v_operator, __pyx_t_5, Py_EQ); __Pyx_XGOTREF(__pyx_t_1); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 498, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
  __pyx_t_3 = __Pyx_PyObject_IsTrue(__pyx_t_1); if (unlikely((__pyx_t_3 < 0))) __PYX_ERR(0, 498, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;
  __pyx_t_2 = __pyx_t_3;
  __pyx_L51_bool_binop_done:;
  if (__pyx_t_2) {

    {
      __Pyx_PyThreadState_declare
      __Pyx_PyThreadState_assign
      __Pyx_ExceptionSave(&__pyx_t_15, &__pyx_t_14, &__pyx_t_13);
      __Pyx_XGOTREF(__pyx_t_15);
      __Pyx_XGOTREF(__pyx_t_14);
      __Pyx_XGOTREF(__pyx_t_13);
       {

        __pyx_t_5 = NULL;
        if (unlikely(__pyx_v_vals2 == Py_None)) {
          PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "values");
          __PYX_ERR(0, 500, __pyx_L53_error)
        }
        __pyx_t_9 = __Pyx_PyDict_Values(__pyx_v_vals2); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 500, __pyx_L53_error)
        __Pyx_GOTREF(__pyx_t_9);
        __pyx_t_11 = 1;
        {
          PyObject *__pyx_callargs[2] = {__pyx_t_5, __pyx_t_9};
          __pyx_t_1 = __Pyx_PyObject_FastCall((PyObject*)__pyx_builtin_min, __pyx_callargs+__pyx_t_11, (2-__pyx_t_11) | (__pyx_t_11*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
          __Pyx_XDECREF(__pyx_t_5); __pyx_t_5 = 0;
          __Pyx_DECREF(__pyx_t_9); __pyx_t_9 = 0;
          if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 500, __pyx_L53_error)
          __Pyx_GOTREF(__pyx_t_1);
        }
        __pyx_v_min_v2 = __pyx_t_1;
        __pyx_t_1 = 0;

        __pyx_t_9 = NULL;
        if (unlikely(__pyx_v_vals1 == Py_None)) {
          PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "values");
          __PYX_ERR(0, 501, __pyx_L53_error)
        }
        __pyx_t_5 = __Pyx_PyDict_Values(__pyx_v_vals1); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 501, __pyx_L53_error)
        __Pyx_GOTREF(__pyx_t_5);
        __pyx_t_11 = 1;
        {
          PyObject *__pyx_callargs[2] = {__pyx_t_9, __pyx_t_5};
          __pyx_t_1 = __Pyx_PyObject_FastCall((PyObject*)__pyx_builtin_max, __pyx_callargs+__pyx_t_11, (2-__pyx_t_11) | (__pyx_t_11*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
          __Pyx_XDECREF(__pyx_t_9); __pyx_t_9 = 0;
          __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
          if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 501, __pyx_L53_error)
          __Pyx_GOTREF(__pyx_t_1);
        }
        __pyx_v_max_v1 = __pyx_t_1;
        __pyx_t_1 = 0;

        __Pyx_GetModuleGlobalName(__pyx_t_1, __pyx_mstate_global->__pyx_n_u_ComparisonOp); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 502, __pyx_L53_error)
        __Pyx_GOTREF(__pyx_t_1);
        __pyx_t_5 = __Pyx_PyObject_GetAttrStr(__pyx_t_1, __pyx_mstate_global->__pyx_n_u_GT); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 502, __pyx_L53_error)
        __Pyx_GOTREF(__pyx_t_5);
        __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;
        __pyx_t_1 = PyObject_RichCompare(__pyx_v_operator, __pyx_t_5, Py_EQ); __Pyx_XGOTREF(__pyx_t_1); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 502, __pyx_L53_error)
        __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
        __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_t_1); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 502, __pyx_L53_error)
        __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;
        if (__pyx_t_2) {

          __pyx_t_6 = 0;
          if (unlikely(__pyx_v_vals1 == Py_None)) {
            PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "items");
            __PYX_ERR(0, 503, __pyx_L53_error)
          }
          __pyx_t_5 = __Pyx_dict_iterator(__pyx_v_vals1, 1, __pyx_mstate_global->__pyx_n_u_items, (&__pyx_t_7), (&__pyx_t_8)); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 503, __pyx_L53_error)
          __Pyx_GOTREF(__pyx_t_5);
          __Pyx_XDECREF(__pyx_t_1);
          __pyx_t_1 = __pyx_t_5;
          __pyx_t_5 = 0;
          while (1) {
            __pyx_t_10 = __Pyx_dict_iter_next(__pyx_t_1, __pyx_t_7, &__pyx_t_6, &__pyx_t_5, &__pyx_t_9, NULL, __pyx_t_8);
            if (unlikely(__pyx_t_10 == 0)) break;
            if (unlikely(__pyx_t_10 == -1)) __PYX_ERR(0, 503, __pyx_L53_error)
            __Pyx_GOTREF(__pyx_t_5);
            __Pyx_GOTREF(__pyx_t_9);
            __Pyx_XDECREF_SET(__pyx_v_c1, __pyx_t_5);
            __pyx_t_5 = 0;
            __Pyx_XDECREF_SET(__pyx_v_v1, __pyx_t_9);
            __pyx_t_9 = 0;

            __pyx_t_9 = PyObject_RichCompare(__pyx_v_v1, __pyx_v_min_v2, Py_GT); __Pyx_XGOTREF(__pyx_t_9); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 504, __pyx_L53_error)
            __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_t_9); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 504, __pyx_L53_error)
            __Pyx_DECREF(__pyx_t_9); __pyx_t_9 = 0;
            if (__pyx_t_2) {

              __pyx_t_5 = __pyx_v_valid_z1;
              __Pyx_INCREF(__pyx_t_5);
              __pyx_t_11 = 0;
              {
                PyObject *__pyx_callargs[2] = {__pyx_t_5, __pyx_v_c1};
                __pyx_t_9 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_add, __pyx_callargs+__pyx_t_11, (2-__pyx_t_11) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
                __Pyx_XDECREF(__pyx_t_5); __pyx_t_5 = 0;
                if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 505, __pyx_L53_error)
                __Pyx_GOTREF(__pyx_t_9);
              }
              __Pyx_DECREF(__pyx_t_9); __pyx_t_9 = 0;

            }
          }
          __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;

          __pyx_t_7 = 0;
          if (unlikely(__pyx_v_vals2 == Py_None)) {
            PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "items");
            __PYX_ERR(0, 506, __pyx_L53_error)
          }
          __pyx_t_9 = __Pyx_dict_iterator(__pyx_v_vals2, 1, __pyx_mstate_global->__pyx_n_u_items, (&__pyx_t_6), (&__pyx_t_8)); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 506, __pyx_L53_error)
          __Pyx_GOTREF(__pyx_t_9);
          __Pyx_XDECREF(__pyx_t_1);
          __pyx_t_1 = __pyx_t_9;
          __pyx_t_9 = 0;
          while (1) {
            __pyx_t_10 = __Pyx_dict_iter_next(__pyx_t_1, __pyx_t_6, &__pyx_t_7, &__pyx_t_9, &__pyx_t_5, NULL, __pyx_t_8);
            if (unlikely(__pyx_t_10 == 0)) break;
            if (unlikely(__pyx_t_10 == -1)) __PYX_ERR(0, 506, __pyx_L53_error)
            __Pyx_GOTREF(__pyx_t_9);
            __Pyx_GOTREF(__pyx_t_5);
            __Pyx_XDECREF_SET(__pyx_v_c2, __pyx_t_9);
            __pyx_t_9 = 0;
            __Pyx_XDECREF_SET(__pyx_v_v2, __pyx_t_5);
            __pyx_t_5 = 0;

            __pyx_t_5 = PyObject_RichCompare(__pyx_v_max_v1, __pyx_v_v2, Py_GT); __Pyx_XGOTREF(__pyx_t_5); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 507, __pyx_L53_error)
            __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_t_5); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 507, __pyx_L53_error)
            __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
            if (__pyx_t_2) {

              __pyx_t_9 = __pyx_v_valid_z2;
              __Pyx_INCREF(__pyx_t_9);
              __pyx_t_11 = 0;
              {
                PyObject *__pyx_callargs[2] = {__pyx_t_9, __pyx_v_c2};
                __pyx_t_5 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_add, __pyx_callargs+__pyx_t_11, (2-__pyx_t_11) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
                __Pyx_XDECREF(__pyx_t_9); __pyx_t_9 = 0;
                if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 508, __pyx_L53_error)
                __Pyx_GOTREF(__pyx_t_5);
              }
              __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;

            }
          }
          __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;

          goto __pyx_L59;
        }

         {
          __pyx_t_6 = 0;
          if (unlikely(__pyx_v_vals1 == Py_None)) {
            PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "items");
            __PYX_ERR(0, 510, __pyx_L53_error)
          }
          __pyx_t_5 = __Pyx_dict_iterator(__pyx_v_vals1, 1, __pyx_mstate_global->__pyx_n_u_items, (&__pyx_t_7), (&__pyx_t_8)); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 510, __pyx_L53_error)
          __Pyx_GOTREF(__pyx_t_5);
          __Pyx_XDECREF(__pyx_t_1);
          __pyx_t_1 = __pyx_t_5;
          __pyx_t_5 = 0;
          while (1) {
            __pyx_t_10 = __Pyx_dict_iter_next(__pyx_t_1, __pyx_t_7, &__pyx_t_6, &__pyx_t_5, &__pyx_t_9, NULL, __pyx_t_8);
            if (unlikely(__pyx_t_10 == 0)) break;
            if (unlikely(__pyx_t_10 == -1)) __PYX_ERR(0, 510, __pyx_L53_error)
            __Pyx_GOTREF(__pyx_t_5);
            __Pyx_GOTREF(__pyx_t_9);
            __Pyx_XDECREF_SET(__pyx_v_c1, __pyx_t_5);
            __pyx_t_5 = 0;
            __Pyx_XDECREF_SET(__pyx_v_v1, __pyx_t_9);
            __pyx_t_9 = 0;

            __pyx_t_9 = PyObject_RichCompare(__pyx_v_v1, __pyx_v_min_v2, Py_GE); __Pyx_XGOTREF(__pyx_t_9); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 511, __pyx_L53_error)
            __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_t_9); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 511, __pyx_L53_error)
            __Pyx_DECREF(__pyx_t_9); __pyx_t_9 = 0;
            if (__pyx_t_2) {

              __pyx_t_5 = __pyx_v_valid_z1;
              __Pyx_INCREF(__pyx_t_5);
              __pyx_t_11 = 0;
              {
                PyObject *__pyx_callargs[2] = {__pyx_t_5, __pyx_v_c1};
                __pyx_t_9 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_add, __pyx_callargs+__pyx_t_11, (2-__pyx_t_11) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
                __Pyx_XDECREF(__pyx_t_5); __pyx_t_5 = 0;
                if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 512, __pyx_L53_error)
                __Pyx_GOTREF(__pyx_t_9);
              }
              __Pyx_DECREF(__pyx_t_9); __pyx_t_9 = 0;

            }
          }
          __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;

          __pyx_t_7 = 0;
          if (unlikely(__pyx_v_vals2 == Py_None)) {
            PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "items");
            __PYX_ERR(0, 513, __pyx_L53_error)
          }
          __pyx_t_9 = __Pyx_dict_iterator(__pyx_v_vals2, 1, __pyx_mstate_global->__pyx_n_u_items, (&__pyx_t_6), (&__pyx_t_8)); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 513, __pyx_L53_error)
          __Pyx_GOTREF(__pyx_t_9);
          __Pyx_XDECREF(__pyx_t_1);
          __pyx_t_1 = __pyx_t_9;
          __pyx_t_9 = 0;
          while (1) {
            __pyx_t_10 = __Pyx_dict_iter_next(__pyx_t_1, __pyx_t_6, &__pyx_t_7, &__pyx_t_9, &__pyx_t_5, NULL, __pyx_t_8);
            if (unlikely(__pyx_t_10 == 0)) break;
            if (unlikely(__pyx_t_10 == -1)) __PYX_ERR(0, 513, __pyx_L53_error)
            __Pyx_GOTREF(__pyx_t_9);
            __Pyx_GOTREF(__pyx_t_5);
            __Pyx_XDECREF_SET(__pyx_v_c2, __pyx_t_9);
            __pyx_t_9 = 0;
            __Pyx_XDECREF_SET(__pyx_v_v2, __pyx_t_5);
            __pyx_t_5 = 0;

            __pyx_t_5 = PyObject_RichCompare(__pyx_v_max_v1, __pyx_v_v2, Py_GE); __Pyx_XGOTREF(__pyx_t_5); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 514, __pyx_L53_error)
            __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_t_5); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 514, __pyx_L53_error)
            __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
            if (__pyx_t_2) {

              __pyx_t_9 = __pyx_v_valid_z2;
              __Pyx_INCREF(__pyx_t_9);
              __pyx_t_11 = 0;
              {
                PyObject *__pyx_callargs[2] = {__pyx_t_9, __pyx_v_c2};
                __pyx_t_5 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_add, __pyx_callargs+__pyx_t_11, (2-__pyx_t_11) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
                __Pyx_XDECREF(__pyx_t_9); __pyx_t_9 = 0;
                if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 515, __pyx_L53_error)
                __Pyx_GOTREF(__pyx_t_5);
              }
              __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;

            }
          }
          __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;
        }
        __pyx_L59:;

        __Pyx_XDECREF(__pyx_r);
        __pyx_t_1 = PyTuple_New(2); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 516, __pyx_L53_error)
        __Pyx_GOTREF(__pyx_t_1);
        __Pyx_INCREF(__pyx_v_valid_z1);
        __Pyx_GIVEREF(__pyx_v_valid_z1);
        if (__Pyx_PyTuple_SET_ITEM(__pyx_t_1, 0, __pyx_v_valid_z1) != (0)) __PYX_ERR(0, 516, __pyx_L53_error);
        __Pyx_INCREF(__pyx_v_valid_z2);
        __Pyx_GIVEREF(__pyx_v_valid_z2);
        if (__Pyx_PyTuple_SET_ITEM(__pyx_t_1, 1, __pyx_v_valid_z2) != (0)) __PYX_ERR(0, 516, __pyx_L53_error);
        __pyx_r = __pyx_t_1;
        __pyx_t_1 = 0;
        goto __pyx_L57_try_return;

      }
      __pyx_L53_error:;
      __Pyx_XDECREF(__pyx_t_1); __pyx_t_1 = 0;
      __Pyx_XDECREF(__pyx_t_5); __pyx_t_5 = 0;
      __Pyx_XDECREF(__pyx_t_9); __pyx_t_9 = 0;

      __pyx_t_8 = __Pyx_PyErr_ExceptionMatches(((PyObject *)(((PyTypeObject*)PyExc_TypeError))));
      if (__pyx_t_8) {
        __Pyx_ErrRestore(0,0,0);
        goto __pyx_L54_exception_handled;
      }
      goto __pyx_L55_except_error;

      __pyx_L55_except_error:;
      __Pyx_XGIVEREF(__pyx_t_15);
      __Pyx_XGIVEREF(__pyx_t_14);
      __Pyx_XGIVEREF(__pyx_t_13);
      __Pyx_ExceptionReset(__pyx_t_15, __pyx_t_14, __pyx_t_13);
      goto __pyx_L1_error;
      __pyx_L57_try_return:;
      __Pyx_XGIVEREF(__pyx_t_15);
      __Pyx_XGIVEREF(__pyx_t_14);
      __Pyx_XGIVEREF(__pyx_t_13);
      __Pyx_ExceptionReset(__pyx_t_15, __pyx_t_14, __pyx_t_13);
      goto __pyx_L0;
      __pyx_L54_exception_handled:;
      __Pyx_XGIVEREF(__pyx_t_15);
      __Pyx_XGIVEREF(__pyx_t_14);
      __Pyx_XGIVEREF(__pyx_t_13);
      __Pyx_ExceptionReset(__pyx_t_15, __pyx_t_14, __pyx_t_13);
    }

  }

  __pyx_t_6 = 0;
  if (unlikely(__pyx_v_vals1 == Py_None)) {
    PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "items");
    __PYX_ERR(0, 520, __pyx_L1_error)
  }
  __pyx_t_5 = __Pyx_dict_iterator(__pyx_v_vals1, 1, __pyx_mstate_global->__pyx_n_u_items, (&__pyx_t_7), (&__pyx_t_8)); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 520, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_5);
  __Pyx_XDECREF(__pyx_t_1);
  __pyx_t_1 = __pyx_t_5;
  __pyx_t_5 = 0;
  while (1) {
    __pyx_t_10 = __Pyx_dict_iter_next(__pyx_t_1, __pyx_t_7, &__pyx_t_6, &__pyx_t_5, &__pyx_t_9, NULL, __pyx_t_8);
    if (unlikely(__pyx_t_10 == 0)) break;
    if (unlikely(__pyx_t_10 == -1)) __PYX_ERR(0, 520, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_5);
    __Pyx_GOTREF(__pyx_t_9);
    __Pyx_XDECREF_SET(__pyx_v_c1, __pyx_t_5);
    __pyx_t_5 = 0;
    __Pyx_XDECREF_SET(__pyx_v_v1, __pyx_t_9);
    __pyx_t_9 = 0;

    __pyx_t_12 = 0;
    if (unlikely(__pyx_v_vals2 == Py_None)) {
      PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "items");
      __PYX_ERR(0, 521, __pyx_L1_error)
    }
    __pyx_t_5 = __Pyx_dict_iterator(__pyx_v_vals2, 1, __pyx_mstate_global->__pyx_n_u_items, (&__pyx_t_16), (&__pyx_t_10)); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 521, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_5);
    __Pyx_XDECREF(__pyx_t_9);
    __pyx_t_9 = __pyx_t_5;
    __pyx_t_5 = 0;
    while (1) {
      __pyx_t_18 = __Pyx_dict_iter_next(__pyx_t_9, __pyx_t_16, &__pyx_t_12, &__pyx_t_5, &__pyx_t_17, NULL, __pyx_t_10);
      if (unlikely(__pyx_t_18 == 0)) break;
      if (unlikely(__pyx_t_18 == -1)) __PYX_ERR(0, 521, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_5);
      __Pyx_GOTREF(__pyx_t_17);
      __Pyx_XDECREF_SET(__pyx_v_c2, __pyx_t_5);
      __pyx_t_5 = 0;
      __Pyx_XDECREF_SET(__pyx_v_v2, __pyx_t_17);
      __pyx_t_17 = 0;

      {
        __Pyx_PyThreadState_declare
        __Pyx_PyThreadState_assign
        __Pyx_ExceptionSave(&__pyx_t_13, &__pyx_t_14, &__pyx_t_15);
        __Pyx_XGOTREF(__pyx_t_13);
        __Pyx_XGOTREF(__pyx_t_14);
        __Pyx_XGOTREF(__pyx_t_15);
         {

          __pyx_t_5 = NULL;
          __Pyx_GetModuleGlobalName(__pyx_t_19, __pyx_mstate_global->__pyx_n_u_safe_compare_predicate_values); if (unlikely(!__pyx_t_19)) __PYX_ERR(0, 523, __pyx_L78_error)
          __Pyx_GOTREF(__pyx_t_19);
          __pyx_t_11 = 1;
          #if CYTHON_UNPACK_METHODS
          if (unlikely(PyMethod_Check(__pyx_t_19))) {
            __pyx_t_5 = PyMethod_GET_SELF(__pyx_t_19);
            assert(__pyx_t_5);
            PyObject* __pyx__function = PyMethod_GET_FUNCTION(__pyx_t_19);
            __Pyx_INCREF(__pyx_t_5);
            __Pyx_INCREF(__pyx__function);
            __Pyx_DECREF_SET(__pyx_t_19, __pyx__function);
            __pyx_t_11 = 0;
          }
          #endif
          {
            PyObject *__pyx_callargs[4] = {__pyx_t_5, __pyx_v_v1, __pyx_v_v2, __pyx_v_operator};
            __pyx_t_17 = __Pyx_PyObject_FastCall((PyObject*)__pyx_t_19, __pyx_callargs+__pyx_t_11, (4-__pyx_t_11) | (__pyx_t_11*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
            __Pyx_XDECREF(__pyx_t_5); __pyx_t_5 = 0;
            __Pyx_DECREF(__pyx_t_19); __pyx_t_19 = 0;
            if (unlikely(!__pyx_t_17)) __PYX_ERR(0, 523, __pyx_L78_error)
            __Pyx_GOTREF(__pyx_t_17);
          }
          __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_t_17); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 523, __pyx_L78_error)
          __Pyx_DECREF(__pyx_t_17); __pyx_t_17 = 0;
          if (__pyx_t_2) {

            __pyx_t_19 = __pyx_v_valid_z1;
            __Pyx_INCREF(__pyx_t_19);
            __pyx_t_11 = 0;
            {
              PyObject *__pyx_callargs[2] = {__pyx_t_19, __pyx_v_c1};
              __pyx_t_17 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_add, __pyx_callargs+__pyx_t_11, (2-__pyx_t_11) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
              __Pyx_XDECREF(__pyx_t_19); __pyx_t_19 = 0;
              if (unlikely(!__pyx_t_17)) __PYX_ERR(0, 524, __pyx_L78_error)
              __Pyx_GOTREF(__pyx_t_17);
            }
            __Pyx_DECREF(__pyx_t_17); __pyx_t_17 = 0;

            __pyx_t_19 = __pyx_v_valid_z2;
            __Pyx_INCREF(__pyx_t_19);
            __pyx_t_11 = 0;
            {
              PyObject *__pyx_callargs[2] = {__pyx_t_19, __pyx_v_c2};
              __pyx_t_17 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_add, __pyx_callargs+__pyx_t_11, (2-__pyx_t_11) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
              __Pyx_XDECREF(__pyx_t_19); __pyx_t_19 = 0;
              if (unlikely(!__pyx_t_17)) __PYX_ERR(0, 525, __pyx_L78_error)
              __Pyx_GOTREF(__pyx_t_17);
            }
            __Pyx_DECREF(__pyx_t_17); __pyx_t_17 = 0;

          }

        }
        __Pyx_XDECREF(__pyx_t_13); __pyx_t_13 = 0;
        __Pyx_XDECREF(__pyx_t_14); __pyx_t_14 = 0;
        __Pyx_XDECREF(__pyx_t_15); __pyx_t_15 = 0;
        goto __pyx_L85_try_end;
        __pyx_L78_error:;
        __Pyx_XDECREF(__pyx_t_17); __pyx_t_17 = 0;
        __Pyx_XDECREF(__pyx_t_19); __pyx_t_19 = 0;
        __Pyx_XDECREF(__pyx_t_5); __pyx_t_5 = 0;

        __pyx_t_18 = __Pyx_PyErr_ExceptionMatches2(((PyObject *)(((PyTypeObject*)PyExc_TypeError))), ((PyObject *)(((PyTypeObject*)PyExc_ValueError))));
        if (__pyx_t_18) {
          __Pyx_AddTraceback("_fast_match.prune_pairwise_values_fast", __pyx_clineno, __pyx_lineno, __pyx_filename);
          if (__Pyx_GetException(&__pyx_t_17, &__pyx_t_19, &__pyx_t_5) < 0) __PYX_ERR(0, 526, __pyx_L80_except_error)
          __Pyx_XGOTREF(__pyx_t_17);
          __Pyx_XGOTREF(__pyx_t_19);
          __Pyx_XGOTREF(__pyx_t_5);

          goto __pyx_L87_except_continue;
          __pyx_L87_except_continue:;
          __Pyx_DECREF(__pyx_t_17); __pyx_t_17 = 0;
          __Pyx_DECREF(__pyx_t_19); __pyx_t_19 = 0;
          __Pyx_XDECREF(__pyx_t_5); __pyx_t_5 = 0;
          goto __pyx_L84_try_continue;
        }
        goto __pyx_L80_except_error;

        __pyx_L80_except_error:;
        __Pyx_XGIVEREF(__pyx_t_13);
        __Pyx_XGIVEREF(__pyx_t_14);
        __Pyx_XGIVEREF(__pyx_t_15);
        __Pyx_ExceptionReset(__pyx_t_13, __pyx_t_14, __pyx_t_15);
        goto __pyx_L1_error;
        __pyx_L84_try_continue:;
        __Pyx_XGIVEREF(__pyx_t_13);
        __Pyx_XGIVEREF(__pyx_t_14);
        __Pyx_XGIVEREF(__pyx_t_15);
        __Pyx_ExceptionReset(__pyx_t_13, __pyx_t_14, __pyx_t_15);
        goto __pyx_L76_continue;
        __pyx_L85_try_end:;
      }
      __pyx_L76_continue:;
    }
    __Pyx_DECREF(__pyx_t_9); __pyx_t_9 = 0;
  }
  __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;

  __Pyx_XDECREF(__pyx_r);
  __pyx_t_1 = PyTuple_New(2); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 529, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_1);
  __Pyx_INCREF(__pyx_v_valid_z1);
  __Pyx_GIVEREF(__pyx_v_valid_z1);
  if (__Pyx_PyTuple_SET_ITEM(__pyx_t_1, 0, __pyx_v_valid_z1) != (0)) __PYX_ERR(0, 529, __pyx_L1_error);
  __Pyx_INCREF(__pyx_v_valid_z2);
  __Pyx_GIVEREF(__pyx_v_valid_z2);
  if (__Pyx_PyTuple_SET_ITEM(__pyx_t_1, 1, __pyx_v_valid_z2) != (0)) __PYX_ERR(0, 529, __pyx_L1_error);
  __pyx_r = __pyx_t_1;
  __pyx_t_1 = 0;
  goto __pyx_L0;

  __pyx_L1_error:;
  __Pyx_XDECREF(__pyx_t_1);
  __Pyx_XDECREF(__pyx_t_5);
  __Pyx_XDECREF(__pyx_t_9);
  __Pyx_XDECREF(__pyx_t_17);
  __Pyx_XDECREF(__pyx_t_19);
  __Pyx_AddTraceback("_fast_match.prune_pairwise_values_fast", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __pyx_r = NULL;
  __pyx_L0:;
  __Pyx_XDECREF(__pyx_v_valid_z1);
  __Pyx_XDECREF(__pyx_v_valid_z2);
  __Pyx_XDECREF(__pyx_v_c1);
  __Pyx_XDECREF(__pyx_v_v1);
  __Pyx_XDECREF(__pyx_v_c2);
  __Pyx_XDECREF(__pyx_v_v2);
  __Pyx_XDECREF(__pyx_v_matching_c2s);
  __Pyx_XDECREF(__pyx_v_unique_vals1);
  __Pyx_XDECREF(__pyx_v_unique_vals2);
  __Pyx_XDECREF(__pyx_v_max_v2);
  __Pyx_XDECREF(__pyx_v_min_v1);
  __Pyx_XDECREF(__pyx_v_min_v2);
  __Pyx_XDECREF(__pyx_v_max_v1);
  __Pyx_XDECREF(__pyx_v_val_to_c2);
  __Pyx_XDECREF(__pyx_v_bucket);
  __Pyx_XGIVEREF(__pyx_r);
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}

static PyObject *__pyx_pw_11_fast_match_13filter_dis_items_fast(PyObject *__pyx_self,
#if CYTHON_METH_FASTCALL
PyObject *const *__pyx_args, Py_ssize_t __pyx_nargs, PyObject *__pyx_kwds
#else
PyObject *__pyx_args, PyObject *__pyx_kwds
#endif
);
PyDoc_STRVAR(__pyx_doc_11_fast_match_12filter_dis_items_fast, "Filter candidate_set by a cached Mdis score, retaining uncached items.");
static PyMethodDef __pyx_mdef_11_fast_match_13filter_dis_items_fast = {"filter_dis_items_fast", (PyCFunction)(void(*)(void))(__Pyx_PyCFunction_FastCallWithKeywords)__pyx_pw_11_fast_match_13filter_dis_items_fast, __Pyx_METH_FASTCALL|METH_KEYWORDS, __pyx_doc_11_fast_match_12filter_dis_items_fast};
static PyObject *__pyx_pw_11_fast_match_13filter_dis_items_fast(PyObject *__pyx_self,
#if CYTHON_METH_FASTCALL
PyObject *const *__pyx_args, Py_ssize_t __pyx_nargs, PyObject *__pyx_kwds
#else
PyObject *__pyx_args, PyObject *__pyx_kwds
#endif
) {
  PyObject *__pyx_v_dis_cache = 0;
  PyObject *__pyx_v_anchor_id = 0;
  PyObject *__pyx_v_operator = 0;
  double __pyx_v_threshold;
  PyObject *__pyx_v_candidate_set = 0;
  #if !CYTHON_METH_FASTCALL
  CYTHON_UNUSED Py_ssize_t __pyx_nargs;
  #endif
  CYTHON_UNUSED PyObject *const *__pyx_kwvalues;
  PyObject* values[5] = {0,0,0,0,0};
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  PyObject *__pyx_r = 0;
  __Pyx_RefNannyDeclarations
  __Pyx_RefNannySetupContext("filter_dis_items_fast (wrapper)", 0);
  #if !CYTHON_METH_FASTCALL
  #if CYTHON_ASSUME_SAFE_SIZE
  __pyx_nargs = PyTuple_GET_SIZE(__pyx_args);
  #else
  __pyx_nargs = PyTuple_Size(__pyx_args); if (unlikely(__pyx_nargs < 0)) return NULL;
  #endif
  #endif
  __pyx_kwvalues = __Pyx_KwValues_FASTCALL(__pyx_args, __pyx_nargs);
  {
    PyObject ** const __pyx_pyargnames[] = {&__pyx_mstate_global->__pyx_n_u_dis_cache,&__pyx_mstate_global->__pyx_n_u_anchor_id,&__pyx_mstate_global->__pyx_n_u_operator,&__pyx_mstate_global->__pyx_n_u_threshold,&__pyx_mstate_global->__pyx_n_u_candidate_set,0};
    const Py_ssize_t __pyx_kwds_len = (__pyx_kwds) ? __Pyx_NumKwargs_FASTCALL(__pyx_kwds) : 0;
    if (unlikely(__pyx_kwds_len) < 0) __PYX_ERR(0, 532, __pyx_L3_error)
    if (__pyx_kwds_len > 0) {
      switch (__pyx_nargs) {
        case  5:
        values[4] = __Pyx_ArgRef_FASTCALL(__pyx_args, 4);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[4])) __PYX_ERR(0, 532, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  4:
        values[3] = __Pyx_ArgRef_FASTCALL(__pyx_args, 3);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[3])) __PYX_ERR(0, 532, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  3:
        values[2] = __Pyx_ArgRef_FASTCALL(__pyx_args, 2);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[2])) __PYX_ERR(0, 532, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  2:
        values[1] = __Pyx_ArgRef_FASTCALL(__pyx_args, 1);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[1])) __PYX_ERR(0, 532, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  1:
        values[0] = __Pyx_ArgRef_FASTCALL(__pyx_args, 0);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[0])) __PYX_ERR(0, 532, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  0: break;
        default: goto __pyx_L5_argtuple_error;
      }
      const Py_ssize_t kwd_pos_args = __pyx_nargs;
      if (__Pyx_ParseKeywords(__pyx_kwds, __pyx_kwvalues, __pyx_pyargnames, 0, values, kwd_pos_args, __pyx_kwds_len, "filter_dis_items_fast", 0) < (0)) __PYX_ERR(0, 532, __pyx_L3_error)
      for (Py_ssize_t i = __pyx_nargs; i < 5; i++) {
        if (unlikely(!values[i])) { __Pyx_RaiseArgtupleInvalid("filter_dis_items_fast", 1, 5, 5, i); __PYX_ERR(0, 532, __pyx_L3_error) }
      }
    } else if (unlikely(__pyx_nargs != 5)) {
      goto __pyx_L5_argtuple_error;
    } else {
      values[0] = __Pyx_ArgRef_FASTCALL(__pyx_args, 0);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[0])) __PYX_ERR(0, 532, __pyx_L3_error)
      values[1] = __Pyx_ArgRef_FASTCALL(__pyx_args, 1);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[1])) __PYX_ERR(0, 532, __pyx_L3_error)
      values[2] = __Pyx_ArgRef_FASTCALL(__pyx_args, 2);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[2])) __PYX_ERR(0, 532, __pyx_L3_error)
      values[3] = __Pyx_ArgRef_FASTCALL(__pyx_args, 3);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[3])) __PYX_ERR(0, 532, __pyx_L3_error)
      values[4] = __Pyx_ArgRef_FASTCALL(__pyx_args, 4);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[4])) __PYX_ERR(0, 532, __pyx_L3_error)
    }
    __pyx_v_dis_cache = ((PyObject*)values[0]);
    __pyx_v_anchor_id = values[1];
    __pyx_v_operator = values[2];
    __pyx_v_threshold = __Pyx_PyFloat_AsDouble(values[3]); if (unlikely((__pyx_v_threshold == (double)-1) && PyErr_Occurred())) __PYX_ERR(0, 532, __pyx_L3_error)
    __pyx_v_candidate_set = values[4];
  }
  goto __pyx_L6_skip;
  __pyx_L5_argtuple_error:;
  __Pyx_RaiseArgtupleInvalid("filter_dis_items_fast", 1, 5, 5, __pyx_nargs); __PYX_ERR(0, 532, __pyx_L3_error)
  __pyx_L6_skip:;
  goto __pyx_L4_argument_unpacking_done;
  __pyx_L3_error:;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  __Pyx_AddTraceback("_fast_match.filter_dis_items_fast", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __Pyx_RefNannyFinishContext();
  return NULL;
  __pyx_L4_argument_unpacking_done:;
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_dis_cache), (&PyDict_Type), 1, "dis_cache", 1))) __PYX_ERR(0, 532, __pyx_L1_error)
  __pyx_r = __pyx_pf_11_fast_match_12filter_dis_items_fast(__pyx_self, __pyx_v_dis_cache, __pyx_v_anchor_id, __pyx_v_operator, __pyx_v_threshold, __pyx_v_candidate_set);

  goto __pyx_L0;
  __pyx_L1_error:;
  __pyx_r = NULL;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  goto __pyx_L7_cleaned_up;
  __pyx_L0:;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  __pyx_L7_cleaned_up:;
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}

static PyObject *__pyx_pf_11_fast_match_12filter_dis_items_fast(CYTHON_UNUSED PyObject *__pyx_self, PyObject *__pyx_v_dis_cache, PyObject *__pyx_v_anchor_id, PyObject *__pyx_v_operator, double __pyx_v_threshold, PyObject *__pyx_v_candidate_set) {
  PyObject *__pyx_v_result = 0;
  PyObject *__pyx_v_w = 0;
  PyObject *__pyx_v_score = 0;
  PyObject *__pyx_r = NULL;
  __Pyx_RefNannyDeclarations
  PyObject *__pyx_t_1 = NULL;
  int __pyx_t_2;
  Py_ssize_t __pyx_t_3;
  PyObject *(*__pyx_t_4)(PyObject *);
  PyObject *__pyx_t_5 = NULL;
  PyObject *__pyx_t_6 = NULL;
  int __pyx_t_7;
  size_t __pyx_t_8;
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  __Pyx_RefNannySetupContext("filter_dis_items_fast", 0);

  __pyx_t_1 = PySet_New(0); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 534, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_1);
  __pyx_v_result = __pyx_t_1;
  __pyx_t_1 = 0;

  __pyx_t_2 = (__Pyx_PyUnicode_Equals(__pyx_v_operator, __pyx_mstate_global->__pyx_kp_u_, Py_EQ)); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 537, __pyx_L1_error)
  if (__pyx_t_2) {

    if (likely(PyList_CheckExact(__pyx_v_candidate_set)) || PyTuple_CheckExact(__pyx_v_candidate_set)) {
      __pyx_t_1 = __pyx_v_candidate_set; __Pyx_INCREF(__pyx_t_1);
      __pyx_t_3 = 0;
      __pyx_t_4 = NULL;
    } else {
      __pyx_t_3 = -1; __pyx_t_1 = PyObject_GetIter(__pyx_v_candidate_set); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 538, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_1);
      __pyx_t_4 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_1); if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 538, __pyx_L1_error)
    }
    for (;;) {
      if (likely(!__pyx_t_4)) {
        if (likely(PyList_CheckExact(__pyx_t_1))) {
          {
            Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_t_1);
            #if !CYTHON_ASSUME_SAFE_SIZE
            if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 538, __pyx_L1_error)
            #endif
            if (__pyx_t_3 >= __pyx_temp) break;
          }
          __pyx_t_5 = __Pyx_PyList_GetItemRefFast(__pyx_t_1, __pyx_t_3, __Pyx_ReferenceSharing_OwnStrongReference);
          ++__pyx_t_3;
        } else {
          {
            Py_ssize_t __pyx_temp = __Pyx_PyTuple_GET_SIZE(__pyx_t_1);
            #if !CYTHON_ASSUME_SAFE_SIZE
            if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 538, __pyx_L1_error)
            #endif
            if (__pyx_t_3 >= __pyx_temp) break;
          }
          #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
          __pyx_t_5 = __Pyx_NewRef(PyTuple_GET_ITEM(__pyx_t_1, __pyx_t_3));
          #else
          __pyx_t_5 = __Pyx_PySequence_ITEM(__pyx_t_1, __pyx_t_3);
          #endif
          ++__pyx_t_3;
        }
        if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 538, __pyx_L1_error)
      } else {
        __pyx_t_5 = __pyx_t_4(__pyx_t_1);
        if (unlikely(!__pyx_t_5)) {
          PyObject* exc_type = PyErr_Occurred();
          if (exc_type) {
            if (unlikely(!__Pyx_PyErr_GivenExceptionMatches(exc_type, PyExc_StopIteration))) __PYX_ERR(0, 538, __pyx_L1_error)
            PyErr_Clear();
          }
          break;
        }
      }
      __Pyx_GOTREF(__pyx_t_5);
      __Pyx_XDECREF_SET(__pyx_v_w, __pyx_t_5);
      __pyx_t_5 = 0;

      if (unlikely(__pyx_v_dis_cache == Py_None)) {
        PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "get");
        __PYX_ERR(0, 539, __pyx_L1_error)
      }
      __pyx_t_5 = PyTuple_New(2); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 539, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_5);
      __Pyx_INCREF(__pyx_v_anchor_id);
      __Pyx_GIVEREF(__pyx_v_anchor_id);
      if (__Pyx_PyTuple_SET_ITEM(__pyx_t_5, 0, __pyx_v_anchor_id) != (0)) __PYX_ERR(0, 539, __pyx_L1_error);
      __Pyx_INCREF(__pyx_v_w);
      __Pyx_GIVEREF(__pyx_v_w);
      if (__Pyx_PyTuple_SET_ITEM(__pyx_t_5, 1, __pyx_v_w) != (0)) __PYX_ERR(0, 539, __pyx_L1_error);
      __pyx_t_6 = __Pyx_PyDict_GetItemDefault(__pyx_v_dis_cache, __pyx_t_5, Py_None); if (unlikely(!__pyx_t_6)) __PYX_ERR(0, 539, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_6);
      __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
      __Pyx_XDECREF_SET(__pyx_v_score, __pyx_t_6);
      __pyx_t_6 = 0;

      __pyx_t_7 = (__pyx_v_score == Py_None);
      if (!__pyx_t_7) {
      } else {
        __pyx_t_2 = __pyx_t_7;
        goto __pyx_L7_bool_binop_done;
      }
      __pyx_t_6 = PyFloat_FromDouble(__pyx_v_threshold); if (unlikely(!__pyx_t_6)) __PYX_ERR(0, 540, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_6);
      __pyx_t_5 = PyObject_RichCompare(__pyx_v_score, __pyx_t_6, Py_GE); __Pyx_XGOTREF(__pyx_t_5); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 540, __pyx_L1_error)
      __Pyx_DECREF(__pyx_t_6); __pyx_t_6 = 0;
      __pyx_t_7 = __Pyx_PyObject_IsTrue(__pyx_t_5); if (unlikely((__pyx_t_7 < 0))) __PYX_ERR(0, 540, __pyx_L1_error)
      __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
      __pyx_t_2 = __pyx_t_7;
      __pyx_L7_bool_binop_done:;
      if (__pyx_t_2) {

        __pyx_t_6 = __pyx_v_result;
        __Pyx_INCREF(__pyx_t_6);
        __pyx_t_8 = 0;
        {
          PyObject *__pyx_callargs[2] = {__pyx_t_6, __pyx_v_w};
          __pyx_t_5 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_add, __pyx_callargs+__pyx_t_8, (2-__pyx_t_8) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
          __Pyx_XDECREF(__pyx_t_6); __pyx_t_6 = 0;
          if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 541, __pyx_L1_error)
          __Pyx_GOTREF(__pyx_t_5);
        }
        __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;

      }

    }
    __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;

    __Pyx_XDECREF(__pyx_r);
    __Pyx_INCREF(__pyx_v_result);
    __pyx_r = __pyx_v_result;
    goto __pyx_L0;

  }

  __pyx_t_2 = (__Pyx_PyUnicode_Equals(__pyx_v_operator, __pyx_mstate_global->__pyx_kp_u__2, Py_EQ)); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 544, __pyx_L1_error)
  if (__pyx_t_2) {

    if (likely(PyList_CheckExact(__pyx_v_candidate_set)) || PyTuple_CheckExact(__pyx_v_candidate_set)) {
      __pyx_t_1 = __pyx_v_candidate_set; __Pyx_INCREF(__pyx_t_1);
      __pyx_t_3 = 0;
      __pyx_t_4 = NULL;
    } else {
      __pyx_t_3 = -1; __pyx_t_1 = PyObject_GetIter(__pyx_v_candidate_set); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 545, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_1);
      __pyx_t_4 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_1); if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 545, __pyx_L1_error)
    }
    for (;;) {
      if (likely(!__pyx_t_4)) {
        if (likely(PyList_CheckExact(__pyx_t_1))) {
          {
            Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_t_1);
            #if !CYTHON_ASSUME_SAFE_SIZE
            if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 545, __pyx_L1_error)
            #endif
            if (__pyx_t_3 >= __pyx_temp) break;
          }
          __pyx_t_5 = __Pyx_PyList_GetItemRefFast(__pyx_t_1, __pyx_t_3, __Pyx_ReferenceSharing_OwnStrongReference);
          ++__pyx_t_3;
        } else {
          {
            Py_ssize_t __pyx_temp = __Pyx_PyTuple_GET_SIZE(__pyx_t_1);
            #if !CYTHON_ASSUME_SAFE_SIZE
            if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 545, __pyx_L1_error)
            #endif
            if (__pyx_t_3 >= __pyx_temp) break;
          }
          #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
          __pyx_t_5 = __Pyx_NewRef(PyTuple_GET_ITEM(__pyx_t_1, __pyx_t_3));
          #else
          __pyx_t_5 = __Pyx_PySequence_ITEM(__pyx_t_1, __pyx_t_3);
          #endif
          ++__pyx_t_3;
        }
        if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 545, __pyx_L1_error)
      } else {
        __pyx_t_5 = __pyx_t_4(__pyx_t_1);
        if (unlikely(!__pyx_t_5)) {
          PyObject* exc_type = PyErr_Occurred();
          if (exc_type) {
            if (unlikely(!__Pyx_PyErr_GivenExceptionMatches(exc_type, PyExc_StopIteration))) __PYX_ERR(0, 545, __pyx_L1_error)
            PyErr_Clear();
          }
          break;
        }
      }
      __Pyx_GOTREF(__pyx_t_5);
      __Pyx_XDECREF_SET(__pyx_v_w, __pyx_t_5);
      __pyx_t_5 = 0;

      if (unlikely(__pyx_v_dis_cache == Py_None)) {
        PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "get");
        __PYX_ERR(0, 546, __pyx_L1_error)
      }
      __pyx_t_5 = PyTuple_New(2); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 546, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_5);
      __Pyx_INCREF(__pyx_v_anchor_id);
      __Pyx_GIVEREF(__pyx_v_anchor_id);
      if (__Pyx_PyTuple_SET_ITEM(__pyx_t_5, 0, __pyx_v_anchor_id) != (0)) __PYX_ERR(0, 546, __pyx_L1_error);
      __Pyx_INCREF(__pyx_v_w);
      __Pyx_GIVEREF(__pyx_v_w);
      if (__Pyx_PyTuple_SET_ITEM(__pyx_t_5, 1, __pyx_v_w) != (0)) __PYX_ERR(0, 546, __pyx_L1_error);
      __pyx_t_6 = __Pyx_PyDict_GetItemDefault(__pyx_v_dis_cache, __pyx_t_5, Py_None); if (unlikely(!__pyx_t_6)) __PYX_ERR(0, 546, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_6);
      __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
      __Pyx_XDECREF_SET(__pyx_v_score, __pyx_t_6);
      __pyx_t_6 = 0;

      __pyx_t_7 = (__pyx_v_score == Py_None);
      if (!__pyx_t_7) {
      } else {
        __pyx_t_2 = __pyx_t_7;
        goto __pyx_L14_bool_binop_done;
      }
      __pyx_t_6 = PyFloat_FromDouble(__pyx_v_threshold); if (unlikely(!__pyx_t_6)) __PYX_ERR(0, 547, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_6);
      __pyx_t_5 = PyObject_RichCompare(__pyx_v_score, __pyx_t_6, Py_GT); __Pyx_XGOTREF(__pyx_t_5); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 547, __pyx_L1_error)
      __Pyx_DECREF(__pyx_t_6); __pyx_t_6 = 0;
      __pyx_t_7 = __Pyx_PyObject_IsTrue(__pyx_t_5); if (unlikely((__pyx_t_7 < 0))) __PYX_ERR(0, 547, __pyx_L1_error)
      __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
      __pyx_t_2 = __pyx_t_7;
      __pyx_L14_bool_binop_done:;
      if (__pyx_t_2) {

        __pyx_t_6 = __pyx_v_result;
        __Pyx_INCREF(__pyx_t_6);
        __pyx_t_8 = 0;
        {
          PyObject *__pyx_callargs[2] = {__pyx_t_6, __pyx_v_w};
          __pyx_t_5 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_add, __pyx_callargs+__pyx_t_8, (2-__pyx_t_8) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
          __Pyx_XDECREF(__pyx_t_6); __pyx_t_6 = 0;
          if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 548, __pyx_L1_error)
          __Pyx_GOTREF(__pyx_t_5);
        }
        __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;

      }

    }
    __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;

    __Pyx_XDECREF(__pyx_r);
    __Pyx_INCREF(__pyx_v_result);
    __pyx_r = __pyx_v_result;
    goto __pyx_L0;

  }

  __pyx_t_2 = (__Pyx_PyUnicode_Equals(__pyx_v_operator, __pyx_mstate_global->__pyx_kp_u__3, Py_EQ)); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 551, __pyx_L1_error)
  if (__pyx_t_2) {

    if (likely(PyList_CheckExact(__pyx_v_candidate_set)) || PyTuple_CheckExact(__pyx_v_candidate_set)) {
      __pyx_t_1 = __pyx_v_candidate_set; __Pyx_INCREF(__pyx_t_1);
      __pyx_t_3 = 0;
      __pyx_t_4 = NULL;
    } else {
      __pyx_t_3 = -1; __pyx_t_1 = PyObject_GetIter(__pyx_v_candidate_set); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 552, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_1);
      __pyx_t_4 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_1); if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 552, __pyx_L1_error)
    }
    for (;;) {
      if (likely(!__pyx_t_4)) {
        if (likely(PyList_CheckExact(__pyx_t_1))) {
          {
            Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_t_1);
            #if !CYTHON_ASSUME_SAFE_SIZE
            if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 552, __pyx_L1_error)
            #endif
            if (__pyx_t_3 >= __pyx_temp) break;
          }
          __pyx_t_5 = __Pyx_PyList_GetItemRefFast(__pyx_t_1, __pyx_t_3, __Pyx_ReferenceSharing_OwnStrongReference);
          ++__pyx_t_3;
        } else {
          {
            Py_ssize_t __pyx_temp = __Pyx_PyTuple_GET_SIZE(__pyx_t_1);
            #if !CYTHON_ASSUME_SAFE_SIZE
            if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 552, __pyx_L1_error)
            #endif
            if (__pyx_t_3 >= __pyx_temp) break;
          }
          #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
          __pyx_t_5 = __Pyx_NewRef(PyTuple_GET_ITEM(__pyx_t_1, __pyx_t_3));
          #else
          __pyx_t_5 = __Pyx_PySequence_ITEM(__pyx_t_1, __pyx_t_3);
          #endif
          ++__pyx_t_3;
        }
        if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 552, __pyx_L1_error)
      } else {
        __pyx_t_5 = __pyx_t_4(__pyx_t_1);
        if (unlikely(!__pyx_t_5)) {
          PyObject* exc_type = PyErr_Occurred();
          if (exc_type) {
            if (unlikely(!__Pyx_PyErr_GivenExceptionMatches(exc_type, PyExc_StopIteration))) __PYX_ERR(0, 552, __pyx_L1_error)
            PyErr_Clear();
          }
          break;
        }
      }
      __Pyx_GOTREF(__pyx_t_5);
      __Pyx_XDECREF_SET(__pyx_v_w, __pyx_t_5);
      __pyx_t_5 = 0;

      if (unlikely(__pyx_v_dis_cache == Py_None)) {
        PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "get");
        __PYX_ERR(0, 553, __pyx_L1_error)
      }
      __pyx_t_5 = PyTuple_New(2); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 553, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_5);
      __Pyx_INCREF(__pyx_v_anchor_id);
      __Pyx_GIVEREF(__pyx_v_anchor_id);
      if (__Pyx_PyTuple_SET_ITEM(__pyx_t_5, 0, __pyx_v_anchor_id) != (0)) __PYX_ERR(0, 553, __pyx_L1_error);
      __Pyx_INCREF(__pyx_v_w);
      __Pyx_GIVEREF(__pyx_v_w);
      if (__Pyx_PyTuple_SET_ITEM(__pyx_t_5, 1, __pyx_v_w) != (0)) __PYX_ERR(0, 553, __pyx_L1_error);
      __pyx_t_6 = __Pyx_PyDict_GetItemDefault(__pyx_v_dis_cache, __pyx_t_5, Py_None); if (unlikely(!__pyx_t_6)) __PYX_ERR(0, 553, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_6);
      __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
      __Pyx_XDECREF_SET(__pyx_v_score, __pyx_t_6);
      __pyx_t_6 = 0;

      __pyx_t_7 = (__pyx_v_score == Py_None);
      if (!__pyx_t_7) {
      } else {
        __pyx_t_2 = __pyx_t_7;
        goto __pyx_L21_bool_binop_done;
      }
      __pyx_t_6 = PyFloat_FromDouble(__pyx_v_threshold); if (unlikely(!__pyx_t_6)) __PYX_ERR(0, 554, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_6);
      __pyx_t_5 = PyObject_RichCompare(__pyx_v_score, __pyx_t_6, Py_LE); __Pyx_XGOTREF(__pyx_t_5); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 554, __pyx_L1_error)
      __Pyx_DECREF(__pyx_t_6); __pyx_t_6 = 0;
      __pyx_t_7 = __Pyx_PyObject_IsTrue(__pyx_t_5); if (unlikely((__pyx_t_7 < 0))) __PYX_ERR(0, 554, __pyx_L1_error)
      __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
      __pyx_t_2 = __pyx_t_7;
      __pyx_L21_bool_binop_done:;
      if (__pyx_t_2) {

        __pyx_t_6 = __pyx_v_result;
        __Pyx_INCREF(__pyx_t_6);
        __pyx_t_8 = 0;
        {
          PyObject *__pyx_callargs[2] = {__pyx_t_6, __pyx_v_w};
          __pyx_t_5 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_add, __pyx_callargs+__pyx_t_8, (2-__pyx_t_8) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
          __Pyx_XDECREF(__pyx_t_6); __pyx_t_6 = 0;
          if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 555, __pyx_L1_error)
          __Pyx_GOTREF(__pyx_t_5);
        }
        __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;

      }

    }
    __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;

    __Pyx_XDECREF(__pyx_r);
    __Pyx_INCREF(__pyx_v_result);
    __pyx_r = __pyx_v_result;
    goto __pyx_L0;

  }

  __pyx_t_2 = (__Pyx_PyUnicode_Equals(__pyx_v_operator, __pyx_mstate_global->__pyx_kp_u__4, Py_EQ)); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 558, __pyx_L1_error)
  if (__pyx_t_2) {

    if (likely(PyList_CheckExact(__pyx_v_candidate_set)) || PyTuple_CheckExact(__pyx_v_candidate_set)) {
      __pyx_t_1 = __pyx_v_candidate_set; __Pyx_INCREF(__pyx_t_1);
      __pyx_t_3 = 0;
      __pyx_t_4 = NULL;
    } else {
      __pyx_t_3 = -1; __pyx_t_1 = PyObject_GetIter(__pyx_v_candidate_set); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 559, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_1);
      __pyx_t_4 = (CYTHON_COMPILING_IN_LIMITED_API) ? PyIter_Next : __Pyx_PyObject_GetIterNextFunc(__pyx_t_1); if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 559, __pyx_L1_error)
    }
    for (;;) {
      if (likely(!__pyx_t_4)) {
        if (likely(PyList_CheckExact(__pyx_t_1))) {
          {
            Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_t_1);
            #if !CYTHON_ASSUME_SAFE_SIZE
            if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 559, __pyx_L1_error)
            #endif
            if (__pyx_t_3 >= __pyx_temp) break;
          }
          __pyx_t_5 = __Pyx_PyList_GetItemRefFast(__pyx_t_1, __pyx_t_3, __Pyx_ReferenceSharing_OwnStrongReference);
          ++__pyx_t_3;
        } else {
          {
            Py_ssize_t __pyx_temp = __Pyx_PyTuple_GET_SIZE(__pyx_t_1);
            #if !CYTHON_ASSUME_SAFE_SIZE
            if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 559, __pyx_L1_error)
            #endif
            if (__pyx_t_3 >= __pyx_temp) break;
          }
          #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
          __pyx_t_5 = __Pyx_NewRef(PyTuple_GET_ITEM(__pyx_t_1, __pyx_t_3));
          #else
          __pyx_t_5 = __Pyx_PySequence_ITEM(__pyx_t_1, __pyx_t_3);
          #endif
          ++__pyx_t_3;
        }
        if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 559, __pyx_L1_error)
      } else {
        __pyx_t_5 = __pyx_t_4(__pyx_t_1);
        if (unlikely(!__pyx_t_5)) {
          PyObject* exc_type = PyErr_Occurred();
          if (exc_type) {
            if (unlikely(!__Pyx_PyErr_GivenExceptionMatches(exc_type, PyExc_StopIteration))) __PYX_ERR(0, 559, __pyx_L1_error)
            PyErr_Clear();
          }
          break;
        }
      }
      __Pyx_GOTREF(__pyx_t_5);
      __Pyx_XDECREF_SET(__pyx_v_w, __pyx_t_5);
      __pyx_t_5 = 0;

      if (unlikely(__pyx_v_dis_cache == Py_None)) {
        PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "get");
        __PYX_ERR(0, 560, __pyx_L1_error)
      }
      __pyx_t_5 = PyTuple_New(2); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 560, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_5);
      __Pyx_INCREF(__pyx_v_anchor_id);
      __Pyx_GIVEREF(__pyx_v_anchor_id);
      if (__Pyx_PyTuple_SET_ITEM(__pyx_t_5, 0, __pyx_v_anchor_id) != (0)) __PYX_ERR(0, 560, __pyx_L1_error);
      __Pyx_INCREF(__pyx_v_w);
      __Pyx_GIVEREF(__pyx_v_w);
      if (__Pyx_PyTuple_SET_ITEM(__pyx_t_5, 1, __pyx_v_w) != (0)) __PYX_ERR(0, 560, __pyx_L1_error);
      __pyx_t_6 = __Pyx_PyDict_GetItemDefault(__pyx_v_dis_cache, __pyx_t_5, Py_None); if (unlikely(!__pyx_t_6)) __PYX_ERR(0, 560, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_6);
      __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
      __Pyx_XDECREF_SET(__pyx_v_score, __pyx_t_6);
      __pyx_t_6 = 0;

      __pyx_t_7 = (__pyx_v_score == Py_None);
      if (!__pyx_t_7) {
      } else {
        __pyx_t_2 = __pyx_t_7;
        goto __pyx_L28_bool_binop_done;
      }
      __pyx_t_6 = PyFloat_FromDouble(__pyx_v_threshold); if (unlikely(!__pyx_t_6)) __PYX_ERR(0, 561, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_6);
      __pyx_t_5 = PyObject_RichCompare(__pyx_v_score, __pyx_t_6, Py_LT); __Pyx_XGOTREF(__pyx_t_5); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 561, __pyx_L1_error)
      __Pyx_DECREF(__pyx_t_6); __pyx_t_6 = 0;
      __pyx_t_7 = __Pyx_PyObject_IsTrue(__pyx_t_5); if (unlikely((__pyx_t_7 < 0))) __PYX_ERR(0, 561, __pyx_L1_error)
      __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
      __pyx_t_2 = __pyx_t_7;
      __pyx_L28_bool_binop_done:;
      if (__pyx_t_2) {

        __pyx_t_6 = __pyx_v_result;
        __Pyx_INCREF(__pyx_t_6);
        __pyx_t_8 = 0;
        {
          PyObject *__pyx_callargs[2] = {__pyx_t_6, __pyx_v_w};
          __pyx_t_5 = __Pyx_PyObject_FastCallMethod((PyObject*)__pyx_mstate_global->__pyx_n_u_add, __pyx_callargs+__pyx_t_8, (2-__pyx_t_8) | (1*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
          __Pyx_XDECREF(__pyx_t_6); __pyx_t_6 = 0;
          if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 562, __pyx_L1_error)
          __Pyx_GOTREF(__pyx_t_5);
        }
        __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;

      }

    }
    __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;

    __Pyx_XDECREF(__pyx_r);
    __Pyx_INCREF(__pyx_v_result);
    __pyx_r = __pyx_v_result;
    goto __pyx_L0;

  }

  __Pyx_XDECREF(__pyx_r);
  __pyx_t_1 = PySet_New(__pyx_v_candidate_set); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 565, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_1);
  __pyx_r = __pyx_t_1;
  __pyx_t_1 = 0;
  goto __pyx_L0;

  __pyx_L1_error:;
  __Pyx_XDECREF(__pyx_t_1);
  __Pyx_XDECREF(__pyx_t_5);
  __Pyx_XDECREF(__pyx_t_6);
  __Pyx_AddTraceback("_fast_match.filter_dis_items_fast", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __pyx_r = NULL;
  __pyx_L0:;
  __Pyx_XDECREF(__pyx_v_result);
  __Pyx_XDECREF(__pyx_v_w);
  __Pyx_XDECREF(__pyx_v_score);
  __Pyx_XGIVEREF(__pyx_r);
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}

static PyObject *__pyx_pw_11_fast_match_15verify_pivot_pairwise_binding_fast(PyObject *__pyx_self,
#if CYTHON_METH_FASTCALL
PyObject *const *__pyx_args, Py_ssize_t __pyx_nargs, PyObject *__pyx_kwds
#else
PyObject *__pyx_args, PyObject *__pyx_kwds
#endif
);
PyDoc_STRVAR(__pyx_doc_11_fast_match_14verify_pivot_pairwise_binding_fast, "\n    Exact PairWise verification for bindings where all variables are pivots.\n    ");
static PyMethodDef __pyx_mdef_11_fast_match_15verify_pivot_pairwise_binding_fast = {"verify_pivot_pairwise_binding_fast", (PyCFunction)(void(*)(void))(__Pyx_PyCFunction_FastCallWithKeywords)__pyx_pw_11_fast_match_15verify_pivot_pairwise_binding_fast, __Pyx_METH_FASTCALL|METH_KEYWORDS, __pyx_doc_11_fast_match_14verify_pivot_pairwise_binding_fast};
static PyObject *__pyx_pw_11_fast_match_15verify_pivot_pairwise_binding_fast(PyObject *__pyx_self,
#if CYTHON_METH_FASTCALL
PyObject *const *__pyx_args, Py_ssize_t __pyx_nargs, PyObject *__pyx_kwds
#else
PyObject *__pyx_args, PyObject *__pyx_kwds
#endif
) {
  PyObject *__pyx_v_user_attrs = 0;
  PyObject *__pyx_v_item_attrs = 0;
  PyObject *__pyx_v_pairwise_plans = 0;
  #if !CYTHON_METH_FASTCALL
  CYTHON_UNUSED Py_ssize_t __pyx_nargs;
  #endif
  CYTHON_UNUSED PyObject *const *__pyx_kwvalues;
  PyObject* values[3] = {0,0,0};
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  PyObject *__pyx_r = 0;
  __Pyx_RefNannyDeclarations
  __Pyx_RefNannySetupContext("verify_pivot_pairwise_binding_fast (wrapper)", 0);
  #if !CYTHON_METH_FASTCALL
  #if CYTHON_ASSUME_SAFE_SIZE
  __pyx_nargs = PyTuple_GET_SIZE(__pyx_args);
  #else
  __pyx_nargs = PyTuple_Size(__pyx_args); if (unlikely(__pyx_nargs < 0)) return NULL;
  #endif
  #endif
  __pyx_kwvalues = __Pyx_KwValues_FASTCALL(__pyx_args, __pyx_nargs);
  {
    PyObject ** const __pyx_pyargnames[] = {&__pyx_mstate_global->__pyx_n_u_user_attrs,&__pyx_mstate_global->__pyx_n_u_item_attrs,&__pyx_mstate_global->__pyx_n_u_pairwise_plans,0};
    const Py_ssize_t __pyx_kwds_len = (__pyx_kwds) ? __Pyx_NumKwargs_FASTCALL(__pyx_kwds) : 0;
    if (unlikely(__pyx_kwds_len) < 0) __PYX_ERR(0, 568, __pyx_L3_error)
    if (__pyx_kwds_len > 0) {
      switch (__pyx_nargs) {
        case  3:
        values[2] = __Pyx_ArgRef_FASTCALL(__pyx_args, 2);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[2])) __PYX_ERR(0, 568, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  2:
        values[1] = __Pyx_ArgRef_FASTCALL(__pyx_args, 1);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[1])) __PYX_ERR(0, 568, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  1:
        values[0] = __Pyx_ArgRef_FASTCALL(__pyx_args, 0);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[0])) __PYX_ERR(0, 568, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  0: break;
        default: goto __pyx_L5_argtuple_error;
      }
      const Py_ssize_t kwd_pos_args = __pyx_nargs;
      if (__Pyx_ParseKeywords(__pyx_kwds, __pyx_kwvalues, __pyx_pyargnames, 0, values, kwd_pos_args, __pyx_kwds_len, "verify_pivot_pairwise_binding_fast", 0) < (0)) __PYX_ERR(0, 568, __pyx_L3_error)
      for (Py_ssize_t i = __pyx_nargs; i < 3; i++) {
        if (unlikely(!values[i])) { __Pyx_RaiseArgtupleInvalid("verify_pivot_pairwise_binding_fast", 1, 3, 3, i); __PYX_ERR(0, 568, __pyx_L3_error) }
      }
    } else if (unlikely(__pyx_nargs != 3)) {
      goto __pyx_L5_argtuple_error;
    } else {
      values[0] = __Pyx_ArgRef_FASTCALL(__pyx_args, 0);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[0])) __PYX_ERR(0, 568, __pyx_L3_error)
      values[1] = __Pyx_ArgRef_FASTCALL(__pyx_args, 1);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[1])) __PYX_ERR(0, 568, __pyx_L3_error)
      values[2] = __Pyx_ArgRef_FASTCALL(__pyx_args, 2);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[2])) __PYX_ERR(0, 568, __pyx_L3_error)
    }
    __pyx_v_user_attrs = ((PyObject*)values[0]);
    __pyx_v_item_attrs = ((PyObject*)values[1]);
    __pyx_v_pairwise_plans = ((PyObject*)values[2]);
  }
  goto __pyx_L6_skip;
  __pyx_L5_argtuple_error:;
  __Pyx_RaiseArgtupleInvalid("verify_pivot_pairwise_binding_fast", 1, 3, 3, __pyx_nargs); __PYX_ERR(0, 568, __pyx_L3_error)
  __pyx_L6_skip:;
  goto __pyx_L4_argument_unpacking_done;
  __pyx_L3_error:;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  __Pyx_AddTraceback("_fast_match.verify_pivot_pairwise_binding_fast", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __Pyx_RefNannyFinishContext();
  return NULL;
  __pyx_L4_argument_unpacking_done:;
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_user_attrs), (&PyDict_Type), 1, "user_attrs", 1))) __PYX_ERR(0, 568, __pyx_L1_error)
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_item_attrs), (&PyDict_Type), 1, "item_attrs", 1))) __PYX_ERR(0, 568, __pyx_L1_error)
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_pairwise_plans), (&PyList_Type), 1, "pairwise_plans", 1))) __PYX_ERR(0, 568, __pyx_L1_error)
  __pyx_r = __pyx_pf_11_fast_match_14verify_pivot_pairwise_binding_fast(__pyx_self, __pyx_v_user_attrs, __pyx_v_item_attrs, __pyx_v_pairwise_plans);

  goto __pyx_L0;
  __pyx_L1_error:;
  __pyx_r = NULL;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  goto __pyx_L7_cleaned_up;
  __pyx_L0:;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  __pyx_L7_cleaned_up:;
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}

static PyObject *__pyx_pf_11_fast_match_14verify_pivot_pairwise_binding_fast(CYTHON_UNUSED PyObject *__pyx_self, PyObject *__pyx_v_user_attrs, PyObject *__pyx_v_item_attrs, PyObject *__pyx_v_pairwise_plans) {
  PyObject *__pyx_v_plan = 0;
  PyObject *__pyx_v_attr1 = 0;
  PyObject *__pyx_v_attr2 = 0;
  PyObject *__pyx_v_operator = 0;
  PyObject *__pyx_v_value1 = 0;
  PyObject *__pyx_v_value2 = 0;
  int __pyx_v_lhs_is_user;
  int __pyx_v_rhs_is_user;
  PyObject *__pyx_r = NULL;
  __Pyx_RefNannyDeclarations
  PyObject *__pyx_t_1 = NULL;
  Py_ssize_t __pyx_t_2;
  PyObject *__pyx_t_3 = NULL;
  int __pyx_t_4;
  PyObject *__pyx_t_5 = NULL;
  int __pyx_t_6;
  PyObject *__pyx_t_7 = NULL;
  size_t __pyx_t_8;
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  __Pyx_RefNannySetupContext("verify_pivot_pairwise_binding_fast", 0);

  if (unlikely(__pyx_v_pairwise_plans == Py_None)) {
    PyErr_SetString(PyExc_TypeError, "'NoneType' object is not iterable");
    __PYX_ERR(0, 575, __pyx_L1_error)
  }
  __pyx_t_1 = __pyx_v_pairwise_plans; __Pyx_INCREF(__pyx_t_1);
  __pyx_t_2 = 0;
  for (;;) {
    {
      Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_t_1);
      #if !CYTHON_ASSUME_SAFE_SIZE
      if (unlikely((__pyx_temp < 0))) __PYX_ERR(0, 575, __pyx_L1_error)
      #endif
      if (__pyx_t_2 >= __pyx_temp) break;
    }
    __pyx_t_3 = __Pyx_PyList_GetItemRefFast(__pyx_t_1, __pyx_t_2, __Pyx_ReferenceSharing_OwnStrongReference);
    ++__pyx_t_2;
    if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 575, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_3);
    __Pyx_XDECREF_SET(__pyx_v_plan, __pyx_t_3);
    __pyx_t_3 = 0;

    __pyx_t_3 = __Pyx_GetItemInt(__pyx_v_plan, 0, long, 1, __Pyx_PyLong_From_long, 0, 0, 0, 1, __Pyx_ReferenceSharing_OwnStrongReference); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 576, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_3);
    __pyx_t_4 = __Pyx_PyObject_IsTrue(__pyx_t_3); if (unlikely((__pyx_t_4 == (int)-1) && PyErr_Occurred())) __PYX_ERR(0, 576, __pyx_L1_error)
    __Pyx_DECREF(__pyx_t_3); __pyx_t_3 = 0;
    __pyx_v_lhs_is_user = __pyx_t_4;

    __pyx_t_3 = __Pyx_GetItemInt(__pyx_v_plan, 1, long, 1, __Pyx_PyLong_From_long, 0, 0, 0, 1, __Pyx_ReferenceSharing_OwnStrongReference); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 577, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_3);
    __Pyx_XDECREF_SET(__pyx_v_attr1, __pyx_t_3);
    __pyx_t_3 = 0;

    __pyx_t_3 = __Pyx_GetItemInt(__pyx_v_plan, 2, long, 1, __Pyx_PyLong_From_long, 0, 0, 0, 1, __Pyx_ReferenceSharing_OwnStrongReference); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 578, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_3);
    __pyx_t_4 = __Pyx_PyObject_IsTrue(__pyx_t_3); if (unlikely((__pyx_t_4 == (int)-1) && PyErr_Occurred())) __PYX_ERR(0, 578, __pyx_L1_error)
    __Pyx_DECREF(__pyx_t_3); __pyx_t_3 = 0;
    __pyx_v_rhs_is_user = __pyx_t_4;

    __pyx_t_3 = __Pyx_GetItemInt(__pyx_v_plan, 3, long, 1, __Pyx_PyLong_From_long, 0, 0, 0, 1, __Pyx_ReferenceSharing_OwnStrongReference); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 579, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_3);
    __Pyx_XDECREF_SET(__pyx_v_attr2, __pyx_t_3);
    __pyx_t_3 = 0;

    __pyx_t_3 = __Pyx_GetItemInt(__pyx_v_plan, 4, long, 1, __Pyx_PyLong_From_long, 0, 0, 0, 1, __Pyx_ReferenceSharing_OwnStrongReference); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 580, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_3);
    __Pyx_XDECREF_SET(__pyx_v_operator, __pyx_t_3);
    __pyx_t_3 = 0;

    if (__pyx_v_lhs_is_user) {
      if (unlikely(__pyx_v_user_attrs == Py_None)) {
        PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "get");
        __PYX_ERR(0, 582, __pyx_L1_error)
      }
      __pyx_t_5 = __Pyx_PyDict_GetItemDefault(__pyx_v_user_attrs, __pyx_v_attr1, Py_None); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 582, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_5);
      __pyx_t_3 = __pyx_t_5;
      __pyx_t_5 = 0;
    } else {
      if (unlikely(__pyx_v_item_attrs == Py_None)) {
        PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "get");
        __PYX_ERR(0, 582, __pyx_L1_error)
      }
      __pyx_t_5 = __Pyx_PyDict_GetItemDefault(__pyx_v_item_attrs, __pyx_v_attr1, Py_None); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 582, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_5);
      __pyx_t_3 = __pyx_t_5;
      __pyx_t_5 = 0;
    }
    __Pyx_XDECREF_SET(__pyx_v_value1, __pyx_t_3);
    __pyx_t_3 = 0;

    if (__pyx_v_rhs_is_user) {
      if (unlikely(__pyx_v_user_attrs == Py_None)) {
        PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "get");
        __PYX_ERR(0, 583, __pyx_L1_error)
      }
      __pyx_t_5 = __Pyx_PyDict_GetItemDefault(__pyx_v_user_attrs, __pyx_v_attr2, Py_None); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 583, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_5);
      __pyx_t_3 = __pyx_t_5;
      __pyx_t_5 = 0;
    } else {
      if (unlikely(__pyx_v_item_attrs == Py_None)) {
        PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "get");
        __PYX_ERR(0, 583, __pyx_L1_error)
      }
      __pyx_t_5 = __Pyx_PyDict_GetItemDefault(__pyx_v_item_attrs, __pyx_v_attr2, Py_None); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 583, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_5);
      __pyx_t_3 = __pyx_t_5;
      __pyx_t_5 = 0;
    }
    __Pyx_XDECREF_SET(__pyx_v_value2, __pyx_t_3);
    __pyx_t_3 = 0;

    __pyx_t_6 = (__pyx_v_value1 == Py_None);
    if (!__pyx_t_6) {
    } else {
      __pyx_t_4 = __pyx_t_6;
      goto __pyx_L6_bool_binop_done;
    }
    __pyx_t_6 = (__pyx_v_value2 == Py_None);
    __pyx_t_4 = __pyx_t_6;
    __pyx_L6_bool_binop_done:;
    if (__pyx_t_4) {

      __Pyx_XDECREF(__pyx_r);
      __Pyx_INCREF(Py_False);
      __pyx_r = Py_False;
      __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;
      goto __pyx_L0;

    }

    __pyx_t_5 = NULL;
    __Pyx_GetModuleGlobalName(__pyx_t_7, __pyx_mstate_global->__pyx_n_u_safe_compare_predicate_values); if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 586, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_7);
    __pyx_t_8 = 1;
    #if CYTHON_UNPACK_METHODS
    if (unlikely(PyMethod_Check(__pyx_t_7))) {
      __pyx_t_5 = PyMethod_GET_SELF(__pyx_t_7);
      assert(__pyx_t_5);
      PyObject* __pyx__function = PyMethod_GET_FUNCTION(__pyx_t_7);
      __Pyx_INCREF(__pyx_t_5);
      __Pyx_INCREF(__pyx__function);
      __Pyx_DECREF_SET(__pyx_t_7, __pyx__function);
      __pyx_t_8 = 0;
    }
    #endif
    {
      PyObject *__pyx_callargs[4] = {__pyx_t_5, __pyx_v_value1, __pyx_v_value2, __pyx_v_operator};
      __pyx_t_3 = __Pyx_PyObject_FastCall((PyObject*)__pyx_t_7, __pyx_callargs+__pyx_t_8, (4-__pyx_t_8) | (__pyx_t_8*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
      __Pyx_XDECREF(__pyx_t_5); __pyx_t_5 = 0;
      __Pyx_DECREF(__pyx_t_7); __pyx_t_7 = 0;
      if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 586, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_3);
    }
    __pyx_t_4 = __Pyx_PyObject_IsTrue(__pyx_t_3); if (unlikely((__pyx_t_4 < 0))) __PYX_ERR(0, 586, __pyx_L1_error)
    __Pyx_DECREF(__pyx_t_3); __pyx_t_3 = 0;
    __pyx_t_6 = (!__pyx_t_4);
    if (__pyx_t_6) {

      __Pyx_XDECREF(__pyx_r);
      __Pyx_INCREF(Py_False);
      __pyx_r = Py_False;
      __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;
      goto __pyx_L0;

    }

  }
  __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;

  __Pyx_XDECREF(__pyx_r);
  __Pyx_INCREF(Py_True);
  __pyx_r = Py_True;
  goto __pyx_L0;

  __pyx_L1_error:;
  __Pyx_XDECREF(__pyx_t_1);
  __Pyx_XDECREF(__pyx_t_3);
  __Pyx_XDECREF(__pyx_t_5);
  __Pyx_XDECREF(__pyx_t_7);
  __Pyx_AddTraceback("_fast_match.verify_pivot_pairwise_binding_fast", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __pyx_r = NULL;
  __pyx_L0:;
  __Pyx_XDECREF(__pyx_v_plan);
  __Pyx_XDECREF(__pyx_v_attr1);
  __Pyx_XDECREF(__pyx_v_attr2);
  __Pyx_XDECREF(__pyx_v_operator);
  __Pyx_XDECREF(__pyx_v_value1);
  __Pyx_XDECREF(__pyx_v_value2);
  __Pyx_XGIVEREF(__pyx_r);
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}

static PyObject *__pyx_pw_11_fast_match_17cleanup_lazy_heap_fast(PyObject *__pyx_self,
#if CYTHON_METH_FASTCALL
PyObject *const *__pyx_args, Py_ssize_t __pyx_nargs, PyObject *__pyx_kwds
#else
PyObject *__pyx_args, PyObject *__pyx_kwds
#endif
);
PyDoc_STRVAR(__pyx_doc_11_fast_match_16cleanup_lazy_heap_fast, "Drop stale lazy-heap entries whose score no longer matches the active map.");
static PyMethodDef __pyx_mdef_11_fast_match_17cleanup_lazy_heap_fast = {"cleanup_lazy_heap_fast", (PyCFunction)(void(*)(void))(__Pyx_PyCFunction_FastCallWithKeywords)__pyx_pw_11_fast_match_17cleanup_lazy_heap_fast, __Pyx_METH_FASTCALL|METH_KEYWORDS, __pyx_doc_11_fast_match_16cleanup_lazy_heap_fast};
static PyObject *__pyx_pw_11_fast_match_17cleanup_lazy_heap_fast(PyObject *__pyx_self,
#if CYTHON_METH_FASTCALL
PyObject *const *__pyx_args, Py_ssize_t __pyx_nargs, PyObject *__pyx_kwds
#else
PyObject *__pyx_args, PyObject *__pyx_kwds
#endif
) {
  PyObject *__pyx_v_heap = 0;
  PyObject *__pyx_v_active_scores = 0;
  #if !CYTHON_METH_FASTCALL
  CYTHON_UNUSED Py_ssize_t __pyx_nargs;
  #endif
  CYTHON_UNUSED PyObject *const *__pyx_kwvalues;
  PyObject* values[2] = {0,0};
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  PyObject *__pyx_r = 0;
  __Pyx_RefNannyDeclarations
  __Pyx_RefNannySetupContext("cleanup_lazy_heap_fast (wrapper)", 0);
  #if !CYTHON_METH_FASTCALL
  #if CYTHON_ASSUME_SAFE_SIZE
  __pyx_nargs = PyTuple_GET_SIZE(__pyx_args);
  #else
  __pyx_nargs = PyTuple_Size(__pyx_args); if (unlikely(__pyx_nargs < 0)) return NULL;
  #endif
  #endif
  __pyx_kwvalues = __Pyx_KwValues_FASTCALL(__pyx_args, __pyx_nargs);
  {
    PyObject ** const __pyx_pyargnames[] = {&__pyx_mstate_global->__pyx_n_u_heap,&__pyx_mstate_global->__pyx_n_u_active_scores,0};
    const Py_ssize_t __pyx_kwds_len = (__pyx_kwds) ? __Pyx_NumKwargs_FASTCALL(__pyx_kwds) : 0;
    if (unlikely(__pyx_kwds_len) < 0) __PYX_ERR(0, 592, __pyx_L3_error)
    if (__pyx_kwds_len > 0) {
      switch (__pyx_nargs) {
        case  2:
        values[1] = __Pyx_ArgRef_FASTCALL(__pyx_args, 1);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[1])) __PYX_ERR(0, 592, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  1:
        values[0] = __Pyx_ArgRef_FASTCALL(__pyx_args, 0);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[0])) __PYX_ERR(0, 592, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  0: break;
        default: goto __pyx_L5_argtuple_error;
      }
      const Py_ssize_t kwd_pos_args = __pyx_nargs;
      if (__Pyx_ParseKeywords(__pyx_kwds, __pyx_kwvalues, __pyx_pyargnames, 0, values, kwd_pos_args, __pyx_kwds_len, "cleanup_lazy_heap_fast", 0) < (0)) __PYX_ERR(0, 592, __pyx_L3_error)
      for (Py_ssize_t i = __pyx_nargs; i < 2; i++) {
        if (unlikely(!values[i])) { __Pyx_RaiseArgtupleInvalid("cleanup_lazy_heap_fast", 1, 2, 2, i); __PYX_ERR(0, 592, __pyx_L3_error) }
      }
    } else if (unlikely(__pyx_nargs != 2)) {
      goto __pyx_L5_argtuple_error;
    } else {
      values[0] = __Pyx_ArgRef_FASTCALL(__pyx_args, 0);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[0])) __PYX_ERR(0, 592, __pyx_L3_error)
      values[1] = __Pyx_ArgRef_FASTCALL(__pyx_args, 1);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[1])) __PYX_ERR(0, 592, __pyx_L3_error)
    }
    __pyx_v_heap = ((PyObject*)values[0]);
    __pyx_v_active_scores = ((PyObject*)values[1]);
  }
  goto __pyx_L6_skip;
  __pyx_L5_argtuple_error:;
  __Pyx_RaiseArgtupleInvalid("cleanup_lazy_heap_fast", 1, 2, 2, __pyx_nargs); __PYX_ERR(0, 592, __pyx_L3_error)
  __pyx_L6_skip:;
  goto __pyx_L4_argument_unpacking_done;
  __pyx_L3_error:;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  __Pyx_AddTraceback("_fast_match.cleanup_lazy_heap_fast", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __Pyx_RefNannyFinishContext();
  return NULL;
  __pyx_L4_argument_unpacking_done:;
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_heap), (&PyList_Type), 1, "heap", 1))) __PYX_ERR(0, 592, __pyx_L1_error)
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_active_scores), (&PyDict_Type), 1, "active_scores", 1))) __PYX_ERR(0, 592, __pyx_L1_error)
  __pyx_r = __pyx_pf_11_fast_match_16cleanup_lazy_heap_fast(__pyx_self, __pyx_v_heap, __pyx_v_active_scores);

  goto __pyx_L0;
  __pyx_L1_error:;
  __pyx_r = NULL;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  goto __pyx_L7_cleaned_up;
  __pyx_L0:;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  __pyx_L7_cleaned_up:;
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}

static PyObject *__pyx_pf_11_fast_match_16cleanup_lazy_heap_fast(CYTHON_UNUSED PyObject *__pyx_self, PyObject *__pyx_v_heap, PyObject *__pyx_v_active_scores) {
  PyObject *__pyx_r = NULL;
  __Pyx_RefNannyDeclarations
  int __pyx_t_1;
  int __pyx_t_2;
  PyObject *__pyx_t_3 = NULL;
  PyObject *__pyx_t_4 = NULL;
  PyObject *__pyx_t_5 = NULL;
  PyObject *__pyx_t_6 = NULL;
  size_t __pyx_t_7;
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  __Pyx_RefNannySetupContext("cleanup_lazy_heap_fast", 0);

  while (1) {
    if (__pyx_v_heap == Py_None) __pyx_t_2 = 0;
    else
    {
      Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_v_heap);
      if (unlikely(((!CYTHON_ASSUME_SAFE_SIZE) && __pyx_temp < 0))) __PYX_ERR(0, 594, __pyx_L1_error)
      __pyx_t_2 = (__pyx_temp != 0);
    }

    if (__pyx_t_2) {
    } else {
      __pyx_t_1 = __pyx_t_2;
      goto __pyx_L5_bool_binop_done;
    }
    if (unlikely(__pyx_v_active_scores == Py_None)) {
      PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "get");
      __PYX_ERR(0, 594, __pyx_L1_error)
    }
    if (unlikely(__pyx_v_heap == Py_None)) {
      PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
      __PYX_ERR(0, 594, __pyx_L1_error)
    }
    __pyx_t_3 = __Pyx_GetItemInt(__Pyx_PyList_GET_ITEM(__pyx_v_heap, 0), 2, long, 1, __Pyx_PyLong_From_long, 0, 0, 0, 1, __Pyx_ReferenceSharing_SharedReference); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 594, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_3);
    __pyx_t_4 = __Pyx_PyDict_GetItemDefault(__pyx_v_active_scores, __pyx_t_3, Py_None); if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 594, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_4);
    __Pyx_DECREF(__pyx_t_3); __pyx_t_3 = 0;
    if (unlikely(__pyx_v_heap == Py_None)) {
      PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
      __PYX_ERR(0, 594, __pyx_L1_error)
    }
    __pyx_t_3 = __Pyx_GetItemInt(__Pyx_PyList_GET_ITEM(__pyx_v_heap, 0), 0, long, 1, __Pyx_PyLong_From_long, 0, 0, 0, 1, __Pyx_ReferenceSharing_SharedReference); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 594, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_3);
    __pyx_t_5 = PyObject_RichCompare(__pyx_t_4, __pyx_t_3, Py_NE); __Pyx_XGOTREF(__pyx_t_5); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 594, __pyx_L1_error)
    __Pyx_DECREF(__pyx_t_4); __pyx_t_4 = 0;
    __Pyx_DECREF(__pyx_t_3); __pyx_t_3 = 0;
    __pyx_t_2 = __Pyx_PyObject_IsTrue(__pyx_t_5); if (unlikely((__pyx_t_2 < 0))) __PYX_ERR(0, 594, __pyx_L1_error)
    __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
    __pyx_t_1 = __pyx_t_2;
    __pyx_L5_bool_binop_done:;
    if (!__pyx_t_1) break;

    __pyx_t_3 = NULL;
    __Pyx_GetModuleGlobalName(__pyx_t_4, __pyx_mstate_global->__pyx_n_u_heapq); if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 595, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_4);
    __pyx_t_6 = __Pyx_PyObject_GetAttrStr(__pyx_t_4, __pyx_mstate_global->__pyx_n_u_heappop); if (unlikely(!__pyx_t_6)) __PYX_ERR(0, 595, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_6);
    __Pyx_DECREF(__pyx_t_4); __pyx_t_4 = 0;
    __pyx_t_7 = 1;
    #if CYTHON_UNPACK_METHODS
    if (unlikely(PyMethod_Check(__pyx_t_6))) {
      __pyx_t_3 = PyMethod_GET_SELF(__pyx_t_6);
      assert(__pyx_t_3);
      PyObject* __pyx__function = PyMethod_GET_FUNCTION(__pyx_t_6);
      __Pyx_INCREF(__pyx_t_3);
      __Pyx_INCREF(__pyx__function);
      __Pyx_DECREF_SET(__pyx_t_6, __pyx__function);
      __pyx_t_7 = 0;
    }
    #endif
    {
      PyObject *__pyx_callargs[2] = {__pyx_t_3, __pyx_v_heap};
      __pyx_t_5 = __Pyx_PyObject_FastCall((PyObject*)__pyx_t_6, __pyx_callargs+__pyx_t_7, (2-__pyx_t_7) | (__pyx_t_7*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
      __Pyx_XDECREF(__pyx_t_3); __pyx_t_3 = 0;
      __Pyx_DECREF(__pyx_t_6); __pyx_t_6 = 0;
      if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 595, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_5);
    }
    __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
  }

  __pyx_r = Py_None; __Pyx_INCREF(Py_None);
  goto __pyx_L0;
  __pyx_L1_error:;
  __Pyx_XDECREF(__pyx_t_3);
  __Pyx_XDECREF(__pyx_t_4);
  __Pyx_XDECREF(__pyx_t_5);
  __Pyx_XDECREF(__pyx_t_6);
  __Pyx_AddTraceback("_fast_match.cleanup_lazy_heap_fast", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __pyx_r = NULL;
  __pyx_L0:;
  __Pyx_XGIVEREF(__pyx_r);
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}

static PyObject *__pyx_pw_11_fast_match_19update_unique_topk_heap_fast(PyObject *__pyx_self,
#if CYTHON_METH_FASTCALL
PyObject *const *__pyx_args, Py_ssize_t __pyx_nargs, PyObject *__pyx_kwds
#else
PyObject *__pyx_args, PyObject *__pyx_kwds
#endif
);
PyDoc_STRVAR(__pyx_doc_11_fast_match_18update_unique_topk_heap_fast, "Unique-item top-k heap maintenance with lazy invalidation.");
static PyMethodDef __pyx_mdef_11_fast_match_19update_unique_topk_heap_fast = {"update_unique_topk_heap_fast", (PyCFunction)(void(*)(void))(__Pyx_PyCFunction_FastCallWithKeywords)__pyx_pw_11_fast_match_19update_unique_topk_heap_fast, __Pyx_METH_FASTCALL|METH_KEYWORDS, __pyx_doc_11_fast_match_18update_unique_topk_heap_fast};
static PyObject *__pyx_pw_11_fast_match_19update_unique_topk_heap_fast(PyObject *__pyx_self,
#if CYTHON_METH_FASTCALL
PyObject *const *__pyx_args, Py_ssize_t __pyx_nargs, PyObject *__pyx_kwds
#else
PyObject *__pyx_args, PyObject *__pyx_kwds
#endif
) {
  PyObject *__pyx_v_item = 0;
  double __pyx_v_score;
  PyObject *__pyx_v_heap = 0;
  PyObject *__pyx_v_active_scores = 0;
  int __pyx_v_k;
  PyObject *__pyx_v_counter_state = 0;
  #if !CYTHON_METH_FASTCALL
  CYTHON_UNUSED Py_ssize_t __pyx_nargs;
  #endif
  CYTHON_UNUSED PyObject *const *__pyx_kwvalues;
  PyObject* values[6] = {0,0,0,0,0,0};
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  PyObject *__pyx_r = 0;
  __Pyx_RefNannyDeclarations
  __Pyx_RefNannySetupContext("update_unique_topk_heap_fast (wrapper)", 0);
  #if !CYTHON_METH_FASTCALL
  #if CYTHON_ASSUME_SAFE_SIZE
  __pyx_nargs = PyTuple_GET_SIZE(__pyx_args);
  #else
  __pyx_nargs = PyTuple_Size(__pyx_args); if (unlikely(__pyx_nargs < 0)) return NULL;
  #endif
  #endif
  __pyx_kwvalues = __Pyx_KwValues_FASTCALL(__pyx_args, __pyx_nargs);
  {
    PyObject ** const __pyx_pyargnames[] = {&__pyx_mstate_global->__pyx_n_u_item,&__pyx_mstate_global->__pyx_n_u_score,&__pyx_mstate_global->__pyx_n_u_heap,&__pyx_mstate_global->__pyx_n_u_active_scores,&__pyx_mstate_global->__pyx_n_u_k,&__pyx_mstate_global->__pyx_n_u_counter_state,0};
    const Py_ssize_t __pyx_kwds_len = (__pyx_kwds) ? __Pyx_NumKwargs_FASTCALL(__pyx_kwds) : 0;
    if (unlikely(__pyx_kwds_len) < 0) __PYX_ERR(0, 598, __pyx_L3_error)
    if (__pyx_kwds_len > 0) {
      switch (__pyx_nargs) {
        case  6:
        values[5] = __Pyx_ArgRef_FASTCALL(__pyx_args, 5);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[5])) __PYX_ERR(0, 598, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  5:
        values[4] = __Pyx_ArgRef_FASTCALL(__pyx_args, 4);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[4])) __PYX_ERR(0, 598, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  4:
        values[3] = __Pyx_ArgRef_FASTCALL(__pyx_args, 3);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[3])) __PYX_ERR(0, 598, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  3:
        values[2] = __Pyx_ArgRef_FASTCALL(__pyx_args, 2);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[2])) __PYX_ERR(0, 598, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  2:
        values[1] = __Pyx_ArgRef_FASTCALL(__pyx_args, 1);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[1])) __PYX_ERR(0, 598, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  1:
        values[0] = __Pyx_ArgRef_FASTCALL(__pyx_args, 0);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[0])) __PYX_ERR(0, 598, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  0: break;
        default: goto __pyx_L5_argtuple_error;
      }
      const Py_ssize_t kwd_pos_args = __pyx_nargs;
      if (__Pyx_ParseKeywords(__pyx_kwds, __pyx_kwvalues, __pyx_pyargnames, 0, values, kwd_pos_args, __pyx_kwds_len, "update_unique_topk_heap_fast", 0) < (0)) __PYX_ERR(0, 598, __pyx_L3_error)
      for (Py_ssize_t i = __pyx_nargs; i < 6; i++) {
        if (unlikely(!values[i])) { __Pyx_RaiseArgtupleInvalid("update_unique_topk_heap_fast", 1, 6, 6, i); __PYX_ERR(0, 598, __pyx_L3_error) }
      }
    } else if (unlikely(__pyx_nargs != 6)) {
      goto __pyx_L5_argtuple_error;
    } else {
      values[0] = __Pyx_ArgRef_FASTCALL(__pyx_args, 0);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[0])) __PYX_ERR(0, 598, __pyx_L3_error)
      values[1] = __Pyx_ArgRef_FASTCALL(__pyx_args, 1);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[1])) __PYX_ERR(0, 598, __pyx_L3_error)
      values[2] = __Pyx_ArgRef_FASTCALL(__pyx_args, 2);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[2])) __PYX_ERR(0, 598, __pyx_L3_error)
      values[3] = __Pyx_ArgRef_FASTCALL(__pyx_args, 3);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[3])) __PYX_ERR(0, 598, __pyx_L3_error)
      values[4] = __Pyx_ArgRef_FASTCALL(__pyx_args, 4);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[4])) __PYX_ERR(0, 598, __pyx_L3_error)
      values[5] = __Pyx_ArgRef_FASTCALL(__pyx_args, 5);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[5])) __PYX_ERR(0, 598, __pyx_L3_error)
    }
    __pyx_v_item = values[0];
    __pyx_v_score = __Pyx_PyFloat_AsDouble(values[1]); if (unlikely((__pyx_v_score == (double)-1) && PyErr_Occurred())) __PYX_ERR(0, 598, __pyx_L3_error)
    __pyx_v_heap = ((PyObject*)values[2]);
    __pyx_v_active_scores = ((PyObject*)values[3]);
    __pyx_v_k = __Pyx_PyLong_As_int(values[4]); if (unlikely((__pyx_v_k == (int)-1) && PyErr_Occurred())) __PYX_ERR(0, 598, __pyx_L3_error)
    __pyx_v_counter_state = ((PyObject*)values[5]);
  }
  goto __pyx_L6_skip;
  __pyx_L5_argtuple_error:;
  __Pyx_RaiseArgtupleInvalid("update_unique_topk_heap_fast", 1, 6, 6, __pyx_nargs); __PYX_ERR(0, 598, __pyx_L3_error)
  __pyx_L6_skip:;
  goto __pyx_L4_argument_unpacking_done;
  __pyx_L3_error:;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  __Pyx_AddTraceback("_fast_match.update_unique_topk_heap_fast", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __Pyx_RefNannyFinishContext();
  return NULL;
  __pyx_L4_argument_unpacking_done:;
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_heap), (&PyList_Type), 1, "heap", 1))) __PYX_ERR(0, 598, __pyx_L1_error)
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_active_scores), (&PyDict_Type), 1, "active_scores", 1))) __PYX_ERR(0, 598, __pyx_L1_error)
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_counter_state), (&PyList_Type), 1, "counter_state", 1))) __PYX_ERR(0, 598, __pyx_L1_error)
  __pyx_r = __pyx_pf_11_fast_match_18update_unique_topk_heap_fast(__pyx_self, __pyx_v_item, __pyx_v_score, __pyx_v_heap, __pyx_v_active_scores, __pyx_v_k, __pyx_v_counter_state);

  goto __pyx_L0;
  __pyx_L1_error:;
  __pyx_r = NULL;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  goto __pyx_L7_cleaned_up;
  __pyx_L0:;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  __pyx_L7_cleaned_up:;
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}

static PyObject *__pyx_pf_11_fast_match_18update_unique_topk_heap_fast(CYTHON_UNUSED PyObject *__pyx_self, PyObject *__pyx_v_item, double __pyx_v_score, PyObject *__pyx_v_heap, PyObject *__pyx_v_active_scores, int __pyx_v_k, PyObject *__pyx_v_counter_state) {
  PyObject *__pyx_v_current = 0;
  PyObject *__pyx_v_root_item = 0;
  PyObject *__pyx_v_active_item = 0;
  PyObject *__pyx_v_active_score = 0;
  double __pyx_v_root_score;
  int __pyx_v_next_counter;
  PyObject *__pyx_v_rebuilt = NULL;
  PyObject *__pyx_r = NULL;
  __Pyx_RefNannyDeclarations
  int __pyx_t_1;
  PyObject *__pyx_t_2 = NULL;
  PyObject *__pyx_t_3 = NULL;
  PyObject *__pyx_t_4 = NULL;
  Py_ssize_t __pyx_t_5;
  int __pyx_t_6;
  PyObject *__pyx_t_7 = NULL;
  PyObject *__pyx_t_8 = NULL;
  PyObject *__pyx_t_9 = NULL;
  PyObject *__pyx_t_10 = NULL;
  size_t __pyx_t_11;
  int __pyx_t_12;
  double __pyx_t_13;
  long __pyx_t_14;
  long __pyx_t_15;
  long __pyx_t_16;
  Py_ssize_t __pyx_t_17;
  int __pyx_t_18;
  Py_ssize_t __pyx_t_19;
  int __pyx_t_20;
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  __Pyx_RefNannySetupContext("update_unique_topk_heap_fast", 0);

  __pyx_t_1 = (__pyx_v_k < 1);
  if (__pyx_t_1) {

    __pyx_v_k = 1;

  }

  if (unlikely(__pyx_v_active_scores == Py_None)) {
    PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "get");
    __PYX_ERR(0, 607, __pyx_L1_error)
  }
  __pyx_t_2 = __Pyx_PyDict_GetItemDefault(__pyx_v_active_scores, __pyx_v_item, Py_None); if (unlikely(!__pyx_t_2)) __PYX_ERR(0, 607, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_2);
  __pyx_v_current = __pyx_t_2;
  __pyx_t_2 = 0;

  __pyx_t_1 = (__pyx_v_current != Py_None);
  if (__pyx_t_1) {

    __pyx_t_2 = PyFloat_FromDouble(__pyx_v_score); if (unlikely(!__pyx_t_2)) __PYX_ERR(0, 609, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_2);
    __pyx_t_3 = PyObject_RichCompare(__pyx_t_2, __pyx_v_current, Py_LE); __Pyx_XGOTREF(__pyx_t_3); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 609, __pyx_L1_error)
    __Pyx_DECREF(__pyx_t_2); __pyx_t_2 = 0;
    __pyx_t_1 = __Pyx_PyObject_IsTrue(__pyx_t_3); if (unlikely((__pyx_t_1 < 0))) __PYX_ERR(0, 609, __pyx_L1_error)
    __Pyx_DECREF(__pyx_t_3); __pyx_t_3 = 0;
    if (__pyx_t_1) {

      __Pyx_XDECREF(__pyx_r);
      __pyx_r = Py_None; __Pyx_INCREF(Py_None);
      goto __pyx_L0;

    }

    if (unlikely(__pyx_v_counter_state == Py_None)) {
      PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
      __PYX_ERR(0, 611, __pyx_L1_error)
    }
    __Pyx_INCREF(__pyx_v_counter_state);
    __pyx_t_4 = __pyx_v_counter_state;
    __pyx_t_5 = 0;
    if (unlikely(__pyx_t_4 == Py_None)) {
      PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
      __PYX_ERR(0, 611, __pyx_L1_error)
    }
    __pyx_t_3 = __Pyx_PyLong_AddObjC(__Pyx_PyList_GET_ITEM(__pyx_t_4, __pyx_t_5), __pyx_mstate_global->__pyx_int_1, 1, 1, 0); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 611, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_3);
    if (unlikely(__pyx_t_4 == Py_None)) {
      PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
      __PYX_ERR(0, 611, __pyx_L1_error)
    }
    if (unlikely((__Pyx_SetItemInt(__pyx_t_4, __pyx_t_5, __pyx_t_3, Py_ssize_t, 1, PyLong_FromSsize_t, 1, 0, 0, 1, __Pyx_ReferenceSharing_SharedReference) < 0))) __PYX_ERR(0, 611, __pyx_L1_error)
    __Pyx_DECREF(__pyx_t_3); __pyx_t_3 = 0;
    __Pyx_DECREF(__pyx_t_4); __pyx_t_4 = 0;

    if (unlikely(__pyx_v_counter_state == Py_None)) {
      PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
      __PYX_ERR(0, 612, __pyx_L1_error)
    }
    __pyx_t_6 = __Pyx_PyLong_As_int(__Pyx_PyList_GET_ITEM(__pyx_v_counter_state, 0)); if (unlikely((__pyx_t_6 == (int)-1) && PyErr_Occurred())) __PYX_ERR(0, 612, __pyx_L1_error)
    __pyx_v_next_counter = __pyx_t_6;

    __pyx_t_3 = PyFloat_FromDouble(__pyx_v_score); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 613, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_3);
    if (unlikely(__pyx_v_active_scores == Py_None)) {
      PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
      __PYX_ERR(0, 613, __pyx_L1_error)
    }
    if (unlikely((PyDict_SetItem(__pyx_v_active_scores, __pyx_v_item, __pyx_t_3) < 0))) __PYX_ERR(0, 613, __pyx_L1_error)
    __Pyx_DECREF(__pyx_t_3); __pyx_t_3 = 0;

    __pyx_t_2 = NULL;
    __Pyx_GetModuleGlobalName(__pyx_t_7, __pyx_mstate_global->__pyx_n_u_heapq); if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 614, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_7);
    __pyx_t_8 = __Pyx_PyObject_GetAttrStr(__pyx_t_7, __pyx_mstate_global->__pyx_n_u_heappush); if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 614, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_8);
    __Pyx_DECREF(__pyx_t_7); __pyx_t_7 = 0;
    __pyx_t_7 = PyFloat_FromDouble(__pyx_v_score); if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 614, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_7);
    __pyx_t_9 = __Pyx_PyLong_From_int(__pyx_v_next_counter); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 614, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_9);
    __pyx_t_10 = PyTuple_New(3); if (unlikely(!__pyx_t_10)) __PYX_ERR(0, 614, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_10);
    __Pyx_GIVEREF(__pyx_t_7);
    if (__Pyx_PyTuple_SET_ITEM(__pyx_t_10, 0, __pyx_t_7) != (0)) __PYX_ERR(0, 614, __pyx_L1_error);
    __Pyx_GIVEREF(__pyx_t_9);
    if (__Pyx_PyTuple_SET_ITEM(__pyx_t_10, 1, __pyx_t_9) != (0)) __PYX_ERR(0, 614, __pyx_L1_error);
    __Pyx_INCREF(__pyx_v_item);
    __Pyx_GIVEREF(__pyx_v_item);
    if (__Pyx_PyTuple_SET_ITEM(__pyx_t_10, 2, __pyx_v_item) != (0)) __PYX_ERR(0, 614, __pyx_L1_error);
    __pyx_t_7 = 0;
    __pyx_t_9 = 0;
    __pyx_t_11 = 1;
    #if CYTHON_UNPACK_METHODS
    if (unlikely(PyMethod_Check(__pyx_t_8))) {
      __pyx_t_2 = PyMethod_GET_SELF(__pyx_t_8);
      assert(__pyx_t_2);
      PyObject* __pyx__function = PyMethod_GET_FUNCTION(__pyx_t_8);
      __Pyx_INCREF(__pyx_t_2);
      __Pyx_INCREF(__pyx__function);
      __Pyx_DECREF_SET(__pyx_t_8, __pyx__function);
      __pyx_t_11 = 0;
    }
    #endif
    {
      PyObject *__pyx_callargs[3] = {__pyx_t_2, __pyx_v_heap, __pyx_t_10};
      __pyx_t_3 = __Pyx_PyObject_FastCall((PyObject*)__pyx_t_8, __pyx_callargs+__pyx_t_11, (3-__pyx_t_11) | (__pyx_t_11*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
      __Pyx_XDECREF(__pyx_t_2); __pyx_t_2 = 0;
      __Pyx_DECREF(__pyx_t_10); __pyx_t_10 = 0;
      __Pyx_DECREF(__pyx_t_8); __pyx_t_8 = 0;
      if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 614, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_3);
    }
    __Pyx_DECREF(__pyx_t_3); __pyx_t_3 = 0;

    goto __pyx_L4;
  }

  if (unlikely(__pyx_v_active_scores == Py_None)) {
    PyErr_SetString(PyExc_TypeError, "object of type 'NoneType' has no len()");
    __PYX_ERR(0, 615, __pyx_L1_error)
  }
  __pyx_t_5 = PyDict_Size(__pyx_v_active_scores); if (unlikely(__pyx_t_5 == ((Py_ssize_t)-1))) __PYX_ERR(0, 615, __pyx_L1_error)
  __pyx_t_1 = (__pyx_t_5 < __pyx_v_k);
  if (__pyx_t_1) {

    if (unlikely(__pyx_v_counter_state == Py_None)) {
      PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
      __PYX_ERR(0, 616, __pyx_L1_error)
    }
    __Pyx_INCREF(__pyx_v_counter_state);
    __pyx_t_4 = __pyx_v_counter_state;
    __pyx_t_5 = 0;
    if (unlikely(__pyx_t_4 == Py_None)) {
      PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
      __PYX_ERR(0, 616, __pyx_L1_error)
    }
    __pyx_t_3 = __Pyx_PyLong_AddObjC(__Pyx_PyList_GET_ITEM(__pyx_t_4, __pyx_t_5), __pyx_mstate_global->__pyx_int_1, 1, 1, 0); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 616, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_3);
    if (unlikely(__pyx_t_4 == Py_None)) {
      PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
      __PYX_ERR(0, 616, __pyx_L1_error)
    }
    if (unlikely((__Pyx_SetItemInt(__pyx_t_4, __pyx_t_5, __pyx_t_3, Py_ssize_t, 1, PyLong_FromSsize_t, 1, 0, 0, 1, __Pyx_ReferenceSharing_SharedReference) < 0))) __PYX_ERR(0, 616, __pyx_L1_error)
    __Pyx_DECREF(__pyx_t_3); __pyx_t_3 = 0;
    __Pyx_DECREF(__pyx_t_4); __pyx_t_4 = 0;

    if (unlikely(__pyx_v_counter_state == Py_None)) {
      PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
      __PYX_ERR(0, 617, __pyx_L1_error)
    }
    __pyx_t_6 = __Pyx_PyLong_As_int(__Pyx_PyList_GET_ITEM(__pyx_v_counter_state, 0)); if (unlikely((__pyx_t_6 == (int)-1) && PyErr_Occurred())) __PYX_ERR(0, 617, __pyx_L1_error)
    __pyx_v_next_counter = __pyx_t_6;

    __pyx_t_3 = PyFloat_FromDouble(__pyx_v_score); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 618, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_3);
    if (unlikely(__pyx_v_active_scores == Py_None)) {
      PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
      __PYX_ERR(0, 618, __pyx_L1_error)
    }
    if (unlikely((PyDict_SetItem(__pyx_v_active_scores, __pyx_v_item, __pyx_t_3) < 0))) __PYX_ERR(0, 618, __pyx_L1_error)
    __Pyx_DECREF(__pyx_t_3); __pyx_t_3 = 0;

    __pyx_t_8 = NULL;
    __Pyx_GetModuleGlobalName(__pyx_t_10, __pyx_mstate_global->__pyx_n_u_heapq); if (unlikely(!__pyx_t_10)) __PYX_ERR(0, 619, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_10);
    __pyx_t_2 = __Pyx_PyObject_GetAttrStr(__pyx_t_10, __pyx_mstate_global->__pyx_n_u_heappush); if (unlikely(!__pyx_t_2)) __PYX_ERR(0, 619, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_2);
    __Pyx_DECREF(__pyx_t_10); __pyx_t_10 = 0;
    __pyx_t_10 = PyFloat_FromDouble(__pyx_v_score); if (unlikely(!__pyx_t_10)) __PYX_ERR(0, 619, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_10);
    __pyx_t_9 = __Pyx_PyLong_From_int(__pyx_v_next_counter); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 619, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_9);
    __pyx_t_7 = PyTuple_New(3); if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 619, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_7);
    __Pyx_GIVEREF(__pyx_t_10);
    if (__Pyx_PyTuple_SET_ITEM(__pyx_t_7, 0, __pyx_t_10) != (0)) __PYX_ERR(0, 619, __pyx_L1_error);
    __Pyx_GIVEREF(__pyx_t_9);
    if (__Pyx_PyTuple_SET_ITEM(__pyx_t_7, 1, __pyx_t_9) != (0)) __PYX_ERR(0, 619, __pyx_L1_error);
    __Pyx_INCREF(__pyx_v_item);
    __Pyx_GIVEREF(__pyx_v_item);
    if (__Pyx_PyTuple_SET_ITEM(__pyx_t_7, 2, __pyx_v_item) != (0)) __PYX_ERR(0, 619, __pyx_L1_error);
    __pyx_t_10 = 0;
    __pyx_t_9 = 0;
    __pyx_t_11 = 1;
    #if CYTHON_UNPACK_METHODS
    if (unlikely(PyMethod_Check(__pyx_t_2))) {
      __pyx_t_8 = PyMethod_GET_SELF(__pyx_t_2);
      assert(__pyx_t_8);
      PyObject* __pyx__function = PyMethod_GET_FUNCTION(__pyx_t_2);
      __Pyx_INCREF(__pyx_t_8);
      __Pyx_INCREF(__pyx__function);
      __Pyx_DECREF_SET(__pyx_t_2, __pyx__function);
      __pyx_t_11 = 0;
    }
    #endif
    {
      PyObject *__pyx_callargs[3] = {__pyx_t_8, __pyx_v_heap, __pyx_t_7};
      __pyx_t_3 = __Pyx_PyObject_FastCall((PyObject*)__pyx_t_2, __pyx_callargs+__pyx_t_11, (3-__pyx_t_11) | (__pyx_t_11*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
      __Pyx_XDECREF(__pyx_t_8); __pyx_t_8 = 0;
      __Pyx_DECREF(__pyx_t_7); __pyx_t_7 = 0;
      __Pyx_DECREF(__pyx_t_2); __pyx_t_2 = 0;
      if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 619, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_3);
    }
    __Pyx_DECREF(__pyx_t_3); __pyx_t_3 = 0;

    goto __pyx_L4;
  }

   {
    __pyx_t_2 = NULL;
    __Pyx_GetModuleGlobalName(__pyx_t_7, __pyx_mstate_global->__pyx_n_u_cleanup_lazy_heap_fast); if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 621, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_7);
    __pyx_t_11 = 1;
    #if CYTHON_UNPACK_METHODS
    if (unlikely(PyMethod_Check(__pyx_t_7))) {
      __pyx_t_2 = PyMethod_GET_SELF(__pyx_t_7);
      assert(__pyx_t_2);
      PyObject* __pyx__function = PyMethod_GET_FUNCTION(__pyx_t_7);
      __Pyx_INCREF(__pyx_t_2);
      __Pyx_INCREF(__pyx__function);
      __Pyx_DECREF_SET(__pyx_t_7, __pyx__function);
      __pyx_t_11 = 0;
    }
    #endif
    {
      PyObject *__pyx_callargs[3] = {__pyx_t_2, __pyx_v_heap, __pyx_v_active_scores};
      __pyx_t_3 = __Pyx_PyObject_FastCall((PyObject*)__pyx_t_7, __pyx_callargs+__pyx_t_11, (3-__pyx_t_11) | (__pyx_t_11*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
      __Pyx_XDECREF(__pyx_t_2); __pyx_t_2 = 0;
      __Pyx_DECREF(__pyx_t_7); __pyx_t_7 = 0;
      if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 621, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_3);
    }
    __Pyx_DECREF(__pyx_t_3); __pyx_t_3 = 0;

    if (__pyx_v_heap == Py_None) __pyx_t_1 = 0;
    else
    {
      Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_v_heap);
      if (unlikely(((!CYTHON_ASSUME_SAFE_SIZE) && __pyx_temp < 0))) __PYX_ERR(0, 622, __pyx_L1_error)
      __pyx_t_1 = (__pyx_temp != 0);
    }

    __pyx_t_12 = (!__pyx_t_1);
    if (__pyx_t_12) {

      if (unlikely(__pyx_v_counter_state == Py_None)) {
        PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
        __PYX_ERR(0, 623, __pyx_L1_error)
      }
      __Pyx_INCREF(__pyx_v_counter_state);
      __pyx_t_4 = __pyx_v_counter_state;
      __pyx_t_5 = 0;
      if (unlikely(__pyx_t_4 == Py_None)) {
        PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
        __PYX_ERR(0, 623, __pyx_L1_error)
      }
      __pyx_t_3 = __Pyx_PyLong_AddObjC(__Pyx_PyList_GET_ITEM(__pyx_t_4, __pyx_t_5), __pyx_mstate_global->__pyx_int_1, 1, 1, 0); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 623, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_3);
      if (unlikely(__pyx_t_4 == Py_None)) {
        PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
        __PYX_ERR(0, 623, __pyx_L1_error)
      }
      if (unlikely((__Pyx_SetItemInt(__pyx_t_4, __pyx_t_5, __pyx_t_3, Py_ssize_t, 1, PyLong_FromSsize_t, 1, 0, 0, 1, __Pyx_ReferenceSharing_SharedReference) < 0))) __PYX_ERR(0, 623, __pyx_L1_error)
      __Pyx_DECREF(__pyx_t_3); __pyx_t_3 = 0;
      __Pyx_DECREF(__pyx_t_4); __pyx_t_4 = 0;

      if (unlikely(__pyx_v_counter_state == Py_None)) {
        PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
        __PYX_ERR(0, 624, __pyx_L1_error)
      }
      __pyx_t_6 = __Pyx_PyLong_As_int(__Pyx_PyList_GET_ITEM(__pyx_v_counter_state, 0)); if (unlikely((__pyx_t_6 == (int)-1) && PyErr_Occurred())) __PYX_ERR(0, 624, __pyx_L1_error)
      __pyx_v_next_counter = __pyx_t_6;

      __pyx_t_3 = PyFloat_FromDouble(__pyx_v_score); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 625, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_3);
      if (unlikely(__pyx_v_active_scores == Py_None)) {
        PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
        __PYX_ERR(0, 625, __pyx_L1_error)
      }
      if (unlikely((PyDict_SetItem(__pyx_v_active_scores, __pyx_v_item, __pyx_t_3) < 0))) __PYX_ERR(0, 625, __pyx_L1_error)
      __Pyx_DECREF(__pyx_t_3); __pyx_t_3 = 0;

      __pyx_t_7 = NULL;
      __Pyx_GetModuleGlobalName(__pyx_t_2, __pyx_mstate_global->__pyx_n_u_heapq); if (unlikely(!__pyx_t_2)) __PYX_ERR(0, 626, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_2);
      __pyx_t_8 = __Pyx_PyObject_GetAttrStr(__pyx_t_2, __pyx_mstate_global->__pyx_n_u_heappush); if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 626, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_8);
      __Pyx_DECREF(__pyx_t_2); __pyx_t_2 = 0;
      __pyx_t_2 = PyFloat_FromDouble(__pyx_v_score); if (unlikely(!__pyx_t_2)) __PYX_ERR(0, 626, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_2);
      __pyx_t_9 = __Pyx_PyLong_From_int(__pyx_v_next_counter); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 626, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_9);
      __pyx_t_10 = PyTuple_New(3); if (unlikely(!__pyx_t_10)) __PYX_ERR(0, 626, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_10);
      __Pyx_GIVEREF(__pyx_t_2);
      if (__Pyx_PyTuple_SET_ITEM(__pyx_t_10, 0, __pyx_t_2) != (0)) __PYX_ERR(0, 626, __pyx_L1_error);
      __Pyx_GIVEREF(__pyx_t_9);
      if (__Pyx_PyTuple_SET_ITEM(__pyx_t_10, 1, __pyx_t_9) != (0)) __PYX_ERR(0, 626, __pyx_L1_error);
      __Pyx_INCREF(__pyx_v_item);
      __Pyx_GIVEREF(__pyx_v_item);
      if (__Pyx_PyTuple_SET_ITEM(__pyx_t_10, 2, __pyx_v_item) != (0)) __PYX_ERR(0, 626, __pyx_L1_error);
      __pyx_t_2 = 0;
      __pyx_t_9 = 0;
      __pyx_t_11 = 1;
      #if CYTHON_UNPACK_METHODS
      if (unlikely(PyMethod_Check(__pyx_t_8))) {
        __pyx_t_7 = PyMethod_GET_SELF(__pyx_t_8);
        assert(__pyx_t_7);
        PyObject* __pyx__function = PyMethod_GET_FUNCTION(__pyx_t_8);
        __Pyx_INCREF(__pyx_t_7);
        __Pyx_INCREF(__pyx__function);
        __Pyx_DECREF_SET(__pyx_t_8, __pyx__function);
        __pyx_t_11 = 0;
      }
      #endif
      {
        PyObject *__pyx_callargs[3] = {__pyx_t_7, __pyx_v_heap, __pyx_t_10};
        __pyx_t_3 = __Pyx_PyObject_FastCall((PyObject*)__pyx_t_8, __pyx_callargs+__pyx_t_11, (3-__pyx_t_11) | (__pyx_t_11*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
        __Pyx_XDECREF(__pyx_t_7); __pyx_t_7 = 0;
        __Pyx_DECREF(__pyx_t_10); __pyx_t_10 = 0;
        __Pyx_DECREF(__pyx_t_8); __pyx_t_8 = 0;
        if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 626, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_3);
      }
      __Pyx_DECREF(__pyx_t_3); __pyx_t_3 = 0;

      goto __pyx_L6;
    }

     {
      if (unlikely(__pyx_v_heap == Py_None)) {
        PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
        __PYX_ERR(0, 628, __pyx_L1_error)
      }
      __pyx_t_3 = __Pyx_GetItemInt(__Pyx_PyList_GET_ITEM(__pyx_v_heap, 0), 0, long, 1, __Pyx_PyLong_From_long, 0, 0, 0, 1, __Pyx_ReferenceSharing_SharedReference); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 628, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_3);
      __pyx_t_13 = __Pyx_PyFloat_AsDouble(__pyx_t_3); if (unlikely((__pyx_t_13 == (double)-1) && PyErr_Occurred())) __PYX_ERR(0, 628, __pyx_L1_error)
      __Pyx_DECREF(__pyx_t_3); __pyx_t_3 = 0;
      __pyx_v_root_score = __pyx_t_13;

      if (unlikely(__pyx_v_heap == Py_None)) {
        PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
        __PYX_ERR(0, 629, __pyx_L1_error)
      }
      __pyx_t_3 = __Pyx_GetItemInt(__Pyx_PyList_GET_ITEM(__pyx_v_heap, 0), 2, long, 1, __Pyx_PyLong_From_long, 0, 0, 0, 1, __Pyx_ReferenceSharing_SharedReference); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 629, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_3);
      __pyx_v_root_item = __pyx_t_3;
      __pyx_t_3 = 0;

      __pyx_t_12 = (__pyx_v_score <= __pyx_v_root_score);
      if (__pyx_t_12) {

        __Pyx_XDECREF(__pyx_r);
        __pyx_r = Py_None; __Pyx_INCREF(Py_None);
        goto __pyx_L0;

      }

      __pyx_t_8 = NULL;
      __Pyx_GetModuleGlobalName(__pyx_t_10, __pyx_mstate_global->__pyx_n_u_heapq); if (unlikely(!__pyx_t_10)) __PYX_ERR(0, 632, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_10);
      __pyx_t_7 = __Pyx_PyObject_GetAttrStr(__pyx_t_10, __pyx_mstate_global->__pyx_n_u_heappop); if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 632, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_7);
      __Pyx_DECREF(__pyx_t_10); __pyx_t_10 = 0;
      __pyx_t_11 = 1;
      #if CYTHON_UNPACK_METHODS
      if (unlikely(PyMethod_Check(__pyx_t_7))) {
        __pyx_t_8 = PyMethod_GET_SELF(__pyx_t_7);
        assert(__pyx_t_8);
        PyObject* __pyx__function = PyMethod_GET_FUNCTION(__pyx_t_7);
        __Pyx_INCREF(__pyx_t_8);
        __Pyx_INCREF(__pyx__function);
        __Pyx_DECREF_SET(__pyx_t_7, __pyx__function);
        __pyx_t_11 = 0;
      }
      #endif
      {
        PyObject *__pyx_callargs[2] = {__pyx_t_8, __pyx_v_heap};
        __pyx_t_3 = __Pyx_PyObject_FastCall((PyObject*)__pyx_t_7, __pyx_callargs+__pyx_t_11, (2-__pyx_t_11) | (__pyx_t_11*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
        __Pyx_XDECREF(__pyx_t_8); __pyx_t_8 = 0;
        __Pyx_DECREF(__pyx_t_7); __pyx_t_7 = 0;
        if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 632, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_3);
      }
      __Pyx_DECREF(__pyx_t_3); __pyx_t_3 = 0;

      if (unlikely(__pyx_v_active_scores == Py_None)) {
        PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "pop");
        __PYX_ERR(0, 633, __pyx_L1_error)
      }
      __pyx_t_6 = __Pyx_PyDict_Pop_ignore(__pyx_v_active_scores, __pyx_v_root_item, Py_None); if (unlikely(__pyx_t_6 == ((int)-1))) __PYX_ERR(0, 633, __pyx_L1_error)

      if (unlikely(__pyx_v_counter_state == Py_None)) {
        PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
        __PYX_ERR(0, 634, __pyx_L1_error)
      }
      __Pyx_INCREF(__pyx_v_counter_state);
      __pyx_t_4 = __pyx_v_counter_state;
      __pyx_t_5 = 0;
      if (unlikely(__pyx_t_4 == Py_None)) {
        PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
        __PYX_ERR(0, 634, __pyx_L1_error)
      }
      __pyx_t_3 = __Pyx_PyLong_AddObjC(__Pyx_PyList_GET_ITEM(__pyx_t_4, __pyx_t_5), __pyx_mstate_global->__pyx_int_1, 1, 1, 0); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 634, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_3);
      if (unlikely(__pyx_t_4 == Py_None)) {
        PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
        __PYX_ERR(0, 634, __pyx_L1_error)
      }
      if (unlikely((__Pyx_SetItemInt(__pyx_t_4, __pyx_t_5, __pyx_t_3, Py_ssize_t, 1, PyLong_FromSsize_t, 1, 0, 0, 1, __Pyx_ReferenceSharing_SharedReference) < 0))) __PYX_ERR(0, 634, __pyx_L1_error)
      __Pyx_DECREF(__pyx_t_3); __pyx_t_3 = 0;
      __Pyx_DECREF(__pyx_t_4); __pyx_t_4 = 0;

      if (unlikely(__pyx_v_counter_state == Py_None)) {
        PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
        __PYX_ERR(0, 635, __pyx_L1_error)
      }
      __pyx_t_6 = __Pyx_PyLong_As_int(__Pyx_PyList_GET_ITEM(__pyx_v_counter_state, 0)); if (unlikely((__pyx_t_6 == (int)-1) && PyErr_Occurred())) __PYX_ERR(0, 635, __pyx_L1_error)
      __pyx_v_next_counter = __pyx_t_6;

      __pyx_t_3 = PyFloat_FromDouble(__pyx_v_score); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 636, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_3);
      if (unlikely(__pyx_v_active_scores == Py_None)) {
        PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
        __PYX_ERR(0, 636, __pyx_L1_error)
      }
      if (unlikely((PyDict_SetItem(__pyx_v_active_scores, __pyx_v_item, __pyx_t_3) < 0))) __PYX_ERR(0, 636, __pyx_L1_error)
      __Pyx_DECREF(__pyx_t_3); __pyx_t_3 = 0;

      __pyx_t_7 = NULL;
      __Pyx_GetModuleGlobalName(__pyx_t_8, __pyx_mstate_global->__pyx_n_u_heapq); if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 637, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_8);
      __pyx_t_10 = __Pyx_PyObject_GetAttrStr(__pyx_t_8, __pyx_mstate_global->__pyx_n_u_heappush); if (unlikely(!__pyx_t_10)) __PYX_ERR(0, 637, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_10);
      __Pyx_DECREF(__pyx_t_8); __pyx_t_8 = 0;
      __pyx_t_8 = PyFloat_FromDouble(__pyx_v_score); if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 637, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_8);
      __pyx_t_9 = __Pyx_PyLong_From_int(__pyx_v_next_counter); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 637, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_9);
      __pyx_t_2 = PyTuple_New(3); if (unlikely(!__pyx_t_2)) __PYX_ERR(0, 637, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_2);
      __Pyx_GIVEREF(__pyx_t_8);
      if (__Pyx_PyTuple_SET_ITEM(__pyx_t_2, 0, __pyx_t_8) != (0)) __PYX_ERR(0, 637, __pyx_L1_error);
      __Pyx_GIVEREF(__pyx_t_9);
      if (__Pyx_PyTuple_SET_ITEM(__pyx_t_2, 1, __pyx_t_9) != (0)) __PYX_ERR(0, 637, __pyx_L1_error);
      __Pyx_INCREF(__pyx_v_item);
      __Pyx_GIVEREF(__pyx_v_item);
      if (__Pyx_PyTuple_SET_ITEM(__pyx_t_2, 2, __pyx_v_item) != (0)) __PYX_ERR(0, 637, __pyx_L1_error);
      __pyx_t_8 = 0;
      __pyx_t_9 = 0;
      __pyx_t_11 = 1;
      #if CYTHON_UNPACK_METHODS
      if (unlikely(PyMethod_Check(__pyx_t_10))) {
        __pyx_t_7 = PyMethod_GET_SELF(__pyx_t_10);
        assert(__pyx_t_7);
        PyObject* __pyx__function = PyMethod_GET_FUNCTION(__pyx_t_10);
        __Pyx_INCREF(__pyx_t_7);
        __Pyx_INCREF(__pyx__function);
        __Pyx_DECREF_SET(__pyx_t_10, __pyx__function);
        __pyx_t_11 = 0;
      }
      #endif
      {
        PyObject *__pyx_callargs[3] = {__pyx_t_7, __pyx_v_heap, __pyx_t_2};
        __pyx_t_3 = __Pyx_PyObject_FastCall((PyObject*)__pyx_t_10, __pyx_callargs+__pyx_t_11, (3-__pyx_t_11) | (__pyx_t_11*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
        __Pyx_XDECREF(__pyx_t_7); __pyx_t_7 = 0;
        __Pyx_DECREF(__pyx_t_2); __pyx_t_2 = 0;
        __Pyx_DECREF(__pyx_t_10); __pyx_t_10 = 0;
        if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 637, __pyx_L1_error)
        __Pyx_GOTREF(__pyx_t_3);
      }
      __Pyx_DECREF(__pyx_t_3); __pyx_t_3 = 0;
    }
    __pyx_L6:;
  }
  __pyx_L4:;

  if (unlikely(__pyx_v_heap == Py_None)) {
    PyErr_SetString(PyExc_TypeError, "object of type 'NoneType' has no len()");
    __PYX_ERR(0, 639, __pyx_L1_error)
  }
  __pyx_t_5 = __Pyx_PyList_GET_SIZE(__pyx_v_heap); if (unlikely(__pyx_t_5 == ((Py_ssize_t)-1))) __PYX_ERR(0, 639, __pyx_L1_error)
  __pyx_t_14 = (4 * __pyx_v_k);
  __pyx_t_15 = 32;
  __pyx_t_12 = (__pyx_t_14 > __pyx_t_15);
  if (__pyx_t_12) {
    __pyx_t_16 = __pyx_t_14;
  } else {
    __pyx_t_16 = __pyx_t_15;
  }
  __pyx_t_12 = (__pyx_t_5 > __pyx_t_16);
  if (__pyx_t_12) {

    __pyx_t_3 = PyList_New(0); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 640, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_3);
    __pyx_v_rebuilt = ((PyObject*)__pyx_t_3);
    __pyx_t_3 = 0;

    __pyx_t_5 = 0;
    if (unlikely(__pyx_v_active_scores == Py_None)) {
      PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "items");
      __PYX_ERR(0, 641, __pyx_L1_error)
    }
    __pyx_t_10 = __Pyx_dict_iterator(__pyx_v_active_scores, 1, __pyx_mstate_global->__pyx_n_u_items, (&__pyx_t_17), (&__pyx_t_6)); if (unlikely(!__pyx_t_10)) __PYX_ERR(0, 641, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_10);
    __Pyx_XDECREF(__pyx_t_3);
    __pyx_t_3 = __pyx_t_10;
    __pyx_t_10 = 0;
    while (1) {
      __pyx_t_18 = __Pyx_dict_iter_next(__pyx_t_3, __pyx_t_17, &__pyx_t_5, &__pyx_t_10, &__pyx_t_2, NULL, __pyx_t_6);
      if (unlikely(__pyx_t_18 == 0)) break;
      if (unlikely(__pyx_t_18 == -1)) __PYX_ERR(0, 641, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_10);
      __Pyx_GOTREF(__pyx_t_2);
      __Pyx_XDECREF_SET(__pyx_v_active_item, __pyx_t_10);
      __pyx_t_10 = 0;
      __Pyx_XDECREF_SET(__pyx_v_active_score, __pyx_t_2);
      __pyx_t_2 = 0;

      if (unlikely(__pyx_v_counter_state == Py_None)) {
        PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
        __PYX_ERR(0, 642, __pyx_L1_error)
      }
      __Pyx_INCREF(__pyx_v_counter_state);
      __pyx_t_4 = __pyx_v_counter_state;
      __pyx_t_19 = 0;
      if (unlikely(__pyx_t_4 == Py_None)) {
        PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
        __PYX_ERR(0, 642, __pyx_L1_error)
      }
      __pyx_t_2 = __Pyx_PyLong_AddObjC(__Pyx_PyList_GET_ITEM(__pyx_t_4, __pyx_t_19), __pyx_mstate_global->__pyx_int_1, 1, 1, 0); if (unlikely(!__pyx_t_2)) __PYX_ERR(0, 642, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_2);
      if (unlikely(__pyx_t_4 == Py_None)) {
        PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
        __PYX_ERR(0, 642, __pyx_L1_error)
      }
      if (unlikely((__Pyx_SetItemInt(__pyx_t_4, __pyx_t_19, __pyx_t_2, Py_ssize_t, 1, PyLong_FromSsize_t, 1, 0, 0, 1, __Pyx_ReferenceSharing_SharedReference) < 0))) __PYX_ERR(0, 642, __pyx_L1_error)
      __Pyx_DECREF(__pyx_t_2); __pyx_t_2 = 0;
      __Pyx_DECREF(__pyx_t_4); __pyx_t_4 = 0;

      if (unlikely(__pyx_v_counter_state == Py_None)) {
        PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
        __PYX_ERR(0, 643, __pyx_L1_error)
      }
      __pyx_t_2 = PyTuple_New(3); if (unlikely(!__pyx_t_2)) __PYX_ERR(0, 643, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_2);
      __Pyx_INCREF(__pyx_v_active_score);
      __Pyx_GIVEREF(__pyx_v_active_score);
      if (__Pyx_PyTuple_SET_ITEM(__pyx_t_2, 0, __pyx_v_active_score) != (0)) __PYX_ERR(0, 643, __pyx_L1_error);
      __Pyx_INCREF(__Pyx_PyList_GET_ITEM(__pyx_v_counter_state, 0));
      __Pyx_GIVEREF(__Pyx_PyList_GET_ITEM(__pyx_v_counter_state, 0));
      if (__Pyx_PyTuple_SET_ITEM(__pyx_t_2, 1, __Pyx_PyList_GET_ITEM(__pyx_v_counter_state, 0)) != (0)) __PYX_ERR(0, 643, __pyx_L1_error);
      __Pyx_INCREF(__pyx_v_active_item);
      __Pyx_GIVEREF(__pyx_v_active_item);
      if (__Pyx_PyTuple_SET_ITEM(__pyx_t_2, 2, __pyx_v_active_item) != (0)) __PYX_ERR(0, 643, __pyx_L1_error);
      __pyx_t_20 = __Pyx_PyList_Append(__pyx_v_rebuilt, __pyx_t_2); if (unlikely(__pyx_t_20 == ((int)-1))) __PYX_ERR(0, 643, __pyx_L1_error)
      __Pyx_DECREF(__pyx_t_2); __pyx_t_2 = 0;
    }
    __Pyx_DECREF(__pyx_t_3); __pyx_t_3 = 0;

    if (unlikely(__pyx_v_heap == Py_None)) {
      PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
      __PYX_ERR(0, 644, __pyx_L1_error)
    }
    if (__Pyx_PyObject_SetSlice(__pyx_v_heap, __pyx_v_rebuilt, 0, 0, NULL, NULL, NULL, 0, 0, 0) < (0)) __PYX_ERR(0, 644, __pyx_L1_error)

    __pyx_t_2 = NULL;
    __Pyx_GetModuleGlobalName(__pyx_t_10, __pyx_mstate_global->__pyx_n_u_heapq); if (unlikely(!__pyx_t_10)) __PYX_ERR(0, 645, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_10);
    __pyx_t_7 = __Pyx_PyObject_GetAttrStr(__pyx_t_10, __pyx_mstate_global->__pyx_n_u_heapify); if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 645, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_7);
    __Pyx_DECREF(__pyx_t_10); __pyx_t_10 = 0;
    __pyx_t_11 = 1;
    #if CYTHON_UNPACK_METHODS
    if (unlikely(PyMethod_Check(__pyx_t_7))) {
      __pyx_t_2 = PyMethod_GET_SELF(__pyx_t_7);
      assert(__pyx_t_2);
      PyObject* __pyx__function = PyMethod_GET_FUNCTION(__pyx_t_7);
      __Pyx_INCREF(__pyx_t_2);
      __Pyx_INCREF(__pyx__function);
      __Pyx_DECREF_SET(__pyx_t_7, __pyx__function);
      __pyx_t_11 = 0;
    }
    #endif
    {
      PyObject *__pyx_callargs[2] = {__pyx_t_2, __pyx_v_heap};
      __pyx_t_3 = __Pyx_PyObject_FastCall((PyObject*)__pyx_t_7, __pyx_callargs+__pyx_t_11, (2-__pyx_t_11) | (__pyx_t_11*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
      __Pyx_XDECREF(__pyx_t_2); __pyx_t_2 = 0;
      __Pyx_DECREF(__pyx_t_7); __pyx_t_7 = 0;
      if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 645, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_3);
    }
    __Pyx_DECREF(__pyx_t_3); __pyx_t_3 = 0;

  }

  __pyx_r = Py_None; __Pyx_INCREF(Py_None);
  goto __pyx_L0;
  __pyx_L1_error:;
  __Pyx_XDECREF(__pyx_t_2);
  __Pyx_XDECREF(__pyx_t_3);
  __Pyx_XDECREF(__pyx_t_4);
  __Pyx_XDECREF(__pyx_t_7);
  __Pyx_XDECREF(__pyx_t_8);
  __Pyx_XDECREF(__pyx_t_9);
  __Pyx_XDECREF(__pyx_t_10);
  __Pyx_AddTraceback("_fast_match.update_unique_topk_heap_fast", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __pyx_r = NULL;
  __pyx_L0:;
  __Pyx_XDECREF(__pyx_v_current);
  __Pyx_XDECREF(__pyx_v_root_item);
  __Pyx_XDECREF(__pyx_v_active_item);
  __Pyx_XDECREF(__pyx_v_active_score);
  __Pyx_XDECREF(__pyx_v_rebuilt);
  __Pyx_XGIVEREF(__pyx_r);
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}

static PyObject *__pyx_pw_11_fast_match_21update_heap_and_tau_fast(PyObject *__pyx_self,
#if CYTHON_METH_FASTCALL
PyObject *const *__pyx_args, Py_ssize_t __pyx_nargs, PyObject *__pyx_kwds
#else
PyObject *__pyx_args, PyObject *__pyx_kwds
#endif
);
PyDoc_STRVAR(__pyx_doc_11_fast_match_20update_heap_and_tau_fast, "Update one per-anchor heap and refresh tau_dict[anchor].");
static PyMethodDef __pyx_mdef_11_fast_match_21update_heap_and_tau_fast = {"update_heap_and_tau_fast", (PyCFunction)(void(*)(void))(__Pyx_PyCFunction_FastCallWithKeywords)__pyx_pw_11_fast_match_21update_heap_and_tau_fast, __Pyx_METH_FASTCALL|METH_KEYWORDS, __pyx_doc_11_fast_match_20update_heap_and_tau_fast};
static PyObject *__pyx_pw_11_fast_match_21update_heap_and_tau_fast(PyObject *__pyx_self,
#if CYTHON_METH_FASTCALL
PyObject *const *__pyx_args, Py_ssize_t __pyx_nargs, PyObject *__pyx_kwds
#else
PyObject *__pyx_args, PyObject *__pyx_kwds
#endif
) {
  PyObject *__pyx_v_anchor = 0;
  PyObject *__pyx_v_item = 0;
  double __pyx_v_score;
  PyObject *__pyx_v_heap = 0;
  PyObject *__pyx_v_tau_dict = 0;
  int __pyx_v_k;
  PyObject *__pyx_v_active_scores = 0;
  PyObject *__pyx_v_counter_state = 0;
  #if !CYTHON_METH_FASTCALL
  CYTHON_UNUSED Py_ssize_t __pyx_nargs;
  #endif
  CYTHON_UNUSED PyObject *const *__pyx_kwvalues;
  PyObject* values[8] = {0,0,0,0,0,0,0,0};
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  PyObject *__pyx_r = 0;
  __Pyx_RefNannyDeclarations
  __Pyx_RefNannySetupContext("update_heap_and_tau_fast (wrapper)", 0);
  #if !CYTHON_METH_FASTCALL
  #if CYTHON_ASSUME_SAFE_SIZE
  __pyx_nargs = PyTuple_GET_SIZE(__pyx_args);
  #else
  __pyx_nargs = PyTuple_Size(__pyx_args); if (unlikely(__pyx_nargs < 0)) return NULL;
  #endif
  #endif
  __pyx_kwvalues = __Pyx_KwValues_FASTCALL(__pyx_args, __pyx_nargs);
  {
    PyObject ** const __pyx_pyargnames[] = {&__pyx_mstate_global->__pyx_n_u_anchor,&__pyx_mstate_global->__pyx_n_u_item,&__pyx_mstate_global->__pyx_n_u_score,&__pyx_mstate_global->__pyx_n_u_heap,&__pyx_mstate_global->__pyx_n_u_tau_dict,&__pyx_mstate_global->__pyx_n_u_k,&__pyx_mstate_global->__pyx_n_u_active_scores,&__pyx_mstate_global->__pyx_n_u_counter_state,0};
    const Py_ssize_t __pyx_kwds_len = (__pyx_kwds) ? __Pyx_NumKwargs_FASTCALL(__pyx_kwds) : 0;
    if (unlikely(__pyx_kwds_len) < 0) __PYX_ERR(0, 648, __pyx_L3_error)
    if (__pyx_kwds_len > 0) {
      switch (__pyx_nargs) {
        case  8:
        values[7] = __Pyx_ArgRef_FASTCALL(__pyx_args, 7);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[7])) __PYX_ERR(0, 648, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  7:
        values[6] = __Pyx_ArgRef_FASTCALL(__pyx_args, 6);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[6])) __PYX_ERR(0, 648, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  6:
        values[5] = __Pyx_ArgRef_FASTCALL(__pyx_args, 5);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[5])) __PYX_ERR(0, 648, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  5:
        values[4] = __Pyx_ArgRef_FASTCALL(__pyx_args, 4);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[4])) __PYX_ERR(0, 648, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  4:
        values[3] = __Pyx_ArgRef_FASTCALL(__pyx_args, 3);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[3])) __PYX_ERR(0, 648, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  3:
        values[2] = __Pyx_ArgRef_FASTCALL(__pyx_args, 2);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[2])) __PYX_ERR(0, 648, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  2:
        values[1] = __Pyx_ArgRef_FASTCALL(__pyx_args, 1);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[1])) __PYX_ERR(0, 648, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  1:
        values[0] = __Pyx_ArgRef_FASTCALL(__pyx_args, 0);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[0])) __PYX_ERR(0, 648, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  0: break;
        default: goto __pyx_L5_argtuple_error;
      }
      const Py_ssize_t kwd_pos_args = __pyx_nargs;
      if (__Pyx_ParseKeywords(__pyx_kwds, __pyx_kwvalues, __pyx_pyargnames, 0, values, kwd_pos_args, __pyx_kwds_len, "update_heap_and_tau_fast", 0) < (0)) __PYX_ERR(0, 648, __pyx_L3_error)
      for (Py_ssize_t i = __pyx_nargs; i < 8; i++) {
        if (unlikely(!values[i])) { __Pyx_RaiseArgtupleInvalid("update_heap_and_tau_fast", 1, 8, 8, i); __PYX_ERR(0, 648, __pyx_L3_error) }
      }
    } else if (unlikely(__pyx_nargs != 8)) {
      goto __pyx_L5_argtuple_error;
    } else {
      values[0] = __Pyx_ArgRef_FASTCALL(__pyx_args, 0);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[0])) __PYX_ERR(0, 648, __pyx_L3_error)
      values[1] = __Pyx_ArgRef_FASTCALL(__pyx_args, 1);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[1])) __PYX_ERR(0, 648, __pyx_L3_error)
      values[2] = __Pyx_ArgRef_FASTCALL(__pyx_args, 2);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[2])) __PYX_ERR(0, 648, __pyx_L3_error)
      values[3] = __Pyx_ArgRef_FASTCALL(__pyx_args, 3);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[3])) __PYX_ERR(0, 648, __pyx_L3_error)
      values[4] = __Pyx_ArgRef_FASTCALL(__pyx_args, 4);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[4])) __PYX_ERR(0, 648, __pyx_L3_error)
      values[5] = __Pyx_ArgRef_FASTCALL(__pyx_args, 5);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[5])) __PYX_ERR(0, 648, __pyx_L3_error)
      values[6] = __Pyx_ArgRef_FASTCALL(__pyx_args, 6);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[6])) __PYX_ERR(0, 648, __pyx_L3_error)
      values[7] = __Pyx_ArgRef_FASTCALL(__pyx_args, 7);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[7])) __PYX_ERR(0, 648, __pyx_L3_error)
    }
    __pyx_v_anchor = values[0];
    __pyx_v_item = values[1];
    __pyx_v_score = __Pyx_PyFloat_AsDouble(values[2]); if (unlikely((__pyx_v_score == (double)-1) && PyErr_Occurred())) __PYX_ERR(0, 648, __pyx_L3_error)
    __pyx_v_heap = ((PyObject*)values[3]);
    __pyx_v_tau_dict = ((PyObject*)values[4]);
    __pyx_v_k = __Pyx_PyLong_As_int(values[5]); if (unlikely((__pyx_v_k == (int)-1) && PyErr_Occurred())) __PYX_ERR(0, 648, __pyx_L3_error)
    __pyx_v_active_scores = ((PyObject*)values[6]);
    __pyx_v_counter_state = ((PyObject*)values[7]);
  }
  goto __pyx_L6_skip;
  __pyx_L5_argtuple_error:;
  __Pyx_RaiseArgtupleInvalid("update_heap_and_tau_fast", 1, 8, 8, __pyx_nargs); __PYX_ERR(0, 648, __pyx_L3_error)
  __pyx_L6_skip:;
  goto __pyx_L4_argument_unpacking_done;
  __pyx_L3_error:;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  __Pyx_AddTraceback("_fast_match.update_heap_and_tau_fast", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __Pyx_RefNannyFinishContext();
  return NULL;
  __pyx_L4_argument_unpacking_done:;
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_heap), (&PyList_Type), 1, "heap", 1))) __PYX_ERR(0, 648, __pyx_L1_error)
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_tau_dict), (&PyDict_Type), 1, "tau_dict", 1))) __PYX_ERR(0, 648, __pyx_L1_error)
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_active_scores), (&PyDict_Type), 1, "active_scores", 1))) __PYX_ERR(0, 648, __pyx_L1_error)
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_counter_state), (&PyList_Type), 1, "counter_state", 1))) __PYX_ERR(0, 648, __pyx_L1_error)
  __pyx_r = __pyx_pf_11_fast_match_20update_heap_and_tau_fast(__pyx_self, __pyx_v_anchor, __pyx_v_item, __pyx_v_score, __pyx_v_heap, __pyx_v_tau_dict, __pyx_v_k, __pyx_v_active_scores, __pyx_v_counter_state);

  goto __pyx_L0;
  __pyx_L1_error:;
  __pyx_r = NULL;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  goto __pyx_L7_cleaned_up;
  __pyx_L0:;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  __pyx_L7_cleaned_up:;
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}

static PyObject *__pyx_pf_11_fast_match_20update_heap_and_tau_fast(CYTHON_UNUSED PyObject *__pyx_self, PyObject *__pyx_v_anchor, PyObject *__pyx_v_item, double __pyx_v_score, PyObject *__pyx_v_heap, PyObject *__pyx_v_tau_dict, int __pyx_v_k, PyObject *__pyx_v_active_scores, PyObject *__pyx_v_counter_state) {
  PyObject *__pyx_r = NULL;
  __Pyx_RefNannyDeclarations
  PyObject *__pyx_t_1 = NULL;
  PyObject *__pyx_t_2 = NULL;
  PyObject *__pyx_t_3 = NULL;
  PyObject *__pyx_t_4 = NULL;
  PyObject *__pyx_t_5 = NULL;
  size_t __pyx_t_6;
  int __pyx_t_7;
  Py_ssize_t __pyx_t_8;
  Py_ssize_t __pyx_t_9;
  int __pyx_t_10;
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  __Pyx_RefNannySetupContext("update_heap_and_tau_fast", 0);

  __pyx_t_2 = NULL;
  __Pyx_GetModuleGlobalName(__pyx_t_3, __pyx_mstate_global->__pyx_n_u_update_unique_topk_heap_fast); if (unlikely(!__pyx_t_3)) __PYX_ERR(0, 650, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_3);
  __pyx_t_4 = PyFloat_FromDouble(__pyx_v_score); if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 650, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_4);
  __pyx_t_5 = __Pyx_PyLong_From_int(__pyx_v_k); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 650, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_5);
  __pyx_t_6 = 1;
  #if CYTHON_UNPACK_METHODS
  if (unlikely(PyMethod_Check(__pyx_t_3))) {
    __pyx_t_2 = PyMethod_GET_SELF(__pyx_t_3);
    assert(__pyx_t_2);
    PyObject* __pyx__function = PyMethod_GET_FUNCTION(__pyx_t_3);
    __Pyx_INCREF(__pyx_t_2);
    __Pyx_INCREF(__pyx__function);
    __Pyx_DECREF_SET(__pyx_t_3, __pyx__function);
    __pyx_t_6 = 0;
  }
  #endif
  {
    PyObject *__pyx_callargs[7] = {__pyx_t_2, __pyx_v_item, __pyx_t_4, __pyx_v_heap, __pyx_v_active_scores, __pyx_t_5, __pyx_v_counter_state};
    __pyx_t_1 = __Pyx_PyObject_FastCall((PyObject*)__pyx_t_3, __pyx_callargs+__pyx_t_6, (7-__pyx_t_6) | (__pyx_t_6*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
    __Pyx_XDECREF(__pyx_t_2); __pyx_t_2 = 0;
    __Pyx_DECREF(__pyx_t_4); __pyx_t_4 = 0;
    __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
    __Pyx_DECREF(__pyx_t_3); __pyx_t_3 = 0;
    if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 650, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_1);
  }
  __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;

  __pyx_t_3 = NULL;
  __Pyx_GetModuleGlobalName(__pyx_t_5, __pyx_mstate_global->__pyx_n_u_cleanup_lazy_heap_fast); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 651, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_5);
  __pyx_t_6 = 1;
  #if CYTHON_UNPACK_METHODS
  if (unlikely(PyMethod_Check(__pyx_t_5))) {
    __pyx_t_3 = PyMethod_GET_SELF(__pyx_t_5);
    assert(__pyx_t_3);
    PyObject* __pyx__function = PyMethod_GET_FUNCTION(__pyx_t_5);
    __Pyx_INCREF(__pyx_t_3);
    __Pyx_INCREF(__pyx__function);
    __Pyx_DECREF_SET(__pyx_t_5, __pyx__function);
    __pyx_t_6 = 0;
  }
  #endif
  {
    PyObject *__pyx_callargs[3] = {__pyx_t_3, __pyx_v_heap, __pyx_v_active_scores};
    __pyx_t_1 = __Pyx_PyObject_FastCall((PyObject*)__pyx_t_5, __pyx_callargs+__pyx_t_6, (3-__pyx_t_6) | (__pyx_t_6*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
    __Pyx_XDECREF(__pyx_t_3); __pyx_t_3 = 0;
    __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
    if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 651, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_1);
  }
  __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;

  if (unlikely(__pyx_v_active_scores == Py_None)) {
    PyErr_SetString(PyExc_TypeError, "object of type 'NoneType' has no len()");
    __PYX_ERR(0, 652, __pyx_L1_error)
  }
  __pyx_t_8 = PyDict_Size(__pyx_v_active_scores); if (unlikely(__pyx_t_8 == ((Py_ssize_t)-1))) __PYX_ERR(0, 652, __pyx_L1_error)
  __pyx_t_10 = (__pyx_v_k > 0);
  if (__pyx_t_10) {
    __pyx_t_9 = __pyx_v_k;
  } else {
    __pyx_t_9 = 1;
  }
  __pyx_t_10 = (__pyx_t_8 == __pyx_t_9);
  if (__pyx_t_10) {
  } else {
    __pyx_t_7 = __pyx_t_10;
    goto __pyx_L4_bool_binop_done;
  }
  if (__pyx_v_heap == Py_None) __pyx_t_10 = 0;
  else
  {
    Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_v_heap);
    if (unlikely(((!CYTHON_ASSUME_SAFE_SIZE) && __pyx_temp < 0))) __PYX_ERR(0, 652, __pyx_L1_error)
    __pyx_t_10 = (__pyx_temp != 0);
  }

  __pyx_t_7 = __pyx_t_10;
  __pyx_L4_bool_binop_done:;
  if (__pyx_t_7) {

    if (unlikely(__pyx_v_heap == Py_None)) {
      PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
      __PYX_ERR(0, 653, __pyx_L1_error)
    }
    __pyx_t_1 = __Pyx_GetItemInt(__Pyx_PyList_GET_ITEM(__pyx_v_heap, 0), 0, long, 1, __Pyx_PyLong_From_long, 0, 0, 0, 1, __Pyx_ReferenceSharing_SharedReference); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 653, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_1);
    if (unlikely(__pyx_v_tau_dict == Py_None)) {
      PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
      __PYX_ERR(0, 653, __pyx_L1_error)
    }
    if (unlikely((PyDict_SetItem(__pyx_v_tau_dict, __pyx_v_anchor, __pyx_t_1) < 0))) __PYX_ERR(0, 653, __pyx_L1_error)
    __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;

    goto __pyx_L3;
  }

   {
    if (unlikely(__pyx_v_tau_dict == Py_None)) {
      PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
      __PYX_ERR(0, 655, __pyx_L1_error)
    }
    if (unlikely((PyDict_SetItem(__pyx_v_tau_dict, __pyx_v_anchor, __pyx_mstate_global->__pyx_float_0_0) < 0))) __PYX_ERR(0, 655, __pyx_L1_error)
  }
  __pyx_L3:;

  __pyx_r = Py_None; __Pyx_INCREF(Py_None);
  goto __pyx_L0;
  __pyx_L1_error:;
  __Pyx_XDECREF(__pyx_t_1);
  __Pyx_XDECREF(__pyx_t_2);
  __Pyx_XDECREF(__pyx_t_3);
  __Pyx_XDECREF(__pyx_t_4);
  __Pyx_XDECREF(__pyx_t_5);
  __Pyx_AddTraceback("_fast_match.update_heap_and_tau_fast", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __pyx_r = NULL;
  __pyx_L0:;
  __Pyx_XGIVEREF(__pyx_r);
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}

static PyObject *__pyx_pw_11_fast_match_23update_global_best_and_tau_fast(PyObject *__pyx_self,
#if CYTHON_METH_FASTCALL
PyObject *const *__pyx_args, Py_ssize_t __pyx_nargs, PyObject *__pyx_kwds
#else
PyObject *__pyx_args, PyObject *__pyx_kwds
#endif
);
PyDoc_STRVAR(__pyx_doc_11_fast_match_22update_global_best_and_tau_fast, "Track best(item) and return the current global k-th threshold.");
static PyMethodDef __pyx_mdef_11_fast_match_23update_global_best_and_tau_fast = {"update_global_best_and_tau_fast", (PyCFunction)(void(*)(void))(__Pyx_PyCFunction_FastCallWithKeywords)__pyx_pw_11_fast_match_23update_global_best_and_tau_fast, __Pyx_METH_FASTCALL|METH_KEYWORDS, __pyx_doc_11_fast_match_22update_global_best_and_tau_fast};
static PyObject *__pyx_pw_11_fast_match_23update_global_best_and_tau_fast(PyObject *__pyx_self,
#if CYTHON_METH_FASTCALL
PyObject *const *__pyx_args, Py_ssize_t __pyx_nargs, PyObject *__pyx_kwds
#else
PyObject *__pyx_args, PyObject *__pyx_kwds
#endif
) {
  PyObject *__pyx_v_item = 0;
  double __pyx_v_score;
  PyObject *__pyx_v_best_scores = 0;
  PyObject *__pyx_v_global_heap = 0;
  PyObject *__pyx_v_global_heap_scores = 0;
  int __pyx_v_alt_k;
  PyObject *__pyx_v_counter_state = 0;
  #if !CYTHON_METH_FASTCALL
  CYTHON_UNUSED Py_ssize_t __pyx_nargs;
  #endif
  CYTHON_UNUSED PyObject *const *__pyx_kwvalues;
  PyObject* values[7] = {0,0,0,0,0,0,0};
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  PyObject *__pyx_r = 0;
  __Pyx_RefNannyDeclarations
  __Pyx_RefNannySetupContext("update_global_best_and_tau_fast (wrapper)", 0);
  #if !CYTHON_METH_FASTCALL
  #if CYTHON_ASSUME_SAFE_SIZE
  __pyx_nargs = PyTuple_GET_SIZE(__pyx_args);
  #else
  __pyx_nargs = PyTuple_Size(__pyx_args); if (unlikely(__pyx_nargs < 0)) return NULL;
  #endif
  #endif
  __pyx_kwvalues = __Pyx_KwValues_FASTCALL(__pyx_args, __pyx_nargs);
  {
    PyObject ** const __pyx_pyargnames[] = {&__pyx_mstate_global->__pyx_n_u_item,&__pyx_mstate_global->__pyx_n_u_score,&__pyx_mstate_global->__pyx_n_u_best_scores,&__pyx_mstate_global->__pyx_n_u_global_heap,&__pyx_mstate_global->__pyx_n_u_global_heap_scores,&__pyx_mstate_global->__pyx_n_u_alt_k,&__pyx_mstate_global->__pyx_n_u_counter_state,0};
    const Py_ssize_t __pyx_kwds_len = (__pyx_kwds) ? __Pyx_NumKwargs_FASTCALL(__pyx_kwds) : 0;
    if (unlikely(__pyx_kwds_len) < 0) __PYX_ERR(0, 658, __pyx_L3_error)
    if (__pyx_kwds_len > 0) {
      switch (__pyx_nargs) {
        case  7:
        values[6] = __Pyx_ArgRef_FASTCALL(__pyx_args, 6);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[6])) __PYX_ERR(0, 658, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  6:
        values[5] = __Pyx_ArgRef_FASTCALL(__pyx_args, 5);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[5])) __PYX_ERR(0, 658, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  5:
        values[4] = __Pyx_ArgRef_FASTCALL(__pyx_args, 4);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[4])) __PYX_ERR(0, 658, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  4:
        values[3] = __Pyx_ArgRef_FASTCALL(__pyx_args, 3);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[3])) __PYX_ERR(0, 658, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  3:
        values[2] = __Pyx_ArgRef_FASTCALL(__pyx_args, 2);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[2])) __PYX_ERR(0, 658, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  2:
        values[1] = __Pyx_ArgRef_FASTCALL(__pyx_args, 1);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[1])) __PYX_ERR(0, 658, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  1:
        values[0] = __Pyx_ArgRef_FASTCALL(__pyx_args, 0);
        if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[0])) __PYX_ERR(0, 658, __pyx_L3_error)
        CYTHON_FALLTHROUGH;
        case  0: break;
        default: goto __pyx_L5_argtuple_error;
      }
      const Py_ssize_t kwd_pos_args = __pyx_nargs;
      if (__Pyx_ParseKeywords(__pyx_kwds, __pyx_kwvalues, __pyx_pyargnames, 0, values, kwd_pos_args, __pyx_kwds_len, "update_global_best_and_tau_fast", 0) < (0)) __PYX_ERR(0, 658, __pyx_L3_error)
      for (Py_ssize_t i = __pyx_nargs; i < 7; i++) {
        if (unlikely(!values[i])) { __Pyx_RaiseArgtupleInvalid("update_global_best_and_tau_fast", 1, 7, 7, i); __PYX_ERR(0, 658, __pyx_L3_error) }
      }
    } else if (unlikely(__pyx_nargs != 7)) {
      goto __pyx_L5_argtuple_error;
    } else {
      values[0] = __Pyx_ArgRef_FASTCALL(__pyx_args, 0);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[0])) __PYX_ERR(0, 658, __pyx_L3_error)
      values[1] = __Pyx_ArgRef_FASTCALL(__pyx_args, 1);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[1])) __PYX_ERR(0, 658, __pyx_L3_error)
      values[2] = __Pyx_ArgRef_FASTCALL(__pyx_args, 2);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[2])) __PYX_ERR(0, 658, __pyx_L3_error)
      values[3] = __Pyx_ArgRef_FASTCALL(__pyx_args, 3);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[3])) __PYX_ERR(0, 658, __pyx_L3_error)
      values[4] = __Pyx_ArgRef_FASTCALL(__pyx_args, 4);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[4])) __PYX_ERR(0, 658, __pyx_L3_error)
      values[5] = __Pyx_ArgRef_FASTCALL(__pyx_args, 5);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[5])) __PYX_ERR(0, 658, __pyx_L3_error)
      values[6] = __Pyx_ArgRef_FASTCALL(__pyx_args, 6);
      if (!CYTHON_ASSUME_SAFE_MACROS && unlikely(!values[6])) __PYX_ERR(0, 658, __pyx_L3_error)
    }
    __pyx_v_item = values[0];
    __pyx_v_score = __Pyx_PyFloat_AsDouble(values[1]); if (unlikely((__pyx_v_score == (double)-1) && PyErr_Occurred())) __PYX_ERR(0, 658, __pyx_L3_error)
    __pyx_v_best_scores = ((PyObject*)values[2]);
    __pyx_v_global_heap = ((PyObject*)values[3]);
    __pyx_v_global_heap_scores = ((PyObject*)values[4]);
    __pyx_v_alt_k = __Pyx_PyLong_As_int(values[5]); if (unlikely((__pyx_v_alt_k == (int)-1) && PyErr_Occurred())) __PYX_ERR(0, 658, __pyx_L3_error)
    __pyx_v_counter_state = ((PyObject*)values[6]);
  }
  goto __pyx_L6_skip;
  __pyx_L5_argtuple_error:;
  __Pyx_RaiseArgtupleInvalid("update_global_best_and_tau_fast", 1, 7, 7, __pyx_nargs); __PYX_ERR(0, 658, __pyx_L3_error)
  __pyx_L6_skip:;
  goto __pyx_L4_argument_unpacking_done;
  __pyx_L3_error:;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  __Pyx_AddTraceback("_fast_match.update_global_best_and_tau_fast", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __Pyx_RefNannyFinishContext();
  return NULL;
  __pyx_L4_argument_unpacking_done:;
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_best_scores), (&PyDict_Type), 1, "best_scores", 1))) __PYX_ERR(0, 658, __pyx_L1_error)
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_global_heap), (&PyList_Type), 1, "global_heap", 1))) __PYX_ERR(0, 658, __pyx_L1_error)
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_global_heap_scores), (&PyDict_Type), 1, "global_heap_scores", 1))) __PYX_ERR(0, 658, __pyx_L1_error)
  if (unlikely(!__Pyx_ArgTypeTest(((PyObject *)__pyx_v_counter_state), (&PyList_Type), 1, "counter_state", 1))) __PYX_ERR(0, 658, __pyx_L1_error)
  __pyx_r = __pyx_pf_11_fast_match_22update_global_best_and_tau_fast(__pyx_self, __pyx_v_item, __pyx_v_score, __pyx_v_best_scores, __pyx_v_global_heap, __pyx_v_global_heap_scores, __pyx_v_alt_k, __pyx_v_counter_state);

  goto __pyx_L0;
  __pyx_L1_error:;
  __pyx_r = NULL;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  goto __pyx_L7_cleaned_up;
  __pyx_L0:;
  for (Py_ssize_t __pyx_temp=0; __pyx_temp < (Py_ssize_t)(sizeof(values)/sizeof(values[0])); ++__pyx_temp) {
    Py_XDECREF(values[__pyx_temp]);
  }
  __pyx_L7_cleaned_up:;
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}

static PyObject *__pyx_pf_11_fast_match_22update_global_best_and_tau_fast(CYTHON_UNUSED PyObject *__pyx_self, PyObject *__pyx_v_item, double __pyx_v_score, PyObject *__pyx_v_best_scores, PyObject *__pyx_v_global_heap, PyObject *__pyx_v_global_heap_scores, int __pyx_v_alt_k, PyObject *__pyx_v_counter_state) {
  PyObject *__pyx_v_current = 0;
  PyObject *__pyx_r = NULL;
  __Pyx_RefNannyDeclarations
  int __pyx_t_1;
  PyObject *__pyx_t_2 = NULL;
  int __pyx_t_3;
  PyObject *__pyx_t_4 = NULL;
  PyObject *__pyx_t_5 = NULL;
  size_t __pyx_t_6;
  Py_ssize_t __pyx_t_7;
  PyObject *__pyx_t_8 = NULL;
  PyObject *__pyx_t_9 = NULL;
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  __Pyx_RefNannySetupContext("update_global_best_and_tau_fast", 0);

  __pyx_t_1 = (__pyx_v_alt_k < 1);
  if (__pyx_t_1) {

    __pyx_v_alt_k = 1;

  }

  if (unlikely(__pyx_v_best_scores == Py_None)) {
    PyErr_Format(PyExc_AttributeError, "'NoneType' object has no attribute '%.30s'", "get");
    __PYX_ERR(0, 665, __pyx_L1_error)
  }
  __pyx_t_2 = __Pyx_PyDict_GetItemDefault(__pyx_v_best_scores, __pyx_v_item, Py_None); if (unlikely(!__pyx_t_2)) __PYX_ERR(0, 665, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_2);
  __pyx_v_current = __pyx_t_2;
  __pyx_t_2 = 0;

  __pyx_t_3 = (__pyx_v_current != Py_None);
  if (__pyx_t_3) {
  } else {
    __pyx_t_1 = __pyx_t_3;
    goto __pyx_L5_bool_binop_done;
  }
  __pyx_t_2 = PyFloat_FromDouble(__pyx_v_score); if (unlikely(!__pyx_t_2)) __PYX_ERR(0, 666, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_2);
  __pyx_t_4 = PyObject_RichCompare(__pyx_t_2, __pyx_v_current, Py_LE); __Pyx_XGOTREF(__pyx_t_4); if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 666, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_2); __pyx_t_2 = 0;
  __pyx_t_3 = __Pyx_PyObject_IsTrue(__pyx_t_4); if (unlikely((__pyx_t_3 < 0))) __PYX_ERR(0, 666, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_4); __pyx_t_4 = 0;
  __pyx_t_1 = __pyx_t_3;
  __pyx_L5_bool_binop_done:;
  if (__pyx_t_1) {

    __pyx_t_2 = NULL;
    __Pyx_GetModuleGlobalName(__pyx_t_5, __pyx_mstate_global->__pyx_n_u_cleanup_lazy_heap_fast); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 667, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_5);
    __pyx_t_6 = 1;
    #if CYTHON_UNPACK_METHODS
    if (unlikely(PyMethod_Check(__pyx_t_5))) {
      __pyx_t_2 = PyMethod_GET_SELF(__pyx_t_5);
      assert(__pyx_t_2);
      PyObject* __pyx__function = PyMethod_GET_FUNCTION(__pyx_t_5);
      __Pyx_INCREF(__pyx_t_2);
      __Pyx_INCREF(__pyx__function);
      __Pyx_DECREF_SET(__pyx_t_5, __pyx__function);
      __pyx_t_6 = 0;
    }
    #endif
    {
      PyObject *__pyx_callargs[3] = {__pyx_t_2, __pyx_v_global_heap, __pyx_v_global_heap_scores};
      __pyx_t_4 = __Pyx_PyObject_FastCall((PyObject*)__pyx_t_5, __pyx_callargs+__pyx_t_6, (3-__pyx_t_6) | (__pyx_t_6*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
      __Pyx_XDECREF(__pyx_t_2); __pyx_t_2 = 0;
      __Pyx_DECREF(__pyx_t_5); __pyx_t_5 = 0;
      if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 667, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_4);
    }
    __Pyx_DECREF(__pyx_t_4); __pyx_t_4 = 0;

    if (unlikely(__pyx_v_global_heap_scores == Py_None)) {
      PyErr_SetString(PyExc_TypeError, "object of type 'NoneType' has no len()");
      __PYX_ERR(0, 668, __pyx_L1_error)
    }
    __pyx_t_7 = PyDict_Size(__pyx_v_global_heap_scores); if (unlikely(__pyx_t_7 == ((Py_ssize_t)-1))) __PYX_ERR(0, 668, __pyx_L1_error)
    __pyx_t_3 = (__pyx_t_7 == __pyx_v_alt_k);
    if (__pyx_t_3) {
    } else {
      __pyx_t_1 = __pyx_t_3;
      goto __pyx_L8_bool_binop_done;
    }
    if (__pyx_v_global_heap == Py_None) __pyx_t_3 = 0;
    else
    {
      Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_v_global_heap);
      if (unlikely(((!CYTHON_ASSUME_SAFE_SIZE) && __pyx_temp < 0))) __PYX_ERR(0, 668, __pyx_L1_error)
      __pyx_t_3 = (__pyx_temp != 0);
    }

    __pyx_t_1 = __pyx_t_3;
    __pyx_L8_bool_binop_done:;
    if (__pyx_t_1) {

      __Pyx_XDECREF(__pyx_r);
      if (unlikely(__pyx_v_global_heap == Py_None)) {
        PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
        __PYX_ERR(0, 669, __pyx_L1_error)
      }
      __pyx_t_4 = __Pyx_GetItemInt(__Pyx_PyList_GET_ITEM(__pyx_v_global_heap, 0), 0, long, 1, __Pyx_PyLong_From_long, 0, 0, 0, 1, __Pyx_ReferenceSharing_SharedReference); if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 669, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_4);
      __pyx_r = __pyx_t_4;
      __pyx_t_4 = 0;
      goto __pyx_L0;

    }

    __Pyx_XDECREF(__pyx_r);
    __Pyx_INCREF(__pyx_mstate_global->__pyx_float_0_0);
    __pyx_r = __pyx_mstate_global->__pyx_float_0_0;
    goto __pyx_L0;

  }

  __pyx_t_4 = PyFloat_FromDouble(__pyx_v_score); if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 672, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_4);
  if (unlikely(__pyx_v_best_scores == Py_None)) {
    PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
    __PYX_ERR(0, 672, __pyx_L1_error)
  }
  if (unlikely((PyDict_SetItem(__pyx_v_best_scores, __pyx_v_item, __pyx_t_4) < 0))) __PYX_ERR(0, 672, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_4); __pyx_t_4 = 0;

  __pyx_t_5 = NULL;
  __Pyx_GetModuleGlobalName(__pyx_t_2, __pyx_mstate_global->__pyx_n_u_update_unique_topk_heap_fast); if (unlikely(!__pyx_t_2)) __PYX_ERR(0, 673, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_2);
  __pyx_t_8 = PyFloat_FromDouble(__pyx_v_score); if (unlikely(!__pyx_t_8)) __PYX_ERR(0, 673, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_8);
  __pyx_t_9 = __Pyx_PyLong_From_int(__pyx_v_alt_k); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 673, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_9);
  __pyx_t_6 = 1;
  #if CYTHON_UNPACK_METHODS
  if (unlikely(PyMethod_Check(__pyx_t_2))) {
    __pyx_t_5 = PyMethod_GET_SELF(__pyx_t_2);
    assert(__pyx_t_5);
    PyObject* __pyx__function = PyMethod_GET_FUNCTION(__pyx_t_2);
    __Pyx_INCREF(__pyx_t_5);
    __Pyx_INCREF(__pyx__function);
    __Pyx_DECREF_SET(__pyx_t_2, __pyx__function);
    __pyx_t_6 = 0;
  }
  #endif
  {
    PyObject *__pyx_callargs[7] = {__pyx_t_5, __pyx_v_item, __pyx_t_8, __pyx_v_global_heap, __pyx_v_global_heap_scores, __pyx_t_9, __pyx_v_counter_state};
    __pyx_t_4 = __Pyx_PyObject_FastCall((PyObject*)__pyx_t_2, __pyx_callargs+__pyx_t_6, (7-__pyx_t_6) | (__pyx_t_6*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
    __Pyx_XDECREF(__pyx_t_5); __pyx_t_5 = 0;
    __Pyx_DECREF(__pyx_t_8); __pyx_t_8 = 0;
    __Pyx_DECREF(__pyx_t_9); __pyx_t_9 = 0;
    __Pyx_DECREF(__pyx_t_2); __pyx_t_2 = 0;
    if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 673, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_4);
  }
  __Pyx_DECREF(__pyx_t_4); __pyx_t_4 = 0;

  __pyx_t_2 = NULL;
  __Pyx_GetModuleGlobalName(__pyx_t_9, __pyx_mstate_global->__pyx_n_u_cleanup_lazy_heap_fast); if (unlikely(!__pyx_t_9)) __PYX_ERR(0, 674, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_9);
  __pyx_t_6 = 1;
  #if CYTHON_UNPACK_METHODS
  if (unlikely(PyMethod_Check(__pyx_t_9))) {
    __pyx_t_2 = PyMethod_GET_SELF(__pyx_t_9);
    assert(__pyx_t_2);
    PyObject* __pyx__function = PyMethod_GET_FUNCTION(__pyx_t_9);
    __Pyx_INCREF(__pyx_t_2);
    __Pyx_INCREF(__pyx__function);
    __Pyx_DECREF_SET(__pyx_t_9, __pyx__function);
    __pyx_t_6 = 0;
  }
  #endif
  {
    PyObject *__pyx_callargs[3] = {__pyx_t_2, __pyx_v_global_heap, __pyx_v_global_heap_scores};
    __pyx_t_4 = __Pyx_PyObject_FastCall((PyObject*)__pyx_t_9, __pyx_callargs+__pyx_t_6, (3-__pyx_t_6) | (__pyx_t_6*__Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET));
    __Pyx_XDECREF(__pyx_t_2); __pyx_t_2 = 0;
    __Pyx_DECREF(__pyx_t_9); __pyx_t_9 = 0;
    if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 674, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_4);
  }
  __Pyx_DECREF(__pyx_t_4); __pyx_t_4 = 0;

  if (unlikely(__pyx_v_global_heap_scores == Py_None)) {
    PyErr_SetString(PyExc_TypeError, "object of type 'NoneType' has no len()");
    __PYX_ERR(0, 675, __pyx_L1_error)
  }
  __pyx_t_7 = PyDict_Size(__pyx_v_global_heap_scores); if (unlikely(__pyx_t_7 == ((Py_ssize_t)-1))) __PYX_ERR(0, 675, __pyx_L1_error)
  __pyx_t_3 = (__pyx_t_7 == __pyx_v_alt_k);
  if (__pyx_t_3) {
  } else {
    __pyx_t_1 = __pyx_t_3;
    goto __pyx_L11_bool_binop_done;
  }
  if (__pyx_v_global_heap == Py_None) __pyx_t_3 = 0;
  else
  {
    Py_ssize_t __pyx_temp = __Pyx_PyList_GET_SIZE(__pyx_v_global_heap);
    if (unlikely(((!CYTHON_ASSUME_SAFE_SIZE) && __pyx_temp < 0))) __PYX_ERR(0, 675, __pyx_L1_error)
    __pyx_t_3 = (__pyx_temp != 0);
  }

  __pyx_t_1 = __pyx_t_3;
  __pyx_L11_bool_binop_done:;
  if (__pyx_t_1) {

    __Pyx_XDECREF(__pyx_r);
    if (unlikely(__pyx_v_global_heap == Py_None)) {
      PyErr_SetString(PyExc_TypeError, "'NoneType' object is not subscriptable");
      __PYX_ERR(0, 676, __pyx_L1_error)
    }
    __pyx_t_4 = __Pyx_GetItemInt(__Pyx_PyList_GET_ITEM(__pyx_v_global_heap, 0), 0, long, 1, __Pyx_PyLong_From_long, 0, 0, 0, 1, __Pyx_ReferenceSharing_SharedReference); if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 676, __pyx_L1_error)
    __Pyx_GOTREF(__pyx_t_4);
    __pyx_r = __pyx_t_4;
    __pyx_t_4 = 0;
    goto __pyx_L0;

  }

  __Pyx_XDECREF(__pyx_r);
  __Pyx_INCREF(__pyx_mstate_global->__pyx_float_0_0);
  __pyx_r = __pyx_mstate_global->__pyx_float_0_0;
  goto __pyx_L0;

  __pyx_L1_error:;
  __Pyx_XDECREF(__pyx_t_2);
  __Pyx_XDECREF(__pyx_t_4);
  __Pyx_XDECREF(__pyx_t_5);
  __Pyx_XDECREF(__pyx_t_8);
  __Pyx_XDECREF(__pyx_t_9);
  __Pyx_AddTraceback("_fast_match.update_global_best_and_tau_fast", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __pyx_r = NULL;
  __pyx_L0:;
  __Pyx_XDECREF(__pyx_v_current);
  __Pyx_XGIVEREF(__pyx_r);
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}

static PyMethodDef __pyx_methods[] = {
  {0, 0, 0, 0}
};

static CYTHON_SMALL_CODE int __Pyx_InitCachedBuiltins(__pyx_mstatetype *__pyx_mstate);
static CYTHON_SMALL_CODE int __Pyx_InitCachedConstants(__pyx_mstatetype *__pyx_mstate);
static CYTHON_SMALL_CODE int __Pyx_InitGlobals(void);
static CYTHON_SMALL_CODE int __Pyx_InitConstants(__pyx_mstatetype *__pyx_mstate);
static CYTHON_SMALL_CODE int __Pyx_modinit_global_init_code(__pyx_mstatetype *__pyx_mstate);
static CYTHON_SMALL_CODE int __Pyx_modinit_variable_export_code(__pyx_mstatetype *__pyx_mstate);
static CYTHON_SMALL_CODE int __Pyx_modinit_function_export_code(__pyx_mstatetype *__pyx_mstate);
static CYTHON_SMALL_CODE int __Pyx_modinit_type_init_code(__pyx_mstatetype *__pyx_mstate);
static CYTHON_SMALL_CODE int __Pyx_modinit_type_import_code(__pyx_mstatetype *__pyx_mstate);
static CYTHON_SMALL_CODE int __Pyx_modinit_variable_import_code(__pyx_mstatetype *__pyx_mstate);
static CYTHON_SMALL_CODE int __Pyx_modinit_function_import_code(__pyx_mstatetype *__pyx_mstate);
static CYTHON_SMALL_CODE int __Pyx_CreateCodeObjects(__pyx_mstatetype *__pyx_mstate);

static int __Pyx_modinit_global_init_code(__pyx_mstatetype *__pyx_mstate) {
  __Pyx_RefNannyDeclarations
  CYTHON_UNUSED_VAR(__pyx_mstate);
  __Pyx_RefNannySetupContext("__Pyx_modinit_global_init_code", 0);

  __Pyx_RefNannyFinishContext();
  return 0;
}

static int __Pyx_modinit_variable_export_code(__pyx_mstatetype *__pyx_mstate) {
  __Pyx_RefNannyDeclarations
  CYTHON_UNUSED_VAR(__pyx_mstate);
  __Pyx_RefNannySetupContext("__Pyx_modinit_variable_export_code", 0);

  __Pyx_RefNannyFinishContext();
  return 0;
}

static int __Pyx_modinit_function_export_code(__pyx_mstatetype *__pyx_mstate) {
  __Pyx_RefNannyDeclarations
  CYTHON_UNUSED_VAR(__pyx_mstate);
  __Pyx_RefNannySetupContext("__Pyx_modinit_function_export_code", 0);

  __Pyx_RefNannyFinishContext();
  return 0;
}

static int __Pyx_modinit_type_init_code(__pyx_mstatetype *__pyx_mstate) {
  __Pyx_RefNannyDeclarations
  CYTHON_UNUSED_VAR(__pyx_mstate);
  __Pyx_RefNannySetupContext("__Pyx_modinit_type_init_code", 0);

  __Pyx_RefNannyFinishContext();
  return 0;
}

static int __Pyx_modinit_type_import_code(__pyx_mstatetype *__pyx_mstate) {
  __Pyx_RefNannyDeclarations
  CYTHON_UNUSED_VAR(__pyx_mstate);
  __Pyx_RefNannySetupContext("__Pyx_modinit_type_import_code", 0);

  __Pyx_RefNannyFinishContext();
  return 0;
}

static int __Pyx_modinit_variable_import_code(__pyx_mstatetype *__pyx_mstate) {
  __Pyx_RefNannyDeclarations
  CYTHON_UNUSED_VAR(__pyx_mstate);
  __Pyx_RefNannySetupContext("__Pyx_modinit_variable_import_code", 0);

  __Pyx_RefNannyFinishContext();
  return 0;
}

static int __Pyx_modinit_function_import_code(__pyx_mstatetype *__pyx_mstate) {
  __Pyx_RefNannyDeclarations
  CYTHON_UNUSED_VAR(__pyx_mstate);
  __Pyx_RefNannySetupContext("__Pyx_modinit_function_import_code", 0);

  __Pyx_RefNannyFinishContext();
  return 0;
}

#if CYTHON_PEP489_MULTI_PHASE_INIT
static PyObject* __pyx_pymod_create(PyObject *spec, PyModuleDef *def);
static int __pyx_pymod_exec__fast_match(PyObject* module);
static PyModuleDef_Slot __pyx_moduledef_slots[] = {
  {Py_mod_create, (void*)__pyx_pymod_create},
  {Py_mod_exec, (void*)__pyx_pymod_exec__fast_match},
  #if CYTHON_COMPILING_IN_CPYTHON_FREETHREADING
  {Py_mod_gil, __Pyx_FREETHREADING_COMPATIBLE},
  #endif
  #if PY_VERSION_HEX >= 0x030C0000 && CYTHON_USE_MODULE_STATE
  {Py_mod_multiple_interpreters, Py_MOD_MULTIPLE_INTERPRETERS_NOT_SUPPORTED},
  #endif
  {0, NULL}
};
#endif

#ifdef __cplusplus
namespace {
  struct PyModuleDef __pyx_moduledef =
  #else
  static struct PyModuleDef __pyx_moduledef =
  #endif
  {
      PyModuleDef_HEAD_INIT,
      "_fast_match",
      __pyx_k_Cython_accelerated_path_matchin,
    #if CYTHON_USE_MODULE_STATE
      sizeof(__pyx_mstatetype),
    #else
      (CYTHON_PEP489_MULTI_PHASE_INIT) ? 0 : -1,
    #endif
      __pyx_methods ,
    #if CYTHON_PEP489_MULTI_PHASE_INIT
      __pyx_moduledef_slots,
    #else
      NULL,
    #endif
    #if CYTHON_USE_MODULE_STATE
      __pyx_m_traverse,
      __pyx_m_clear,
      NULL
    #else
      NULL,
      NULL,
      NULL
    #endif
  };
  #ifdef __cplusplus
}
#endif

#ifndef CYTHON_NO_PYINIT_EXPORT
  #define __Pyx_PyMODINIT_FUNC PyMODINIT_FUNC
#else
  #ifdef __cplusplus
  #define __Pyx_PyMODINIT_FUNC extern "C" PyObject *
  #else
  #define __Pyx_PyMODINIT_FUNC PyObject *
  #endif
#endif

__Pyx_PyMODINIT_FUNC PyInit__fast_match(void) CYTHON_SMALL_CODE;
__Pyx_PyMODINIT_FUNC PyInit__fast_match(void)
#if CYTHON_PEP489_MULTI_PHASE_INIT
{
  return PyModuleDef_Init(&__pyx_moduledef);
}

#if CYTHON_COMPILING_IN_LIMITED_API && (__PYX_LIMITED_VERSION_HEX < 0x03090000\
      || ((defined(_WIN32) || defined(WIN32) || defined(MS_WINDOWS)) && __PYX_LIMITED_VERSION_HEX < 0x030A0000))
static PY_INT64_T __Pyx_GetCurrentInterpreterId(void) {
    {
        PyObject *module = PyImport_ImportModule("_interpreters");
        if (!module) {
            PyErr_Clear();
            module = PyImport_ImportModule("_xxsubinterpreters");
            if (!module) goto bad;
        }
        PyObject *current = PyObject_CallMethod(module, "get_current", NULL);
        Py_DECREF(module);
        if (!current) goto bad;
        if (PyTuple_Check(current)) {
            PyObject *new_current = PySequence_GetItem(current, 0);
            Py_DECREF(current);
            current = new_current;
            if (!new_current) goto bad;
        }
        long long as_c_int = PyLong_AsLongLong(current);
        Py_DECREF(current);
        return as_c_int;
    }
  bad:
    PySys_WriteStderr("__Pyx_GetCurrentInterpreterId failed. Try setting the C define CYTHON_PEP489_MULTI_PHASE_INIT=0\n");
    return -1;
}
#endif
#if !CYTHON_USE_MODULE_STATE
static CYTHON_SMALL_CODE int __Pyx_check_single_interpreter(void) {
    static PY_INT64_T main_interpreter_id = -1;
#if CYTHON_COMPILING_IN_GRAAL && defined(GRAALPY_VERSION_NUM) && GRAALPY_VERSION_NUM > 0x19000000
    PY_INT64_T current_id = GraalPyInterpreterState_GetIDFromThreadState(PyThreadState_Get());
#elif CYTHON_COMPILING_IN_GRAAL
    PY_INT64_T current_id = PyInterpreterState_GetIDFromThreadState(PyThreadState_Get());
#elif CYTHON_COMPILING_IN_LIMITED_API && (__PYX_LIMITED_VERSION_HEX < 0x03090000\
      || ((defined(_WIN32) || defined(WIN32) || defined(MS_WINDOWS)) && __PYX_LIMITED_VERSION_HEX < 0x030A0000))
    PY_INT64_T current_id = __Pyx_GetCurrentInterpreterId();
#elif CYTHON_COMPILING_IN_LIMITED_API
    PY_INT64_T current_id = PyInterpreterState_GetID(PyInterpreterState_Get());
#else
    PY_INT64_T current_id = PyInterpreterState_GetID(PyThreadState_Get()->interp);
#endif
    if (unlikely(current_id == -1)) {
        return -1;
    }
    if (main_interpreter_id == -1) {
        main_interpreter_id = current_id;
        return 0;
    } else if (unlikely(main_interpreter_id != current_id)) {
        PyErr_SetString(
            PyExc_ImportError,
            "Interpreter change detected - this module can only be loaded into one interpreter per process.");
        return -1;
    }
    return 0;
}
#endif
static CYTHON_SMALL_CODE int __Pyx_copy_spec_to_module(PyObject *spec, PyObject *moddict, const char* from_name, const char* to_name, int allow_none)
{
    PyObject *value = PyObject_GetAttrString(spec, from_name);
    int result = 0;
    if (likely(value)) {
        if (allow_none || value != Py_None) {
            result = PyDict_SetItemString(moddict, to_name, value);
        }
        Py_DECREF(value);
    } else if (PyErr_ExceptionMatches(PyExc_AttributeError)) {
        PyErr_Clear();
    } else {
        result = -1;
    }
    return result;
}
static CYTHON_SMALL_CODE PyObject* __pyx_pymod_create(PyObject *spec, PyModuleDef *def) {
    PyObject *module = NULL, *moddict, *modname;
    CYTHON_UNUSED_VAR(def);
    #if !CYTHON_USE_MODULE_STATE
    if (__Pyx_check_single_interpreter())
        return NULL;
    #endif
    if (__pyx_m)
        return __Pyx_NewRef(__pyx_m);
    modname = PyObject_GetAttrString(spec, "name");
    if (unlikely(!modname)) goto bad;
    module = PyModule_NewObject(modname);
    Py_DECREF(modname);
    if (unlikely(!module)) goto bad;
    moddict = PyModule_GetDict(module);
    if (unlikely(!moddict)) goto bad;
    if (unlikely(__Pyx_copy_spec_to_module(spec, moddict, "loader", "__loader__", 1) < 0)) goto bad;
    if (unlikely(__Pyx_copy_spec_to_module(spec, moddict, "origin", "__file__", 1) < 0)) goto bad;
    if (unlikely(__Pyx_copy_spec_to_module(spec, moddict, "parent", "__package__", 1) < 0)) goto bad;
    if (unlikely(__Pyx_copy_spec_to_module(spec, moddict, "submodule_search_locations", "__path__", 0) < 0)) goto bad;
    return module;
bad:
    Py_XDECREF(module);
    return NULL;
}

static CYTHON_SMALL_CODE int __pyx_pymod_exec__fast_match(PyObject *__pyx_pyinit_module)
#endif
{
  int stringtab_initialized = 0;
  #if CYTHON_USE_MODULE_STATE
  int pystate_addmodule_run = 0;
  #endif
  __pyx_mstatetype *__pyx_mstate = NULL;
  PyObject *__pyx_t_1 = NULL;
  PyObject *__pyx_t_2 = NULL;
  Py_ssize_t __pyx_t_3;
  PyObject *__pyx_t_4 = NULL;
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  __Pyx_RefNannyDeclarations
  #if CYTHON_PEP489_MULTI_PHASE_INIT
  if (__pyx_m) {
    if (__pyx_m == __pyx_pyinit_module) return 0;
    PyErr_SetString(PyExc_RuntimeError, "Module '_fast_match' has already been imported. Re-initialisation is not supported.");
    return -1;
  }
  #else
  if (__pyx_m) return __Pyx_NewRef(__pyx_m);
  #endif

  #if CYTHON_PEP489_MULTI_PHASE_INIT
  __pyx_t_1 = __pyx_pyinit_module;
  Py_INCREF(__pyx_t_1);
  #else
  __pyx_t_1 = PyModule_Create(&__pyx_moduledef); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 1, __pyx_L1_error)
  #endif
  #if CYTHON_USE_MODULE_STATE
  {
    int add_module_result = __Pyx_State_AddModule(__pyx_t_1, &__pyx_moduledef);
    __pyx_t_1 = 0;
    if (unlikely((add_module_result < 0))) __PYX_ERR(0, 1, __pyx_L1_error)
    pystate_addmodule_run = 1;
  }
  #else
  __pyx_m = __pyx_t_1;
  #endif
  #if CYTHON_COMPILING_IN_CPYTHON_FREETHREADING
  PyUnstable_Module_SetGIL(__pyx_m, Py_MOD_GIL_USED);
  #endif
  __pyx_mstate = __pyx_mstate_global;
  CYTHON_UNUSED_VAR(__pyx_t_1);
  __pyx_mstate->__pyx_d = PyModule_GetDict(__pyx_m); if (unlikely(!__pyx_mstate->__pyx_d)) __PYX_ERR(0, 1, __pyx_L1_error)
  Py_INCREF(__pyx_mstate->__pyx_d);
  __pyx_mstate->__pyx_b = __Pyx_PyImport_AddModuleRef(__Pyx_BUILTIN_MODULE_NAME); if (unlikely(!__pyx_mstate->__pyx_b)) __PYX_ERR(0, 1, __pyx_L1_error)
  __pyx_mstate->__pyx_cython_runtime = __Pyx_PyImport_AddModuleRef("cython_runtime"); if (unlikely(!__pyx_mstate->__pyx_cython_runtime)) __PYX_ERR(0, 1, __pyx_L1_error)
  if (PyObject_SetAttrString(__pyx_m, "__builtins__", __pyx_mstate->__pyx_b) < 0) __PYX_ERR(0, 1, __pyx_L1_error)

  #if CYTHON_REFNANNY
  __Pyx_RefNanny = __Pyx_RefNannyImportAPI("refnanny");
  if (!__Pyx_RefNanny) {
    PyErr_Clear();
    __Pyx_RefNanny = __Pyx_RefNannyImportAPI("Cython.Runtime.refnanny");
    if (!__Pyx_RefNanny)
        Py_FatalError("failed to import 'refnanny' module");
  }
  #endif

__Pyx_RefNannySetupContext("PyInit__fast_match", 0);
  __Pyx_init_runtime_version();
  if (__Pyx_check_binary_version(__PYX_LIMITED_VERSION_HEX, __Pyx_get_runtime_version(), CYTHON_COMPILING_IN_LIMITED_API) < (0)) __PYX_ERR(0, 1, __pyx_L1_error)
  __pyx_mstate->__pyx_empty_tuple = PyTuple_New(0); if (unlikely(!__pyx_mstate->__pyx_empty_tuple)) __PYX_ERR(0, 1, __pyx_L1_error)
  __pyx_mstate->__pyx_empty_bytes = PyBytes_FromStringAndSize("", 0); if (unlikely(!__pyx_mstate->__pyx_empty_bytes)) __PYX_ERR(0, 1, __pyx_L1_error)
  __pyx_mstate->__pyx_empty_unicode = PyUnicode_FromStringAndSize("", 0); if (unlikely(!__pyx_mstate->__pyx_empty_unicode)) __PYX_ERR(0, 1, __pyx_L1_error)

  if (__Pyx_InitConstants(__pyx_mstate) < (0)) __PYX_ERR(0, 1, __pyx_L1_error)
  stringtab_initialized = 1;
  if (__Pyx_InitGlobals() < (0)) __PYX_ERR(0, 1, __pyx_L1_error)
  if (__pyx_module_is_main__fast_match) {
    if (PyObject_SetAttr(__pyx_m, __pyx_mstate_global->__pyx_n_u_name, __pyx_mstate_global->__pyx_n_u_main) < (0)) __PYX_ERR(0, 1, __pyx_L1_error)
  }
  {
    PyObject *modules = PyImport_GetModuleDict(); if (unlikely(!modules)) __PYX_ERR(0, 1, __pyx_L1_error)
    if (!PyDict_GetItemString(modules, "_fast_match")) {
      if (unlikely((PyDict_SetItemString(modules, "_fast_match", __pyx_m) < 0))) __PYX_ERR(0, 1, __pyx_L1_error)
    }
  }

  if (__Pyx_InitCachedBuiltins(__pyx_mstate) < (0)) __PYX_ERR(0, 1, __pyx_L1_error)

  if (__Pyx_InitCachedConstants(__pyx_mstate) < (0)) __PYX_ERR(0, 1, __pyx_L1_error)
  if (__Pyx_CreateCodeObjects(__pyx_mstate) < (0)) __PYX_ERR(0, 1, __pyx_L1_error)

  (void)__Pyx_modinit_global_init_code(__pyx_mstate);
  (void)__Pyx_modinit_variable_export_code(__pyx_mstate);
  (void)__Pyx_modinit_function_export_code(__pyx_mstate);
  (void)__Pyx_modinit_type_init_code(__pyx_mstate);
  (void)__Pyx_modinit_type_import_code(__pyx_mstate);
  (void)__Pyx_modinit_variable_import_code(__pyx_mstate);
  (void)__Pyx_modinit_function_import_code(__pyx_mstate);

  __pyx_t_1 = __Pyx_Import(__pyx_mstate_global->__pyx_n_u_heapq, 0, 0, NULL, 0); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 8, __pyx_L1_error)
  __pyx_t_2 = __pyx_t_1;
  __Pyx_GOTREF(__pyx_t_2);
  if (PyDict_SetItem(__pyx_mstate_global->__pyx_d, __pyx_mstate_global->__pyx_n_u_heapq, __pyx_t_2) < (0)) __PYX_ERR(0, 8, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_2); __pyx_t_2 = 0;

  {
    PyObject* const __pyx_imported_names[] = {__pyx_mstate_global->__pyx_n_u_ComparisonOp,__pyx_mstate_global->__pyx_n_u_safe_compare_predicate_values};
    __pyx_t_1 = __Pyx_Import(__pyx_mstate_global->__pyx_n_u_data_structure, __pyx_imported_names, 2, NULL, 0); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 10, __pyx_L1_error)
  }
  __pyx_t_2 = __pyx_t_1;
  __Pyx_GOTREF(__pyx_t_2);
  {
    PyObject* const __pyx_imported_names[] = {__pyx_mstate_global->__pyx_n_u_ComparisonOp,__pyx_mstate_global->__pyx_n_u_safe_compare_predicate_values};
    for (__pyx_t_3=0; __pyx_t_3 < 2; __pyx_t_3++) {
      __pyx_t_4 = __Pyx_ImportFrom(__pyx_t_2, __pyx_imported_names[__pyx_t_3]); if (unlikely(!__pyx_t_4)) __PYX_ERR(0, 10, __pyx_L1_error)
      __Pyx_GOTREF(__pyx_t_4);
      if (PyDict_SetItem(__pyx_mstate_global->__pyx_d, __pyx_imported_names[__pyx_t_3], __pyx_t_4) < (0)) __PYX_ERR(0, 10, __pyx_L1_error)
      __Pyx_DECREF(__pyx_t_4); __pyx_t_4 = 0;
    }
  }
  __Pyx_DECREF(__pyx_t_2); __pyx_t_2 = 0;

  __pyx_t_2 = __Pyx_CyFunction_New(&__pyx_mdef_11_fast_match_1match_path_compact_fast, 0, __pyx_mstate_global->__pyx_n_u_match_path_compact_fast, NULL, __pyx_mstate_global->__pyx_n_u_fast_match, __pyx_mstate_global->__pyx_d, ((PyObject *)__pyx_mstate_global->__pyx_codeobj_tab[0])); if (unlikely(!__pyx_t_2)) __PYX_ERR(0, 95, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_2);
  #if CYTHON_COMPILING_IN_CPYTHON && PY_VERSION_HEX >= 0x030E0000
  PyUnstable_Object_EnableDeferredRefcount(__pyx_t_2);
  #endif
  __Pyx_CyFunction_SetDefaultsTuple(__pyx_t_2, __pyx_mstate_global->__pyx_tuple[0]);
  if (PyDict_SetItem(__pyx_mstate_global->__pyx_d, __pyx_mstate_global->__pyx_n_u_match_path_compact_fast, __pyx_t_2) < (0)) __PYX_ERR(0, 95, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_2); __pyx_t_2 = 0;

  __pyx_t_2 = __Pyx_CyFunction_New(&__pyx_mdef_11_fast_match_3match_star_paths_batch_fast, 0, __pyx_mstate_global->__pyx_n_u_match_star_paths_batch_fast, NULL, __pyx_mstate_global->__pyx_n_u_fast_match, __pyx_mstate_global->__pyx_d, ((PyObject *)__pyx_mstate_global->__pyx_codeobj_tab[1])); if (unlikely(!__pyx_t_2)) __PYX_ERR(0, 117, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_2);
  #if CYTHON_COMPILING_IN_CPYTHON && PY_VERSION_HEX >= 0x030E0000
  PyUnstable_Object_EnableDeferredRefcount(__pyx_t_2);
  #endif
  __Pyx_CyFunction_SetDefaultsTuple(__pyx_t_2, __pyx_mstate_global->__pyx_tuple[0]);
  if (PyDict_SetItem(__pyx_mstate_global->__pyx_d, __pyx_mstate_global->__pyx_n_u_match_star_paths_batch_fast, __pyx_t_2) < (0)) __PYX_ERR(0, 117, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_2); __pyx_t_2 = 0;

  __pyx_t_2 = __Pyx_CyFunction_New(&__pyx_mdef_11_fast_match_5match_bridge_batch, 0, __pyx_mstate_global->__pyx_n_u_match_bridge_batch, NULL, __pyx_mstate_global->__pyx_n_u_fast_match, __pyx_mstate_global->__pyx_d, ((PyObject *)__pyx_mstate_global->__pyx_codeobj_tab[2])); if (unlikely(!__pyx_t_2)) __PYX_ERR(0, 157, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_2);
  #if CYTHON_COMPILING_IN_CPYTHON && PY_VERSION_HEX >= 0x030E0000
  PyUnstable_Object_EnableDeferredRefcount(__pyx_t_2);
  #endif
  __Pyx_CyFunction_SetDefaultsTuple(__pyx_t_2, __pyx_mstate_global->__pyx_tuple[1]);
  if (PyDict_SetItem(__pyx_mstate_global->__pyx_d, __pyx_mstate_global->__pyx_n_u_match_bridge_batch, __pyx_t_2) < (0)) __PYX_ERR(0, 157, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_2); __pyx_t_2 = 0;

  __pyx_t_2 = __Pyx_CyFunction_New(&__pyx_mdef_11_fast_match_7derive_bridge_vw_pairs_fast, 0, __pyx_mstate_global->__pyx_n_u_derive_bridge_vw_pairs_fast, NULL, __pyx_mstate_global->__pyx_n_u_fast_match, __pyx_mstate_global->__pyx_d, ((PyObject *)__pyx_mstate_global->__pyx_codeobj_tab[3])); if (unlikely(!__pyx_t_2)) __PYX_ERR(0, 278, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_2);
  #if CYTHON_COMPILING_IN_CPYTHON && PY_VERSION_HEX >= 0x030E0000
  PyUnstable_Object_EnableDeferredRefcount(__pyx_t_2);
  #endif
  if (PyDict_SetItem(__pyx_mstate_global->__pyx_d, __pyx_mstate_global->__pyx_n_u_derive_bridge_vw_pairs_fast, __pyx_t_2) < (0)) __PYX_ERR(0, 278, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_2); __pyx_t_2 = 0;

  __pyx_t_2 = __Pyx_CyFunction_New(&__pyx_mdef_11_fast_match_9propagate_candidates_dag_fast, 0, __pyx_mstate_global->__pyx_n_u_propagate_candidates_dag_fast, NULL, __pyx_mstate_global->__pyx_n_u_fast_match, __pyx_mstate_global->__pyx_d, ((PyObject *)__pyx_mstate_global->__pyx_codeobj_tab[4])); if (unlikely(!__pyx_t_2)) __PYX_ERR(0, 334, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_2);
  #if CYTHON_COMPILING_IN_CPYTHON && PY_VERSION_HEX >= 0x030E0000
  PyUnstable_Object_EnableDeferredRefcount(__pyx_t_2);
  #endif
  if (PyDict_SetItem(__pyx_mstate_global->__pyx_d, __pyx_mstate_global->__pyx_n_u_propagate_candidates_dag_fast, __pyx_t_2) < (0)) __PYX_ERR(0, 334, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_2); __pyx_t_2 = 0;

  __pyx_t_2 = __Pyx_CyFunction_New(&__pyx_mdef_11_fast_match_11prune_pairwise_values_fast, 0, __pyx_mstate_global->__pyx_n_u_prune_pairwise_values_fast, NULL, __pyx_mstate_global->__pyx_n_u_fast_match, __pyx_mstate_global->__pyx_d, ((PyObject *)__pyx_mstate_global->__pyx_codeobj_tab[5])); if (unlikely(!__pyx_t_2)) __PYX_ERR(0, 434, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_2);
  #if CYTHON_COMPILING_IN_CPYTHON && PY_VERSION_HEX >= 0x030E0000
  PyUnstable_Object_EnableDeferredRefcount(__pyx_t_2);
  #endif
  if (PyDict_SetItem(__pyx_mstate_global->__pyx_d, __pyx_mstate_global->__pyx_n_u_prune_pairwise_values_fast, __pyx_t_2) < (0)) __PYX_ERR(0, 434, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_2); __pyx_t_2 = 0;

  __pyx_t_2 = __Pyx_CyFunction_New(&__pyx_mdef_11_fast_match_13filter_dis_items_fast, 0, __pyx_mstate_global->__pyx_n_u_filter_dis_items_fast, NULL, __pyx_mstate_global->__pyx_n_u_fast_match, __pyx_mstate_global->__pyx_d, ((PyObject *)__pyx_mstate_global->__pyx_codeobj_tab[6])); if (unlikely(!__pyx_t_2)) __PYX_ERR(0, 532, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_2);
  #if CYTHON_COMPILING_IN_CPYTHON && PY_VERSION_HEX >= 0x030E0000
  PyUnstable_Object_EnableDeferredRefcount(__pyx_t_2);
  #endif
  if (PyDict_SetItem(__pyx_mstate_global->__pyx_d, __pyx_mstate_global->__pyx_n_u_filter_dis_items_fast, __pyx_t_2) < (0)) __PYX_ERR(0, 532, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_2); __pyx_t_2 = 0;

  __pyx_t_2 = __Pyx_CyFunction_New(&__pyx_mdef_11_fast_match_15verify_pivot_pairwise_binding_fast, 0, __pyx_mstate_global->__pyx_n_u_verify_pivot_pairwise_binding_fa, NULL, __pyx_mstate_global->__pyx_n_u_fast_match, __pyx_mstate_global->__pyx_d, ((PyObject *)__pyx_mstate_global->__pyx_codeobj_tab[7])); if (unlikely(!__pyx_t_2)) __PYX_ERR(0, 568, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_2);
  #if CYTHON_COMPILING_IN_CPYTHON && PY_VERSION_HEX >= 0x030E0000
  PyUnstable_Object_EnableDeferredRefcount(__pyx_t_2);
  #endif
  if (PyDict_SetItem(__pyx_mstate_global->__pyx_d, __pyx_mstate_global->__pyx_n_u_verify_pivot_pairwise_binding_fa, __pyx_t_2) < (0)) __PYX_ERR(0, 568, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_2); __pyx_t_2 = 0;

  __pyx_t_2 = __Pyx_CyFunction_New(&__pyx_mdef_11_fast_match_17cleanup_lazy_heap_fast, 0, __pyx_mstate_global->__pyx_n_u_cleanup_lazy_heap_fast, NULL, __pyx_mstate_global->__pyx_n_u_fast_match, __pyx_mstate_global->__pyx_d, ((PyObject *)__pyx_mstate_global->__pyx_codeobj_tab[8])); if (unlikely(!__pyx_t_2)) __PYX_ERR(0, 592, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_2);
  #if CYTHON_COMPILING_IN_CPYTHON && PY_VERSION_HEX >= 0x030E0000
  PyUnstable_Object_EnableDeferredRefcount(__pyx_t_2);
  #endif
  if (PyDict_SetItem(__pyx_mstate_global->__pyx_d, __pyx_mstate_global->__pyx_n_u_cleanup_lazy_heap_fast, __pyx_t_2) < (0)) __PYX_ERR(0, 592, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_2); __pyx_t_2 = 0;

  __pyx_t_2 = __Pyx_CyFunction_New(&__pyx_mdef_11_fast_match_19update_unique_topk_heap_fast, 0, __pyx_mstate_global->__pyx_n_u_update_unique_topk_heap_fast, NULL, __pyx_mstate_global->__pyx_n_u_fast_match, __pyx_mstate_global->__pyx_d, ((PyObject *)__pyx_mstate_global->__pyx_codeobj_tab[9])); if (unlikely(!__pyx_t_2)) __PYX_ERR(0, 598, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_2);
  #if CYTHON_COMPILING_IN_CPYTHON && PY_VERSION_HEX >= 0x030E0000
  PyUnstable_Object_EnableDeferredRefcount(__pyx_t_2);
  #endif
  if (PyDict_SetItem(__pyx_mstate_global->__pyx_d, __pyx_mstate_global->__pyx_n_u_update_unique_topk_heap_fast, __pyx_t_2) < (0)) __PYX_ERR(0, 598, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_2); __pyx_t_2 = 0;

  __pyx_t_2 = __Pyx_CyFunction_New(&__pyx_mdef_11_fast_match_21update_heap_and_tau_fast, 0, __pyx_mstate_global->__pyx_n_u_update_heap_and_tau_fast, NULL, __pyx_mstate_global->__pyx_n_u_fast_match, __pyx_mstate_global->__pyx_d, ((PyObject *)__pyx_mstate_global->__pyx_codeobj_tab[10])); if (unlikely(!__pyx_t_2)) __PYX_ERR(0, 648, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_2);
  #if CYTHON_COMPILING_IN_CPYTHON && PY_VERSION_HEX >= 0x030E0000
  PyUnstable_Object_EnableDeferredRefcount(__pyx_t_2);
  #endif
  if (PyDict_SetItem(__pyx_mstate_global->__pyx_d, __pyx_mstate_global->__pyx_n_u_update_heap_and_tau_fast, __pyx_t_2) < (0)) __PYX_ERR(0, 648, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_2); __pyx_t_2 = 0;

  __pyx_t_2 = __Pyx_CyFunction_New(&__pyx_mdef_11_fast_match_23update_global_best_and_tau_fast, 0, __pyx_mstate_global->__pyx_n_u_update_global_best_and_tau_fast, NULL, __pyx_mstate_global->__pyx_n_u_fast_match, __pyx_mstate_global->__pyx_d, ((PyObject *)__pyx_mstate_global->__pyx_codeobj_tab[11])); if (unlikely(!__pyx_t_2)) __PYX_ERR(0, 658, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_2);
  #if CYTHON_COMPILING_IN_CPYTHON && PY_VERSION_HEX >= 0x030E0000
  PyUnstable_Object_EnableDeferredRefcount(__pyx_t_2);
  #endif
  if (PyDict_SetItem(__pyx_mstate_global->__pyx_d, __pyx_mstate_global->__pyx_n_u_update_global_best_and_tau_fast, __pyx_t_2) < (0)) __PYX_ERR(0, 658, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_2); __pyx_t_2 = 0;

  __pyx_t_2 = __Pyx_PyDict_NewPresized(0); if (unlikely(!__pyx_t_2)) __PYX_ERR(0, 1, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_2);
  if (PyDict_SetItem(__pyx_mstate_global->__pyx_d, __pyx_mstate_global->__pyx_n_u_test, __pyx_t_2) < (0)) __PYX_ERR(0, 1, __pyx_L1_error)
  __Pyx_DECREF(__pyx_t_2); __pyx_t_2 = 0;

  goto __pyx_L0;
  __pyx_L1_error:;
  __Pyx_XDECREF(__pyx_t_2);
  __Pyx_XDECREF(__pyx_t_4);
  if (__pyx_m) {
    if (__pyx_mstate->__pyx_d && stringtab_initialized) {
      __Pyx_AddTraceback("init _fast_match", __pyx_clineno, __pyx_lineno, __pyx_filename);
    }
    #if !CYTHON_USE_MODULE_STATE
    Py_CLEAR(__pyx_m);
    #else
    Py_DECREF(__pyx_m);
    if (pystate_addmodule_run) {
      PyObject *tp, *value, *tb;
      PyErr_Fetch(&tp, &value, &tb);
      PyState_RemoveModule(&__pyx_moduledef);
      PyErr_Restore(tp, value, tb);
    }
    #endif
  } else if (!PyErr_Occurred()) {
    PyErr_SetString(PyExc_ImportError, "init _fast_match");
  }
  __pyx_L0:;
  __Pyx_RefNannyFinishContext();
  #if CYTHON_PEP489_MULTI_PHASE_INIT
  return (__pyx_m != NULL) ? 0 : -1;
  #else
  return __pyx_m;
  #endif
}

static int __Pyx_InitCachedBuiltins(__pyx_mstatetype *__pyx_mstate) {
  CYTHON_UNUSED_VAR(__pyx_mstate);
  __pyx_builtin_max = __Pyx_GetBuiltinName(__pyx_mstate->__pyx_n_u_max); if (!__pyx_builtin_max) __PYX_ERR(0, 478, __pyx_L1_error)
  __pyx_builtin_min = __Pyx_GetBuiltinName(__pyx_mstate->__pyx_n_u_min); if (!__pyx_builtin_min) __PYX_ERR(0, 479, __pyx_L1_error)

  __pyx_mstate->__pyx_umethod_PyDict_Type_get.type = (PyObject*)&PyDict_Type;
  __pyx_mstate->__pyx_umethod_PyDict_Type_get.method_name = &__pyx_mstate->__pyx_n_u_get;
  __pyx_mstate->__pyx_umethod_PyDict_Type_items.type = (PyObject*)&PyDict_Type;
  __pyx_mstate->__pyx_umethod_PyDict_Type_items.method_name = &__pyx_mstate->__pyx_n_u_items;
  __pyx_mstate->__pyx_umethod_PyDict_Type_pop.type = (PyObject*)&PyDict_Type;
  __pyx_mstate->__pyx_umethod_PyDict_Type_pop.method_name = &__pyx_mstate->__pyx_n_u_pop;
  __pyx_mstate->__pyx_umethod_PyDict_Type_values.type = (PyObject*)&PyDict_Type;
  __pyx_mstate->__pyx_umethod_PyDict_Type_values.method_name = &__pyx_mstate->__pyx_n_u_values;
  return 0;
  __pyx_L1_error:;
  return -1;
}

static int __Pyx_InitCachedConstants(__pyx_mstatetype *__pyx_mstate) {
  __Pyx_RefNannyDeclarations
  CYTHON_UNUSED_VAR(__pyx_mstate);
  __Pyx_RefNannySetupContext("__Pyx_InitCachedConstants", 0);

  __pyx_mstate_global->__pyx_tuple[0] = PyTuple_Pack(1, Py_None); if (unlikely(!__pyx_mstate_global->__pyx_tuple[0])) __PYX_ERR(0, 95, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_mstate_global->__pyx_tuple[0]);
  __Pyx_GIVEREF(__pyx_mstate_global->__pyx_tuple[0]);

  __pyx_mstate_global->__pyx_tuple[1] = PyTuple_Pack(2, Py_None, Py_None); if (unlikely(!__pyx_mstate_global->__pyx_tuple[1])) __PYX_ERR(0, 157, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_mstate_global->__pyx_tuple[1]);
  __Pyx_GIVEREF(__pyx_mstate_global->__pyx_tuple[1]);
  #if CYTHON_IMMORTAL_CONSTANTS
  {
    PyObject **table = __pyx_mstate->__pyx_tuple;
    for (Py_ssize_t i=0; i<2; ++i) {
      #if CYTHON_COMPILING_IN_CPYTHON_FREETHREADING
      #if PY_VERSION_HEX < 0x030E0000
      if (_Py_IsOwnedByCurrentThread(table[i]) && Py_REFCNT(table[i]) == 1)
      #else
      if (PyUnstable_Object_IsUniquelyReferenced(table[i]))
      #endif
      {
        Py_SET_REFCNT(table[i], _Py_IMMORTAL_REFCNT_LOCAL);
      }
      #else
      Py_SET_REFCNT(table[i], _Py_IMMORTAL_INITIAL_REFCNT);
      #endif
    }
  }
  #endif
  __Pyx_RefNannyFinishContext();
  return 0;
  __pyx_L1_error:;
  __Pyx_RefNannyFinishContext();
  return -1;
}

static int __Pyx_InitConstants(__pyx_mstatetype *__pyx_mstate) {
  CYTHON_UNUSED_VAR(__pyx_mstate);
  {
    const struct { const unsigned int length: 10; } index[] = {{2},{179},{1},{2},{1},{1},{1},{8},{15},{12},{2},{5},{5},{2},{2},{2},{2},{2},{20},{11},{12},{13},{3},{9},{5},{17},{6},{9},{18},{5},{5},{14},{11},{12},{12},{9},{6},{2},{2},{5},{13},{5},{9},{6},{10},{9},{17},{22},{18},{13},{13},{7},{11},{11},{11},{14},{27},{9},{14},{9},{10},{11},{10},{2},{14},{6},{11},{15},{21},{8},{20},{13},{8},{3},{11},{18},{13},{11},{12},{4},{7},{7},{8},{5},{8},{1},{12},{13},{7},{13},{10},{6},{4},{10},{5},{7},{1},{8},{11},{8},{18},{23},{27},{12},{3},{6},{6},{6},{3},{6},{6},{10},{7},{4},{5},{8},{8},{13},{12},{9},{12},{8},{8},{14},{11},{8},{3},{13},{5},{5},{14},{11},{10},{11},{8},{13},{7},{13},{9},{10},{11},{4},{3},{9},{14},{29},{26},{10},{12},{20},{21},{7},{6},{20},{11},{9},{10},{30},{5},{3},{12},{10},{10},{11},{11},{17},{10},{8},{12},{8},{9},{12},{12},{31},{24},{28},{10},{1},{2},{2},{9},{8},{8},{8},{5},{5},{6},{6},{6},{3},{34},{1},{8},{6},{16},{7},{8},{37},{324},{258},{157},{766},{148},{383},{715},{46},{640},{164},{79}};
    #if (CYTHON_COMPRESS_STRINGS) == 2
const char* const cstring = "BZh91AY&SYe\215\355V\000\002\376\177\377\377\377\377\377\377\377\377\377\277\377\377\363\277\377\377\377\300@@@@@@@@@@@@\000@\000`\013\237\r\021ZS\024\354\030\200\000\000\001z\017QA\247\236\257a*hBOD\021\246\200\364\201\3524\006\232i\246\320\321\264\230&\3224\020f\24014\323M4\321\223 mCM\224\304h%4\232\004&\232\004d\t\222\236\010\2004\001\240\r\000\000\000\000\000\032\003!\240\000\006\247\251\204IA\240\323\t\246\230\203L\322`\020\0324\304\320h4\000\0324144\311\240\000\320\300\232\003D\242h\r\031\000\032bb\003\324z\236P\014\230\232i\221\246\200\000\0004\000\000ba\006\020\020`\000\230\000!\211\202`\000\010\300\t\223\004\311\200\001\031\030\230\214\0040\000\000\221\020\211\241M\r\2517\251=#\312mM\251\265\032=C@4\320\000\320\304\320\000\365\0312\000\304\0324\006\217Sjh\323rc\312\314\375\236\\\211\356\256\224\310\216\251\374\207\364\226\322:\3166\350Cj\264\205\267\025\222\266\241+\255s*4\023K\254\254\005\212\377\277\363\033\030\233cC\030\330\306\014m6\337^\245QT\027\336_\242\204\\\254\026\336\366\022\304*\001Do\225P\251+\013\030\314\203cI\261\r\202M\241\261\260L\222*\005X\275\260\244\332#\000\332]\254D\030\237\370\201\250$\300\221LI#\033HlM\240M\244\332m\013~P\002\2430/\3202PU\310W8,e\255c.3)J\\\241\003@C 1^\213$\262\214ChM\253\242\036+\n\353\331\246X\272\245j\240\305\202X\315\312\312v\t!IB9\331\336\200\207\004\222Sq\335\300\335\301\242\0234;\024j(\244m\304\305$\2465\250j\r.T\252gL\343\273\241\326\r`-\223gMl\001\2611y\014\327\0073\032\366h ;\234\034X\224c\037\006x\rT[\002*1\201{@\340\207\233\"\022\276\365k\"\373\255kY\r\252\232t\214^\2336\025k+\002\362P2\2734\014\221<A\252Z\215<\366cF\201\252\265\354\250\215lY?\307xc\351\"\3266\303\352\335\2552\2666\336\354\266\376\330\367\321\277\306\233\326q\230\267]~\340\320\r\243w\316\266n\346\003\177\036\226\345\335Z\225\007\233\236\016\364\310\222j\311\311\245W\235\036\003\256\255\340\263@G_\206\265\261\203Z\301\257\314\236\251\223c\272\376\217\233}\255\005\361k\331\021_\272z\316@\342\326\363\314\213\021\363U\302\332\361\353\233""\234\350\335=\311PX\272Kb\322\352\265\202\3217\327k\317\033\245q/\3206\376\217vft5\355t\005\3353=\335\336p\362I\246\273\364\302S\253\221\236\232:\326\357\244\264\241p\326f`A\001\226\257\243\362j\305J\201\334\322v\0231\027\201\024\223b!\022:A+viEJ\206>W(\370vp\275+\315JR\205\025\371\374\017j\326&\036\342%\\\031A_y\021K\025\226N\214\321\214\202\226n\300\340p\021t@4m\356\361\022i\331\022\202\312\230\027\207\315\365\360\272\335\034\005\205\274\377i\234\314\177\247\343\316\327\255\\\rhVv\177\331i\261\025x\376]\022\257\233s\360a\215\207\306\303AzY\003&\"$\243\232\315\n\303\253\221A\020\021\\\361W,\034\270\035J\222h]\004\322\341\022\2117\233\311\024\242\267\347//\013[N\014gQ\302\002\034\020\360\331\356\352*)\352\332SJ\254\252t\212\035b\312,(J\033X\337\033\376+_-#\347-\212\226\255\2062\275\252\376v\254Z\236u(V\331\312\344\345l\247\212K\013\257+\374\320\325\2364\305_\365p\306\265}\020\225UGw[g\004\356\362Y\342d\255%\354\221\245a\244\257l\242O\205bp\266\313\rZ\366N\204Q\030\221\020\205\224\207\210wV\224>G\367E\rt\310X\034\244+@V\230\003\234 \2651\233t\326\314\320N\246\377\360k>\027)\371\372\215T\312\265\220[\235\013b\016\204\3075\360\350\206(\332i\307\312\34773|N\r\301\215\252\310\201\247\214\315\202\207\243\220h\224\241E\3532m#uGy\231\211\263JQ\270\307\226pJgG\211\272(\262`\262\360\r\363\003U#TZ\346\265hp\234\226\243\225TRU\265\352A\020g\326\340\255\003P\355\322\330\234\316M!D\215\366\232xc\014\241\331oo;u\271\t\215Z\247\013'bD\3318<\223\220!F\031\241\347\330*.\345a\227\231\261\313cq\335;'\302\355-\346\201\240sk\231\310]]\210\202 \305\265Q\351\3308\226g\023>{\223\023&E\361\010\235\343\2152\263\226\013\304h6\206\207 \335\253\273\233\201\207cwD\351)\366\311\265\303\317\236IVu}\345\n\323\007D\336D\332\230\214wI\333\210\377\346\274*\360\035\\\321X\302\366\314\364\264\240I\231v\006\n\021\220\026\322\200KZ4d\204\332\212\247\334\215\253\004\324\222'+\353\307\035H\036'\211\004\254\330\354v\270\216\206\246\337_\223r\003\223\006\357G\232\2676&b]\231>\325\010X\2347\311\272\214\314\372""\273\335w5.\206|@\336\2611\031e\233\3446W!\010\022B\326\327\331.\003]\356\304\345\351\326\353\205\023^\034g\227\250\2239\205\245\271\262\335\273\032\354\364.\362mn\322a\305\237\001\267\330\027k7Q\244\271\271a\324\313\257:\2222\307:\3076\267f8[\254\351kk\016\373\327s\216w\333\013\345\023FQ4\331\207\022\363\353\343;h\3237\354\220\353\307l:6\344&H\301\233\026\263a.p\307\217\022\221v\276\307v\204v5\334!$\324\002\311\016\304\202\031\270\356vH\365>=\365o\034\372g\237\034g\260\035\271\316\030Dh\353\256\266$kSM\035km\242\347r\031\320(\354\241TrS7i\224\230\263+\305v\332\032y\002\023$\252&\005-\227!k\311\256u\3545\370\265m\347\344\336\203\357\317-\344(\337\242\216\226W\002$\331\267\210\311\013g\211q\263[MM\351s\354\344\346\316\270\350\316|G\300\305IY'\343#)\370\262\214\032\331;\313\ry\003\311\345\027Q>\016P\346\332\363DjD\345[dC\314\362\242\024\2663\3651\343<\014\336\017\014JPyq\014<<!\341\244\354HD\224\273\351\336\220\004\307z\344f\351\032\003\270J^\031\0204\265h\35388SF8\306\344!+\007\0142,\326~\363\022,\222\025\272)v\211\342\013%(\272\035\3129&\233}-\243\326\272\334\356\013\353\241\032\265<\212\274\311>G\202C\313H{\247\357c\225\003\005+\344\213A\243P\333&n\233\332\265Nk\261\342\226\030\275\357\207\223\t\006\203\242\236BD\231*\010\305\307c7pr\013*\351z>\366\3427P70uu\025\303N\336w\330n\210#\241%G\235\022yqyM\335\312\247w\265 \236\233\310\242\240\026AX z\302L\330.fhibU\006\016\364\302\227\220\334\003\266r\246C\323\000\261y\\\261J\316\312\246\005 \224\205y\302.\202V\t\216\213\333s\033\206\334\333\203v\343n-\335\211\327\317\260\200M#\236e<5\327?&\r\273\253\230\352\\\301g]\225cd\217\0220\\\260KH\224B\223\247\035\001\013\251\215tp\2109rc\223r9vW\r\032\362\315\340\334\245\314\355\006\354\233 1o\221\330rn[\230\332Z\335=\336\333\337hn\355\320\362\371\253\220\031\033M\306\3116\260\316\253j\024mQ2\232A\333?\035\337\241\021\312\346&6\2306\r\373\320\3332\262\\[\303L\213\216\261\000\201\363\241\000\313i\013\020\010\266L\202\363\021\006\333\203\010\003\204\343y\022fo7\272\037\007\223""\313R\204t\032<6\332RT\316\370`3\177\345\"!\261\233>\2203\246F\234J\371FS\253\332\232\202|v\177\235H\233\243d>\240h\340j\204!\260$\224\307,\325)\317\177\230\341 \340\276\t\037\246\321\370t\204\365\210F\275Xr\353E]c\247\257C\347?\263\316\262\254}\265\002!\024%\232\374.\031\304\177x\330\271\314<\202\373N\303,\306\230\241\206\320p\366;\311\227\260t\003\235!P$9\242\037\021U\323\303{\n\232le\272\255W\260p\003,V\006\006\016\343\010\257\223\226\300\337\023\t\2130\320\305\211S\236\354&s_G\213\305\006\326\317J%\365\351\356\027'j\2468D\004\r$\305\026/\024>Y\302W\374\032,\255V{\025\021\037z\216\204\365\210\2114\314\242FbQ\n\"\334lG|\001d\364@^\333,\212V3\327I6\261\376\256\244'\346!a\255\021\027QLC&,\\\220\033\024\020\210B\252\250\252kB\242\"\026\225X3\2013\030\341\276\261\261\004j4\326\336\246\255C\222m\326\n\207\"I\344\330\006\350\016\312\204G\243\314\331\177\254&\370\343\344VOP\026.\240\350e\003\301)\3012a\257\214I\250e\016\273\216\205B\245<u\035\362\315T\263\307!qc@\310p?\245\025\006?\016\200\024\305If\212\343h\023\251\302m\010 \202\251\262\223)B\340\304t\240\242t\3159+'\210\204)\304\3556\254g\253HI\212'\010\312\333\323\241\323\032\246\320\236P\022\200\262\267\"\214\020$\265\347\254\232\202\030\240#\234\003\253\027jJ\2336\\\025\321\207\247\016\273\360a\320\304\304\225\206\334Ow\014\266\310\342%Kd\2376\026o\223vHU%`\256\376\373p\377`6\374\260k\232\326w\306\0230\242\235)d\003QQyv\177\3014\2052AS\334%\237\006\2342\345\266\272\007\r\300\265\316\210]\212]\317Y\324\240\212\305\203\3177\253u0\332BB\270\320\010L\350\307\217\001\036:\262&\034\313\335~^7)_\013\004\354\323T\203\234w\210\257rm\355;_8\227n\211\225\263#a\300\353j+H\274\273\331\243V\343\266[\256\3563Z\265_2\255q\211\013z\222\264i\210\262\227\212@E\220\262\300\252\006Q\t\264^RrQ`\254\204\252\230\210\256,\330!\005\242\321\204\204f\321G\220\016`\362GI\202;KIR\367\n\310\235\254\342))\"T\223\304\304\341y\301\367\210\232\272\262i\2314\332\nJ\010\304\314\"\004\222\321\004Ft\r\001\322,\014\346$\0249\232\321;\305%*c\241%:\201d""\na#\030\2349j\205P\202\211\370`X-\020T\251\201v\030::\334V\351\tD)\276\035\361Y\254\272R\010\010A\"\305c\010\272;\274\034\322\022j\340\355F\322\211\031,`6\300\343\237\370\273\222)\302\204\203,oj\260";
    PyObject *data = __Pyx_DecompressString(cstring, 2893, 2);
    if (unlikely(!data)) __PYX_ERR(0, 1, __pyx_L1_error)
    const char* const bytes = __Pyx_PyBytes_AsString(data);
    #if !CYTHON_ASSUME_SAFE_MACROS
    if (likely(bytes)); else { Py_DECREF(data); __PYX_ERR(0, 1, __pyx_L1_error) }
    #endif
    #elif (CYTHON_COMPRESS_STRINGS) != 0
const char* const cstring = "x\332\325WMw\323f\026&\324\001\007\034b\023\003!\t`\023\n\245\024\246\016\014m\t)'%\201\241\037\264\2060m\347\234\216\216,\275\216E\034I\226d;\201v\332\245\226Z\276K-\265\324\322K\226]z\251\245\177\002?a\236\373\276\262c>f\016\323\231\315\234\343HW\357\347\275\317}\356G>_}hy\254\3445T\257tw\317kXf\311pK:k\0325\346\250\036k\356\225\\\31714\2179\264\310,}\267\361\335\325\033\237\336(\251\246^r\330S\246yn\311m\327\264\246\352\272\314-Y\365R\255m4=\303,y{6s\257\225\036\324K{V\273d2\246\227<\253dc\335\370\006\257\301\314\222\313<\022J\227T\323\264<\3253,S\301v\303\334\272T\322\r\007\227\030\035F\273\357\251M\227]\373\374\366\352\355kwT]W\260\232)u\325\365\224\035\325\323\032\327\354\275\335\273\326\216\255:\206k\231\337\332\033\325{\212az\367\024\273a\334\337\270\277\371\365\306\327\233\0177\024\345\273\275]\374\255\303,\345!\333\365\036\261\272*\256P\014\217\355\244\242\253Y\016\033\227]\\\2506=E\203\351.\t\333\364\020\3672W\261\231\243tUSkX\216|*\206\256\272{\246fX\327\260\333j\003\022\034\341yN\205\036\3135U\333\356\252\216\256\270\036\263\335\032\203\t\362\226\232c\350[L\336\222\312\256\315\264\241\334\351\326\332\3326\363\264\212\266\254\221q\264\020W\001\007\240\250ma\010\217\246ZcM\027\002\356\325\032FS\247\225\256\224:\252\243(\002~e\213yd1}2\325l\333\330\367lOi0\325\026\240jM\350\214\235\212\347\250\032#\2155\002\027\250m9\252\335\320\254\266\t^\300\002\334\256\265\035\207A\033\371\022\212\245\"\251\225\212\270\032\232\252\330\341\2645\257\3550\2359\204\357\3106\305V\r\307\025\227\353\206\013\020\200\255nIs\024\310\260\234\026\032f\335\022\202\230\220\222em\267m\022]9\335d\273\200\315c\272\\TW\r,\034\343\212\220l\325k(vS5\335:H\013[\350V\202D\352 \007\231^\267\034\341+\223\031[\215\032|\273\243\332\3031\341?E\251\267MM!@\267\232VMm\n\020\307\304\324\271\r\025Lq\230<W|t\025\326Q\233$v\225t\030\313\351\317\250\357\321\313\266\304\227\335v\033\364n5,[\331a\236j\300/C}\\\300\275\177\256!AW\010\300!\363 \247\n\223\324\325\311F\341z\"\243+,~j\341Dk{\273I\300\300S\315""\206KG\264]\300\002\2031)\201\033z\253F\037rD\3008\344\006\001'\207A\014G\314\271r\361\376\024\"[\321\226\335\035u\027?\245S\021\317\345\035\346l1}\3070\361\243Az.\343VKo7\231\242\230\212 \260\211\3102\211`&\316\026\264RLu\207\346\221bF\210\216\273j\004\223\211`WR\332\n\0310\2117\3145-=\345\223\3302\372r-\2332\241\000\022\277}\314m\350\261k\223\"D\332\256\3412\311\244T)\n\270T\304\351\002!\020\240\335\364\204\350\262\326\360\335f\246\306\304G\327\245,\322\031\346\024\333\350X\342\234\221\220\016\211y\272\013\334\030y}$\220Q\256\355X\266\272EYa\224\037\\EW\267\204\013l\247\215\270\036i\r\264\332L\022>\005O\251\303\331\2556P\024\300R\036l\2505\370`?\227\354\217\215\331\3530\221\374\245\235\016\3530\007\307\217{\302\331\347\224c\221M \236\020Dx(\256Zg\222G\370\200A\272\241\221\tRA\031A\254\245\010rK\315 \351H\336\270\r\241\205\365\004\365\230\344n\033\266L\033R\034\313#.\305\255\344\202\247\266\021\367\232\007\354@9X/2\244G)Y\361\032\024\265VSo\233\006<E\252\270\2251y\271m\213\334\233\206\272\310\343\000\\\2413\t\320tZ\244\200\267\214\247'y\226\275\275\237vE\304\211\270\354t*\235e\362\207g!\\ \030:H\"\337\317*\351{Y\350$\224\021@U\304S\312X\353\300\013H%\212\244\316\310\3515\003\2640%\037\272\335\217e\275\301\033\033\272\373\234\021^\355J\302\355\245\213~\233x\231?0y\210\036\271\376\321\245\260\222dO\005\277\206\333q!\311\316\365\347\256\364\257\254\364*r\321\031\276\366\362\310\201\311\354o\236\217U3~k\2209\014\371\263\340n\340q9\022\034\n4^\344O\302\362 \223\365'\375j\222\235\r0\225\363\037\362\253\341\257\361\323\027\023In\036\242\027U\242\265$7\343{\230\316\237\341\325\244p\226ka!\311\317\363+\341fT\210\256\304\233\275\343\275JR8\021T\305\212\201X\226/\006\033\274\220\024\317\366\317\376)\362\342J\274\226\024O\005\273\334\r\313\311\334i\214s5)\316\361c\241\232\314]\n\335\250\034\255\304z\257\334\273\363;\346/\204w\303VtX\354\373\366E%\231[\340+\241\026\025\222\305\217\242\215\270\020\227\223\305\313\321\211H\215Z4\365\347p:z\004q\361\"\016[<C\367`\374\034\367\302J""\2706\310\235\n\236\207\227\243\353\321v\357\364\213\363/\366\215)\014\262\205\240\020\274\037\374\034~\026\335\217War\346\250\017\014\007\231E x\210\020\334\013\336\223\260l\004\205$W\014\036\360\315\360x\270\022\251tJ'x\314'\371\343\360\220P-\1772\320y\231\357#\376\216{k\321\304\037\336\373?\276\367\250\177\335W\375\026Xt$\245\332</\274\314\036\230\234\002E6\373\205rx\246\177\355N\257\225dO\366O^\243\003\247\337\366\233\357\317_\215\252\203\354Q\377v\340\362r\222\233\035\352\263\300\277\n;QU\272\340z\240\222\002j\320\345\365p-|\022\235\007\323\362\263\301\215\240\303\253\304\216\005^\031\244\013\3008\360F\272\347\345\261\003\223\3470\237\001\205\206<\277\031\314\006\353|\202\354x\306\013C\024ns\250Z\014\326\350\362uP\236\361\233`n\356D\360#\327\3032B\010zt\021\010\000\021\036O\362\247y\201_\014\301\374\343\301\005\\\333zm\337\002\377,\\\217&\2009\366\265\222\374\034\317\3605\242:m~\313\315\247\310z\260\363p\370\372\307+\307\316\370nP\016>\207\217\226\021Pg\020\005K\275\357\301\323\321\371\377\351\362\3274\361\302\353\241\021\271\361G=5\311N\221\371\217\371\004?\301\267\302\352k\0373\376\036\177\217\337\206\206\210\336u~\210\263\360&\254-\222\356\347a\343\\)\314\300S\3257\246\273\274\006\330\206\323\2037w/A\257\177\263\233\022\312h\032t\271\205\3453\371\375\020\374?7b\312\317\370\367\220h:o8s\032\204\357\317V\220\322.P\366K\212\213|=\234\300\314\276p\016TB\256\002\367o\005U\311\376)?\033\200\353\243\340\315\007\231`\r\331\367-#\024N->A\251n\236\337\340\255\360H\377\303\225\336\255\027\233\277\027\220h\377\305\350Q\377\223`)x\002\203\277\247D\234\006\360Q\377F\177\006\201\0355\342\006\274@\203\2034\"s\344(\307?\356\2138_\033d\216\371;\342\\\244\324\303\277\355\"\310T\332OQ\272FFgQ+\202:E\315\251\340\027\201\227\030i \252\263\323\376\217\"\344\267\302\277\307\225d\n\307\371\n\377\202f\336u\323 \213\314\300;\264\000:#Rs\243\300^\340\2532\200i\344\003\244\224\\!8\r\310>\t/Eg\343\326\000\343E$\237\232\200>'#\352\221\260#\315\235\310\231\005\3324\207\375\345$]\276=<\361""\235\356\000\"\256_\006\030'Q\216\227\351\370${<(\023r\337\364O_FH\327{kc\347\315\006\237\200\270\017\243\253q\253\227\351\211dv\217\226O\373?\300|5m\014\222\314\344\3139\231\014\307\013\375 S \320\203\277q\007\026\341\352\347\360,\2009\346\253t\320\003x\271\314\257\003\256\226Hj\301\021|\256\204\r\224\326v\374eO\245\\\217a\"\3611\377'X\234\201\222\310\227\231|0\315\2534\234\007\276\242\205x\202\254T!\n6\304i`\336\247\274\002\307QP!\t\177\205\020^\016\177\212\005\275n\021\222\203\034Y\235\313\007S\010]\225\032\003dd\344\377J\360%>\237\021\202\202w\371\3400RqZ[\310\343\352\3505\317/H\325\363\375|9,\206UQ\021S\257\250\244\336lp\005$^K23\376.\005B\222A\274]\010\357G\340\356\021\277L\252O\005'\300\035W\006NQ\\.<\362\201\310\327\023\024$h\025\250|\235\020E\001\016\376\024\235\305\027\241\032\276\373\307\031\376\003\264\253Ib\274\362\361\312\262E\376e\270\205\332\t\313.J\016\316\363\362\000\030\315P\026 8\212%\324n\035\265\362\233^a\200\217I\024\201\343\321jo\202\246o\241\262O$\305\023\224\235\376\302E\307\365+\026_F\033u\017e\242*:/\321\035-\244\245\010\355\322\351y\372\274A\210G\037\242M\243\016*\315>\203a\257t\232jd\201\nc\231j\033\325\366\212$\014h\000\307\000jD\352*\342\345A\370\030\215\005\232.*\342O\243\203Qy\324p\221k\277\t\337<\210\0305\030\266^G\374\367\261zU\244Qj/\321\024D\233\361l\274\336\233\2406\223\370\277O\374yI\370\317\341\357M\034,\t?=FT\352ns\340\336?\"\221\035Y\232\224\2208\236\244\324\021)\342g\204\200\036\312Ne\220=\017\t\376_\022\034\203\343\277\346G\302\362\260\360\256\206\357\241\024\221\263\362\347\000\331b,J\301\003\240\000g\344\242\365\370`|\251'*\302\263p\026\304#\304\347\370\024\325\206\005\344|\032Z\3748\316\304k\330(\252J\271\277P\201}\264r\220\277\024\356\3059\3308:\262_\272\001\323\017\366.\275x\3073e\377\373\312\261\251\352\375\331\313\321,\265X\205\3010/\212\370\232\366\253\310\326\225W\240\032\303g\0145\332\365\034U\201\260\226P\211\377Cf\251\225\223\335\322+P\335FL/\201\350h\363\376\270]\225\270\030Wcud\324r\274\024WS\254\236\305""\213\275\352\177q\346\030V\303cGX}\030-\001\210\267ae\362\265a\335\313\022\377:\376#\314#MMP\335\333\226\271gX\367\010\273\216\010\tQ\224~\001\376Y\321f\367g\226\300\243:\010#R\374\n&\035\321\344J\022She)]\225\020\207'\303n\264\323_\371\366\367n\377\257\337'\031\244\004*1\211,$\375\243\347\200\363\305(\023\211&\3649&\227\245\006iD\rO\250\343_\251\353/T\271\377\246\254\006T\210\356\300\323\014\271\371\021\326h\204\214\260%m\014\016RV\036\244\003\225\177\002\001\347\3538";
    PyObject *data = __Pyx_DecompressString(cstring, 2901, 1);
    if (unlikely(!data)) __PYX_ERR(0, 1, __pyx_L1_error)
    const char* const bytes = __Pyx_PyBytes_AsString(data);
    #if !CYTHON_ASSUME_SAFE_MACROS
    if (likely(bytes)); else { Py_DECREF(data); __PYX_ERR(0, 1, __pyx_L1_error) }
    #endif
    #else
const char* const bytes = ">=Note that Cython is deliberately stricter than PEP-484 and rejects subclasses of builtin types. If you need to pass subclasses then set the 'annotation_typing' directive to False.><=<.?add_note_fast_match.pyxComparisonOpEQF_intF_phiGEGTLELTNE__Pyx_PyDict_NextRefactive_itemactive_scoreactive_scoresaddalt_candsalt_kalt_matches_per_wanchoranchor_idasyncio.coroutinesattr1attr2backward_stepsbest_scoresbridge_candsbridge_specsbridge_vwbucketc1c2c_intcandidate_setcg_incg_labelscg_outchild_intschild_var__class_getitem__cleanup_lazy_heap_fastcline_in_tracebackcompact_graphcounter_statecurrentcurrent_intcurrent_setcurrent_vardata_structurederive_bridge_vw_pairs_fastdis_cachedo_label_checkedge_infoedge_labeledge_lookupedges_infoelexpected_labelfailed_fast_matchfast_path_plansfilter_dis_items_fastfilteredforward_neighbor_mapforward_steps__func__getglobal_heapglobal_heap_scoreshas_prefilterhas_pw_evalhas_w_filterheapheapifyheappopheappushheapqhop_metaiin_neighborsint_prefilterint_set_is_coroutineis_forwardis_fwditemitem_attrsitemsjoin_okklast_varlhs_is_user__main__match_bridge_batchmatch_path_compact_fastmatch_star_paths_batch_fastmatching_c2smaxmax_v1max_v2mergedminmin_v1min_v2__module__n_childn_idn_intn_parent__name__needs_pw_evalneighbor_mapneighborsnext_counternext_setnext_varnode_label_mapnode_labelsoperatoroutout_neighborsp_idxp_intpairwise_plansparent_intsparent_varpath_resultpath_seqpath_sequencepath_wsper_v_matchespivot_intpivot_intspivot_matchplanpopprefilterprefilter_setspropagate_candidates_dag_fastprune_pairwise_values_fastpw_eval_fn__qualname__reachable_child_intsreachable_parent_intsrebuiltresultreverse_neighbor_maprhs_is_userroot_itemroot_score_safe_compare_predicate_valuesscoreseq__set_name__setdefaultshared_varshared_varsskip_checksskip_label_checksstep_counttau_dictterminal_var__test__thresholdunique_vals1unique_vals2update_global_best_and_tau_fastupdate_heap_and_tau_fastupdate_unique_topk_heap_fastuser_attrsvv1v2val_to_c2valid_wsvalid_z1valid_z2vals1vals2va""lue1value2valuesvarverify_pivot_pairwise_binding_fastww0_candsw0_varw_candidate_intsw_matchy0_cands\200\001\360\020\000\005\006\360\020\000\005\014\320\013#\2401\330\010\027\220\177\240k\260\021\330\010\030\320\030+\320+;\2701\200\001\360\020\000\005\035\230A\360\n\000\005\010\200t\2101\330\010\017\210q\340\004\007\200t\2109\220C\220t\2301\330\010\017\210q\220\006\220c\230\024\230U\240!\340\004\010\210\005\210Q\330\010\023\2201\330\010\014\210N\230-\240\177\260j\300\001\330\014\032\230-\240t\2501\250A\330\014\017\210t\2201\330\020\035\230Q\330\021\036\230c\240\021\330\020\032\230+\240T\250\021\250+\260T\270\022\2701\330\021\025\220Q\330\020\035\230Q\340\020\035\230Q\330\020\024\220E\230\021\330\024\036\320\036/\250t\2601\260A\330\024\027\220x\230s\240!\330\030\031\330\024\036\230a\330\024\030\230\016\240a\330\030'\240s\250!\250;\260d\270!\270?\310!\330\030$\240C\240q\250\007\250t\2601\260O\3001\330\030\033\230;\240c\250\021\330\034,\250E\260\021\260!\330\034)\250\025\250a\250q\330\030\033\2305\240\r\250R\250q\330\034&\240a\330\034\035\330\024\027\220q\330\030\037\230t\2401\240A\340\014\027\220{\240)\2503\250k\270\031\300\"\300A\330\014\017\210t\2201\330\020\021\340\010\021\220\021\220%\220|\2409\250G\260=\300\001\330\004\013\2101\200\001\340\004\034\230A\360\006\000\005\010\200y\220\003\2201\330\010\014\210E\220\021\330\014\024\220I\230T\240\022\240;\250a\330\014\017\210v\220S\230\005\230S\240\006\240c\250\021\330\020\026\220d\230!\2301\330\010\017\210q\340\004\007\200y\220\003\2201\330\010\014\210E\220\021\330\014\024\220I\230T\240\022\240;\250a\330\014\017\210v\220S\230\005\230S\240\006\240b\250\001\330\020\026\220d\230!\2301\330\010\017\210q\340\004\007\200y\220\003\2201\330\010\014\210E\220\021\330\014\024\220I\230T\240\022\240;\250a\330\014\017\210v\220S\230\005\230S\240\006\240c\250\021\330\020\026\220d\230!\2301\330\010\017\210q\340\004\007\200y\220\003\2201\330\010\014\210E\220\021\330\014\024\220I\230T\240\022\240;\250a\330\014\017\210v\220S\230\005\230S""\240\006\240b\250\001\330\020\026\220d\230!\2301\330\010\017\210q\340\004\013\2103\210a\210q\200\001\360\n\000\005\006\360\020\000\005\032\230\021\360\010\000\005\t\210\005\210T\320\021!\240\035\320.?\270q\330\010\026\320\026.\250a\330\014\r\330\014\r\330\014\r\330\014\r\330\014\r\330\014\r\330\014\r\330\014\032\320\032-\250Q\340\010\013\210<\220s\230!\330\014\023\2201\330\010\014\210E\220\033\230K\240v\250Q\330\014\017\210t\2203\220a\330\020\026\220a\220w\230f\240A\240U\250\"\250A\330\020\023\2204\220v\230Q\230a\330\024\033\2301\340\020\026\220a\220w\230c\240\021\240!\330\004\013\2101\200\001\360\016\000\005\037\230a\330\004\036\230a\360\n\000\005\010\200t\2106\220\023\220D\230\001\330\010\017\210z\230\021\340\004\007\200y\220\003\220<\230q\330\010\024\220A\330\010\014\210D\220\006\220e\2306\240\021\330\014\025\220Y\230d\240!\2401\330\014\017\210w\220c\230\021\330\020\034\230A\330\020\031\230\021\230&\240\001\330\014\022\220$\220a\220q\330\010\014\210D\220\006\220e\2306\240\021\330\014\033\2309\240D\250\001\250\021\330\014\017\210q\330\020\030\230\004\230A\230Q\330\020\034\230A\330\010\017\210z\230\021\340\004\007\200y\220\003\220<\230q\330\010\027\220s\230!\2305\240\007\240q\330\010\027\220s\230!\2305\240\007\240q\330\010\014\210D\220\006\220e\2306\240\021\330\014\017\210s\220!\220>\240\022\2402\240T\250\035\260d\270#\270W\300A\330\020\030\230\004\230A\230Q\330\010\014\210D\220\006\220e\2306\240\021\330\014\017\210s\220!\220>\240\022\2402\240T\250\035\260d\270#\270W\300A\330\020\030\230\004\230A\230Q\330\010\017\210z\230\021\340\004\007\200y\220\003\220<\230t\2403\240i\250s\260,\270a\330\010\t\330\014\025\220S\230\001\230\025\230g\240Q\330\014\025\220S\230\001\230\025\230g\240Q\330\014\017\210y\230\003\230<\240q\330\020\024\220D\230\006\230e\2406\250\021\330\024\027\220s\230\"\230A\330\030 \240\004\240A\240Q\330\020\024\220D\230\006\230e\2406\250\021\330\024\027\220w\230b\240\001\330\030 \240\004\240A\240Q\340\020\024\220D\230\006\230e\2406\250\021\330\024\027""\220s\230#\230Q\330\030 \240\004\240A\240Q\330\020\024\220D\230\006\230e\2406\250\021\330\024\027\220w\230c\240\021\330\030 \240\004\240A\240Q\330\014\023\220:\230Q\330\017\020\360\006\000\005\010\200y\220\003\220<\230t\2403\240i\250s\260,\270a\330\010\t\330\014\025\220S\230\001\230\025\230g\240Q\330\014\025\220S\230\001\230\025\230g\240Q\330\014\017\210y\230\003\230<\240q\330\020\024\220D\230\006\230e\2406\250\021\330\024\027\220s\230\"\230A\330\030 \240\004\240A\240Q\330\020\024\220D\230\006\230e\2406\250\021\330\024\027\220w\230b\240\001\330\030 \240\004\240A\240Q\340\020\024\220D\230\006\230e\2406\250\021\330\024\027\220s\230#\230Q\330\030 \240\004\240A\240Q\330\020\024\220D\230\006\230e\2406\250\021\330\024\027\220w\230c\240\021\330\030 \240\004\240A\240Q\330\014\023\220:\230Q\330\017\020\360\006\000\005\t\210\004\210F\220%\220v\230Q\330\010\014\210D\220\006\220e\2306\240\021\330\014\r\330\020\023\320\0231\260\021\260$\260d\270!\330\024\034\230D\240\001\240\021\330\024\034\230D\240\001\240\021\330\024\037\230q\330\020\021\340\004\013\210:\220Q\200\001\360\016\000\005\t\210\010\220\001\330\010\026\220d\230!\2301\330\010\020\220\004\220A\220Q\330\010\026\220d\230!\2301\330\010\020\220\004\220A\220Q\330\010\023\2204\220q\230\001\340\010\021\220\032\2304\230q\240\n\320*;\270:\300T\310\021\310!\330\010\021\220\032\2304\230q\240\n\320*;\270:\300T\310\021\310!\330\010\013\2107\220#\220U\230#\230W\240C\240q\330\014\023\2201\330\010\013\2104\320\017-\250Q\250h\260h\270a\330\014\023\2201\340\004\013\2101\200\001\360\014\000\005\010\200r\210\022\2101\330\010\014\210A\340\004\016\210m\2304\230q\240\001\330\004\007\200x\210w\220a\330\010\013\2106\220\023\220A\330\014\r\330\010\025\220Q\220f\230A\330\010\027\220}\240A\240Q\330\010\025\220Q\220h\230a\330\010\r\210Y\220a\220w\230g\240^\2601\330\t\014\210A\210_\230B\230a\330\010\025\220Q\220f\230A\330\010\027\220}\240A\240Q\330\010\025\220Q\220h\230a\330\010\r\210Y\220a\220w\230g\240^\2601\340\010\036\230a\230v\240Q\330\010""\013\2104\210q\330\014\031\230\021\230&\240\001\330\014\033\230=\250\001\250\021\330\014\031\230\021\230(\240!\330\014\021\220\031\230!\2307\240'\250\036\260q\340\014\031\230\024\230Q\230b\240\001\240\021\330\014\030\230\004\230A\230R\230q\240\001\330\014\017\210v\220S\230\001\330\020\021\330\014\021\220\030\230\021\230!\330\014\031\230\024\230Q\230k\250\021\330\014\031\230\021\230&\240\001\330\014\033\230=\250\001\250\021\330\014\031\230\021\230(\240!\330\014\021\220\031\230!\2307\240'\250\036\260q\340\004\007\200s\210!\2106\220\026\220t\2302\230R\230q\330\010\022\220!\330\010\014\210M\320\031)\250\035\260f\270A\330\014\031\230\021\230&\240\001\330\014\023\2207\230\"\230N\250-\260q\270\004\270A\330\010\014\210F\220!\330\010\r\210X\220Q\220a\200\001\360\020\000\005\006\330\004\005\360\030\000\005\010\200t\2109\220C\220t\2301\330\010\017\210q\340\004\021\220\023\220A\220Z\230r\240\021\330\004\007\200{\220#\220Q\330\010\016\210a\330\010\014\210I\220U\230!\2303\230a\230q\330\014\017\210q\220\n\230!\230;\240h\250a\250u\260J\270a\270q\330\010\017\210q\360\006\000\005\016\210]\230!\330\004\014\210M\230\021\330\004\020\220\r\230Q\360\006\000\005\020\210q\330\004\010\210\005\210U\220!\2201\330\010\026\220h\230a\230q\330\010\023\2208\2301\230B\230b\240\001\330\010\024\220K\230t\2402\240]\260!\330\010\013\210:\220S\230\001\340\014\022\220!\330\014\020\220\t\230\025\230a\230s\240!\2401\330\020\023\2201\220J\230a\230z\250\021\330\014\023\2201\330\010\020\220\007\220q\330\014\r\330\014\r\330\014\025\220Q\220a\330\014\025\220Q\220a\330\014\032\230$\230a\230q\330\014\020\320\020!\240\024\240Q\240c\250\021\330\014\031\230\024\230Q\230a\360\006\000\005\023\220+\230W\240A\330\004\017\210x\220q\230\001\330\004\023\320\023$\240G\2501\340\004\n\210!\330\004\010\210\t\220\025\220a\220s\230!\2301\330\010\024\220J\230a\230q\330\010\022\220(\230!\2305\240\001\330\010\021\220\021\340\010\014\210E\220\025\220a\220q\330\014\033\2308\2401\240B\240a\240q\330\014\033\2308\2401\240B\240a\240q\330""\014\033\2308\2401\240B\240a\240q\330\014\033\2308\2401\240B\240a\240q\330\014\035\230X\240Q\240b\250\001\250\021\330\014\035\230X\240Q\240b\250\001\250\021\330\014\033\2308\2401\240B\240a\240q\330\014\034\230J\240g\250Q\340\014\032\230&\240\001\240\021\330\014\032\230!\340\014\020\220\017\230q\330\020\023\2201\330\024 \240\006\240d\250\"\250M\270\021\340\024 \240\005\240T\250\022\250=\270\001\330\020\023\220:\230S\240\001\330\024\025\330\020\024\220H\230A\330\024\027\220\177\240d\250)\2601\260F\270#\270Q\330\030\031\330\024\027\220q\330\030\033\2305\240\007\240q\330\034\035\330\031\032\330\030\033\2304\230z\250\021\250*\260A\330\034\035\330\024\034\230D\240\001\240\021\340\014\017\210t\2201\330\020\031\230\021\330\020\021\330\014\022\220!\220<\230q\340\010\013\2101\330\014\017\210q\220\r\230Q\330\014\r\360\006\000\t\014\210=\230\004\230I\240S\250\001\330\014\027\220v\230Q\230j\250\002\250!\330\014\017\210t\2201\330\020\023\2201\220M\240\021\330\020\021\330\014\022\220!\220<\230q\340\010\013\2101\210M\230\021\340\004\013\2101\200\001\340\004\n\210%\210t\220=\240\004\240A\240T\250\021\250\"\250A\250T\260\023\260D\270\001\270\022\2701\270A\330\010\r\210X\220Q\220a\200\001\360\032\000\005\010\200t\210>\230\024\230T\240\021\330\010\017\210q\340\004\r\210]\230!\330\004\014\210M\230\021\340\004\010\210\014\220K\230~\250Q\330\010\026\220e\2304\230q\240\001\330\010\025\220U\230$\230a\230q\330\010\013\2104\210|\2303\230d\240!\330\014\023\2201\340\010\"\240!\330\010\022\220#\220Q\220a\330\010\014\210L\230\n\240!\330\014\017\210s\220!\220=\240\003\2403\240a\240q\330\020\037\230z\250\034\260Q\330\020\024\220I\230Q\330\024 \240\014\250D\260\002\260'\270\021\330\024\027\220z\240\023\240A\330\030\031\330\024\030\230\t\240\021\330\030\033\2306\240\023\240A\330\0340\260\004\260A\260Q\330\024\027\220s\230!\320\0331\260\023\260A\330\030\031\340\020'\240y\260\014\270A\330\020\024\220I\230Q\330\024 \320 4\260D\270\002\270'\300\021\330\024\027\220z\240\023\240A\330\030\031\330\024\030""\230\t\240\021\330\030\033\2306\240\023\240A\330\0340\260\004\260A\260Q\330\034\035\330\024\027\220s\230!\320\0331\260\023\260A\330\030\031\330\014\017\210s\220!\320\023)\250\023\250A\330\020\021\340\010\013\2104\210q\330\014\023\2201\330\010\r\210Q\210m\2301\340\004\010\210\014\220K\230~\250Q\330\010\025\220U\230$\230a\230q\330\010\026\220e\2304\230q\240\001\330\010\013\2104\210{\230#\230T\240\021\330\014\023\2201\340\010#\2401\330\010\023\2203\220a\220q\330\010\014\210L\230\n\240!\330\014\017\210s\220!\220<\230s\240#\240Q\240a\330\020'\240y\260\014\270A\330\020\024\220I\230Q\330\024 \320 4\260D\270\002\270'\300\021\330\024\027\220z\240\023\240A\330\030\031\330\024\030\230\t\240\021\330\030\033\2306\240\023\240A\330\0341\260\024\260Q\260a\330\024\027\220s\230!\320\0332\260#\260Q\330\030\031\340\020'\240z\260\034\270Q\330\020\024\220I\230Q\330\024 \320 4\260D\270\002\270'\300\021\330\024\027\220z\240\023\240A\330\030\031\330\024\030\230\t\240\021\330\030\033\2306\240\023\240A\330\0341\260\024\260Q\260a\330\034\035\330\024\027\220s\230!\320\0332\260#\260Q\330\030\031\330\014\017\210s\220!\320\023*\250#\250Q\330\020\021\340\010\013\2104\210q\330\014\023\2201\330\010\r\210Q\210n\230A\340\004\013\2101\200\001\360\010\000\005\010\200v\210R\210q\330\010\020\220\001\340\004\016\210k\230\024\230Q\230a\330\004\007\200x\210w\220e\2304\230v\240S\250\001\330\010\036\230a\230}\250A\330\010\013\2103\210a\320\017#\2403\240f\250D\260\001\330\014\023\220;\230a\230r\240\021\240!\330\010\017\210q\340\004\017\210q\220\010\230\001\330\004 \240\001\240\026\240w\250m\320;O\310w\320VW\330\004\032\230!\230=\250\001\330\004\007\200s\210!\320\013\037\230s\240&\250\004\250A\330\010\017\210{\230!\2302\230Q\230a\330\004\013\2101\200\001\340\004 \240\001\240\026\240w\250f\260O\3003\300a\330\004\032\230!\2306\240\021\330\004\007\200s\210!\210?\230$\230e\2402\240R\240w\250c\260\024\260Q\330\010\020\220\001\220\032\2304\230q\240\002\240!\2401\340\010\020\220\001\220\032\2301";
    PyObject *data = NULL;
    CYTHON_UNUSED_VAR(__Pyx_DecompressString);
    #endif
    PyObject **stringtab = __pyx_mstate->__pyx_string_tab;
    Py_ssize_t pos = 0;
    for (int i = 0; i < 197; i++) {
      Py_ssize_t bytes_length = index[i].length;
      PyObject *string = PyUnicode_DecodeUTF8(bytes + pos, bytes_length, NULL);
      if (likely(string) && i >= 9) PyUnicode_InternInPlace(&string);
      if (unlikely(!string)) {
        Py_XDECREF(data);
        __PYX_ERR(0, 1, __pyx_L1_error)
      }
      stringtab[i] = string;
      pos += bytes_length;
    }
    for (int i = 197; i < 209; i++) {
      Py_ssize_t bytes_length = index[i].length;
      PyObject *string = PyBytes_FromStringAndSize(bytes + pos, bytes_length);
      stringtab[i] = string;
      pos += bytes_length;
      if (unlikely(!string)) {
        Py_XDECREF(data);
        __PYX_ERR(0, 1, __pyx_L1_error)
      }
    }
    Py_XDECREF(data);
    for (Py_ssize_t i = 0; i < 209; i++) {
      if (unlikely(PyObject_Hash(stringtab[i]) == -1)) {
        __PYX_ERR(0, 1, __pyx_L1_error)
      }
    }
    #if CYTHON_IMMORTAL_CONSTANTS
    {
      PyObject **table = stringtab + 197;
      for (Py_ssize_t i=0; i<12; ++i) {
        #if CYTHON_COMPILING_IN_CPYTHON_FREETHREADING
        #if PY_VERSION_HEX < 0x030E0000
        if (_Py_IsOwnedByCurrentThread(table[i]) && Py_REFCNT(table[i]) == 1)
        #else
        if (PyUnstable_Object_IsUniquelyReferenced(table[i]))
        #endif
        {
          Py_SET_REFCNT(table[i], _Py_IMMORTAL_REFCNT_LOCAL);
        }
        #else
        Py_SET_REFCNT(table[i], _Py_IMMORTAL_INITIAL_REFCNT);
        #endif
      }
    }
    #endif
  }
  {
    PyObject **numbertab = __pyx_mstate->__pyx_number_tab;
    double const c_constants[] = {0.0};
    for (int i = 0; i < 1; i++) {
      numbertab[i] = PyFloat_FromDouble(c_constants[i]);
      if (unlikely(!numbertab[i])) __PYX_ERR(0, 1, __pyx_L1_error)
    }
  }
  {
    PyObject **numbertab = __pyx_mstate->__pyx_number_tab + 1;
    int8_t const cint_constants_1[] = {1};
    for (int i = 0; i < 1; i++) {
      numbertab[i] = PyLong_FromLong(cint_constants_1[i - 0]);
      if (unlikely(!numbertab[i])) __PYX_ERR(0, 1, __pyx_L1_error)
    }
  }
  #if CYTHON_IMMORTAL_CONSTANTS
  {
    PyObject **table = __pyx_mstate->__pyx_number_tab;
    for (Py_ssize_t i=0; i<2; ++i) {
      #if CYTHON_COMPILING_IN_CPYTHON_FREETHREADING
      #if PY_VERSION_HEX < 0x030E0000
      if (_Py_IsOwnedByCurrentThread(table[i]) && Py_REFCNT(table[i]) == 1)
      #else
      if (PyUnstable_Object_IsUniquelyReferenced(table[i]))
      #endif
      {
        Py_SET_REFCNT(table[i], _Py_IMMORTAL_REFCNT_LOCAL);
      }
      #else
      Py_SET_REFCNT(table[i], _Py_IMMORTAL_INITIAL_REFCNT);
      #endif
    }
  }
  #endif
  return 0;
  __pyx_L1_error:;
  return -1;
}

typedef struct {
    unsigned int argcount : 4;
    unsigned int num_posonly_args : 1;
    unsigned int num_kwonly_args : 1;
    unsigned int nlocals : 6;
    unsigned int flags : 10;
    unsigned int first_line : 10;
} __Pyx_PyCode_New_function_description;

static PyObject* __Pyx_PyCode_New(
        const __Pyx_PyCode_New_function_description descr,
        PyObject * const *varnames,
        PyObject *filename,
        PyObject *funcname,
        PyObject *line_table,
        PyObject *tuple_dedup_map
);

static int __Pyx_CreateCodeObjects(__pyx_mstatetype *__pyx_mstate) {
  PyObject* tuple_dedup_map = PyDict_New();
  if (unlikely(!tuple_dedup_map)) return -1;
  {
    const __Pyx_PyCode_New_function_description descr = {8, 0, 0, 8, (unsigned int)(CO_OPTIMIZED|CO_NEWLOCALS), 95};
    PyObject* const varnames[] = {__pyx_mstate->__pyx_n_u_compact_graph, __pyx_mstate->__pyx_n_u_path_sequence, __pyx_mstate->__pyx_n_u_pivot_int, __pyx_mstate->__pyx_n_u_edge_lookup, __pyx_mstate->__pyx_n_u_node_label_map, __pyx_mstate->__pyx_n_u_skip_label_checks, __pyx_mstate->__pyx_n_u_prefilter_sets, __pyx_mstate->__pyx_n_u_pw_eval_fn};
    __pyx_mstate_global->__pyx_codeobj_tab[0] = __Pyx_PyCode_New(descr, varnames, __pyx_mstate->__pyx_kp_u_fast_match_pyx, __pyx_mstate->__pyx_n_u_match_path_compact_fast, __pyx_mstate->__pyx_kp_b_iso88591_1_k_1, tuple_dedup_map); if (unlikely(!__pyx_mstate_global->__pyx_codeobj_tab[0])) goto bad;
  }
  {
    const __Pyx_PyCode_New_function_description descr = {5, 0, 0, 14, (unsigned int)(CO_OPTIMIZED|CO_NEWLOCALS), 117};
    PyObject* const varnames[] = {__pyx_mstate->__pyx_n_u_compact_graph, __pyx_mstate->__pyx_n_u_fast_path_plans, __pyx_mstate->__pyx_n_u_pivot_int, __pyx_mstate->__pyx_n_u_int_prefilter, __pyx_mstate->__pyx_n_u_pw_eval_fn, __pyx_mstate->__pyx_n_u_merged, __pyx_mstate->__pyx_n_u_seq, __pyx_mstate->__pyx_n_u_el, __pyx_mstate->__pyx_n_u_node_label_map, __pyx_mstate->__pyx_n_u_skip_checks, __pyx_mstate->__pyx_n_u_path_result, __pyx_mstate->__pyx_n_u_needs_pw_eval, __pyx_mstate->__pyx_n_u_var, __pyx_mstate->__pyx_n_u_int_set};
    __pyx_mstate_global->__pyx_codeobj_tab[1] = __Pyx_PyCode_New(descr, varnames, __pyx_mstate->__pyx_kp_u_fast_match_pyx, __pyx_mstate->__pyx_n_u_match_star_paths_batch_fast, __pyx_mstate->__pyx_kp_b_iso88591_T_q_a_Q_s_1_E_KvQ_t3a_awfAU_A_4, tuple_dedup_map); if (unlikely(!__pyx_mstate_global->__pyx_codeobj_tab[1])) goto bad;
  }
  {
    const __Pyx_PyCode_New_function_description descr = {9, 0, 0, 38, (unsigned int)(CO_OPTIMIZED|CO_NEWLOCALS), 157};
    PyObject* const varnames[] = {__pyx_mstate->__pyx_n_u_compact_graph, __pyx_mstate->__pyx_n_u_path_seq, __pyx_mstate->__pyx_n_u_pivot_ints, __pyx_mstate->__pyx_n_u_edge_lookup, __pyx_mstate->__pyx_n_u_node_label_map, __pyx_mstate->__pyx_n_u_skip_label_checks, __pyx_mstate->__pyx_n_u_int_prefilter, __pyx_mstate->__pyx_n_u_pw_eval_fn, __pyx_mstate->__pyx_n_u_w_candidate_ints, __pyx_mstate->__pyx_n_u_i, __pyx_mstate->__pyx_n_u_step_count, __pyx_mstate->__pyx_n_u_n_id, __pyx_mstate->__pyx_n_u_p_idx, __pyx_mstate->__pyx_n_u_is_forward, __pyx_mstate->__pyx_n_u_do_label_check, __pyx_mstate->__pyx_n_u_has_prefilter, __pyx_mstate->__pyx_n_u_has_pw_eval, __pyx_mstate->__pyx_n_u_out, __pyx_mstate->__pyx_n_u_cg_out, __pyx_mstate->__pyx_n_u_cg_in, __pyx_mstate->__pyx_n_u_cg_labels, __pyx_mstate->__pyx_n_u_hop_meta, __pyx_mstate->__pyx_n_u_current_var, __pyx_mstate->__pyx_n_u_next_var, __pyx_mstate->__pyx_n_u_edge_info, __pyx_mstate->__pyx_n_u_last_var, __pyx_mstate->__pyx_n_u_has_w_filter, __pyx_mstate->__pyx_n_u_pivot_int, __pyx_mstate->__pyx_n_u_result, __pyx_mstate->__pyx_n_u_failed, __pyx_mstate->__pyx_n_u_edge_label, __pyx_mstate->__pyx_n_u_expected_label, __pyx_mstate->__pyx_n_u_prefilter, __pyx_mstate->__pyx_n_u_current_set, __pyx_mstate->__pyx_n_u_next_set, __pyx_mstate->__pyx_n_u_current_int, __pyx_mstate->__pyx_n_u_neighbors, __pyx_mstate->__pyx_n_u_filtered};
    __pyx_mstate_global->__pyx_codeobj_tab[2] = __Pyx_PyCode_New(descr, varnames, __pyx_mstate->__pyx_kp_u_fast_match_pyx, __pyx_mstate->__pyx_n_u_match_bridge_batch, __pyx_mstate->__pyx_kp_b_iso88591_t9Ct1_q_AZr_Q_a_IU_3aq_q_hauJaq, tuple_dedup_map); if (unlikely(!__pyx_mstate_global->__pyx_codeobj_tab[2])) goto bad;
  }
  {
    const __Pyx_PyCode_New_function_description descr = {5, 0, 0, 21, (unsigned int)(CO_OPTIMIZED|CO_NEWLOCALS), 278};
    PyObject* const varnames[] = {__pyx_mstate->__pyx_n_u_y0_cands, __pyx_mstate->__pyx_n_u_w0_cands, __pyx_mstate->__pyx_n_u_bridge_specs, __pyx_mstate->__pyx_n_u_alt_matches_per_w, __pyx_mstate->__pyx_n_u_F_phi, __pyx_mstate->__pyx_n_u_bridge_vw, __pyx_mstate->__pyx_n_u_valid_ws, __pyx_mstate->__pyx_n_u_path_ws, __pyx_mstate->__pyx_n_u_pivot_match, __pyx_mstate->__pyx_n_u_w_match, __pyx_mstate->__pyx_n_u_terminal_var, __pyx_mstate->__pyx_n_u_shared_vars, __pyx_mstate->__pyx_n_u_per_v_matches, __pyx_mstate->__pyx_n_u_w0_var, __pyx_mstate->__pyx_n_u_shared_var, __pyx_mstate->__pyx_n_u_bridge_cands, __pyx_mstate->__pyx_n_u_alt_cands, __pyx_mstate->__pyx_n_u_v, __pyx_mstate->__pyx_n_u_w, __pyx_mstate->__pyx_n_u_join_ok, __pyx_mstate->__pyx_n_u_v};
    __pyx_mstate_global->__pyx_codeobj_tab[3] = __Pyx_PyCode_New(descr, varnames, __pyx_mstate->__pyx_kp_u_fast_match_pyx, __pyx_mstate->__pyx_n_u_derive_bridge_vw_pairs_fast, __pyx_mstate->__pyx_kp_b_iso88591_A_t1_q_t9Ct1_q_c_U_Q_1_N_j_t1A, tuple_dedup_map); if (unlikely(!__pyx_mstate_global->__pyx_codeobj_tab[3])) goto bad;
  }
  {
    const __Pyx_PyCode_New_function_description descr = {4, 0, 0, 24, (unsigned int)(CO_OPTIMIZED|CO_NEWLOCALS), 334};
    PyObject* const varnames[] = {__pyx_mstate->__pyx_n_u_compact_graph, __pyx_mstate->__pyx_n_u_forward_steps, __pyx_mstate->__pyx_n_u_backward_steps, __pyx_mstate->__pyx_n_u_F_int, __pyx_mstate->__pyx_n_u_n_child, __pyx_mstate->__pyx_n_u_n_parent, __pyx_mstate->__pyx_n_u_parent_var, __pyx_mstate->__pyx_n_u_child_var, __pyx_mstate->__pyx_n_u_edges_info, __pyx_mstate->__pyx_n_u_edge_label, __pyx_mstate->__pyx_n_u_parent_ints, __pyx_mstate->__pyx_n_u_child_ints, __pyx_mstate->__pyx_n_u_reachable_child_ints, __pyx_mstate->__pyx_n_u_reachable_parent_ints, __pyx_mstate->__pyx_n_u_is_fwd, __pyx_mstate->__pyx_n_u_cg_out, __pyx_mstate->__pyx_n_u_cg_in, __pyx_mstate->__pyx_n_u_neighbor_map, __pyx_mstate->__pyx_n_u_p_int, __pyx_mstate->__pyx_n_u_neighbors, __pyx_mstate->__pyx_n_u_n_int, __pyx_mstate->__pyx_n_u_reverse_neighbor_map, __pyx_mstate->__pyx_n_u_c_int, __pyx_mstate->__pyx_n_u_forward_neighbor_map};
    __pyx_mstate_global->__pyx_codeobj_tab[4] = __Pyx_PyCode_New(descr, varnames, __pyx_mstate->__pyx_kp_u_fast_match_pyx, __pyx_mstate->__pyx_n_u_propagate_candidates_dag_fast, __pyx_mstate->__pyx_kp_b_iso88591_t_T_q_M_K_Q_e4q_U_aq_4_3d_1_Qa, tuple_dedup_map); if (unlikely(!__pyx_mstate_global->__pyx_codeobj_tab[4])) goto bad;
  }
  {
    const __Pyx_PyCode_New_function_description descr = {3, 0, 0, 18, (unsigned int)(CO_OPTIMIZED|CO_NEWLOCALS), 434};
    PyObject* const varnames[] = {__pyx_mstate->__pyx_n_u_vals1, __pyx_mstate->__pyx_n_u_vals2, __pyx_mstate->__pyx_n_u_operator, __pyx_mstate->__pyx_n_u_valid_z1, __pyx_mstate->__pyx_n_u_valid_z2, __pyx_mstate->__pyx_n_u_c1, __pyx_mstate->__pyx_n_u_v1, __pyx_mstate->__pyx_n_u_c2, __pyx_mstate->__pyx_n_u_v2, __pyx_mstate->__pyx_n_u_matching_c2s, __pyx_mstate->__pyx_n_u_unique_vals1, __pyx_mstate->__pyx_n_u_unique_vals2, __pyx_mstate->__pyx_n_u_max_v2, __pyx_mstate->__pyx_n_u_min_v1, __pyx_mstate->__pyx_n_u_min_v2, __pyx_mstate->__pyx_n_u_max_v1, __pyx_mstate->__pyx_n_u_val_to_c2, __pyx_mstate->__pyx_n_u_bucket};
    __pyx_mstate_global->__pyx_codeobj_tab[5] = __Pyx_PyCode_New(descr, varnames, __pyx_mstate->__pyx_kp_u_fast_match_pyx, __pyx_mstate->__pyx_n_u_prune_pairwise_values_fast, __pyx_mstate->__pyx_kp_b_iso88591_a_a_t6_D_z_y_q_A_D_e6_Yd_1_wc_A, tuple_dedup_map); if (unlikely(!__pyx_mstate_global->__pyx_codeobj_tab[5])) goto bad;
  }
  {
    const __Pyx_PyCode_New_function_description descr = {5, 0, 0, 8, (unsigned int)(CO_OPTIMIZED|CO_NEWLOCALS), 532};
    PyObject* const varnames[] = {__pyx_mstate->__pyx_n_u_dis_cache, __pyx_mstate->__pyx_n_u_anchor_id, __pyx_mstate->__pyx_n_u_operator, __pyx_mstate->__pyx_n_u_threshold, __pyx_mstate->__pyx_n_u_candidate_set, __pyx_mstate->__pyx_n_u_result, __pyx_mstate->__pyx_n_u_w, __pyx_mstate->__pyx_n_u_score};
    __pyx_mstate_global->__pyx_codeobj_tab[6] = __Pyx_PyCode_New(descr, varnames, __pyx_mstate->__pyx_kp_u_fast_match_pyx, __pyx_mstate->__pyx_n_u_filter_dis_items_fast, __pyx_mstate->__pyx_kp_b_iso88591_A_y_1_E_IT_a_vS_S_c_d_1_q_y_1_E, tuple_dedup_map); if (unlikely(!__pyx_mstate_global->__pyx_codeobj_tab[6])) goto bad;
  }
  {
    const __Pyx_PyCode_New_function_description descr = {3, 0, 0, 11, (unsigned int)(CO_OPTIMIZED|CO_NEWLOCALS), 568};
    PyObject* const varnames[] = {__pyx_mstate->__pyx_n_u_user_attrs, __pyx_mstate->__pyx_n_u_item_attrs, __pyx_mstate->__pyx_n_u_pairwise_plans, __pyx_mstate->__pyx_n_u_plan, __pyx_mstate->__pyx_n_u_attr1, __pyx_mstate->__pyx_n_u_attr2, __pyx_mstate->__pyx_n_u_operator, __pyx_mstate->__pyx_n_u_value1, __pyx_mstate->__pyx_n_u_value2, __pyx_mstate->__pyx_n_u_lhs_is_user, __pyx_mstate->__pyx_n_u_rhs_is_user};
    __pyx_mstate_global->__pyx_codeobj_tab[7] = __Pyx_PyCode_New(descr, varnames, __pyx_mstate->__pyx_kp_u_fast_match_pyx, __pyx_mstate->__pyx_n_u_verify_pivot_pairwise_binding_fa, __pyx_mstate->__pyx_kp_b_iso88591_d_1_AQ_d_1_AQ_4q_4q_T_4q_T_7_U, tuple_dedup_map); if (unlikely(!__pyx_mstate_global->__pyx_codeobj_tab[7])) goto bad;
  }
  {
    const __Pyx_PyCode_New_function_description descr = {2, 0, 0, 2, (unsigned int)(CO_OPTIMIZED|CO_NEWLOCALS), 592};
    PyObject* const varnames[] = {__pyx_mstate->__pyx_n_u_heap, __pyx_mstate->__pyx_n_u_active_scores};
    __pyx_mstate_global->__pyx_codeobj_tab[8] = __Pyx_PyCode_New(descr, varnames, __pyx_mstate->__pyx_kp_u_fast_match_pyx, __pyx_mstate->__pyx_n_u_cleanup_lazy_heap_fast, __pyx_mstate->__pyx_kp_b_iso88591_t_AT_AT_D_1A_XQa, tuple_dedup_map); if (unlikely(!__pyx_mstate_global->__pyx_codeobj_tab[8])) goto bad;
  }
  {
    const __Pyx_PyCode_New_function_description descr = {6, 0, 0, 13, (unsigned int)(CO_OPTIMIZED|CO_NEWLOCALS), 598};
    PyObject* const varnames[] = {__pyx_mstate->__pyx_n_u_item, __pyx_mstate->__pyx_n_u_score, __pyx_mstate->__pyx_n_u_heap, __pyx_mstate->__pyx_n_u_active_scores, __pyx_mstate->__pyx_n_u_k, __pyx_mstate->__pyx_n_u_counter_state, __pyx_mstate->__pyx_n_u_current, __pyx_mstate->__pyx_n_u_root_item, __pyx_mstate->__pyx_n_u_active_item, __pyx_mstate->__pyx_n_u_active_score, __pyx_mstate->__pyx_n_u_root_score, __pyx_mstate->__pyx_n_u_next_counter, __pyx_mstate->__pyx_n_u_rebuilt};
    __pyx_mstate_global->__pyx_codeobj_tab[9] = __Pyx_PyCode_New(descr, varnames, __pyx_mstate->__pyx_kp_u_fast_match_pyx, __pyx_mstate->__pyx_n_u_update_unique_topk_heap_fast, __pyx_mstate->__pyx_kp_b_iso88591_r_1_A_m4q_xwa_6_A_QfA_AQ_Qha_Ya, tuple_dedup_map); if (unlikely(!__pyx_mstate_global->__pyx_codeobj_tab[9])) goto bad;
  }
  {
    const __Pyx_PyCode_New_function_description descr = {8, 0, 0, 8, (unsigned int)(CO_OPTIMIZED|CO_NEWLOCALS), 648};
    PyObject* const varnames[] = {__pyx_mstate->__pyx_n_u_anchor, __pyx_mstate->__pyx_n_u_item, __pyx_mstate->__pyx_n_u_score, __pyx_mstate->__pyx_n_u_heap, __pyx_mstate->__pyx_n_u_tau_dict, __pyx_mstate->__pyx_n_u_k, __pyx_mstate->__pyx_n_u_active_scores, __pyx_mstate->__pyx_n_u_counter_state};
    __pyx_mstate_global->__pyx_codeobj_tab[10] = __Pyx_PyCode_New(descr, varnames, __pyx_mstate->__pyx_kp_u_fast_match_pyx, __pyx_mstate->__pyx_n_u_update_heap_and_tau_fast, __pyx_mstate->__pyx_kp_b_iso88591_wfO3a_6_s_e2Rwc_Q_4q_1_1, tuple_dedup_map); if (unlikely(!__pyx_mstate_global->__pyx_codeobj_tab[10])) goto bad;
  }
  {
    const __Pyx_PyCode_New_function_description descr = {7, 0, 0, 8, (unsigned int)(CO_OPTIMIZED|CO_NEWLOCALS), 658};
    PyObject* const varnames[] = {__pyx_mstate->__pyx_n_u_item, __pyx_mstate->__pyx_n_u_score, __pyx_mstate->__pyx_n_u_best_scores, __pyx_mstate->__pyx_n_u_global_heap, __pyx_mstate->__pyx_n_u_global_heap_scores, __pyx_mstate->__pyx_n_u_alt_k, __pyx_mstate->__pyx_n_u_counter_state, __pyx_mstate->__pyx_n_u_current};
    __pyx_mstate_global->__pyx_codeobj_tab[11] = __Pyx_PyCode_New(descr, varnames, __pyx_mstate->__pyx_kp_u_fast_match_pyx, __pyx_mstate->__pyx_n_u_update_global_best_and_tau_fast, __pyx_mstate->__pyx_kp_b_iso88591_vRq_k_Qa_xwe4vS_a_A_3a_3fD_ar_q, tuple_dedup_map); if (unlikely(!__pyx_mstate_global->__pyx_codeobj_tab[11])) goto bad;
  }
  Py_DECREF(tuple_dedup_map);
  return 0;
  bad:
  Py_DECREF(tuple_dedup_map);
  return -1;
}

static int __Pyx_InitGlobals(void) {

  if (likely(__Pyx_init_co_variables() == 0)); else

  if (unlikely(PyErr_Occurred())) __PYX_ERR(0, 1, __pyx_L1_error)

  if (likely(__pyx_CommonTypesMetaclass_init(__pyx_m) == 0)); else

  if (unlikely(PyErr_Occurred())) __PYX_ERR(0, 1, __pyx_L1_error)

  #if CYTHON_COMPILING_IN_LIMITED_API
  {
      PyObject *typesModule=NULL;
      typesModule = PyImport_ImportModule("types");
      if (typesModule) {
          __pyx_mstate_global->__Pyx_CachedMethodType = PyObject_GetAttrString(typesModule, "MethodType");
          Py_DECREF(typesModule);
      }
  }
  #endif

  if (unlikely(PyErr_Occurred())) __PYX_ERR(0, 1, __pyx_L1_error)

  if (likely(__pyx_CyFunction_init(__pyx_m) == 0)); else

  if (unlikely(PyErr_Occurred())) __PYX_ERR(0, 1, __pyx_L1_error)

  return 0;
  __pyx_L1_error:;
  return -1;
}

#ifdef _MSC_VER
#pragma warning( push )

#pragma warning( disable : 4127 )
#endif

#if CYTHON_REFNANNY
static __Pyx_RefNannyAPIStruct *__Pyx_RefNannyImportAPI(const char *modname) {
    PyObject *m = NULL, *p = NULL;
    void *r = NULL;
    m = PyImport_ImportModule(modname);
    if (!m) goto end;
    p = PyObject_GetAttrString(m, "RefNannyAPI");
    if (!p) goto end;
    r = PyLong_AsVoidPtr(p);
end:
    Py_XDECREF(p);
    Py_XDECREF(m);
    return (__Pyx_RefNannyAPIStruct *)r;
}
#endif

#if CYTHON_FAST_THREAD_STATE
static int __Pyx_PyErr_ExceptionMatchesTuple(PyObject *exc_type, PyObject *tuple) {
    Py_ssize_t i, n;
    n = PyTuple_GET_SIZE(tuple);
    for (i=0; i<n; i++) {
        if (exc_type == PyTuple_GET_ITEM(tuple, i)) return 1;
    }
    for (i=0; i<n; i++) {
        if (__Pyx_PyErr_GivenExceptionMatches(exc_type, PyTuple_GET_ITEM(tuple, i))) return 1;
    }
    return 0;
}
static CYTHON_INLINE int __Pyx_PyErr_ExceptionMatchesInState(PyThreadState* tstate, PyObject* err) {
    int result;
    PyObject *exc_type;
#if PY_VERSION_HEX >= 0x030C00A6
    PyObject *current_exception = tstate->current_exception;
    if (unlikely(!current_exception)) return 0;
    exc_type = (PyObject*) Py_TYPE(current_exception);
    if (exc_type == err) return 1;
#else
    exc_type = tstate->curexc_type;
    if (exc_type == err) return 1;
    if (unlikely(!exc_type)) return 0;
#endif
    #if CYTHON_AVOID_BORROWED_REFS
    Py_INCREF(exc_type);
    #endif
    if (unlikely(PyTuple_Check(err))) {
        result = __Pyx_PyErr_ExceptionMatchesTuple(exc_type, err);
    } else {
        result = __Pyx_PyErr_GivenExceptionMatches(exc_type, err);
    }
    #if CYTHON_AVOID_BORROWED_REFS
    Py_DECREF(exc_type);
    #endif
    return result;
}
#endif

#if CYTHON_FAST_THREAD_STATE
static CYTHON_INLINE void __Pyx_ErrRestoreInState(PyThreadState *tstate, PyObject *type, PyObject *value, PyObject *tb) {
#if PY_VERSION_HEX >= 0x030C00A6
    PyObject *tmp_value;
    assert(type == NULL || (value != NULL && type == (PyObject*) Py_TYPE(value)));
    if (value) {
        #if CYTHON_COMPILING_IN_CPYTHON
        if (unlikely(((PyBaseExceptionObject*) value)->traceback != tb))
        #endif
            PyException_SetTraceback(value, tb);
    }
    tmp_value = tstate->current_exception;
    tstate->current_exception = value;
    Py_XDECREF(tmp_value);
    Py_XDECREF(type);
    Py_XDECREF(tb);
#else
    PyObject *tmp_type, *tmp_value, *tmp_tb;
    tmp_type = tstate->curexc_type;
    tmp_value = tstate->curexc_value;
    tmp_tb = tstate->curexc_traceback;
    tstate->curexc_type = type;
    tstate->curexc_value = value;
    tstate->curexc_traceback = tb;
    Py_XDECREF(tmp_type);
    Py_XDECREF(tmp_value);
    Py_XDECREF(tmp_tb);
#endif
}
static CYTHON_INLINE void __Pyx_ErrFetchInState(PyThreadState *tstate, PyObject **type, PyObject **value, PyObject **tb) {
#if PY_VERSION_HEX >= 0x030C00A6
    PyObject* exc_value;
    exc_value = tstate->current_exception;
    tstate->current_exception = 0;
    *value = exc_value;
    *type = NULL;
    *tb = NULL;
    if (exc_value) {
        *type = (PyObject*) Py_TYPE(exc_value);
        Py_INCREF(*type);
        #if CYTHON_COMPILING_IN_CPYTHON
        *tb = ((PyBaseExceptionObject*) exc_value)->traceback;
        Py_XINCREF(*tb);
        #else
        *tb = PyException_GetTraceback(exc_value);
        #endif
    }
#else
    *type = tstate->curexc_type;
    *value = tstate->curexc_value;
    *tb = tstate->curexc_traceback;
    tstate->curexc_type = 0;
    tstate->curexc_value = 0;
    tstate->curexc_traceback = 0;
#endif
}
#endif

#if CYTHON_USE_TYPE_SLOTS
static CYTHON_INLINE PyObject* __Pyx_PyObject_GetAttrStr(PyObject* obj, PyObject* attr_name) {
    PyTypeObject* tp = Py_TYPE(obj);
    if (likely(tp->tp_getattro))
        return tp->tp_getattro(obj, attr_name);
    return PyObject_GetAttr(obj, attr_name);
}
#endif

#if __PYX_LIMITED_VERSION_HEX < 0x030d0000
static void __Pyx_PyObject_GetAttrStr_ClearAttributeError(void) {
    __Pyx_PyThreadState_declare
    __Pyx_PyThreadState_assign
    if (likely(__Pyx_PyErr_ExceptionMatches(PyExc_AttributeError)))
        __Pyx_PyErr_Clear();
}
#endif
static CYTHON_INLINE PyObject* __Pyx_PyObject_GetAttrStrNoError(PyObject* obj, PyObject* attr_name) {
    PyObject *result;
#if __PYX_LIMITED_VERSION_HEX >= 0x030d0000
    (void) PyObject_GetOptionalAttr(obj, attr_name, &result);
    return result;
#else
#if CYTHON_COMPILING_IN_CPYTHON && CYTHON_USE_TYPE_SLOTS
    PyTypeObject* tp = Py_TYPE(obj);
    if (likely(tp->tp_getattro == PyObject_GenericGetAttr)) {
        return _PyObject_GenericGetAttrWithDict(obj, attr_name, NULL, 1);
    }
#endif
    result = __Pyx_PyObject_GetAttrStr(obj, attr_name);
    if (unlikely(!result)) {
        __Pyx_PyObject_GetAttrStr_ClearAttributeError();
    }
    return result;
#endif
}

static PyObject *__Pyx_GetBuiltinName(PyObject *name) {
    PyObject* result = __Pyx_PyObject_GetAttrStrNoError(__pyx_mstate_global->__pyx_b, name);
    if (unlikely(!result) && !PyErr_Occurred()) {
        PyErr_Format(PyExc_NameError,
            "name '%U' is not defined", name);
    }
    return result;
}

static PyObject *__Pyx_GetItemInt_Generic(PyObject *o, PyObject* j) {
    PyObject *r;
    if (unlikely(!j)) return NULL;
    r = PyObject_GetItem(o, j);
    Py_DECREF(j);
    return r;
}
static CYTHON_INLINE PyObject *__Pyx_GetItemInt_List_Fast(PyObject *o, Py_ssize_t i,
                                                              int wraparound, int boundscheck, int unsafe_shared) {
    CYTHON_MAYBE_UNUSED_VAR(unsafe_shared);
#if CYTHON_ASSUME_SAFE_SIZE
    Py_ssize_t wrapped_i = i;
    if (wraparound & unlikely(i < 0)) {
        wrapped_i += PyList_GET_SIZE(o);
    }
    if ((CYTHON_AVOID_BORROWED_REFS || CYTHON_AVOID_THREAD_UNSAFE_BORROWED_REFS || !CYTHON_ASSUME_SAFE_MACROS)) {
        return __Pyx_PyList_GetItemRefFast(o, wrapped_i, unsafe_shared);
    } else
    if ((!boundscheck) || likely(__Pyx_is_valid_index(wrapped_i, PyList_GET_SIZE(o)))) {
        return __Pyx_NewRef(PyList_GET_ITEM(o, wrapped_i));
    }
    return __Pyx_GetItemInt_Generic(o, PyLong_FromSsize_t(i));
#else
    (void)wraparound;
    (void)boundscheck;
    return PySequence_GetItem(o, i);
#endif
}
static CYTHON_INLINE PyObject *__Pyx_GetItemInt_Tuple_Fast(PyObject *o, Py_ssize_t i,
                                                              int wraparound, int boundscheck, int unsafe_shared) {
    CYTHON_MAYBE_UNUSED_VAR(unsafe_shared);
#if CYTHON_ASSUME_SAFE_SIZE && CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
    Py_ssize_t wrapped_i = i;
    if (wraparound & unlikely(i < 0)) {
        wrapped_i += PyTuple_GET_SIZE(o);
    }
    if ((!boundscheck) || likely(__Pyx_is_valid_index(wrapped_i, PyTuple_GET_SIZE(o)))) {
        return __Pyx_NewRef(PyTuple_GET_ITEM(o, wrapped_i));
    }
    return __Pyx_GetItemInt_Generic(o, PyLong_FromSsize_t(i));
#else
    (void)wraparound;
    (void)boundscheck;
    return PySequence_GetItem(o, i);
#endif
}
static CYTHON_INLINE PyObject *__Pyx_GetItemInt_Fast(PyObject *o, Py_ssize_t i, int is_list,
                                                     int wraparound, int boundscheck, int unsafe_shared) {
    CYTHON_MAYBE_UNUSED_VAR(unsafe_shared);
#if CYTHON_ASSUME_SAFE_MACROS && CYTHON_ASSUME_SAFE_SIZE
    if (is_list || PyList_CheckExact(o)) {
        Py_ssize_t n = ((!wraparound) | likely(i >= 0)) ? i : i + PyList_GET_SIZE(o);
        if ((CYTHON_AVOID_BORROWED_REFS || CYTHON_AVOID_THREAD_UNSAFE_BORROWED_REFS)) {
            return __Pyx_PyList_GetItemRefFast(o, n, unsafe_shared);
        } else if ((!boundscheck) || (likely(__Pyx_is_valid_index(n, PyList_GET_SIZE(o))))) {
            return __Pyx_NewRef(PyList_GET_ITEM(o, n));
        }
    } else
    #if !CYTHON_AVOID_BORROWED_REFS
    if (PyTuple_CheckExact(o)) {
        Py_ssize_t n = ((!wraparound) | likely(i >= 0)) ? i : i + PyTuple_GET_SIZE(o);
        if ((!boundscheck) || likely(__Pyx_is_valid_index(n, PyTuple_GET_SIZE(o)))) {
            return __Pyx_NewRef(PyTuple_GET_ITEM(o, n));
        }
    } else
    #endif
#endif
#if CYTHON_USE_TYPE_SLOTS && !CYTHON_COMPILING_IN_PYPY
    {
        PyMappingMethods *mm = Py_TYPE(o)->tp_as_mapping;
        PySequenceMethods *sm = Py_TYPE(o)->tp_as_sequence;
        if (!is_list && mm && mm->mp_subscript) {
            PyObject *r, *key = PyLong_FromSsize_t(i);
            if (unlikely(!key)) return NULL;
            r = mm->mp_subscript(o, key);
            Py_DECREF(key);
            return r;
        }
        if (is_list || likely(sm && sm->sq_item)) {
            if (wraparound && unlikely(i < 0) && likely(sm->sq_length)) {
                Py_ssize_t l = sm->sq_length(o);
                if (likely(l >= 0)) {
                    i += l;
                } else {
                    if (!PyErr_ExceptionMatches(PyExc_OverflowError))
                        return NULL;
                    PyErr_Clear();
                }
            }
            return sm->sq_item(o, i);
        }
    }
#else
    if (is_list || !PyMapping_Check(o)) {
        return PySequence_GetItem(o, i);
    }
#endif
    (void)wraparound;
    (void)boundscheck;
    return __Pyx_GetItemInt_Generic(o, PyLong_FromSsize_t(i));
}

#if CYTHON_COMPILING_IN_CPYTHON
static CYTHON_INLINE PyObject* __Pyx_PyObject_Call(PyObject *func, PyObject *arg, PyObject *kw) {
    PyObject *result;
    ternaryfunc call = Py_TYPE(func)->tp_call;
    if (unlikely(!call))
        return PyObject_Call(func, arg, kw);
    if (unlikely(Py_EnterRecursiveCall(" while calling a Python object")))
        return NULL;
    result = (*call)(func, arg, kw);
    Py_LeaveRecursiveCall();
    if (unlikely(!result) && unlikely(!PyErr_Occurred())) {
        PyErr_SetString(
            PyExc_SystemError,
            "NULL result without error in PyObject_Call");
    }
    return result;
}
#endif

#if CYTHON_COMPILING_IN_CPYTHON
static CYTHON_INLINE PyObject* __Pyx_PyObject_CallMethO(PyObject *func, PyObject *arg) {
    PyObject *self, *result;
    PyCFunction cfunc;
    cfunc = __Pyx_CyOrPyCFunction_GET_FUNCTION(func);
    self = __Pyx_CyOrPyCFunction_GET_SELF(func);
    if (unlikely(Py_EnterRecursiveCall(" while calling a Python object")))
        return NULL;
    result = cfunc(self, arg);
    Py_LeaveRecursiveCall();
    if (unlikely(!result) && unlikely(!PyErr_Occurred())) {
        PyErr_SetString(
            PyExc_SystemError,
            "NULL result without error in PyObject_Call");
    }
    return result;
}
#endif

#if PY_VERSION_HEX < 0x03090000 || CYTHON_COMPILING_IN_LIMITED_API
static PyObject* __Pyx_PyObject_FastCall_fallback(PyObject *func, PyObject * const*args, size_t nargs, PyObject *kwargs) {
    PyObject *argstuple;
    PyObject *result = 0;
    size_t i;
    argstuple = PyTuple_New((Py_ssize_t)nargs);
    if (unlikely(!argstuple)) return NULL;
    for (i = 0; i < nargs; i++) {
        Py_INCREF(args[i]);
        if (__Pyx_PyTuple_SET_ITEM(argstuple, (Py_ssize_t)i, args[i]) != (0)) goto bad;
    }
    result = __Pyx_PyObject_Call(func, argstuple, kwargs);
  bad:
    Py_DECREF(argstuple);
    return result;
}
#endif
#if CYTHON_VECTORCALL && !CYTHON_COMPILING_IN_LIMITED_API
  #if PY_VERSION_HEX < 0x03090000
    #define __Pyx_PyVectorcall_Function(callable) _PyVectorcall_Function(callable)
  #elif CYTHON_COMPILING_IN_CPYTHON
static CYTHON_INLINE vectorcallfunc __Pyx_PyVectorcall_Function(PyObject *callable) {
    PyTypeObject *tp = Py_TYPE(callable);
    #if defined(__Pyx_CyFunction_USED)
    if (__Pyx_CyFunction_CheckExact(callable)) {
        return __Pyx_CyFunction_func_vectorcall(callable);
    }
    #endif
    if (!PyType_HasFeature(tp, Py_TPFLAGS_HAVE_VECTORCALL)) {
        return NULL;
    }
    assert(PyCallable_Check(callable));
    Py_ssize_t offset = tp->tp_vectorcall_offset;
    assert(offset > 0);
    vectorcallfunc ptr;
    memcpy(&ptr, (char *) callable + offset, sizeof(ptr));
    return ptr;
}
  #else
    #define __Pyx_PyVectorcall_Function(callable) PyVectorcall_Function(callable)
  #endif
#endif
static CYTHON_INLINE PyObject* __Pyx_PyObject_FastCallDict(PyObject *func, PyObject *const *args, size_t _nargs, PyObject *kwargs) {
    Py_ssize_t nargs = __Pyx_PyVectorcall_NARGS(_nargs);
#if CYTHON_COMPILING_IN_CPYTHON
    if (nargs == 0 && kwargs == NULL) {
        if (__Pyx_CyOrPyCFunction_Check(func) && likely( __Pyx_CyOrPyCFunction_GET_FLAGS(func) & METH_NOARGS))
            return __Pyx_PyObject_CallMethO(func, NULL);
    }
    else if (nargs == 1 && kwargs == NULL) {
        if (__Pyx_CyOrPyCFunction_Check(func) && likely( __Pyx_CyOrPyCFunction_GET_FLAGS(func) & METH_O))
            return __Pyx_PyObject_CallMethO(func, args[0]);
    }
#endif
    if (kwargs == NULL) {
        #if CYTHON_VECTORCALL
          #if CYTHON_COMPILING_IN_LIMITED_API
            return PyObject_Vectorcall(func, args, _nargs, NULL);
          #else
            vectorcallfunc f = __Pyx_PyVectorcall_Function(func);
            if (f) {
                return f(func, args, _nargs, NULL);
            }
          #endif
        #endif
    }
    if (nargs == 0) {
        return __Pyx_PyObject_Call(func, __pyx_mstate_global->__pyx_empty_tuple, kwargs);
    }
    #if PY_VERSION_HEX >= 0x03090000 && !CYTHON_COMPILING_IN_LIMITED_API
    return PyObject_VectorcallDict(func, args, (size_t)nargs, kwargs);
    #else
    return __Pyx_PyObject_FastCall_fallback(func, args, (size_t)nargs, kwargs);
    #endif
}

static CYTHON_INLINE PyObject* __Pyx_PyObject_Call2Args(PyObject* function, PyObject* arg1, PyObject* arg2) {
    PyObject *args[3] = {NULL, arg1, arg2};
    return __Pyx_PyObject_FastCall(function, args+1, 2 | __Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET);
}

#if CYTHON_COMPILING_IN_LIMITED_API && __PYX_LIMITED_VERSION_HEX < 0x030C0000
static PyObject *__Pyx_SelflessCall(PyObject *method, PyObject *args, PyObject *kwargs) {
    PyObject *result;
    PyObject *selfless_args = PyTuple_GetSlice(args, 1, PyTuple_Size(args));
    if (unlikely(!selfless_args)) return NULL;
    result = PyObject_Call(method, selfless_args, kwargs);
    Py_DECREF(selfless_args);
    return result;
}
#elif CYTHON_COMPILING_IN_PYPY && PY_VERSION_HEX < 0x03090000
static PyObject *__Pyx_SelflessCall(PyObject *method, PyObject **args, Py_ssize_t nargs, PyObject *kwnames) {
        return _PyObject_Vectorcall
            (method, args ? args+1 : NULL, nargs ? nargs-1 : 0, kwnames);
}
#else
static PyObject *__Pyx_SelflessCall(PyObject *method, PyObject *const *args, Py_ssize_t nargs, PyObject *kwnames) {
    return
#if PY_VERSION_HEX < 0x03090000
    _PyObject_Vectorcall
#else
    PyObject_Vectorcall
#endif
        (method, args ? args+1 : NULL, nargs ? (size_t) nargs-1 : 0, kwnames);
}
#endif
static PyMethodDef __Pyx_UnboundCMethod_Def = {
     "CythonUnboundCMethod",
     __PYX_REINTERPRET_FUNCION(PyCFunction, __Pyx_SelflessCall),
#if CYTHON_COMPILING_IN_LIMITED_API && __PYX_LIMITED_VERSION_HEX < 0x030C0000
     METH_VARARGS | METH_KEYWORDS,
#else
     METH_FASTCALL | METH_KEYWORDS,
#endif
     NULL
};
static int __Pyx_TryUnpackUnboundCMethod(__Pyx_CachedCFunction* target) {
    PyObject *method, *result=NULL;
    method = __Pyx_PyObject_GetAttrStr(target->type, *target->method_name);
    if (unlikely(!method))
        return -1;
    result = method;
#if CYTHON_COMPILING_IN_CPYTHON
    if (likely(__Pyx_TypeCheck(method, &PyMethodDescr_Type)))
    {
        PyMethodDescrObject *descr = (PyMethodDescrObject*) method;
        target->func = descr->d_method->ml_meth;
        target->flag = descr->d_method->ml_flags & ~(METH_CLASS | METH_STATIC | METH_COEXIST | METH_STACKLESS);
    } else
#endif
#if CYTHON_COMPILING_IN_PYPY
#else
    if (PyCFunction_Check(method))
#endif
    {
        PyObject *self;
        int self_found;
#if CYTHON_COMPILING_IN_LIMITED_API || CYTHON_COMPILING_IN_PYPY
        self = PyObject_GetAttrString(method, "__self__");
        if (!self) {
            PyErr_Clear();
        }
#else
        self = PyCFunction_GET_SELF(method);
#endif
        self_found = (self && self != Py_None);
#if CYTHON_COMPILING_IN_LIMITED_API || CYTHON_COMPILING_IN_PYPY
        Py_XDECREF(self);
#endif
        if (self_found) {
            PyObject *unbound_method = PyCFunction_New(&__Pyx_UnboundCMethod_Def, method);
            if (unlikely(!unbound_method)) return -1;
            Py_DECREF(method);
            result = unbound_method;
        }
    }
#if !CYTHON_COMPILING_IN_CPYTHON_FREETHREADING
    if (unlikely(target->method)) {
        Py_DECREF(result);
    } else
#endif
    target->method = result;
    return 0;
}

#if CYTHON_COMPILING_IN_CPYTHON
static CYTHON_INLINE PyObject* __Pyx_CallUnboundCMethod1(__Pyx_CachedCFunction* cfunc, PyObject* self, PyObject* arg) {
    int was_initialized =  __Pyx_CachedCFunction_GetAndSetInitializing(cfunc);
    if (likely(was_initialized == 2 && cfunc->func)) {
        int flag = cfunc->flag;
        if (flag == METH_O) {
            return __Pyx_CallCFunction(cfunc, self, arg);
        } else if (flag == METH_FASTCALL) {
            return __Pyx_CallCFunctionFast(cfunc, self, &arg, 1);
        } else if (flag == (METH_FASTCALL | METH_KEYWORDS)) {
            return __Pyx_CallCFunctionFastWithKeywords(cfunc, self, &arg, 1, NULL);
        }
    }
#if CYTHON_COMPILING_IN_CPYTHON_FREETHREADING
    else if (unlikely(was_initialized == 1)) {
        __Pyx_CachedCFunction tmp_cfunc = {
#ifndef __cplusplus
            0
#endif
        };
        tmp_cfunc.type = cfunc->type;
        tmp_cfunc.method_name = cfunc->method_name;
        return __Pyx__CallUnboundCMethod1(&tmp_cfunc, self, arg);
    }
#endif
    PyObject* result = __Pyx__CallUnboundCMethod1(cfunc, self, arg);
    __Pyx_CachedCFunction_SetFinishedInitializing(cfunc);
    return result;
}
#endif
static PyObject* __Pyx__CallUnboundCMethod1(__Pyx_CachedCFunction* cfunc, PyObject* self, PyObject* arg){
    PyObject *result = NULL;
    if (unlikely(!cfunc->func && !cfunc->method) && unlikely(__Pyx_TryUnpackUnboundCMethod(cfunc) < 0)) return NULL;
#if CYTHON_COMPILING_IN_CPYTHON
    if (cfunc->func && (cfunc->flag & METH_VARARGS)) {
        PyObject *args = PyTuple_New(1);
        if (unlikely(!args)) return NULL;
        Py_INCREF(arg);
        PyTuple_SET_ITEM(args, 0, arg);
        if (cfunc->flag & METH_KEYWORDS)
            result = __Pyx_CallCFunctionWithKeywords(cfunc, self, args, NULL);
        else
            result = __Pyx_CallCFunction(cfunc, self, args);
        Py_DECREF(args);
    } else
#endif
    {
        result = __Pyx_PyObject_Call2Args(cfunc->method, self, arg);
    }
    return result;
}

#if CYTHON_COMPILING_IN_CPYTHON
static CYTHON_INLINE PyObject *__Pyx_CallUnboundCMethod2(__Pyx_CachedCFunction *cfunc, PyObject *self, PyObject *arg1, PyObject *arg2) {
    int was_initialized = __Pyx_CachedCFunction_GetAndSetInitializing(cfunc);
    if (likely(was_initialized == 2 && cfunc->func)) {
        PyObject *args[2] = {arg1, arg2};
        if (cfunc->flag == METH_FASTCALL) {
            return __Pyx_CallCFunctionFast(cfunc, self, args, 2);
        }
        if (cfunc->flag == (METH_FASTCALL | METH_KEYWORDS))
            return __Pyx_CallCFunctionFastWithKeywords(cfunc, self, args, 2, NULL);
    }
#if CYTHON_COMPILING_IN_CPYTHON_FREETHREADING
    else if (unlikely(was_initialized == 1)) {
        __Pyx_CachedCFunction tmp_cfunc = {
#ifndef __cplusplus
            0
#endif
        };
        tmp_cfunc.type = cfunc->type;
        tmp_cfunc.method_name = cfunc->method_name;
        return __Pyx__CallUnboundCMethod2(&tmp_cfunc, self, arg1, arg2);
    }
#endif
    PyObject *result = __Pyx__CallUnboundCMethod2(cfunc, self, arg1, arg2);
    __Pyx_CachedCFunction_SetFinishedInitializing(cfunc);
    return result;
}
#endif
static PyObject* __Pyx__CallUnboundCMethod2(__Pyx_CachedCFunction* cfunc, PyObject* self, PyObject* arg1, PyObject* arg2){
    if (unlikely(!cfunc->func && !cfunc->method) && unlikely(__Pyx_TryUnpackUnboundCMethod(cfunc) < 0)) return NULL;
#if CYTHON_COMPILING_IN_CPYTHON
    if (cfunc->func && (cfunc->flag & METH_VARARGS)) {
        PyObject *result = NULL;
        PyObject *args = PyTuple_New(2);
        if (unlikely(!args)) return NULL;
        Py_INCREF(arg1);
        PyTuple_SET_ITEM(args, 0, arg1);
        Py_INCREF(arg2);
        PyTuple_SET_ITEM(args, 1, arg2);
        if (cfunc->flag & METH_KEYWORDS)
            result = __Pyx_CallCFunctionWithKeywords(cfunc, self, args, NULL);
        else
            result = __Pyx_CallCFunction(cfunc, self, args);
        Py_DECREF(args);
        return result;
    }
#endif
    {
        PyObject *args[4] = {NULL, self, arg1, arg2};
        return __Pyx_PyObject_FastCall(cfunc->method, args+1, 3 | __Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET);
    }
}

static PyObject* __Pyx_PyDict_GetItemDefault(PyObject* d, PyObject* key, PyObject* default_value) {
    PyObject* value;
#if !CYTHON_COMPILING_IN_PYPY || PYPY_VERSION_NUM >= 0x07020000
    value = PyDict_GetItemWithError(d, key);
    if (unlikely(!value)) {
        if (unlikely(PyErr_Occurred()))
            return NULL;
        value = default_value;
    }
    Py_INCREF(value);
    if ((1));
#else
    if (PyBytes_CheckExact(key) || PyUnicode_CheckExact(key) || PyLong_CheckExact(key)) {
        value = PyDict_GetItem(d, key);
        if (unlikely(!value)) {
            value = default_value;
        }
        Py_INCREF(value);
    }
#endif
    else {
        if (default_value == Py_None)
            value = __Pyx_CallUnboundCMethod1(&__pyx_mstate_global->__pyx_umethod_PyDict_Type_get, d, key);
        else
            value = __Pyx_CallUnboundCMethod2(&__pyx_mstate_global->__pyx_umethod_PyDict_Type_get, d, key, default_value);
    }
    return value;
}

#if !CYTHON_COMPILING_IN_PYPY
static PyObject *__Pyx_PyDict_GetItem(PyObject *d, PyObject* key) {
    PyObject *value;
    if (unlikely(__Pyx_PyDict_GetItemRef(d, key, &value) == 0)) {
        if (unlikely(PyTuple_Check(key))) {
            PyObject* args = PyTuple_Pack(1, key);
            if (likely(args)) {
                PyErr_SetObject(PyExc_KeyError, args);
                Py_DECREF(args);
            }
        } else {
            PyErr_SetObject(PyExc_KeyError, key);
        }
    }
    return value;
}
#endif

#if !CYTHON_VECTORCALL || PY_VERSION_HEX < 0x03090000
static PyObject *__Pyx_PyObject_FastCallMethod(PyObject *name, PyObject *const *args, size_t nargsf) {
    PyObject *result;
    PyObject *attr = PyObject_GetAttr(args[0], name);
    if (unlikely(!attr))
        return NULL;
    result = __Pyx_PyObject_FastCall(attr, args+1, nargsf - 1);
    Py_DECREF(attr);
    return result;
}
#endif

#if !CYTHON_COMPILING_IN_CPYTHON && CYTHON_METH_FASTCALL
static CYTHON_INLINE PyObject *
__Pyx_PyTuple_FromArray(PyObject *const *src, Py_ssize_t n)
{
    PyObject *res;
    Py_ssize_t i;
    if (n <= 0) {
        return __Pyx_NewRef(__pyx_mstate_global->__pyx_empty_tuple);
    }
    res = PyTuple_New(n);
    if (unlikely(res == NULL)) return NULL;
    for (i = 0; i < n; i++) {
        if (unlikely(__Pyx_PyTuple_SET_ITEM(res, i, src[i]) < (0))) {
            Py_DECREF(res);
            return NULL;
        }
        Py_INCREF(src[i]);
    }
    return res;
}
#elif CYTHON_COMPILING_IN_CPYTHON
static CYTHON_INLINE void __Pyx_copy_object_array(PyObject *const *CYTHON_RESTRICT src, PyObject** CYTHON_RESTRICT dest, Py_ssize_t length) {
    PyObject *v;
    Py_ssize_t i;
    for (i = 0; i < length; i++) {
        v = dest[i] = src[i];
        Py_INCREF(v);
    }
}
static CYTHON_INLINE PyObject *
__Pyx_PyTuple_FromArray(PyObject *const *src, Py_ssize_t n)
{
    PyObject *res;
    if (n <= 0) {
        return __Pyx_NewRef(__pyx_mstate_global->__pyx_empty_tuple);
    }
    res = PyTuple_New(n);
    if (unlikely(res == NULL)) return NULL;
    __Pyx_copy_object_array(src, ((PyTupleObject*)res)->ob_item, n);
    return res;
}
static CYTHON_INLINE PyObject *
__Pyx_PyList_FromArray(PyObject *const *src, Py_ssize_t n)
{
    PyObject *res;
    if (n <= 0) {
        return PyList_New(0);
    }
    res = PyList_New(n);
    if (unlikely(res == NULL)) return NULL;
    __Pyx_copy_object_array(src, ((PyListObject*)res)->ob_item, n);
    return res;
}
#endif

static CYTHON_INLINE int __Pyx_PyBytes_Equals(PyObject* s1, PyObject* s2, int equals) {
#if CYTHON_COMPILING_IN_PYPY || CYTHON_COMPILING_IN_LIMITED_API || CYTHON_COMPILING_IN_GRAAL ||\
        !(CYTHON_ASSUME_SAFE_SIZE && CYTHON_ASSUME_SAFE_MACROS)
    return PyObject_RichCompareBool(s1, s2, equals);
#else
    if (s1 == s2) {
        return (equals == Py_EQ);
    } else if (PyBytes_CheckExact(s1) & PyBytes_CheckExact(s2)) {
        const char *ps1, *ps2;
        Py_ssize_t length = PyBytes_GET_SIZE(s1);
        if (length != PyBytes_GET_SIZE(s2))
            return (equals == Py_NE);
        ps1 = PyBytes_AS_STRING(s1);
        ps2 = PyBytes_AS_STRING(s2);
        if (ps1[0] != ps2[0]) {
            return (equals == Py_NE);
        } else if (length == 1) {
            return (equals == Py_EQ);
        } else {
            int result;
#if CYTHON_USE_UNICODE_INTERNALS && (PY_VERSION_HEX < 0x030B0000)
            Py_hash_t hash1, hash2;
            hash1 = ((PyBytesObject*)s1)->ob_shash;
            hash2 = ((PyBytesObject*)s2)->ob_shash;
            if (hash1 != hash2 && hash1 != -1 && hash2 != -1) {
                return (equals == Py_NE);
            }
#endif
            result = memcmp(ps1, ps2, (size_t)length);
            return (equals == Py_EQ) ? (result == 0) : (result != 0);
        }
    } else if ((s1 == Py_None) & PyBytes_CheckExact(s2)) {
        return (equals == Py_NE);
    } else if ((s2 == Py_None) & PyBytes_CheckExact(s1)) {
        return (equals == Py_NE);
    } else {
        int result;
        PyObject* py_result = PyObject_RichCompare(s1, s2, equals);
        if (!py_result)
            return -1;
        result = __Pyx_PyObject_IsTrue(py_result);
        Py_DECREF(py_result);
        return result;
    }
#endif
}

static CYTHON_INLINE int __Pyx_PyUnicode_Equals(PyObject* s1, PyObject* s2, int equals) {
#if CYTHON_COMPILING_IN_PYPY || CYTHON_COMPILING_IN_LIMITED_API || CYTHON_COMPILING_IN_GRAAL
    return PyObject_RichCompareBool(s1, s2, equals);
#else
    int s1_is_unicode, s2_is_unicode;
    if (s1 == s2) {
        goto return_eq;
    }
    s1_is_unicode = PyUnicode_CheckExact(s1);
    s2_is_unicode = PyUnicode_CheckExact(s2);
    if (s1_is_unicode & s2_is_unicode) {
        Py_ssize_t length, length2;
        int kind;
        void *data1, *data2;
        #if !CYTHON_COMPILING_IN_LIMITED_API
        if (unlikely(__Pyx_PyUnicode_READY(s1) < 0) || unlikely(__Pyx_PyUnicode_READY(s2) < 0))
            return -1;
        #endif
        length = __Pyx_PyUnicode_GET_LENGTH(s1);
        #if !CYTHON_ASSUME_SAFE_SIZE
        if (unlikely(length < 0)) return -1;
        #endif
        length2 = __Pyx_PyUnicode_GET_LENGTH(s2);
        #if !CYTHON_ASSUME_SAFE_SIZE
        if (unlikely(length2 < 0)) return -1;
        #endif
        if (length != length2) {
            goto return_ne;
        }
#if CYTHON_USE_UNICODE_INTERNALS
        {
            Py_hash_t hash1, hash2;
            hash1 = ((PyASCIIObject*)s1)->hash;
            hash2 = ((PyASCIIObject*)s2)->hash;
            if (hash1 != hash2 && hash1 != -1 && hash2 != -1) {
                goto return_ne;
            }
        }
#endif
        kind = __Pyx_PyUnicode_KIND(s1);
        if (kind != __Pyx_PyUnicode_KIND(s2)) {
            goto return_ne;
        }
        data1 = __Pyx_PyUnicode_DATA(s1);
        data2 = __Pyx_PyUnicode_DATA(s2);
        if (__Pyx_PyUnicode_READ(kind, data1, 0) != __Pyx_PyUnicode_READ(kind, data2, 0)) {
            goto return_ne;
        } else if (length == 1) {
            goto return_eq;
        } else {
            int result = memcmp(data1, data2, (size_t)(length * kind));
            return (equals == Py_EQ) ? (result == 0) : (result != 0);
        }
    } else if ((s1 == Py_None) & s2_is_unicode) {
        goto return_ne;
    } else if ((s2 == Py_None) & s1_is_unicode) {
        goto return_ne;
    } else {
        int result;
        PyObject* py_result = PyObject_RichCompare(s1, s2, equals);
        if (!py_result)
            return -1;
        result = __Pyx_PyObject_IsTrue(py_result);
        Py_DECREF(py_result);
        return result;
    }
return_eq:
    return (equals == Py_EQ);
return_ne:
    return (equals == Py_NE);
#endif
}

#if CYTHON_METH_FASTCALL
static CYTHON_INLINE PyObject * __Pyx_GetKwValue_FASTCALL(PyObject *kwnames, PyObject *const *kwvalues, PyObject *s)
{
    Py_ssize_t i, n = __Pyx_PyTuple_GET_SIZE(kwnames);
    #if !CYTHON_ASSUME_SAFE_SIZE
    if (unlikely(n == -1)) return NULL;
    #endif
    for (i = 0; i < n; i++)
    {
        PyObject *namei = __Pyx_PyTuple_GET_ITEM(kwnames, i);
        #if !CYTHON_ASSUME_SAFE_MACROS
        if (unlikely(!namei)) return NULL;
        #endif
        if (s == namei) return kwvalues[i];
    }
    for (i = 0; i < n; i++)
    {
        PyObject *namei = __Pyx_PyTuple_GET_ITEM(kwnames, i);
        #if !CYTHON_ASSUME_SAFE_MACROS
        if (unlikely(!namei)) return NULL;
        #endif
        int eq = __Pyx_PyUnicode_Equals(s, namei, Py_EQ);
        if (unlikely(eq != 0)) {
            if (unlikely(eq < 0)) return NULL;
            return kwvalues[i];
        }
    }
    return NULL;
}
#if CYTHON_COMPILING_IN_CPYTHON && PY_VERSION_HEX >= 0x030d0000 || CYTHON_COMPILING_IN_LIMITED_API
CYTHON_UNUSED static PyObject *__Pyx_KwargsAsDict_FASTCALL(PyObject *kwnames, PyObject *const *kwvalues) {
    Py_ssize_t i, nkwargs;
    PyObject *dict;
#if !CYTHON_ASSUME_SAFE_SIZE
    nkwargs = PyTuple_Size(kwnames);
    if (unlikely(nkwargs < 0)) return NULL;
#else
    nkwargs = PyTuple_GET_SIZE(kwnames);
#endif
    dict = PyDict_New();
    if (unlikely(!dict))
        return NULL;
    for (i=0; i<nkwargs; i++) {
#if !CYTHON_ASSUME_SAFE_MACROS
        PyObject *key = PyTuple_GetItem(kwnames, i);
        if (!key) goto bad;
#else
        PyObject *key = PyTuple_GET_ITEM(kwnames, i);
#endif
        if (unlikely(PyDict_SetItem(dict, key, kwvalues[i]) < 0))
            goto bad;
    }
    return dict;
bad:
    Py_DECREF(dict);
    return NULL;
}
#endif
#endif

static CYTHON_INLINE PyObject* __Pyx_PyObject_CallOneArg(PyObject *func, PyObject *arg) {
    PyObject *args[2] = {NULL, arg};
    return __Pyx_PyObject_FastCall(func, args+1, 1 | __Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET);
}

#if CYTHON_COMPILING_IN_CPYTHON
static CYTHON_INLINE PyObject* __Pyx_CallUnboundCMethod0(__Pyx_CachedCFunction* cfunc, PyObject* self) {
    int was_initialized = __Pyx_CachedCFunction_GetAndSetInitializing(cfunc);
    if (likely(was_initialized == 2 && cfunc->func)) {
        if (likely(cfunc->flag == METH_NOARGS))
            return __Pyx_CallCFunction(cfunc, self, NULL);
        if (likely(cfunc->flag == METH_FASTCALL))
            return __Pyx_CallCFunctionFast(cfunc, self, NULL, 0);
        if (cfunc->flag == (METH_FASTCALL | METH_KEYWORDS))
            return __Pyx_CallCFunctionFastWithKeywords(cfunc, self, NULL, 0, NULL);
        if (likely(cfunc->flag == (METH_VARARGS | METH_KEYWORDS)))
            return __Pyx_CallCFunctionWithKeywords(cfunc, self, __pyx_mstate_global->__pyx_empty_tuple, NULL);
        if (cfunc->flag == METH_VARARGS)
            return __Pyx_CallCFunction(cfunc, self, __pyx_mstate_global->__pyx_empty_tuple);
        return __Pyx__CallUnboundCMethod0(cfunc, self);
    }
#if CYTHON_COMPILING_IN_CPYTHON_FREETHREADING
    else if (unlikely(was_initialized == 1)) {
        __Pyx_CachedCFunction tmp_cfunc = {
#ifndef __cplusplus
            0
#endif
        };
        tmp_cfunc.type = cfunc->type;
        tmp_cfunc.method_name = cfunc->method_name;
        return __Pyx__CallUnboundCMethod0(&tmp_cfunc, self);
    }
#endif
    PyObject *result = __Pyx__CallUnboundCMethod0(cfunc, self);
    __Pyx_CachedCFunction_SetFinishedInitializing(cfunc);
    return result;
}
#endif
static PyObject* __Pyx__CallUnboundCMethod0(__Pyx_CachedCFunction* cfunc, PyObject* self) {
    PyObject *result;
    if (unlikely(!cfunc->method) && unlikely(__Pyx_TryUnpackUnboundCMethod(cfunc) < 0)) return NULL;
    result = __Pyx_PyObject_CallOneArg(cfunc->method, self);
    return result;
}

static CYTHON_INLINE PyObject* __Pyx_PyDict_Items(PyObject* d) {
    return __Pyx_CallUnboundCMethod0(&__pyx_mstate_global->__pyx_umethod_PyDict_Type_items, d);
}

static CYTHON_INLINE PyObject* __Pyx_PyDict_Values(PyObject* d) {
    return __Pyx_CallUnboundCMethod0(&__pyx_mstate_global->__pyx_umethod_PyDict_Type_values, d);
}

#if CYTHON_AVOID_BORROWED_REFS
static int __Pyx_PyDict_NextRef(PyObject *p, PyObject **ppos, PyObject **pkey, PyObject **pvalue) {
    PyObject *next = NULL;
    if (!*ppos) {
        if (pvalue) {
            PyObject *dictview = pkey ? __Pyx_PyDict_Items(p) : __Pyx_PyDict_Values(p);
            if (unlikely(!dictview)) goto bad;
            *ppos = PyObject_GetIter(dictview);
            Py_DECREF(dictview);
        } else {
            *ppos = PyObject_GetIter(p);
        }
        if (unlikely(!*ppos)) goto bad;
    }
    next = PyIter_Next(*ppos);
    if (!next) {
        if (PyErr_Occurred()) goto bad;
        return 0;
    }
    if (pkey && pvalue) {
        *pkey = __Pyx_PySequence_ITEM(next, 0);
        if (unlikely(*pkey)) goto bad;
        *pvalue = __Pyx_PySequence_ITEM(next, 1);
        if (unlikely(*pvalue)) goto bad;
        Py_DECREF(next);
    } else if (pkey) {
        *pkey = next;
    } else {
        assert(pvalue);
        *pvalue = next;
    }
    return 1;
  bad:
    Py_XDECREF(next);
#if !CYTHON_COMPILING_IN_LIMITED_API && PY_VERSION_HEX >= 0x030d0000
    PyErr_FormatUnraisable("Exception ignored in __Pyx_PyDict_NextRef");
#else
    PyErr_WriteUnraisable(__pyx_mstate_global->__pyx_n_u_Pyx_PyDict_NextRef);
#endif
    if (pkey) *pkey = NULL;
    if (pvalue) *pvalue = NULL;
    return 0;
}
#else
static int __Pyx_PyDict_NextRef(PyObject *p, Py_ssize_t *ppos, PyObject **pkey, PyObject **pvalue) {
    int result = PyDict_Next(p, ppos, pkey, pvalue);
    if (likely(result == 1)) {
        if (pkey) Py_INCREF(*pkey);
        if (pvalue) Py_INCREF(*pvalue);
    }
    return result;
}
#endif

static void __Pyx_RaiseDoubleKeywordsError(
    const char* func_name,
    PyObject* kw_name)
{
    PyErr_Format(PyExc_TypeError,
        "%s() got multiple values for keyword argument '%U'", func_name, kw_name);
}

static int __Pyx_ValidateDuplicatePosArgs(
    PyObject *kwds,
    PyObject ** const argnames[],
    PyObject ** const *first_kw_arg,
    const char* function_name)
{
    PyObject ** const *name = argnames;
    while (name != first_kw_arg) {
        PyObject *key = **name;
        int found = PyDict_Contains(kwds, key);
        if (unlikely(found)) {
            if (found == 1) __Pyx_RaiseDoubleKeywordsError(function_name, key);
            goto bad;
        }
        name++;
    }
    return 0;
bad:
    return -1;
}
#if CYTHON_USE_UNICODE_INTERNALS
static CYTHON_INLINE int __Pyx_UnicodeKeywordsEqual(PyObject *s1, PyObject *s2) {
    int kind;
    Py_ssize_t len = PyUnicode_GET_LENGTH(s1);
    if (len != PyUnicode_GET_LENGTH(s2)) return 0;
    kind = PyUnicode_KIND(s1);
    if (kind != PyUnicode_KIND(s2)) return 0;
    const void *data1 = PyUnicode_DATA(s1);
    const void *data2 = PyUnicode_DATA(s2);
    return (memcmp(data1, data2, (size_t) len * (size_t) kind) == 0);
}
#endif
static int __Pyx_MatchKeywordArg_str(
    PyObject *key,
    PyObject ** const argnames[],
    PyObject ** const *first_kw_arg,
    size_t *index_found,
    const char *function_name)
{
    PyObject ** const *name;
    #if CYTHON_USE_UNICODE_INTERNALS
    Py_hash_t key_hash = ((PyASCIIObject*)key)->hash;
    if (unlikely(key_hash == -1)) {
        key_hash = PyObject_Hash(key);
        if (unlikely(key_hash == -1))
            goto bad;
    }
    #endif
    name = first_kw_arg;
    while (*name) {
        PyObject *name_str = **name;
        #if CYTHON_USE_UNICODE_INTERNALS
        if (key_hash == ((PyASCIIObject*)name_str)->hash && __Pyx_UnicodeKeywordsEqual(name_str, key)) {
            *index_found = (size_t) (name - argnames);
            return 1;
        }
        #else
        #if CYTHON_ASSUME_SAFE_SIZE
        if (PyUnicode_GET_LENGTH(name_str) == PyUnicode_GET_LENGTH(key))
        #endif
        {
            int cmp = PyUnicode_Compare(name_str, key);
            if (cmp < 0 && unlikely(PyErr_Occurred())) goto bad;
            if (cmp == 0) {
                *index_found = (size_t) (name - argnames);
                return 1;
            }
        }
        #endif
        name++;
    }
    name = argnames;
    while (name != first_kw_arg) {
        PyObject *name_str = **name;
        #if CYTHON_USE_UNICODE_INTERNALS
        if (unlikely(key_hash == ((PyASCIIObject*)name_str)->hash)) {
            if (__Pyx_UnicodeKeywordsEqual(name_str, key))
                goto arg_passed_twice;
        }
        #else
        #if CYTHON_ASSUME_SAFE_SIZE
        if (PyUnicode_GET_LENGTH(name_str) == PyUnicode_GET_LENGTH(key))
        #endif
        {
            if (unlikely(name_str == key)) goto arg_passed_twice;
            int cmp = PyUnicode_Compare(name_str, key);
            if (cmp < 0 && unlikely(PyErr_Occurred())) goto bad;
            if (cmp == 0) goto arg_passed_twice;
        }
        #endif
        name++;
    }
    return 0;
arg_passed_twice:
    __Pyx_RaiseDoubleKeywordsError(function_name, key);
    goto bad;
bad:
    return -1;
}
static int __Pyx_MatchKeywordArg_nostr(
    PyObject *key,
    PyObject ** const argnames[],
    PyObject ** const *first_kw_arg,
    size_t *index_found,
    const char *function_name)
{
    PyObject ** const *name;
    if (unlikely(!PyUnicode_Check(key))) goto invalid_keyword_type;
    name = first_kw_arg;
    while (*name) {
        int cmp = PyObject_RichCompareBool(**name, key, Py_EQ);
        if (cmp == 1) {
            *index_found = (size_t) (name - argnames);
            return 1;
        }
        if (unlikely(cmp == -1)) goto bad;
        name++;
    }
    name = argnames;
    while (name != first_kw_arg) {
        int cmp = PyObject_RichCompareBool(**name, key, Py_EQ);
        if (unlikely(cmp != 0)) {
            if (cmp == 1) goto arg_passed_twice;
            else goto bad;
        }
        name++;
    }
    return 0;
arg_passed_twice:
    __Pyx_RaiseDoubleKeywordsError(function_name, key);
    goto bad;
invalid_keyword_type:
    PyErr_Format(PyExc_TypeError,
        "%.200s() keywords must be strings", function_name);
    goto bad;
bad:
    return -1;
}
static CYTHON_INLINE int __Pyx_MatchKeywordArg(
    PyObject *key,
    PyObject ** const argnames[],
    PyObject ** const *first_kw_arg,
    size_t *index_found,
    const char *function_name)
{
    return likely(PyUnicode_CheckExact(key)) ?
        __Pyx_MatchKeywordArg_str(key, argnames, first_kw_arg, index_found, function_name) :
        __Pyx_MatchKeywordArg_nostr(key, argnames, first_kw_arg, index_found, function_name);
}
static void __Pyx_RejectUnknownKeyword(
    PyObject *kwds,
    PyObject ** const argnames[],
    PyObject ** const *first_kw_arg,
    const char *function_name)
{
    #if CYTHON_AVOID_BORROWED_REFS
    PyObject *pos = NULL;
    #else
    Py_ssize_t pos = 0;
    #endif
    PyObject *key = NULL;
    __Pyx_BEGIN_CRITICAL_SECTION(kwds);
    while (
        #if CYTHON_AVOID_BORROWED_REFS
        __Pyx_PyDict_NextRef(kwds, &pos, &key, NULL)
        #else
        PyDict_Next(kwds, &pos, &key, NULL)
        #endif
    ) {
        PyObject** const *name = first_kw_arg;
        while (*name && (**name != key)) name++;
        if (!*name) {
            size_t index_found = 0;
            int cmp = __Pyx_MatchKeywordArg(key, argnames, first_kw_arg, &index_found, function_name);
            if (cmp != 1) {
                if (cmp == 0) {
                    PyErr_Format(PyExc_TypeError,
                        "%s() got an unexpected keyword argument '%U'",
                        function_name, key);
                }
                #if CYTHON_AVOID_BORROWED_REFS
                Py_DECREF(key);
                #endif
                break;
            }
        }
        #if CYTHON_AVOID_BORROWED_REFS
        Py_DECREF(key);
        #endif
    }
    __Pyx_END_CRITICAL_SECTION();
    #if CYTHON_AVOID_BORROWED_REFS
    Py_XDECREF(pos);
    #endif
    assert(PyErr_Occurred());
}
static int __Pyx_ParseKeywordDict(
    PyObject *kwds,
    PyObject ** const argnames[],
    PyObject *values[],
    Py_ssize_t num_pos_args,
    Py_ssize_t num_kwargs,
    const char* function_name,
    int ignore_unknown_kwargs)
{
    PyObject** const *name;
    PyObject** const *first_kw_arg = argnames + num_pos_args;
    Py_ssize_t extracted = 0;
#if !CYTHON_COMPILING_IN_PYPY || defined(PyArg_ValidateKeywordArguments)
    if (unlikely(!PyArg_ValidateKeywordArguments(kwds))) return -1;
#endif
    name = first_kw_arg;
    while (*name && num_kwargs > extracted) {
        PyObject * key = **name;
        PyObject *value;
        int found = 0;
        #if __PYX_LIMITED_VERSION_HEX >= 0x030d0000
        found = PyDict_GetItemRef(kwds, key, &value);
        #else
        value = PyDict_GetItemWithError(kwds, key);
        if (value) {
            Py_INCREF(value);
            found = 1;
        } else {
            if (unlikely(PyErr_Occurred())) goto bad;
        }
        #endif
        if (found) {
            if (unlikely(found < 0)) goto bad;
            values[name-argnames] = value;
            extracted++;
        }
        name++;
    }
    if (num_kwargs > extracted) {
        if (ignore_unknown_kwargs) {
            if (unlikely(__Pyx_ValidateDuplicatePosArgs(kwds, argnames, first_kw_arg, function_name) == -1))
                goto bad;
        } else {
            __Pyx_RejectUnknownKeyword(kwds, argnames, first_kw_arg, function_name);
            goto bad;
        }
    }
    return 0;
bad:
    return -1;
}
static int __Pyx_ParseKeywordDictToDict(
    PyObject *kwds,
    PyObject ** const argnames[],
    PyObject *kwds2,
    PyObject *values[],
    Py_ssize_t num_pos_args,
    const char* function_name)
{
    PyObject** const *name;
    PyObject** const *first_kw_arg = argnames + num_pos_args;
    Py_ssize_t len;
#if !CYTHON_COMPILING_IN_PYPY || defined(PyArg_ValidateKeywordArguments)
    if (unlikely(!PyArg_ValidateKeywordArguments(kwds))) return -1;
#endif
    if (PyDict_Update(kwds2, kwds) < 0) goto bad;
    name = first_kw_arg;
    while (*name) {
        PyObject *key = **name;
        PyObject *value;
#if !CYTHON_COMPILING_IN_LIMITED_API && (PY_VERSION_HEX >= 0x030d00A2 || defined(PyDict_Pop))
        int found = PyDict_Pop(kwds2, key, &value);
        if (found) {
            if (unlikely(found < 0)) goto bad;
            values[name-argnames] = value;
        }
#elif __PYX_LIMITED_VERSION_HEX >= 0x030d0000
        int found = PyDict_GetItemRef(kwds2, key, &value);
        if (found) {
            if (unlikely(found < 0)) goto bad;
            values[name-argnames] = value;
            if (unlikely(PyDict_DelItem(kwds2, key) < 0)) goto bad;
        }
#else
    #if CYTHON_COMPILING_IN_CPYTHON
        value = _PyDict_Pop(kwds2, key, kwds2);
    #else
        value = __Pyx_CallUnboundCMethod2(&__pyx_mstate_global->__pyx_umethod_PyDict_Type_pop, kwds2, key, kwds2);
    #endif
        if (value == kwds2) {
            Py_DECREF(value);
        } else {
            if (unlikely(!value)) goto bad;
            values[name-argnames] = value;
        }
#endif
        name++;
    }
    len = PyDict_Size(kwds2);
    if (len > 0) {
        return __Pyx_ValidateDuplicatePosArgs(kwds, argnames, first_kw_arg, function_name);
    } else if (unlikely(len == -1)) {
        goto bad;
    }
    return 0;
bad:
    return -1;
}
static int __Pyx_ParseKeywordsTuple(
    PyObject *kwds,
    PyObject * const *kwvalues,
    PyObject ** const argnames[],
    PyObject *kwds2,
    PyObject *values[],
    Py_ssize_t num_pos_args,
    Py_ssize_t num_kwargs,
    const char* function_name,
    int ignore_unknown_kwargs)
{
    PyObject *key = NULL;
    PyObject** const * name;
    PyObject** const *first_kw_arg = argnames + num_pos_args;
    for (Py_ssize_t pos = 0; pos < num_kwargs; pos++) {
#if CYTHON_AVOID_BORROWED_REFS
        key = __Pyx_PySequence_ITEM(kwds, pos);
#else
        key = __Pyx_PyTuple_GET_ITEM(kwds, pos);
#endif
#if !CYTHON_ASSUME_SAFE_MACROS
        if (unlikely(!key)) goto bad;
#endif
        name = first_kw_arg;
        while (*name && (**name != key)) name++;
        if (*name) {
            PyObject *value = kwvalues[pos];
            values[name-argnames] = __Pyx_NewRef(value);
        } else {
            size_t index_found = 0;
            int cmp = __Pyx_MatchKeywordArg(key, argnames, first_kw_arg, &index_found, function_name);
            if (cmp == 1) {
                PyObject *value = kwvalues[pos];
                values[index_found] = __Pyx_NewRef(value);
            } else {
                if (unlikely(cmp == -1)) goto bad;
                if (kwds2) {
                    PyObject *value = kwvalues[pos];
                    if (unlikely(PyDict_SetItem(kwds2, key, value))) goto bad;
                } else if (!ignore_unknown_kwargs) {
                    goto invalid_keyword;
                }
            }
        }
        #if CYTHON_AVOID_BORROWED_REFS
        Py_DECREF(key);
        key = NULL;
        #endif
    }
    return 0;
invalid_keyword:
    PyErr_Format(PyExc_TypeError,
        "%s() got an unexpected keyword argument '%U'",
        function_name, key);
    goto bad;
bad:
    #if CYTHON_AVOID_BORROWED_REFS
    Py_XDECREF(key);
    #endif
    return -1;
}

static int __Pyx_ParseKeywords(
    PyObject *kwds,
    PyObject * const *kwvalues,
    PyObject ** const argnames[],
    PyObject *kwds2,
    PyObject *values[],
    Py_ssize_t num_pos_args,
    Py_ssize_t num_kwargs,
    const char* function_name,
    int ignore_unknown_kwargs)
{
    if (CYTHON_METH_FASTCALL && likely(PyTuple_Check(kwds)))
        return __Pyx_ParseKeywordsTuple(kwds, kwvalues, argnames, kwds2, values, num_pos_args, num_kwargs, function_name, ignore_unknown_kwargs);
    else if (kwds2)
        return __Pyx_ParseKeywordDictToDict(kwds, argnames, kwds2, values, num_pos_args, function_name);
    else
        return __Pyx_ParseKeywordDict(kwds, argnames, values, num_pos_args, num_kwargs, function_name, ignore_unknown_kwargs);
}

static void __Pyx_RaiseArgtupleInvalid(
    const char* func_name,
    int exact,
    Py_ssize_t num_min,
    Py_ssize_t num_max,
    Py_ssize_t num_found)
{
    Py_ssize_t num_expected;
    const char *more_or_less;
    if (num_found < num_min) {
        num_expected = num_min;
        more_or_less = "at least";
    } else {
        num_expected = num_max;
        more_or_less = "at most";
    }
    if (exact) {
        more_or_less = "exactly";
    }
    PyErr_Format(PyExc_TypeError,
                 "%.200s() takes %.8s %" CYTHON_FORMAT_SSIZE_T "d positional argument%.1s (%" CYTHON_FORMAT_SSIZE_T "d given)",
                 func_name, more_or_less, num_expected,
                 (num_expected == 1) ? "" : "s", num_found);
}

static int __Pyx__ArgTypeTest(PyObject *obj, PyTypeObject *type, const char *name, int exact)
{
    __Pyx_TypeName type_name;
    __Pyx_TypeName obj_type_name;
    PyObject *extra_info = __pyx_mstate_global->__pyx_empty_unicode;
    int from_annotation_subclass = 0;
    if (unlikely(!type)) {
        PyErr_SetString(PyExc_SystemError, "Missing type object");
        return 0;
    }
    else if (!exact) {
        if (likely(__Pyx_TypeCheck(obj, type))) return 1;
    } else if (exact == 2) {
        if (__Pyx_TypeCheck(obj, type)) {
            from_annotation_subclass = 1;
            extra_info = __pyx_mstate_global->__pyx_kp_u_Note_that_Cython_is_deliberately;
        }
    }
    type_name = __Pyx_PyType_GetFullyQualifiedName(type);
    obj_type_name = __Pyx_PyType_GetFullyQualifiedName(Py_TYPE(obj));
    PyErr_Format(PyExc_TypeError,
        "Argument '%.200s' has incorrect type (expected " __Pyx_FMT_TYPENAME
        ", got " __Pyx_FMT_TYPENAME ")"
#if __PYX_LIMITED_VERSION_HEX < 0x030C0000
        "%s%U"
#endif
        , name, type_name, obj_type_name
#if __PYX_LIMITED_VERSION_HEX < 0x030C0000
        , (from_annotation_subclass ? ". " : ""), extra_info
#endif
        );
#if __PYX_LIMITED_VERSION_HEX >= 0x030C0000
    if (exact == 2 && from_annotation_subclass) {
        PyObject *res;
        PyObject *vargs[2];
        vargs[0] = PyErr_GetRaisedException();
        vargs[1] = extra_info;
        res = PyObject_VectorcallMethod(__pyx_mstate_global->__pyx_kp_u_add_note, vargs, 2, NULL);
        Py_XDECREF(res);
        PyErr_SetRaisedException(vargs[0]);
    }
#endif
    __Pyx_DECREF_TypeName(type_name);
    __Pyx_DECREF_TypeName(obj_type_name);
    return 0;
}

static CYTHON_INLINE void __Pyx_RaiseTooManyValuesError(Py_ssize_t expected) {
    PyErr_Format(PyExc_ValueError,
                 "too many values to unpack (expected %" CYTHON_FORMAT_SSIZE_T "d)", expected);
}

static CYTHON_INLINE void __Pyx_RaiseNeedMoreValuesError(Py_ssize_t index) {
    PyErr_Format(PyExc_ValueError,
                 "need more than %" CYTHON_FORMAT_SSIZE_T "d value%.1s to unpack",
                 index, (index == 1) ? "" : "s");
}

static CYTHON_INLINE int __Pyx_IterFinish(void) {
    PyObject* exc_type;
    __Pyx_PyThreadState_declare
    __Pyx_PyThreadState_assign
    exc_type = __Pyx_PyErr_CurrentExceptionType();
    if (unlikely(exc_type)) {
        if (unlikely(!__Pyx_PyErr_GivenExceptionMatches(exc_type, PyExc_StopIteration)))
            return -1;
        __Pyx_PyErr_Clear();
        return 0;
    }
    return 0;
}

static int __Pyx_IternextUnpackEndCheck(PyObject *retval, Py_ssize_t expected) {
    if (unlikely(retval)) {
        Py_DECREF(retval);
        __Pyx_RaiseTooManyValuesError(expected);
        return -1;
    }
    return __Pyx_IterFinish();
}

static int
__Pyx_RaiseUnexpectedTypeError(const char *expected, PyObject *obj)
{
    __Pyx_TypeName obj_type_name = __Pyx_PyType_GetFullyQualifiedName(Py_TYPE(obj));
    PyErr_Format(PyExc_TypeError, "Expected %s, got " __Pyx_FMT_TYPENAME,
                 expected, obj_type_name);
    __Pyx_DECREF_TypeName(obj_type_name);
    return 0;
}

static CYTHON_INLINE PyObject* __Pyx_PyObject_CallNoArg(PyObject *func) {
    PyObject *arg[2] = {NULL, NULL};
    return __Pyx_PyObject_FastCall(func, arg + 1, 0 | __Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET);
}

#if !(CYTHON_VECTORCALL && (__PYX_LIMITED_VERSION_HEX >= 0x030C0000 || (!CYTHON_COMPILING_IN_LIMITED_API && PY_VERSION_HEX >= 0x03090000)))
static int __Pyx_PyObject_GetMethod(PyObject *obj, PyObject *name, PyObject **method) {
    PyObject *attr;
#if CYTHON_UNPACK_METHODS && CYTHON_COMPILING_IN_CPYTHON && CYTHON_USE_PYTYPE_LOOKUP
    __Pyx_TypeName type_name;
    PyTypeObject *tp = Py_TYPE(obj);
    PyObject *descr;
    descrgetfunc f = NULL;
    PyObject **dictptr, *dict;
    int meth_found = 0;
    assert (*method == NULL);
    if (unlikely(tp->tp_getattro != PyObject_GenericGetAttr)) {
        attr = __Pyx_PyObject_GetAttrStr(obj, name);
        goto try_unpack;
    }
    if (unlikely(tp->tp_dict == NULL) && unlikely(PyType_Ready(tp) < 0)) {
        return 0;
    }
    descr = _PyType_Lookup(tp, name);
    if (likely(descr != NULL)) {
        Py_INCREF(descr);
#if defined(Py_TPFLAGS_METHOD_DESCRIPTOR) && Py_TPFLAGS_METHOD_DESCRIPTOR
        if (__Pyx_PyType_HasFeature(Py_TYPE(descr), Py_TPFLAGS_METHOD_DESCRIPTOR))
#else
        #ifdef __Pyx_CyFunction_USED
        if (likely(PyFunction_Check(descr) || __Pyx_IS_TYPE(descr, &PyMethodDescr_Type) || __Pyx_CyFunction_Check(descr)))
        #else
        if (likely(PyFunction_Check(descr) || __Pyx_IS_TYPE(descr, &PyMethodDescr_Type)))
        #endif
#endif
        {
            meth_found = 1;
        } else {
            f = Py_TYPE(descr)->tp_descr_get;
            if (f != NULL && PyDescr_IsData(descr)) {
                attr = f(descr, obj, (PyObject *)Py_TYPE(obj));
                Py_DECREF(descr);
                goto try_unpack;
            }
        }
    }
    dictptr = _PyObject_GetDictPtr(obj);
    if (dictptr != NULL && (dict = *dictptr) != NULL) {
        Py_INCREF(dict);
        attr = __Pyx_PyDict_GetItemStr(dict, name);
        if (attr != NULL) {
            Py_INCREF(attr);
            Py_DECREF(dict);
            Py_XDECREF(descr);
            goto try_unpack;
        }
        Py_DECREF(dict);
    }
    if (meth_found) {
        *method = descr;
        return 1;
    }
    if (f != NULL) {
        attr = f(descr, obj, (PyObject *)Py_TYPE(obj));
        Py_DECREF(descr);
        goto try_unpack;
    }
    if (likely(descr != NULL)) {
        *method = descr;
        return 0;
    }
    type_name = __Pyx_PyType_GetFullyQualifiedName(tp);
    PyErr_Format(PyExc_AttributeError,
                 "'" __Pyx_FMT_TYPENAME "' object has no attribute '%U'",
                 type_name, name);
    __Pyx_DECREF_TypeName(type_name);
    return 0;
#else
    attr = __Pyx_PyObject_GetAttrStr(obj, name);
    goto try_unpack;
#endif
try_unpack:
#if CYTHON_UNPACK_METHODS
    if (likely(attr) && PyMethod_Check(attr) && likely(PyMethod_GET_SELF(attr) == obj)) {
        PyObject *function = PyMethod_GET_FUNCTION(attr);
        Py_INCREF(function);
        Py_DECREF(attr);
        *method = function;
        return 1;
    }
#endif
    *method = attr;
    return 0;
}
#endif

static PyObject* __Pyx_PyObject_CallMethod0(PyObject* obj, PyObject* method_name) {
#if CYTHON_VECTORCALL && (__PYX_LIMITED_VERSION_HEX >= 0x030C0000 || (!CYTHON_COMPILING_IN_LIMITED_API && PY_VERSION_HEX >= 0x03090000))
    PyObject *args[1] = {obj};
    (void) __Pyx_PyObject_CallOneArg;
    (void) __Pyx_PyObject_CallNoArg;
    return PyObject_VectorcallMethod(method_name, args, 1 | PY_VECTORCALL_ARGUMENTS_OFFSET, NULL);
#else
    PyObject *method = NULL, *result = NULL;
    int is_method = __Pyx_PyObject_GetMethod(obj, method_name, &method);
    if (likely(is_method)) {
        result = __Pyx_PyObject_CallOneArg(method, obj);
        Py_DECREF(method);
        return result;
    }
    if (unlikely(!method)) goto bad;
    result = __Pyx_PyObject_CallNoArg(method);
    Py_DECREF(method);
bad:
    return result;
#endif
}

static CYTHON_INLINE void __Pyx_RaiseNoneNotIterableError(void) {
    PyErr_SetString(PyExc_TypeError, "'NoneType' object is not iterable");
}

static void __Pyx_UnpackTupleError(PyObject *t, Py_ssize_t index) {
    if (t == Py_None) {
      __Pyx_RaiseNoneNotIterableError();
    } else {
      Py_ssize_t size = __Pyx_PyTuple_GET_SIZE(t);
 #if !CYTHON_ASSUME_SAFE_SIZE
      if (unlikely(size < 0)) return;
 #endif
      if (size < index) {
        __Pyx_RaiseNeedMoreValuesError(size);
      } else {
        __Pyx_RaiseTooManyValuesError(index);
      }
    }
}

static CYTHON_INLINE int __Pyx_unpack_tuple2(
        PyObject* tuple, PyObject** value1, PyObject** value2, int is_tuple, int has_known_size, int decref_tuple) {
    if (likely(is_tuple || PyTuple_Check(tuple))) {
        Py_ssize_t size;
        if (has_known_size) {
            return __Pyx_unpack_tuple2_exact(tuple, value1, value2, decref_tuple);
        }
        size = __Pyx_PyTuple_GET_SIZE(tuple);
        if (likely(size == 2)) {
            return __Pyx_unpack_tuple2_exact(tuple, value1, value2, decref_tuple);
        }
        if (size >= 0) {
            __Pyx_UnpackTupleError(tuple, 2);
        }
        return -1;
    } else {
        return __Pyx_unpack_tuple2_generic(tuple, value1, value2, has_known_size, decref_tuple);
    }
}
static CYTHON_INLINE int __Pyx_unpack_tuple2_exact(
        PyObject* tuple, PyObject** pvalue1, PyObject** pvalue2, int decref_tuple) {
    PyObject *value1 = NULL, *value2 = NULL;
#if CYTHON_AVOID_BORROWED_REFS || !CYTHON_ASSUME_SAFE_MACROS
    value1 = __Pyx_PySequence_ITEM(tuple, 0);  if (unlikely(!value1)) goto bad;
    value2 = __Pyx_PySequence_ITEM(tuple, 1);  if (unlikely(!value2)) goto bad;
#else
    value1 = PyTuple_GET_ITEM(tuple, 0);  Py_INCREF(value1);
    value2 = PyTuple_GET_ITEM(tuple, 1);  Py_INCREF(value2);
#endif
    if (decref_tuple) {
        Py_DECREF(tuple);
    }
    *pvalue1 = value1;
    *pvalue2 = value2;
    return 0;
#if CYTHON_AVOID_BORROWED_REFS || !CYTHON_ASSUME_SAFE_MACROS
bad:
    Py_XDECREF(value1);
    Py_XDECREF(value2);
    if (decref_tuple) { Py_XDECREF(tuple); }
    return -1;
#endif
}
static int __Pyx_unpack_tuple2_generic(PyObject* tuple, PyObject** pvalue1, PyObject** pvalue2,
                                       int has_known_size, int decref_tuple) {
    Py_ssize_t index;
    PyObject *value1 = NULL, *value2 = NULL, *iter = NULL;
    iternextfunc iternext;
    iter = PyObject_GetIter(tuple);
    if (unlikely(!iter)) goto bad;
    if (decref_tuple) { Py_DECREF(tuple); tuple = NULL; }
    iternext = __Pyx_PyObject_GetIterNextFunc(iter);
    value1 = iternext(iter); if (unlikely(!value1)) { index = 0; goto unpacking_failed; }
    value2 = iternext(iter); if (unlikely(!value2)) { index = 1; goto unpacking_failed; }
    if (!has_known_size && unlikely(__Pyx_IternextUnpackEndCheck(iternext(iter), 2))) goto bad;
    Py_DECREF(iter);
    *pvalue1 = value1;
    *pvalue2 = value2;
    return 0;
unpacking_failed:
    if (!has_known_size && __Pyx_IterFinish() == 0)
        __Pyx_RaiseNeedMoreValuesError(index);
bad:
    Py_XDECREF(iter);
    Py_XDECREF(value1);
    Py_XDECREF(value2);
    if (decref_tuple) { Py_XDECREF(tuple); }
    return -1;
}

#if CYTHON_COMPILING_IN_PYPY
#include <string.h>
#endif
static CYTHON_INLINE PyObject* __Pyx_dict_iterator(PyObject* iterable, int is_dict, PyObject* method_name,
                                                   Py_ssize_t* p_orig_length, int* p_source_is_dict) {
    is_dict = is_dict || likely(PyDict_CheckExact(iterable));
    *p_source_is_dict = is_dict;
    if (is_dict) {
#if !CYTHON_COMPILING_IN_PYPY
        *p_orig_length = PyDict_Size(iterable);
        Py_INCREF(iterable);
        return iterable;
#else
        static PyObject *py_items = NULL, *py_keys = NULL, *py_values = NULL;
        PyObject **pp = NULL;
        if (method_name) {
            const char *name = PyUnicode_AsUTF8(method_name);
            if (strcmp(name, "iteritems") == 0) pp = &py_items;
            else if (strcmp(name, "iterkeys") == 0) pp = &py_keys;
            else if (strcmp(name, "itervalues") == 0) pp = &py_values;
            if (pp) {
                if (!*pp) {
                    *pp = PyUnicode_FromString(name + 4);
                    if (!*pp)
                        return NULL;
                }
                method_name = *pp;
            }
        }
#endif
    }
    *p_orig_length = 0;
    if (method_name) {
        PyObject* iter;
        iterable = __Pyx_PyObject_CallMethod0(iterable, method_name);
        if (!iterable)
            return NULL;
#if !CYTHON_COMPILING_IN_PYPY
        if (PyTuple_CheckExact(iterable) || PyList_CheckExact(iterable))
            return iterable;
#endif
        iter = PyObject_GetIter(iterable);
        Py_DECREF(iterable);
        return iter;
    }
    return PyObject_GetIter(iterable);
}
#if !CYTHON_AVOID_BORROWED_REFS
static CYTHON_INLINE int __Pyx_dict_iter_next_source_is_dict(
        PyObject* iter_obj, CYTHON_NCP_UNUSED Py_ssize_t orig_length, CYTHON_NCP_UNUSED Py_ssize_t* ppos,
        PyObject** pkey, PyObject** pvalue, PyObject** pitem) {
    PyObject *key, *value;
    if (unlikely(orig_length != PyDict_Size(iter_obj))) {
        PyErr_SetString(PyExc_RuntimeError, "dictionary changed size during iteration");
        return -1;
    }
    if (unlikely(!PyDict_Next(iter_obj, ppos, &key, &value))) {
        return 0;
    }
    if (pitem) {
        PyObject* tuple = PyTuple_New(2);
        if (unlikely(!tuple)) {
            return -1;
        }
        Py_INCREF(key);
        Py_INCREF(value);
        #if CYTHON_ASSUME_SAFE_MACROS
        PyTuple_SET_ITEM(tuple, 0, key);
        PyTuple_SET_ITEM(tuple, 1, value);
        #else
        if (unlikely(PyTuple_SetItem(tuple, 0, key) < 0)) {
            Py_DECREF(value);
            Py_DECREF(tuple);
            return -1;
        }
        if (unlikely(PyTuple_SetItem(tuple, 1, value) < 0)) {
            Py_DECREF(tuple);
            return -1;
        }
        #endif
        *pitem = tuple;
    } else {
        if (pkey) {
            Py_INCREF(key);
            *pkey = key;
        }
        if (pvalue) {
            Py_INCREF(value);
            *pvalue = value;
        }
    }
    return 1;
}
#endif
static CYTHON_INLINE int __Pyx_dict_iter_next(
        PyObject* iter_obj, CYTHON_NCP_UNUSED Py_ssize_t orig_length, CYTHON_NCP_UNUSED Py_ssize_t* ppos,
        PyObject** pkey, PyObject** pvalue, PyObject** pitem, int source_is_dict) {
    PyObject* next_item;
#if !CYTHON_AVOID_BORROWED_REFS
    if (source_is_dict) {
        int result;
#if PY_VERSION_HEX >= 0x030d0000 && !CYTHON_COMPILING_IN_LIMITED_API
        Py_BEGIN_CRITICAL_SECTION(iter_obj);
#endif
        result = __Pyx_dict_iter_next_source_is_dict(iter_obj, orig_length, ppos, pkey, pvalue, pitem);
#if PY_VERSION_HEX >= 0x030d0000 && !CYTHON_COMPILING_IN_LIMITED_API
        Py_END_CRITICAL_SECTION();
#endif
        return result;
    } else if (PyTuple_CheckExact(iter_obj)) {
        Py_ssize_t pos = *ppos;
        Py_ssize_t tuple_size = __Pyx_PyTuple_GET_SIZE(iter_obj);
        #if !CYTHON_ASSUME_SAFE_SIZE
        if (unlikely(tuple_size < 0)) return -1;
        #endif
        if (unlikely(pos >= tuple_size)) return 0;
        *ppos = pos + 1;
        #if CYTHON_ASSUME_SAFE_MACROS
        next_item = PyTuple_GET_ITEM(iter_obj, pos);
        #else
        next_item = PyTuple_GetItem(iter_obj, pos);
        if (unlikely(!next_item)) return -1;
        #endif
        Py_INCREF(next_item);
    } else if (PyList_CheckExact(iter_obj)) {
        Py_ssize_t pos = *ppos;
        Py_ssize_t list_size = __Pyx_PyList_GET_SIZE(iter_obj);
        #if !CYTHON_ASSUME_SAFE_SIZE
        if (unlikely(list_size < 0)) return -1;
        #endif
        if (unlikely(pos >= list_size)) return 0;
        *ppos = pos + 1;
        next_item = __Pyx_PyList_GetItemRef(iter_obj, pos);
        if (unlikely(!next_item)) return -1;
    } else
#endif
    {
        next_item = PyIter_Next(iter_obj);
        if (unlikely(!next_item)) {
            return __Pyx_IterFinish();
        }
    }
    if (pitem) {
        *pitem = next_item;
    } else if (pkey && pvalue) {
        if (__Pyx_unpack_tuple2(next_item, pkey, pvalue, source_is_dict, source_is_dict, 1))
            return -1;
    } else if (pkey) {
        *pkey = next_item;
    } else {
        *pvalue = next_item;
    }
    return 1;
}

#if CYTHON_USE_TYPE_SLOTS
static PyObject *__Pyx_PyObject_GetIndex(PyObject *obj, PyObject *index) {
    PyObject *runerr = NULL;
    Py_ssize_t key_value;
    key_value = __Pyx_PyIndex_AsSsize_t(index);
    if (likely(key_value != -1 || !(runerr = PyErr_Occurred()))) {
        return __Pyx_GetItemInt_Fast(obj, key_value, 0, 1, 1, 1);
    }
    if (PyErr_GivenExceptionMatches(runerr, PyExc_OverflowError)) {
        __Pyx_TypeName index_type_name = __Pyx_PyType_GetFullyQualifiedName(Py_TYPE(index));
        PyErr_Clear();
        PyErr_Format(PyExc_IndexError,
            "cannot fit '" __Pyx_FMT_TYPENAME "' into an index-sized integer", index_type_name);
        __Pyx_DECREF_TypeName(index_type_name);
    }
    return NULL;
}
static PyObject *__Pyx_PyObject_GetItem_Slow(PyObject *obj, PyObject *key) {
    __Pyx_TypeName obj_type_name;
    if (likely(PyType_Check(obj))) {
        PyObject *meth = __Pyx_PyObject_GetAttrStrNoError(obj, __pyx_mstate_global->__pyx_n_u_class_getitem);
        if (!meth) {
            PyErr_Clear();
        } else {
            PyObject *result = __Pyx_PyObject_CallOneArg(meth, key);
            Py_DECREF(meth);
            return result;
        }
    }
    obj_type_name = __Pyx_PyType_GetFullyQualifiedName(Py_TYPE(obj));
    PyErr_Format(PyExc_TypeError,
        "'" __Pyx_FMT_TYPENAME "' object is not subscriptable", obj_type_name);
    __Pyx_DECREF_TypeName(obj_type_name);
    return NULL;
}
static PyObject *__Pyx_PyObject_GetItem(PyObject *obj, PyObject *key) {
    PyTypeObject *tp = Py_TYPE(obj);
    PyMappingMethods *mm = tp->tp_as_mapping;
    PySequenceMethods *sm = tp->tp_as_sequence;
    if (likely(mm && mm->mp_subscript)) {
        return mm->mp_subscript(obj, key);
    }
    if (likely(sm && sm->sq_item)) {
        return __Pyx_PyObject_GetIndex(obj, key);
    }
    return __Pyx_PyObject_GetItem_Slow(obj, key);
}
#endif

#if CYTHON_USE_DICT_VERSIONS && CYTHON_USE_TYPE_SLOTS
static CYTHON_INLINE PY_UINT64_T __Pyx_get_tp_dict_version(PyObject *obj) {
    PyObject *dict = Py_TYPE(obj)->tp_dict;
    return likely(dict) ? __PYX_GET_DICT_VERSION(dict) : 0;
}
static CYTHON_INLINE PY_UINT64_T __Pyx_get_object_dict_version(PyObject *obj) {
    PyObject **dictptr = NULL;
    Py_ssize_t offset = Py_TYPE(obj)->tp_dictoffset;
    if (offset) {
#if CYTHON_COMPILING_IN_CPYTHON
        dictptr = (likely(offset > 0)) ? (PyObject **) ((char *)obj + offset) : _PyObject_GetDictPtr(obj);
#else
        dictptr = _PyObject_GetDictPtr(obj);
#endif
    }
    return (dictptr && *dictptr) ? __PYX_GET_DICT_VERSION(*dictptr) : 0;
}
static CYTHON_INLINE int __Pyx_object_dict_version_matches(PyObject* obj, PY_UINT64_T tp_dict_version, PY_UINT64_T obj_dict_version) {
    PyObject *dict = Py_TYPE(obj)->tp_dict;
    if (unlikely(!dict) || unlikely(tp_dict_version != __PYX_GET_DICT_VERSION(dict)))
        return 0;
    return obj_dict_version == __Pyx_get_object_dict_version(obj);
}
#endif

#if CYTHON_USE_DICT_VERSIONS
static PyObject *__Pyx__GetModuleGlobalName(PyObject *name, PY_UINT64_T *dict_version, PyObject **dict_cached_value)
#else
static CYTHON_INLINE PyObject *__Pyx__GetModuleGlobalName(PyObject *name)
#endif
{
    PyObject *result;
#if CYTHON_COMPILING_IN_LIMITED_API
    if (unlikely(!__pyx_m)) {
        if (!PyErr_Occurred())
            PyErr_SetNone(PyExc_NameError);
        return NULL;
    }
    result = PyObject_GetAttr(__pyx_m, name);
    if (likely(result)) {
        return result;
    }
    PyErr_Clear();
#elif CYTHON_AVOID_BORROWED_REFS || CYTHON_AVOID_THREAD_UNSAFE_BORROWED_REFS
    if (unlikely(__Pyx_PyDict_GetItemRef(__pyx_mstate_global->__pyx_d, name, &result) == -1)) PyErr_Clear();
    __PYX_UPDATE_DICT_CACHE(__pyx_mstate_global->__pyx_d, result, *dict_cached_value, *dict_version)
    if (likely(result)) {
        return result;
    }
#else
    result = _PyDict_GetItem_KnownHash(__pyx_mstate_global->__pyx_d, name, ((PyASCIIObject *) name)->hash);
    __PYX_UPDATE_DICT_CACHE(__pyx_mstate_global->__pyx_d, result, *dict_cached_value, *dict_version)
    if (likely(result)) {
        return __Pyx_NewRef(result);
    }
    PyErr_Clear();
#endif
    return __Pyx_GetBuiltinName(name);
}

#if CYTHON_USE_EXC_INFO_STACK && CYTHON_FAST_THREAD_STATE
static _PyErr_StackItem *
__Pyx_PyErr_GetTopmostException(PyThreadState *tstate)
{
    _PyErr_StackItem *exc_info = tstate->exc_info;
    while ((exc_info->exc_value == NULL || exc_info->exc_value == Py_None) &&
           exc_info->previous_item != NULL)
    {
        exc_info = exc_info->previous_item;
    }
    return exc_info;
}
#endif

#if CYTHON_FAST_THREAD_STATE
static CYTHON_INLINE void __Pyx__ExceptionSave(PyThreadState *tstate, PyObject **type, PyObject **value, PyObject **tb) {
  #if CYTHON_USE_EXC_INFO_STACK && PY_VERSION_HEX >= 0x030B00a4
    _PyErr_StackItem *exc_info = __Pyx_PyErr_GetTopmostException(tstate);
    PyObject *exc_value = exc_info->exc_value;
    if (exc_value == NULL || exc_value == Py_None) {
        *value = NULL;
        *type = NULL;
        *tb = NULL;
    } else {
        *value = exc_value;
        Py_INCREF(*value);
        *type = (PyObject*) Py_TYPE(exc_value);
        Py_INCREF(*type);
        *tb = PyException_GetTraceback(exc_value);
    }
  #elif CYTHON_USE_EXC_INFO_STACK
    _PyErr_StackItem *exc_info = __Pyx_PyErr_GetTopmostException(tstate);
    *type = exc_info->exc_type;
    *value = exc_info->exc_value;
    *tb = exc_info->exc_traceback;
    Py_XINCREF(*type);
    Py_XINCREF(*value);
    Py_XINCREF(*tb);
  #else
    *type = tstate->exc_type;
    *value = tstate->exc_value;
    *tb = tstate->exc_traceback;
    Py_XINCREF(*type);
    Py_XINCREF(*value);
    Py_XINCREF(*tb);
  #endif
}
static CYTHON_INLINE void __Pyx__ExceptionReset(PyThreadState *tstate, PyObject *type, PyObject *value, PyObject *tb) {
  #if CYTHON_USE_EXC_INFO_STACK && PY_VERSION_HEX >= 0x030B00a4
    _PyErr_StackItem *exc_info = tstate->exc_info;
    PyObject *tmp_value = exc_info->exc_value;
    exc_info->exc_value = value;
    Py_XDECREF(tmp_value);
    Py_XDECREF(type);
    Py_XDECREF(tb);
  #else
    PyObject *tmp_type, *tmp_value, *tmp_tb;
    #if CYTHON_USE_EXC_INFO_STACK
    _PyErr_StackItem *exc_info = tstate->exc_info;
    tmp_type = exc_info->exc_type;
    tmp_value = exc_info->exc_value;
    tmp_tb = exc_info->exc_traceback;
    exc_info->exc_type = type;
    exc_info->exc_value = value;
    exc_info->exc_traceback = tb;
    #else
    tmp_type = tstate->exc_type;
    tmp_value = tstate->exc_value;
    tmp_tb = tstate->exc_traceback;
    tstate->exc_type = type;
    tstate->exc_value = value;
    tstate->exc_traceback = tb;
    #endif
    Py_XDECREF(tmp_type);
    Py_XDECREF(tmp_value);
    Py_XDECREF(tmp_tb);
  #endif
}
#endif

#if CYTHON_COMPILING_IN_CPYTHON
static int __Pyx_InBases(PyTypeObject *a, PyTypeObject *b) {
    while (a) {
        a = __Pyx_PyType_GetSlot(a, tp_base, PyTypeObject*);
        if (a == b)
            return 1;
    }
    return b == &PyBaseObject_Type;
}
static CYTHON_INLINE int __Pyx_IsSubtype(PyTypeObject *a, PyTypeObject *b) {
    PyObject *mro;
    if (a == b) return 1;
    mro = a->tp_mro;
    if (likely(mro)) {
        Py_ssize_t i, n;
        n = PyTuple_GET_SIZE(mro);
        for (i = 0; i < n; i++) {
            if (PyTuple_GET_ITEM(mro, i) == (PyObject *)b)
                return 1;
        }
        return 0;
    }
    return __Pyx_InBases(a, b);
}
static CYTHON_INLINE int __Pyx_IsAnySubtype2(PyTypeObject *cls, PyTypeObject *a, PyTypeObject *b) {
    PyObject *mro;
    if (cls == a || cls == b) return 1;
    mro = cls->tp_mro;
    if (likely(mro)) {
        Py_ssize_t i, n;
        n = PyTuple_GET_SIZE(mro);
        for (i = 0; i < n; i++) {
            PyObject *base = PyTuple_GET_ITEM(mro, i);
            if (base == (PyObject *)a || base == (PyObject *)b)
                return 1;
        }
        return 0;
    }
    return __Pyx_InBases(cls, a) || __Pyx_InBases(cls, b);
}
static CYTHON_INLINE int __Pyx_inner_PyErr_GivenExceptionMatches2(PyObject *err, PyObject* exc_type1, PyObject *exc_type2) {
    if (exc_type1) {
        return __Pyx_IsAnySubtype2((PyTypeObject*)err, (PyTypeObject*)exc_type1, (PyTypeObject*)exc_type2);
    } else {
        return __Pyx_IsSubtype((PyTypeObject*)err, (PyTypeObject*)exc_type2);
    }
}
static int __Pyx_PyErr_GivenExceptionMatchesTuple(PyObject *exc_type, PyObject *tuple) {
    Py_ssize_t i, n;
    assert(PyExceptionClass_Check(exc_type));
    n = PyTuple_GET_SIZE(tuple);
    for (i=0; i<n; i++) {
        if (exc_type == PyTuple_GET_ITEM(tuple, i)) return 1;
    }
    for (i=0; i<n; i++) {
        PyObject *t = PyTuple_GET_ITEM(tuple, i);
        if (likely(PyExceptionClass_Check(t))) {
            if (__Pyx_inner_PyErr_GivenExceptionMatches2(exc_type, NULL, t)) return 1;
        } else {
        }
    }
    return 0;
}
static CYTHON_INLINE int __Pyx_PyErr_GivenExceptionMatches(PyObject *err, PyObject* exc_type) {
    if (likely(err == exc_type)) return 1;
    if (likely(PyExceptionClass_Check(err))) {
        if (likely(PyExceptionClass_Check(exc_type))) {
            return __Pyx_inner_PyErr_GivenExceptionMatches2(err, NULL, exc_type);
        } else if (likely(PyTuple_Check(exc_type))) {
            return __Pyx_PyErr_GivenExceptionMatchesTuple(err, exc_type);
        } else {
        }
    }
    return PyErr_GivenExceptionMatches(err, exc_type);
}
static CYTHON_INLINE int __Pyx_PyErr_GivenExceptionMatches2(PyObject *err, PyObject *exc_type1, PyObject *exc_type2) {
    assert(PyExceptionClass_Check(exc_type1));
    assert(PyExceptionClass_Check(exc_type2));
    if (likely(err == exc_type1 || err == exc_type2)) return 1;
    if (likely(PyExceptionClass_Check(err))) {
        return __Pyx_inner_PyErr_GivenExceptionMatches2(err, exc_type1, exc_type2);
    }
    return (PyErr_GivenExceptionMatches(err, exc_type1) || PyErr_GivenExceptionMatches(err, exc_type2));
}
#endif

#if CYTHON_FAST_THREAD_STATE
static int __Pyx__GetException(PyThreadState *tstate, PyObject **type, PyObject **value, PyObject **tb)
#else
static int __Pyx_GetException(PyObject **type, PyObject **value, PyObject **tb)
#endif
{
    PyObject *local_type = NULL, *local_value, *local_tb = NULL;
#if CYTHON_FAST_THREAD_STATE
    PyObject *tmp_type, *tmp_value, *tmp_tb;
  #if PY_VERSION_HEX >= 0x030C0000
    local_value = tstate->current_exception;
    tstate->current_exception = 0;
  #else
    local_type = tstate->curexc_type;
    local_value = tstate->curexc_value;
    local_tb = tstate->curexc_traceback;
    tstate->curexc_type = 0;
    tstate->curexc_value = 0;
    tstate->curexc_traceback = 0;
  #endif
#elif __PYX_LIMITED_VERSION_HEX > 0x030C0000
    local_value = PyErr_GetRaisedException();
#else
    PyErr_Fetch(&local_type, &local_value, &local_tb);
#endif
#if __PYX_LIMITED_VERSION_HEX > 0x030C0000
    if (likely(local_value)) {
        local_type = (PyObject*) Py_TYPE(local_value);
        Py_INCREF(local_type);
        local_tb = PyException_GetTraceback(local_value);
    }
#else
    PyErr_NormalizeException(&local_type, &local_value, &local_tb);
#if CYTHON_FAST_THREAD_STATE
    if (unlikely(tstate->curexc_type))
#else
    if (unlikely(PyErr_Occurred()))
#endif
        goto bad;
    if (local_tb) {
        if (unlikely(PyException_SetTraceback(local_value, local_tb) < 0))
            goto bad;
    }
#endif
    Py_XINCREF(local_tb);
    Py_XINCREF(local_type);
    Py_XINCREF(local_value);
    *type = local_type;
    *value = local_value;
    *tb = local_tb;
#if CYTHON_FAST_THREAD_STATE
    #if CYTHON_USE_EXC_INFO_STACK
    {
        _PyErr_StackItem *exc_info = tstate->exc_info;
      #if PY_VERSION_HEX >= 0x030B00a4
        tmp_value = exc_info->exc_value;
        exc_info->exc_value = local_value;
        tmp_type = NULL;
        tmp_tb = NULL;
        Py_XDECREF(local_type);
        Py_XDECREF(local_tb);
      #else
        tmp_type = exc_info->exc_type;
        tmp_value = exc_info->exc_value;
        tmp_tb = exc_info->exc_traceback;
        exc_info->exc_type = local_type;
        exc_info->exc_value = local_value;
        exc_info->exc_traceback = local_tb;
      #endif
    }
    #else
    tmp_type = tstate->exc_type;
    tmp_value = tstate->exc_value;
    tmp_tb = tstate->exc_traceback;
    tstate->exc_type = local_type;
    tstate->exc_value = local_value;
    tstate->exc_traceback = local_tb;
    #endif
    Py_XDECREF(tmp_type);
    Py_XDECREF(tmp_value);
    Py_XDECREF(tmp_tb);
#elif __PYX_LIMITED_VERSION_HEX >= 0x030b0000
    PyErr_SetHandledException(local_value);
    Py_XDECREF(local_value);
    Py_XDECREF(local_type);
    Py_XDECREF(local_tb);
#else
    PyErr_SetExcInfo(local_type, local_value, local_tb);
#endif
    return 0;
#if __PYX_LIMITED_VERSION_HEX <= 0x030C0000
bad:
    *type = 0;
    *value = 0;
    *tb = 0;
    Py_XDECREF(local_type);
    Py_XDECREF(local_value);
    Py_XDECREF(local_tb);
    return -1;
#endif
}

#if !CYTHON_COMPILING_IN_PYPY
static PyObject* __Pyx_Fallback___Pyx_PyLong_AddObjC(PyObject *op1, PyObject *op2, int inplace) {
    return (inplace ? PyNumber_InPlaceAdd : PyNumber_Add)(op1, op2);
}
#if CYTHON_USE_PYLONG_INTERNALS
static PyObject* __Pyx_Unpacked___Pyx_PyLong_AddObjC(PyObject *op1, PyObject *op2, long intval, int inplace, int zerodivision_check) {
    CYTHON_MAYBE_UNUSED_VAR(inplace);
    CYTHON_UNUSED_VAR(zerodivision_check);
    const long b = intval;
    long a;
    const PY_LONG_LONG llb = intval;
    PY_LONG_LONG lla;
    if (unlikely(__Pyx_PyLong_IsZero(op1))) {
        return __Pyx_NewRef(op2);
    }
    const int is_positive = __Pyx_PyLong_IsPos(op1);
    const digit* digits = __Pyx_PyLong_Digits(op1);
    const Py_ssize_t size = __Pyx_PyLong_DigitCount(op1);
    if (likely(size == 1)) {
        a = (long) digits[0];
        if (!is_positive) a *= -1;
    } else {
        switch (size) {
            case 2:
                if (8 * sizeof(long) - 1 > 2 * PyLong_SHIFT) {
                    a = (long) (((((unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0]));
                    if (!is_positive) a *= -1;
                    goto calculate_long;
                } else if (8 * sizeof(PY_LONG_LONG) - 1 > 2 * PyLong_SHIFT) {
                    lla = (PY_LONG_LONG) (((((unsigned PY_LONG_LONG)digits[1]) << PyLong_SHIFT) | (unsigned PY_LONG_LONG)digits[0]));
                    if (!is_positive) lla *= -1;
                    goto calculate_long_long;
                }
                break;
            case 3:
                if (8 * sizeof(long) - 1 > 3 * PyLong_SHIFT) {
                    a = (long) (((((((unsigned long)digits[2]) << PyLong_SHIFT) | (unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0]));
                    if (!is_positive) a *= -1;
                    goto calculate_long;
                } else if (8 * sizeof(PY_LONG_LONG) - 1 > 3 * PyLong_SHIFT) {
                    lla = (PY_LONG_LONG) (((((((unsigned PY_LONG_LONG)digits[2]) << PyLong_SHIFT) | (unsigned PY_LONG_LONG)digits[1]) << PyLong_SHIFT) | (unsigned PY_LONG_LONG)digits[0]));
                    if (!is_positive) lla *= -1;
                    goto calculate_long_long;
                }
                break;
            case 4:
                if (8 * sizeof(long) - 1 > 4 * PyLong_SHIFT) {
                    a = (long) (((((((((unsigned long)digits[3]) << PyLong_SHIFT) | (unsigned long)digits[2]) << PyLong_SHIFT) | (unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0]));
                    if (!is_positive) a *= -1;
                    goto calculate_long;
                } else if (8 * sizeof(PY_LONG_LONG) - 1 > 4 * PyLong_SHIFT) {
                    lla = (PY_LONG_LONG) (((((((((unsigned PY_LONG_LONG)digits[3]) << PyLong_SHIFT) | (unsigned PY_LONG_LONG)digits[2]) << PyLong_SHIFT) | (unsigned PY_LONG_LONG)digits[1]) << PyLong_SHIFT) | (unsigned PY_LONG_LONG)digits[0]));
                    if (!is_positive) lla *= -1;
                    goto calculate_long_long;
                }
                break;
        }
        return PyLong_Type.tp_as_number->nb_add(op1, op2);
    }
    calculate_long:
        {
            long x;
            x = a + b;
            return PyLong_FromLong(x);
        }
    calculate_long_long:
        {
            PY_LONG_LONG llx;
            llx = lla + llb;
            return PyLong_FromLongLong(llx);
        }

}
#endif
static PyObject* __Pyx_Float___Pyx_PyLong_AddObjC(PyObject *float_val, long intval, int zerodivision_check) {
    CYTHON_UNUSED_VAR(zerodivision_check);
    const long b = intval;
    double a = __Pyx_PyFloat_AS_DOUBLE(float_val);
        double result;

        result = ((double)a) + (double)b;
        return PyFloat_FromDouble(result);
}
static CYTHON_INLINE PyObject* __Pyx_PyLong_AddObjC(PyObject *op1, PyObject *op2, long intval, int inplace, int zerodivision_check) {
    CYTHON_MAYBE_UNUSED_VAR(intval);
    CYTHON_UNUSED_VAR(zerodivision_check);
    #if CYTHON_USE_PYLONG_INTERNALS
    if (likely(PyLong_CheckExact(op1))) {
        return __Pyx_Unpacked___Pyx_PyLong_AddObjC(op1, op2, intval, inplace, zerodivision_check);
    }
    #endif
    if (PyFloat_CheckExact(op1)) {
        return __Pyx_Float___Pyx_PyLong_AddObjC(op1, intval, zerodivision_check);
    }
    return __Pyx_Fallback___Pyx_PyLong_AddObjC(op1, op2, inplace);
}
#endif

static int __Pyx_SetItemInt_Generic(PyObject *o, PyObject *j, PyObject *v) {
    int r;
    if (unlikely(!j)) return -1;
    r = PyObject_SetItem(o, j, v);
    Py_DECREF(j);
    return r;
}
static CYTHON_INLINE int __Pyx_SetItemInt_Fast(PyObject *o, Py_ssize_t i, PyObject *v, int is_list,
                                               int wraparound, int boundscheck, int unsafe_shared) {
    CYTHON_MAYBE_UNUSED_VAR(unsafe_shared);
#if CYTHON_ASSUME_SAFE_MACROS && CYTHON_ASSUME_SAFE_SIZE && !CYTHON_AVOID_BORROWED_REFS
    if (is_list || PyList_CheckExact(o)) {
        Py_ssize_t n = (!wraparound) ? i : ((likely(i >= 0)) ? i : i + PyList_GET_SIZE(o));
        if ((CYTHON_AVOID_THREAD_UNSAFE_BORROWED_REFS && !__Pyx_IS_UNIQUELY_REFERENCED(o, unsafe_shared))) {
            Py_INCREF(v);
            return PyList_SetItem(o, n, v);
        } else if ((!boundscheck) || likely(__Pyx_is_valid_index(n, PyList_GET_SIZE(o)))) {
            PyObject* old;
            Py_INCREF(v);
            old = PyList_GET_ITEM(o, n);
            PyList_SET_ITEM(o, n, v);
            Py_DECREF(old);
            return 0;
        }
    } else
#endif
#if CYTHON_USE_TYPE_SLOTS && !CYTHON_COMPILING_IN_PYPY
    {
        PyMappingMethods *mm = Py_TYPE(o)->tp_as_mapping;
        PySequenceMethods *sm = Py_TYPE(o)->tp_as_sequence;
        if (!is_list && mm && mm->mp_ass_subscript) {
            int r;
            PyObject *key = PyLong_FromSsize_t(i);
            if (unlikely(!key)) return -1;
            r = mm->mp_ass_subscript(o, key, v);
            Py_DECREF(key);
            return r;
        }
        if (is_list || likely(sm && sm->sq_ass_item)) {
            if (wraparound && unlikely(i < 0) && likely(sm->sq_length)) {
                Py_ssize_t l = sm->sq_length(o);
                if (likely(l >= 0)) {
                    i += l;
                } else {
                    if (!PyErr_ExceptionMatches(PyExc_OverflowError))
                        return -1;
                    PyErr_Clear();
                }
            }
            return sm->sq_ass_item(o, i, v);
        }
    }
#else
    if (is_list || !PyMapping_Check(o)) {
        return PySequence_SetItem(o, i, v);
    }
#endif
    (void)wraparound;
    (void)boundscheck;
    return __Pyx_SetItemInt_Generic(o, PyLong_FromSsize_t(i), v);
}

static CYTHON_INLINE int __Pyx_PyDict_Pop_ignore(PyObject *d, PyObject *key, PyObject *default_value) {
#if CYTHON_COMPILING_IN_CPYTHON && PY_VERSION_HEX >= 0x030d00A2 || defined(PyDict_Pop)
    int result = PyDict_Pop(d, key, NULL);
    CYTHON_UNUSED_VAR(default_value);
    return (unlikely(result == -1)) ? -1 : 0;
#else
    PyObject *value;
    CYTHON_UNUSED_VAR(default_value);
    #if CYTHON_COMPILING_IN_CPYTHON && PY_VERSION_HEX < 0x030d0000
    value = _PyDict_Pop(d, key, Py_None);
    #else
    value = __Pyx_CallUnboundCMethod2(&__pyx_mstate_global->__pyx_umethod_PyDict_Type_pop, d, key, Py_None);
    #endif
    if (unlikely(value == NULL))
        return -1;
    Py_DECREF(value);
    return 0;
#endif
}

static CYTHON_INLINE int __Pyx_PyObject_SetSlice(PyObject* obj, PyObject* value,
        Py_ssize_t cstart, Py_ssize_t cstop,
        PyObject** _py_start, PyObject** _py_stop, PyObject** _py_slice,
        int has_cstart, int has_cstop, CYTHON_UNUSED int wraparound) {
    __Pyx_TypeName obj_type_name;
#if CYTHON_USE_TYPE_SLOTS
    PyMappingMethods* mp = Py_TYPE(obj)->tp_as_mapping;
    if (likely(mp && mp->mp_ass_subscript))
#endif
    {
        int result;
        PyObject *py_slice, *py_start, *py_stop;
        if (_py_slice) {
            py_slice = *_py_slice;
        } else {
            PyObject* owned_start = NULL;
            PyObject* owned_stop = NULL;
            if (_py_start) {
                py_start = *_py_start;
            } else {
                if (has_cstart) {
                    owned_start = py_start = PyLong_FromSsize_t(cstart);
                    if (unlikely(!py_start)) goto bad;
                } else
                    py_start = Py_None;
            }
            if (_py_stop) {
                py_stop = *_py_stop;
            } else {
                if (has_cstop) {
                    owned_stop = py_stop = PyLong_FromSsize_t(cstop);
                    if (unlikely(!py_stop)) {
                        Py_XDECREF(owned_start);
                        goto bad;
                    }
                } else
                    py_stop = Py_None;
            }
            py_slice = PySlice_New(py_start, py_stop, Py_None);
            Py_XDECREF(owned_start);
            Py_XDECREF(owned_stop);
            if (unlikely(!py_slice)) goto bad;
        }
#if CYTHON_USE_TYPE_SLOTS
        result = mp->mp_ass_subscript(obj, py_slice, value);
#else
        result = value ? PyObject_SetItem(obj, py_slice, value) : PyObject_DelItem(obj, py_slice);
#endif
        if (!_py_slice) {
            Py_DECREF(py_slice);
        }
        return result;
    }
    obj_type_name = __Pyx_PyType_GetFullyQualifiedName(Py_TYPE(obj));
    PyErr_Format(PyExc_TypeError,
        "'" __Pyx_FMT_TYPENAME "' object does not support slice %.10s",
        obj_type_name, value ? "assignment" : "deletion");
    __Pyx_DECREF_TypeName(obj_type_name);
bad:
    return -1;
}

#if __PYX_LIMITED_VERSION_HEX < 0x030d0000
static CYTHON_INLINE int __Pyx_HasAttr(PyObject *o, PyObject *n) {
    PyObject *r;
    if (unlikely(!PyUnicode_Check(n))) {
        PyErr_SetString(PyExc_TypeError,
                        "hasattr(): attribute name must be string");
        return -1;
    }
    r = __Pyx_PyObject_GetAttrStrNoError(o, n);
    if (!r) {
        return (unlikely(PyErr_Occurred())) ? -1 : 0;
    } else {
        Py_DECREF(r);
        return 1;
    }
}
#endif

static int __Pyx__Import_GetModule(PyObject *qualname, PyObject **module) {
    PyObject *imported_module = PyImport_GetModule(qualname);
    if (unlikely(!imported_module)) {
        *module = NULL;
        if (PyErr_Occurred()) {
            return -1;
        }
        return 0;
    }
    *module = imported_module;
    return 1;
}
static int __Pyx__Import_Lookup(PyObject *qualname, PyObject *const *imported_names, Py_ssize_t len_imported_names, PyObject **module) {
    PyObject *imported_module;
    PyObject *top_level_package_name;
    Py_ssize_t i;
    int status, module_found;
    Py_ssize_t dot_index;
    module_found = __Pyx__Import_GetModule(qualname, &imported_module);
    if (unlikely(!module_found || module_found == -1)) {
        *module = NULL;
        return module_found;
    }
    if (imported_names) {
        for (i = 0; i < len_imported_names; i++) {
            PyObject *imported_name = imported_names[i];
#if __PYX_LIMITED_VERSION_HEX < 0x030d0000
            int has_imported_attribute = PyObject_HasAttr(imported_module, imported_name);
#else
            int has_imported_attribute = PyObject_HasAttrWithError(imported_module, imported_name);
            if (unlikely(has_imported_attribute == -1)) goto error;
#endif
            if (!has_imported_attribute) {
                goto not_found;
            }
        }
        *module = imported_module;
        return 1;
    }
    dot_index = PyUnicode_FindChar(qualname, '.', 0, PY_SSIZE_T_MAX, 1);
    if (dot_index == -1) {
        *module = imported_module;
        return 1;
    }
    if (unlikely(dot_index == -2)) goto error;
    top_level_package_name = PyUnicode_Substring(qualname, 0, dot_index);
    if (unlikely(!top_level_package_name)) goto error;
    Py_DECREF(imported_module);
    status = __Pyx__Import_GetModule(top_level_package_name, module);
    Py_DECREF(top_level_package_name);
    return status;
error:
    Py_DECREF(imported_module);
    *module = NULL;
    return -1;
not_found:
    Py_DECREF(imported_module);
    *module = NULL;
    return 0;
}
static PyObject *__Pyx__Import(PyObject *name, PyObject *const *imported_names, Py_ssize_t len_imported_names, PyObject *qualname, PyObject *moddict, int level) {
    PyObject *module = 0;
    PyObject *empty_dict = 0;
    PyObject *from_list = 0;
    int module_found;
    if (!qualname) {
        qualname = name;
    }
    module_found = __Pyx__Import_Lookup(qualname, imported_names, len_imported_names, &module);
    if (likely(module_found == 1)) {
        return module;
    } else if (unlikely(module_found == -1)) {
        return NULL;
    }
    empty_dict = PyDict_New();
    if (unlikely(!empty_dict))
        goto bad;
    if (imported_names) {
#if CYTHON_COMPILING_IN_CPYTHON
        from_list = __Pyx_PyList_FromArray(imported_names, len_imported_names);
        if (unlikely(!from_list))
            goto bad;
#else
        from_list = PyList_New(len_imported_names);
        if (unlikely(!from_list)) goto bad;
        for (Py_ssize_t i=0; i<len_imported_names; ++i) {
            if (PyList_SetItem(from_list, i, __Pyx_NewRef(imported_names[i])) < 0) goto bad;
        }
#endif
    }
    if (level == -1) {
        const char* package_sep = strchr(__Pyx_MODULE_NAME, '.');
        if (package_sep != (0)) {
            module = PyImport_ImportModuleLevelObject(
                name, moddict, empty_dict, from_list, 1);
            if (unlikely(!module)) {
                if (unlikely(!PyErr_ExceptionMatches(PyExc_ImportError)))
                    goto bad;
                PyErr_Clear();
            }
        }
        level = 0;
    }
    if (!module) {
        module = PyImport_ImportModuleLevelObject(
            name, moddict, empty_dict, from_list, level);
    }
bad:
    Py_XDECREF(from_list);
    Py_XDECREF(empty_dict);
    return module;
}

static PyObject *__Pyx_Import(PyObject *name, PyObject *const *imported_names, Py_ssize_t len_imported_names, PyObject *qualname, int level) {
    return __Pyx__Import(name, imported_names, len_imported_names, qualname, __pyx_mstate_global->__pyx_d, level);
}

static PyObject* __Pyx_ImportFrom(PyObject* module, PyObject* name) {
    PyObject* value = __Pyx_PyObject_GetAttrStr(module, name);
    if (unlikely(!value) && PyErr_ExceptionMatches(PyExc_AttributeError)) {
        const char* module_name_str = 0;
        PyObject* module_name = 0;
        PyObject* module_dot = 0;
        PyObject* full_name = 0;
        PyErr_Clear();
        module_name_str = PyModule_GetName(module);
        if (unlikely(!module_name_str)) { goto modbad; }
        module_name = PyUnicode_FromString(module_name_str);
        if (unlikely(!module_name)) { goto modbad; }
        module_dot = PyUnicode_Concat(module_name, __pyx_mstate_global->__pyx_kp_u__5);
        if (unlikely(!module_dot)) { goto modbad; }
        full_name = PyUnicode_Concat(module_dot, name);
        if (unlikely(!full_name)) { goto modbad; }
        #if (CYTHON_COMPILING_IN_PYPY && PYPY_VERSION_NUM  < 0x07030400) ||\
                CYTHON_COMPILING_IN_GRAAL
        {
            PyObject *modules = PyImport_GetModuleDict();
            if (unlikely(!modules))
                goto modbad;
            value = PyObject_GetItem(modules, full_name);
        }
        #else
        value = PyImport_GetModule(full_name);
        #endif
      modbad:
        Py_XDECREF(full_name);
        Py_XDECREF(module_dot);
        Py_XDECREF(module_name);
    }
    if (unlikely(!value)) {
        PyErr_Format(PyExc_ImportError, "cannot import name %S", name);
    }
    return value;
}

static CYTHON_INLINE PyObject *__Pyx_PyDict_SetDefault(PyObject *d, PyObject *key, PyObject *default_value) {
    PyObject* value;
#if __PYX_LIMITED_VERSION_HEX >= 0x030F0000 || (!CYTHON_COMPILING_IN_LIMITED_API && PY_VERSION_HEX >= 0x030d00A4)
    PyDict_SetDefaultRef(d, key, default_value, &value);
#elif CYTHON_COMPILING_IN_LIMITED_API && __PYX_LIMITED_VERSION_HEX >= 0x030C0000
    PyObject *args[] = {d, key, default_value};
    value = PyObject_VectorcallMethod(__pyx_mstate_global->__pyx_n_u_setdefault, args, 3 | PY_VECTORCALL_ARGUMENTS_OFFSET, NULL);
#elif CYTHON_COMPILING_IN_LIMITED_API
    value = PyObject_CallMethodObjArgs(d, __pyx_mstate_global->__pyx_n_u_setdefault, key, default_value, NULL);
#else
    value = PyDict_SetDefault(d, key, default_value);
    if (unlikely(!value)) return NULL;
    Py_INCREF(value);
#endif
    return value;
}

#if CYTHON_COMPILING_IN_LIMITED_API
static Py_ssize_t __Pyx_GetTypeDictOffset(void) {
    PyObject *tp_dictoffset_o;
    Py_ssize_t tp_dictoffset;
    tp_dictoffset_o = PyObject_GetAttrString((PyObject*)(&PyType_Type), "__dictoffset__");
    if (unlikely(!tp_dictoffset_o)) return -1;
    tp_dictoffset = PyLong_AsSsize_t(tp_dictoffset_o);
    Py_DECREF(tp_dictoffset_o);
    if (unlikely(tp_dictoffset == 0)) {
        PyErr_SetString(
            PyExc_TypeError,
            "'type' doesn't have a dictoffset");
        return -1;
    } else if (unlikely(tp_dictoffset < 0)) {
        PyErr_SetString(
            PyExc_TypeError,
            "'type' has an unexpected negative dictoffset. "
            "Please report this as Cython bug");
        return -1;
    }
    return tp_dictoffset;
}
static PyObject *__Pyx_GetTypeDict(PyTypeObject *tp) {
    static Py_ssize_t tp_dictoffset = 0;
    if (unlikely(tp_dictoffset == 0)) {
        tp_dictoffset = __Pyx_GetTypeDictOffset();
        if (unlikely(tp_dictoffset == -1 && PyErr_Occurred())) {
            tp_dictoffset = 0;
            return NULL;
        }
    }
    return *(PyObject**)((char*)tp + tp_dictoffset);
}
#endif

static int __Pyx__SetItemOnTypeDict(PyTypeObject *tp, PyObject *k, PyObject *v) {
    int result;
    PyObject *tp_dict;
#if CYTHON_COMPILING_IN_LIMITED_API
    tp_dict = __Pyx_GetTypeDict(tp);
    if (unlikely(!tp_dict)) return -1;
#else
    tp_dict = tp->tp_dict;
#endif
    result = PyDict_SetItem(tp_dict, k, v);
    if (likely(!result)) {
        PyType_Modified(tp);
        if (unlikely(PyObject_HasAttr(v, __pyx_mstate_global->__pyx_n_u_set_name))) {
            PyObject *setNameResult = PyObject_CallMethodObjArgs(v, __pyx_mstate_global->__pyx_n_u_set_name,  (PyObject *) tp, k, NULL);
            if (!setNameResult) return -1;
            Py_DECREF(setNameResult);
        }
    }
    return result;
}

static int __Pyx_fix_up_extension_type_from_spec(PyType_Spec *spec, PyTypeObject *type) {
#if __PYX_LIMITED_VERSION_HEX > 0x030900B1
    CYTHON_UNUSED_VAR(spec);
    CYTHON_UNUSED_VAR(type);
    CYTHON_UNUSED_VAR(__Pyx__SetItemOnTypeDict);
#else
    const PyType_Slot *slot = spec->slots;
    int changed = 0;
#if !CYTHON_COMPILING_IN_LIMITED_API
    while (slot && slot->slot && slot->slot != Py_tp_members)
        slot++;
    if (slot && slot->slot == Py_tp_members) {
#if !CYTHON_COMPILING_IN_CPYTHON
        const
#endif
            PyMemberDef *memb = (PyMemberDef*) slot->pfunc;
        while (memb && memb->name) {
            if (memb->name[0] == '_' && memb->name[1] == '_') {
                if (strcmp(memb->name, "__weaklistoffset__") == 0) {
                    assert(memb->type == T_PYSSIZET);
                    assert(memb->flags == READONLY);
                    type->tp_weaklistoffset = memb->offset;
                    changed = 1;
                }
                else if (strcmp(memb->name, "__dictoffset__") == 0) {
                    assert(memb->type == T_PYSSIZET);
                    assert(memb->flags == READONLY);
                    type->tp_dictoffset = memb->offset;
                    changed = 1;
                }
#if CYTHON_METH_FASTCALL
                else if (strcmp(memb->name, "__vectorcalloffset__") == 0) {
                    assert(memb->type == T_PYSSIZET);
                    assert(memb->flags == READONLY);
                    type->tp_vectorcall_offset = memb->offset;
                    changed = 1;
                }
#endif
#if !CYTHON_COMPILING_IN_PYPY
                else if (strcmp(memb->name, "__module__") == 0) {
                    PyObject *descr;
                    assert(memb->type == T_OBJECT);
                    assert(memb->flags == 0 || memb->flags == READONLY);
                    descr = PyDescr_NewMember(type, memb);
                    if (unlikely(!descr))
                        return -1;
                    int set_item_result = PyDict_SetItem(type->tp_dict, PyDescr_NAME(descr), descr);
                    Py_DECREF(descr);
                    if (unlikely(set_item_result < 0)) {
                        return -1;
                    }
                    changed = 1;
                }
#endif
            }
            memb++;
        }
    }
#endif
#if !CYTHON_COMPILING_IN_PYPY
    slot = spec->slots;
    while (slot && slot->slot && slot->slot != Py_tp_getset)
        slot++;
    if (slot && slot->slot == Py_tp_getset) {
        PyGetSetDef *getset = (PyGetSetDef*) slot->pfunc;
        while (getset && getset->name) {
            if (getset->name[0] == '_' && getset->name[1] == '_' && strcmp(getset->name, "__module__") == 0) {
                PyObject *descr = PyDescr_NewGetSet(type, getset);
                if (unlikely(!descr))
                    return -1;
                #if CYTHON_COMPILING_IN_LIMITED_API
                PyObject *pyname = PyUnicode_FromString(getset->name);
                if (unlikely(!pyname)) {
                    Py_DECREF(descr);
                    return -1;
                }
                int set_item_result = __Pyx_SetItemOnTypeDict(type, pyname, descr);
                Py_DECREF(pyname);
                #else
                CYTHON_UNUSED_VAR(__Pyx__SetItemOnTypeDict);
                int set_item_result = PyDict_SetItem(type->tp_dict, PyDescr_NAME(descr), descr);
                #endif
                Py_DECREF(descr);
                if (unlikely(set_item_result < 0)) {
                    return -1;
                }
                changed = 1;
            }
            ++getset;
        }
    }
#else
    CYTHON_UNUSED_VAR(__Pyx__SetItemOnTypeDict);
#endif
    if (changed)
        PyType_Modified(type);
#endif
    return 0;
}

#if CYTHON_COMPILING_IN_CPYTHON_FREETHREADING
  static PyObject *__Pyx_PyImport_AddModuleObjectRef(PyObject *name) {
      PyObject *module_dict = PyImport_GetModuleDict();
      PyObject *m;
      if (PyMapping_GetOptionalItem(module_dict, name, &m) < 0) {
          return NULL;
      }
      if (m != NULL && PyModule_Check(m)) {
          return m;
      }
      Py_XDECREF(m);
      m = PyModule_NewObject(name);
      if (m == NULL)
          return NULL;
      if (PyDict_CheckExact(module_dict)) {
          PyObject *new_m;
          (void)PyDict_SetDefaultRef(module_dict, name, m, &new_m);
          Py_DECREF(m);
          return new_m;
      } else {
           if (PyObject_SetItem(module_dict, name, m) != 0) {
                Py_DECREF(m);
                return NULL;
            }
            return m;
      }
  }
  static PyObject *__Pyx_PyImport_AddModuleRef(const char *name) {
      PyObject *py_name = PyUnicode_FromString(name);
      if (!py_name) return NULL;
      PyObject *module = __Pyx_PyImport_AddModuleObjectRef(py_name);
      Py_DECREF(py_name);
      return module;
  }
#elif __PYX_LIMITED_VERSION_HEX >= 0x030d0000
  #define __Pyx_PyImport_AddModuleRef(name) PyImport_AddModuleRef(name)
#else
  static PyObject *__Pyx_PyImport_AddModuleRef(const char *name) {
      PyObject *module = PyImport_AddModule(name);
      Py_XINCREF(module);
      return module;
  }
#endif

static PyObject *__Pyx_FetchSharedCythonABIModule(void) {
    return __Pyx_PyImport_AddModuleRef(__PYX_ABI_MODULE_NAME);
}

#if __PYX_LIMITED_VERSION_HEX < 0x030C0000
static PyObject* __Pyx_PyType_FromMetaclass(PyTypeObject *metaclass, PyObject *module, PyType_Spec *spec, PyObject *bases) {
    PyObject *result = __Pyx_PyType_FromModuleAndSpec(module, spec, bases);
    if (result && metaclass) {
        PyObject *old_tp = (PyObject*)Py_TYPE(result);
    Py_INCREF((PyObject*)metaclass);
#if __PYX_LIMITED_VERSION_HEX >= 0x03090000
        Py_SET_TYPE(result, metaclass);
#else
        result->ob_type = metaclass;
#endif
        Py_DECREF(old_tp);
    }
    return result;
}
#else
#define __Pyx_PyType_FromMetaclass(me, mo, s, b) PyType_FromMetaclass(me, mo, s, b)
#endif
static int __Pyx_VerifyCachedType(PyObject *cached_type,
                               const char *name,
                               Py_ssize_t expected_basicsize) {
    Py_ssize_t basicsize;
    if (!PyType_Check(cached_type)) {
        PyErr_Format(PyExc_TypeError,
            "Shared Cython type %.200s is not a type object", name);
        return -1;
    }
    if (expected_basicsize == 0) {
        return 0;
    }
#if CYTHON_COMPILING_IN_LIMITED_API
    PyObject *py_basicsize;
    py_basicsize = PyObject_GetAttrString(cached_type, "__basicsize__");
    if (unlikely(!py_basicsize)) return -1;
    basicsize = PyLong_AsSsize_t(py_basicsize);
    Py_DECREF(py_basicsize);
    py_basicsize = NULL;
    if (unlikely(basicsize == (Py_ssize_t)-1) && PyErr_Occurred()) return -1;
#else
    basicsize = ((PyTypeObject*) cached_type)->tp_basicsize;
#endif
    if (basicsize != expected_basicsize) {
        PyErr_Format(PyExc_TypeError,
            "Shared Cython type %.200s has the wrong size, try recompiling",
            name);
        return -1;
    }
    return 0;
}
static PyTypeObject *__Pyx_FetchCommonTypeFromSpec(PyTypeObject *metaclass, PyObject *module, PyType_Spec *spec, PyObject *bases) {
    PyObject *abi_module = NULL, *cached_type = NULL, *abi_module_dict, *new_cached_type, *py_object_name;
    int get_item_ref_result;
    const char* object_name = strrchr(spec->name, '.');
    object_name = object_name ? object_name+1 : spec->name;
    py_object_name = PyUnicode_FromString(object_name);
    if (!py_object_name) return NULL;
    abi_module = __Pyx_FetchSharedCythonABIModule();
    if (!abi_module) goto done;
    abi_module_dict = PyModule_GetDict(abi_module);
    if (!abi_module_dict) goto done;
    get_item_ref_result = __Pyx_PyDict_GetItemRef(abi_module_dict, py_object_name, &cached_type);
    if (get_item_ref_result == 1) {
        if (__Pyx_VerifyCachedType(
              cached_type,
              object_name,
              spec->basicsize) < 0) {
            goto bad;
        }
        goto done;
    } else if (unlikely(get_item_ref_result == -1)) {
        goto bad;
    }
    cached_type = __Pyx_PyType_FromMetaclass(
        metaclass,
        CYTHON_USE_MODULE_STATE ? module : abi_module,
        spec, bases);
    if (unlikely(!cached_type)) goto bad;
    if (unlikely(__Pyx_fix_up_extension_type_from_spec(spec, (PyTypeObject *) cached_type) < 0)) goto bad;
    new_cached_type = __Pyx_PyDict_SetDefault(abi_module_dict, py_object_name, cached_type);
    if (unlikely(new_cached_type != cached_type)) {
        if (unlikely(!new_cached_type)) goto bad;
        Py_DECREF(cached_type);
        cached_type = new_cached_type;
        if (__Pyx_VerifyCachedType(
                cached_type,
                object_name,
                spec->basicsize) < 0) {
            goto bad;
        }
        goto done;
    } else {
        Py_DECREF(new_cached_type);
    }
done:
    Py_XDECREF(abi_module);
    Py_DECREF(py_object_name);
    assert(cached_type == NULL || PyType_Check(cached_type));
    return (PyTypeObject *) cached_type;
bad:
    Py_XDECREF(cached_type);
    cached_type = NULL;
    goto done;
}

static PyObject* __pyx_CommonTypesMetaclass_get_module(CYTHON_UNUSED PyObject *self, CYTHON_UNUSED void* context) {
    return PyUnicode_FromString(__PYX_ABI_MODULE_NAME);
}
#if __PYX_LIMITED_VERSION_HEX < 0x030A0000
static PyObject* __pyx_CommonTypesMetaclass_call(CYTHON_UNUSED PyObject *self, CYTHON_UNUSED PyObject *args, CYTHON_UNUSED PyObject *kwds) {
    PyErr_SetString(PyExc_TypeError, "Cannot instantiate Cython internal types");
    return NULL;
}
static int __pyx_CommonTypesMetaclass_setattr(CYTHON_UNUSED PyObject *self, CYTHON_UNUSED PyObject *attr, CYTHON_UNUSED PyObject *value) {
    PyErr_SetString(PyExc_TypeError, "Cython internal types are immutable");
    return -1;
}
#endif
static PyGetSetDef __pyx_CommonTypesMetaclass_getset[] = {
    {"__module__", __pyx_CommonTypesMetaclass_get_module, NULL, NULL, NULL},
    {0, 0, 0, 0, 0}
};
static PyType_Slot __pyx_CommonTypesMetaclass_slots[] = {
    {Py_tp_getset, (void *)__pyx_CommonTypesMetaclass_getset},
    #if __PYX_LIMITED_VERSION_HEX < 0x030A0000
    {Py_tp_call, (void*)__pyx_CommonTypesMetaclass_call},
    {Py_tp_new, (void*)__pyx_CommonTypesMetaclass_call},
    {Py_tp_setattro, (void*)__pyx_CommonTypesMetaclass_setattr},
    #endif
    {0, 0}
};
static PyType_Spec __pyx_CommonTypesMetaclass_spec = {
    __PYX_TYPE_MODULE_PREFIX "_common_types_metatype",
    0,
    0,
    Py_TPFLAGS_IMMUTABLETYPE |
    Py_TPFLAGS_DISALLOW_INSTANTIATION |
    Py_TPFLAGS_DEFAULT,
    __pyx_CommonTypesMetaclass_slots
};
static int __pyx_CommonTypesMetaclass_init(PyObject *module) {
    __pyx_mstatetype *mstate = __Pyx_PyModule_GetState(module);
    PyObject *bases = PyTuple_Pack(1, &PyType_Type);
    if (unlikely(!bases)) {
        return -1;
    }
    mstate->__pyx_CommonTypesMetaclassType = __Pyx_FetchCommonTypeFromSpec(NULL, module, &__pyx_CommonTypesMetaclass_spec, bases);
    Py_DECREF(bases);
    if (unlikely(mstate->__pyx_CommonTypesMetaclassType == NULL)) {
        return -1;
    }
    return 0;
}

#if !CYTHON_USE_TYPE_SPECS || (!CYTHON_COMPILING_IN_LIMITED_API && PY_VERSION_HEX < 0x03090000)
#else
static int __Pyx_call_type_traverse(PyObject *o, int always_call, visitproc visit, void *arg) {
    #if CYTHON_COMPILING_IN_LIMITED_API && __PYX_LIMITED_VERSION_HEX < 0x03090000
    if (__Pyx_get_runtime_version() < 0x03090000) return 0;
    #endif
    if (!always_call) {
        PyTypeObject *base = __Pyx_PyObject_GetSlot(o, tp_base, PyTypeObject*);
        unsigned long flags = PyType_GetFlags(base);
        if (flags & Py_TPFLAGS_HEAPTYPE) {
            return 0;
        }
    }
    Py_VISIT((PyObject*)Py_TYPE(o));
    return 0;
}
#endif

#if CYTHON_COMPILING_IN_LIMITED_API
static PyObject *__Pyx_PyMethod_New(PyObject *func, PyObject *self, PyObject *typ) {
    PyObject *result;
    CYTHON_UNUSED_VAR(typ);
    if (!self)
        return __Pyx_NewRef(func);
    #if __PYX_LIMITED_VERSION_HEX >= 0x030C0000
    {
        PyObject *args[] = {func, self};
        result = PyObject_Vectorcall(__pyx_mstate_global->__Pyx_CachedMethodType, args, 2, NULL);
    }
    #else
    result = PyObject_CallFunctionObjArgs(__pyx_mstate_global->__Pyx_CachedMethodType, func, self, NULL);
    #endif
    return result;
}
#else
static PyObject *__Pyx_PyMethod_New(PyObject *func, PyObject *self, PyObject *typ) {
    CYTHON_UNUSED_VAR(typ);
    if (!self)
        return __Pyx_NewRef(func);
    return PyMethod_New(func, self);
}
#endif

#if CYTHON_METH_FASTCALL && CYTHON_VECTORCALL
static PyObject *__Pyx_PyVectorcall_FastCallDict_kw(PyObject *func, __pyx_vectorcallfunc vc, PyObject *const *args, size_t nargs, PyObject *kw)
{
    PyObject *res = NULL;
    PyObject *kwnames;
    PyObject **newargs;
    PyObject **kwvalues;
    Py_ssize_t i;
    #if CYTHON_AVOID_BORROWED_REFS
    PyObject *pos;
    #else
    Py_ssize_t pos;
    #endif
    size_t j;
    PyObject *key, *value;
    unsigned long keys_are_strings;
    #if !CYTHON_ASSUME_SAFE_SIZE
    Py_ssize_t nkw = PyDict_Size(kw);
    if (unlikely(nkw == -1)) return NULL;
    #else
    Py_ssize_t nkw = PyDict_GET_SIZE(kw);
    #endif
    newargs = (PyObject **)PyMem_Malloc((nargs + (size_t)nkw) * sizeof(args[0]));
    if (unlikely(newargs == NULL)) {
        PyErr_NoMemory();
        return NULL;
    }
    for (j = 0; j < nargs; j++) newargs[j] = args[j];
    kwnames = PyTuple_New(nkw);
    if (unlikely(kwnames == NULL)) {
        PyMem_Free(newargs);
        return NULL;
    }
    kwvalues = newargs + nargs;
    pos = 0;
    i = 0;
    keys_are_strings = Py_TPFLAGS_UNICODE_SUBCLASS;
    while (__Pyx_PyDict_NextRef(kw, &pos, &key, &value)) {
        keys_are_strings &=
        #if CYTHON_COMPILING_IN_LIMITED_API
            PyType_GetFlags(Py_TYPE(key));
        #else
            Py_TYPE(key)->tp_flags;
        #endif
        #if !CYTHON_ASSUME_SAFE_MACROS
        if (unlikely(PyTuple_SetItem(kwnames, i, key) < 0)) goto cleanup;
        #else
        PyTuple_SET_ITEM(kwnames, i, key);
        #endif
        kwvalues[i] = value;
        i++;
    }
    if (unlikely(!keys_are_strings)) {
        PyErr_SetString(PyExc_TypeError, "keywords must be strings");
        goto cleanup;
    }
    res = vc(func, newargs, nargs, kwnames);
cleanup:
    #if CYTHON_AVOID_BORROWED_REFS
    Py_DECREF(pos);
    #endif
    Py_DECREF(kwnames);
    for (i = 0; i < nkw; i++)
        Py_DECREF(kwvalues[i]);
    PyMem_Free(newargs);
    return res;
}
static CYTHON_INLINE PyObject *__Pyx_PyVectorcall_FastCallDict(PyObject *func, __pyx_vectorcallfunc vc, PyObject *const *args, size_t nargs, PyObject *kw)
{
    Py_ssize_t kw_size =
        likely(kw == NULL) ?
        0 :
#if !CYTHON_ASSUME_SAFE_SIZE
        PyDict_Size(kw);
#else
        PyDict_GET_SIZE(kw);
#endif
    if (kw_size == 0) {
        return vc(func, args, nargs, NULL);
    }
#if !CYTHON_ASSUME_SAFE_SIZE
    else if (unlikely(kw_size == -1)) {
        return NULL;
    }
#endif
    return __Pyx_PyVectorcall_FastCallDict_kw(func, vc, args, nargs, kw);
}
#endif

#if CYTHON_COMPILING_IN_LIMITED_API
static CYTHON_INLINE int __Pyx__IsSameCyOrCFunctionNoMethod(PyObject *func, void (*cfunc)(void)) {
    if (__Pyx_CyFunction_Check(func)) {
        return PyCFunction_GetFunction(((__pyx_CyFunctionObject*)func)->func) == (PyCFunction) cfunc;
    } else if (PyCFunction_Check(func)) {
        return PyCFunction_GetFunction(func) == (PyCFunction) cfunc;
    }
    return 0;
}
static CYTHON_INLINE int __Pyx__IsSameCyOrCFunction(PyObject *func, void (*cfunc)(void)) {
    if ((PyObject*)Py_TYPE(func) == __pyx_mstate_global->__Pyx_CachedMethodType) {
        int result;
        PyObject *newFunc = PyObject_GetAttr(func, __pyx_mstate_global->__pyx_n_u_func);
        if (unlikely(!newFunc)) {
            PyErr_Clear();
            return 0;
        }
        result = __Pyx__IsSameCyOrCFunctionNoMethod(newFunc, cfunc);
        Py_DECREF(newFunc);
        return result;
    }
    return __Pyx__IsSameCyOrCFunctionNoMethod(func, cfunc);
}
#else
static CYTHON_INLINE int __Pyx__IsSameCyOrCFunction(PyObject *func, void (*cfunc)(void)) {
    if (PyMethod_Check(func)) {
        func = PyMethod_GET_FUNCTION(func);
    }
    return __Pyx_CyOrPyCFunction_Check(func) && __Pyx_CyOrPyCFunction_GET_FUNCTION(func) == (PyCFunction) cfunc;
}
#endif
static CYTHON_INLINE void __Pyx__CyFunction_SetClassObj(__pyx_CyFunctionObject* f, PyObject* classobj) {
#if PY_VERSION_HEX < 0x030900B1 || CYTHON_COMPILING_IN_LIMITED_API
    __Pyx_Py_XDECREF_SET(
        __Pyx_CyFunction_GetClassObj(f),
            ((classobj) ? __Pyx_NewRef(classobj) : NULL));
#else
    __Pyx_Py_XDECREF_SET(
        ((PyCMethodObject *) (f))->mm_class,
        (PyTypeObject*)((classobj) ? __Pyx_NewRef(classobj) : NULL));
#endif
}
static PyObject *
__Pyx_CyFunction_get_doc_locked(__pyx_CyFunctionObject *op)
{
    if (unlikely(op->func_doc == NULL)) {
#if CYTHON_COMPILING_IN_LIMITED_API
        op->func_doc = PyObject_GetAttrString(op->func, "__doc__");
        if (unlikely(!op->func_doc)) return NULL;
#else
        if (((PyCFunctionObject*)op)->m_ml->ml_doc) {
            op->func_doc = PyUnicode_FromString(((PyCFunctionObject*)op)->m_ml->ml_doc);
            if (unlikely(op->func_doc == NULL))
                return NULL;
        } else {
            Py_INCREF(Py_None);
            return Py_None;
        }
#endif
    }
    Py_INCREF(op->func_doc);
    return op->func_doc;
}
static PyObject *
__Pyx_CyFunction_get_doc(__pyx_CyFunctionObject *op, void *closure) {
    PyObject *result;
    CYTHON_UNUSED_VAR(closure);
    __Pyx_BEGIN_CRITICAL_SECTION(op);
    result = __Pyx_CyFunction_get_doc_locked(op);
    __Pyx_END_CRITICAL_SECTION();
    return result;
}
static int
__Pyx_CyFunction_set_doc(__pyx_CyFunctionObject *op, PyObject *value, void *context)
{
    CYTHON_UNUSED_VAR(context);
    if (value == NULL) {
        value = Py_None;
    }
    Py_INCREF(value);
    __Pyx_BEGIN_CRITICAL_SECTION(op);
    __Pyx_Py_XDECREF_SET(op->func_doc, value);
    __Pyx_END_CRITICAL_SECTION();
    return 0;
}
static PyObject *
__Pyx_CyFunction_get_name_locked(__pyx_CyFunctionObject *op)
{
    if (unlikely(op->func_name == NULL)) {
#if CYTHON_COMPILING_IN_LIMITED_API
        op->func_name = PyObject_GetAttrString(op->func, "__name__");
#else
        op->func_name = PyUnicode_InternFromString(((PyCFunctionObject*)op)->m_ml->ml_name);
#endif
        if (unlikely(op->func_name == NULL))
            return NULL;
    }
    Py_INCREF(op->func_name);
    return op->func_name;
}
static PyObject *
__Pyx_CyFunction_get_name(__pyx_CyFunctionObject *op, void *context)
{
    PyObject *result = NULL;
    CYTHON_UNUSED_VAR(context);
    __Pyx_BEGIN_CRITICAL_SECTION(op);
    result = __Pyx_CyFunction_get_name_locked(op);
    __Pyx_END_CRITICAL_SECTION();
    return result;
}
static int
__Pyx_CyFunction_set_name(__pyx_CyFunctionObject *op, PyObject *value, void *context)
{
    CYTHON_UNUSED_VAR(context);
    if (unlikely(value == NULL || !PyUnicode_Check(value))) {
        PyErr_SetString(PyExc_TypeError,
                        "__name__ must be set to a string object");
        return -1;
    }
    Py_INCREF(value);
    __Pyx_BEGIN_CRITICAL_SECTION(op);
    __Pyx_Py_XDECREF_SET(op->func_name, value);
    __Pyx_END_CRITICAL_SECTION();
    return 0;
}
static PyObject *
__Pyx_CyFunction_get_qualname(__pyx_CyFunctionObject *op, void *context)
{
    CYTHON_UNUSED_VAR(context);
    PyObject *result;
    __Pyx_BEGIN_CRITICAL_SECTION(op);
    Py_INCREF(op->func_qualname);
    result = op->func_qualname;
    __Pyx_END_CRITICAL_SECTION();
    return result;
}
static int
__Pyx_CyFunction_set_qualname(__pyx_CyFunctionObject *op, PyObject *value, void *context)
{
    CYTHON_UNUSED_VAR(context);
    if (unlikely(value == NULL || !PyUnicode_Check(value))) {
        PyErr_SetString(PyExc_TypeError,
                        "__qualname__ must be set to a string object");
        return -1;
    }
    Py_INCREF(value);
    __Pyx_BEGIN_CRITICAL_SECTION(op);
    __Pyx_Py_XDECREF_SET(op->func_qualname, value);
    __Pyx_END_CRITICAL_SECTION();
    return 0;
}
#if CYTHON_COMPILING_IN_LIMITED_API && __PYX_LIMITED_VERSION_HEX < 0x030A0000
static PyObject *
__Pyx_CyFunction_get_dict(__pyx_CyFunctionObject *op, void *context)
{
    CYTHON_UNUSED_VAR(context);
    if (unlikely(op->func_dict == NULL)) {
        op->func_dict = PyDict_New();
        if (unlikely(op->func_dict == NULL))
            return NULL;
    }
    Py_INCREF(op->func_dict);
    return op->func_dict;
}
#endif
static PyObject *
__Pyx_CyFunction_get_globals(__pyx_CyFunctionObject *op, void *context)
{
    CYTHON_UNUSED_VAR(context);
    Py_INCREF(op->func_globals);
    return op->func_globals;
}
static PyObject *
__Pyx_CyFunction_get_closure(__pyx_CyFunctionObject *op, void *context)
{
    CYTHON_UNUSED_VAR(op);
    CYTHON_UNUSED_VAR(context);
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *
__Pyx_CyFunction_get_code(__pyx_CyFunctionObject *op, void *context)
{
    PyObject* result = (op->func_code) ? op->func_code : Py_None;
    CYTHON_UNUSED_VAR(context);
    Py_INCREF(result);
    return result;
}
static int
__Pyx_CyFunction_init_defaults(__pyx_CyFunctionObject *op) {
    int result = 0;
    PyObject *res = op->defaults_getter((PyObject *) op);
    if (unlikely(!res))
        return -1;
    #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
    op->defaults_tuple = PyTuple_GET_ITEM(res, 0);
    Py_INCREF(op->defaults_tuple);
    op->defaults_kwdict = PyTuple_GET_ITEM(res, 1);
    Py_INCREF(op->defaults_kwdict);
    #else
    op->defaults_tuple = __Pyx_PySequence_ITEM(res, 0);
    if (unlikely(!op->defaults_tuple)) result = -1;
    else {
        op->defaults_kwdict = __Pyx_PySequence_ITEM(res, 1);
        if (unlikely(!op->defaults_kwdict)) result = -1;
    }
    #endif
    Py_DECREF(res);
    return result;
}
static int
__Pyx_CyFunction_set_defaults(__pyx_CyFunctionObject *op, PyObject* value, void *context) {
    CYTHON_UNUSED_VAR(context);
    if (!value) {
        value = Py_None;
    } else if (unlikely(value != Py_None && !PyTuple_Check(value))) {
        PyErr_SetString(PyExc_TypeError,
                        "__defaults__ must be set to a tuple object");
        return -1;
    }
    PyErr_WarnEx(PyExc_RuntimeWarning, "changes to cyfunction.__defaults__ will not "
                 "currently affect the values used in function calls", 1);
    Py_INCREF(value);
    __Pyx_BEGIN_CRITICAL_SECTION(op);
    __Pyx_Py_XDECREF_SET(op->defaults_tuple, value);
    __Pyx_END_CRITICAL_SECTION();
    return 0;
}
static PyObject *
__Pyx_CyFunction_get_defaults_locked(__pyx_CyFunctionObject *op) {
    PyObject* result = op->defaults_tuple;
    if (unlikely(!result)) {
        if (op->defaults_getter) {
            if (unlikely(__Pyx_CyFunction_init_defaults(op) < 0)) return NULL;
            result = op->defaults_tuple;
        } else {
            result = Py_None;
        }
    }
    Py_INCREF(result);
    return result;
}
static PyObject *
__Pyx_CyFunction_get_defaults(__pyx_CyFunctionObject *op, void *context) {
    PyObject* result = NULL;
    CYTHON_UNUSED_VAR(context);
    __Pyx_BEGIN_CRITICAL_SECTION(op);
    result = __Pyx_CyFunction_get_defaults_locked(op);
    __Pyx_END_CRITICAL_SECTION();
    return result;
}
static int
__Pyx_CyFunction_set_kwdefaults(__pyx_CyFunctionObject *op, PyObject* value, void *context) {
    CYTHON_UNUSED_VAR(context);
    if (!value) {
        value = Py_None;
    } else if (unlikely(value != Py_None && !PyDict_Check(value))) {
        PyErr_SetString(PyExc_TypeError,
                        "__kwdefaults__ must be set to a dict object");
        return -1;
    }
    PyErr_WarnEx(PyExc_RuntimeWarning, "changes to cyfunction.__kwdefaults__ will not "
                 "currently affect the values used in function calls", 1);
    Py_INCREF(value);
    __Pyx_BEGIN_CRITICAL_SECTION(op);
    __Pyx_Py_XDECREF_SET(op->defaults_kwdict, value);
    __Pyx_END_CRITICAL_SECTION();
    return 0;
}
static PyObject *
__Pyx_CyFunction_get_kwdefaults_locked(__pyx_CyFunctionObject *op) {
    PyObject* result = op->defaults_kwdict;
    if (unlikely(!result)) {
        if (op->defaults_getter) {
            if (unlikely(__Pyx_CyFunction_init_defaults(op) < 0)) return NULL;
            result = op->defaults_kwdict;
        } else {
            result = Py_None;
        }
    }
    Py_INCREF(result);
    return result;
}
static PyObject *
__Pyx_CyFunction_get_kwdefaults(__pyx_CyFunctionObject *op, void *context) {
    PyObject* result;
    CYTHON_UNUSED_VAR(context);
    __Pyx_BEGIN_CRITICAL_SECTION(op);
    result = __Pyx_CyFunction_get_kwdefaults_locked(op);
    __Pyx_END_CRITICAL_SECTION();
    return result;
}
static int
__Pyx_CyFunction_set_annotations(__pyx_CyFunctionObject *op, PyObject* value, void *context) {
    CYTHON_UNUSED_VAR(context);
    if (!value || value == Py_None) {
        value = NULL;
    } else if (unlikely(!PyDict_Check(value))) {
        PyErr_SetString(PyExc_TypeError,
                        "__annotations__ must be set to a dict object");
        return -1;
    }
    Py_XINCREF(value);
    __Pyx_BEGIN_CRITICAL_SECTION(op);
    __Pyx_Py_XDECREF_SET(op->func_annotations, value);
    __Pyx_END_CRITICAL_SECTION();
    return 0;
}
static PyObject *
__Pyx_CyFunction_get_annotations_locked(__pyx_CyFunctionObject *op) {
    PyObject* result = op->func_annotations;
    if (unlikely(!result)) {
        result = PyDict_New();
        if (unlikely(!result)) return NULL;
        op->func_annotations = result;
    }
    Py_INCREF(result);
    return result;
}
static PyObject *
__Pyx_CyFunction_get_annotations(__pyx_CyFunctionObject *op, void *context) {
    PyObject *result;
    CYTHON_UNUSED_VAR(context);
    __Pyx_BEGIN_CRITICAL_SECTION(op);
    result = __Pyx_CyFunction_get_annotations_locked(op);
    __Pyx_END_CRITICAL_SECTION();
    return result;
}
static PyObject *
__Pyx_CyFunction_get_is_coroutine_value(__pyx_CyFunctionObject *op) {
    int is_coroutine = op->flags & __Pyx_CYFUNCTION_COROUTINE;
    if (is_coroutine) {
        PyObject *is_coroutine_value, *module, *fromlist, *marker = __pyx_mstate_global->__pyx_n_u_is_coroutine;
        fromlist = PyList_New(1);
        if (unlikely(!fromlist)) return NULL;
        Py_INCREF(marker);
#if CYTHON_ASSUME_SAFE_MACROS
        PyList_SET_ITEM(fromlist, 0, marker);
#else
        if (unlikely(PyList_SetItem(fromlist, 0, marker) < 0)) {
            Py_DECREF(marker);
            Py_DECREF(fromlist);
            return NULL;
        }
#endif
        module = PyImport_ImportModuleLevelObject(__pyx_mstate_global->__pyx_n_u_asyncio_coroutines, NULL, NULL, fromlist, 0);
        Py_DECREF(fromlist);
        if (unlikely(!module)) goto ignore;
        is_coroutine_value = __Pyx_PyObject_GetAttrStr(module, marker);
        Py_DECREF(module);
        if (likely(is_coroutine_value)) {
            return is_coroutine_value;
        }
ignore:
        PyErr_Clear();
    }
    return __Pyx_PyBool_FromLong(is_coroutine);
}
static PyObject *
__Pyx_CyFunction_get_is_coroutine(__pyx_CyFunctionObject *op, void *context) {
    PyObject *result;
    CYTHON_UNUSED_VAR(context);
    if (op->func_is_coroutine) {
        return __Pyx_NewRef(op->func_is_coroutine);
    }
    result = __Pyx_CyFunction_get_is_coroutine_value(op);
    if (unlikely(!result))
        return NULL;
    __Pyx_BEGIN_CRITICAL_SECTION(op);
    if (op->func_is_coroutine) {
        Py_DECREF(result);
        result = __Pyx_NewRef(op->func_is_coroutine);
    } else {
        op->func_is_coroutine = __Pyx_NewRef(result);
    }
    __Pyx_END_CRITICAL_SECTION();
    return result;
}
static void __Pyx_CyFunction_raise_argument_count_error(__pyx_CyFunctionObject *func, const char* message, Py_ssize_t size) {
#if CYTHON_COMPILING_IN_LIMITED_API
    PyObject *py_name = __Pyx_CyFunction_get_name(func, NULL);
    if (!py_name) return;
    PyErr_Format(PyExc_TypeError,
        "%.200S() %s (%" CYTHON_FORMAT_SSIZE_T "d given)",
        py_name, message, size);
    Py_DECREF(py_name);
#else
    const char* name = ((PyCFunctionObject*)func)->m_ml->ml_name;
    PyErr_Format(PyExc_TypeError,
        "%.200s() %s (%" CYTHON_FORMAT_SSIZE_T "d given)",
        name, message, size);
#endif
}
static void __Pyx_CyFunction_raise_type_error(__pyx_CyFunctionObject *func, const char* message) {
#if CYTHON_COMPILING_IN_LIMITED_API
    PyObject *py_name = __Pyx_CyFunction_get_name(func, NULL);
    if (!py_name) return;
    PyErr_Format(PyExc_TypeError,
        "%.200S() %s",
        py_name, message);
    Py_DECREF(py_name);
#else
    const char* name = ((PyCFunctionObject*)func)->m_ml->ml_name;
    PyErr_Format(PyExc_TypeError,
        "%.200s() %s",
        name, message);
#endif
}
#if CYTHON_COMPILING_IN_LIMITED_API
static PyObject *
__Pyx_CyFunction_get_module(__pyx_CyFunctionObject *op, void *context) {
    CYTHON_UNUSED_VAR(context);
    return PyObject_GetAttrString(op->func, "__module__");
}
static int
__Pyx_CyFunction_set_module(__pyx_CyFunctionObject *op, PyObject* value, void *context) {
    CYTHON_UNUSED_VAR(context);
    return PyObject_SetAttrString(op->func, "__module__", value);
}
#endif
static PyGetSetDef __pyx_CyFunction_getsets[] = {
    {"func_doc", (getter)__Pyx_CyFunction_get_doc, (setter)__Pyx_CyFunction_set_doc, 0, 0},
    {"__doc__",  (getter)__Pyx_CyFunction_get_doc, (setter)__Pyx_CyFunction_set_doc, 0, 0},
    {"func_name", (getter)__Pyx_CyFunction_get_name, (setter)__Pyx_CyFunction_set_name, 0, 0},
    {"__name__", (getter)__Pyx_CyFunction_get_name, (setter)__Pyx_CyFunction_set_name, 0, 0},
    {"__qualname__", (getter)__Pyx_CyFunction_get_qualname, (setter)__Pyx_CyFunction_set_qualname, 0, 0},
#if CYTHON_COMPILING_IN_LIMITED_API && __PYX_LIMITED_VERSION_HEX < 0x030A0000
    {"func_dict", (getter)__Pyx_CyFunction_get_dict, (setter)PyObject_GenericSetDict, 0, 0},
    {"__dict__", (getter)__Pyx_CyFunction_get_dict, (setter)PyObject_GenericSetDict, 0, 0},
#else
    {"func_dict", (getter)PyObject_GenericGetDict, (setter)PyObject_GenericSetDict, 0, 0},
    {"__dict__", (getter)PyObject_GenericGetDict, (setter)PyObject_GenericSetDict, 0, 0},
#endif
    {"func_globals", (getter)__Pyx_CyFunction_get_globals, 0, 0, 0},
    {"__globals__", (getter)__Pyx_CyFunction_get_globals, 0, 0, 0},
    {"func_closure", (getter)__Pyx_CyFunction_get_closure, 0, 0, 0},
    {"__closure__", (getter)__Pyx_CyFunction_get_closure, 0, 0, 0},
    {"func_code", (getter)__Pyx_CyFunction_get_code, 0, 0, 0},
    {"__code__", (getter)__Pyx_CyFunction_get_code, 0, 0, 0},
    {"func_defaults", (getter)__Pyx_CyFunction_get_defaults, (setter)__Pyx_CyFunction_set_defaults, 0, 0},
    {"__defaults__", (getter)__Pyx_CyFunction_get_defaults, (setter)__Pyx_CyFunction_set_defaults, 0, 0},
    {"__kwdefaults__", (getter)__Pyx_CyFunction_get_kwdefaults, (setter)__Pyx_CyFunction_set_kwdefaults, 0, 0},
    {"__annotations__", (getter)__Pyx_CyFunction_get_annotations, (setter)__Pyx_CyFunction_set_annotations, 0, 0},
    {"_is_coroutine", (getter)__Pyx_CyFunction_get_is_coroutine, 0, 0, 0},
#if CYTHON_COMPILING_IN_LIMITED_API
    {"__module__", (getter)__Pyx_CyFunction_get_module, (setter)__Pyx_CyFunction_set_module, 0, 0},
#endif
    {0, 0, 0, 0, 0}
};
static PyMemberDef __pyx_CyFunction_members[] = {
#if !CYTHON_COMPILING_IN_LIMITED_API
    {"__module__", T_OBJECT, offsetof(PyCFunctionObject, m_module), 0, 0},
#endif
#if PY_VERSION_HEX < 0x030C0000 || CYTHON_COMPILING_IN_LIMITED_API
    {"__dictoffset__", T_PYSSIZET, offsetof(__pyx_CyFunctionObject, func_dict), READONLY, 0},
#endif
#if CYTHON_METH_FASTCALL
#if CYTHON_COMPILING_IN_LIMITED_API
    {"__vectorcalloffset__", T_PYSSIZET, offsetof(__pyx_CyFunctionObject, func_vectorcall), READONLY, 0},
#else
    {"__vectorcalloffset__", T_PYSSIZET, offsetof(PyCFunctionObject, vectorcall), READONLY, 0},
#endif
#if CYTHON_COMPILING_IN_LIMITED_API
    {"__weaklistoffset__", T_PYSSIZET, offsetof(__pyx_CyFunctionObject, func_weakreflist), READONLY, 0},
#else
    {"__weaklistoffset__", T_PYSSIZET, offsetof(PyCFunctionObject, m_weakreflist), READONLY, 0},
#endif
#endif
    {0, 0, 0,  0, 0}
};
static PyObject *
__Pyx_CyFunction_reduce(__pyx_CyFunctionObject *m, PyObject *args)
{
    PyObject *result = NULL;
    CYTHON_UNUSED_VAR(args);
    __Pyx_BEGIN_CRITICAL_SECTION(m);
    Py_INCREF(m->func_qualname);
    result = m->func_qualname;
    __Pyx_END_CRITICAL_SECTION();
    return result;
}
static PyMethodDef __pyx_CyFunction_methods[] = {
    {"__reduce__", (PyCFunction)__Pyx_CyFunction_reduce, METH_VARARGS, 0},
    {0, 0, 0, 0}
};
#if CYTHON_COMPILING_IN_LIMITED_API
#define __Pyx_CyFunction_weakreflist(cyfunc) ((cyfunc)->func_weakreflist)
#else
#define __Pyx_CyFunction_weakreflist(cyfunc) (((PyCFunctionObject*)cyfunc)->m_weakreflist)
#endif
static PyObject *__Pyx_CyFunction_Init(__pyx_CyFunctionObject *op, PyMethodDef *ml, int flags, PyObject* qualname,
                                       PyObject *closure, PyObject *module, PyObject* globals, PyObject* code) {
#if !CYTHON_COMPILING_IN_LIMITED_API
    PyCFunctionObject *cf = (PyCFunctionObject*) op;
#endif
    if (unlikely(op == NULL))
        return NULL;
#if CYTHON_COMPILING_IN_LIMITED_API
    op->func = PyCFunction_NewEx(ml, (PyObject*)op, module);
    if (unlikely(!op->func)) return NULL;
#endif
    op->flags = flags;
    __Pyx_CyFunction_weakreflist(op) = NULL;
#if !CYTHON_COMPILING_IN_LIMITED_API
    cf->m_ml = ml;
    cf->m_self = (PyObject *) op;
#endif
    Py_XINCREF(closure);
    op->func_closure = closure;
#if !CYTHON_COMPILING_IN_LIMITED_API
    Py_XINCREF(module);
    cf->m_module = module;
#endif
#if PY_VERSION_HEX < 0x030C0000 || CYTHON_COMPILING_IN_LIMITED_API
    op->func_dict = NULL;
#endif
    op->func_name = NULL;
    Py_INCREF(qualname);
    op->func_qualname = qualname;
    op->func_doc = NULL;
#if PY_VERSION_HEX < 0x030900B1 || CYTHON_COMPILING_IN_LIMITED_API
    op->func_classobj = NULL;
#else
    ((PyCMethodObject*)op)->mm_class = NULL;
#endif
    op->func_globals = globals;
    Py_INCREF(op->func_globals);
    Py_XINCREF(code);
    op->func_code = code;
    op->defaults = NULL;
    op->defaults_tuple = NULL;
    op->defaults_kwdict = NULL;
    op->defaults_getter = NULL;
    op->func_annotations = NULL;
    op->func_is_coroutine = NULL;
#if CYTHON_METH_FASTCALL
    switch (ml->ml_flags & (METH_VARARGS | METH_FASTCALL | METH_NOARGS | METH_O | METH_KEYWORDS | METH_METHOD)) {
    case METH_NOARGS:
        __Pyx_CyFunction_func_vectorcall(op) = __Pyx_CyFunction_Vectorcall_NOARGS;
        break;
    case METH_O:
        __Pyx_CyFunction_func_vectorcall(op) = __Pyx_CyFunction_Vectorcall_O;
        break;
    case METH_METHOD | METH_FASTCALL | METH_KEYWORDS:
        __Pyx_CyFunction_func_vectorcall(op) = __Pyx_CyFunction_Vectorcall_FASTCALL_KEYWORDS_METHOD;
        break;
    case METH_FASTCALL | METH_KEYWORDS:
        __Pyx_CyFunction_func_vectorcall(op) = __Pyx_CyFunction_Vectorcall_FASTCALL_KEYWORDS;
        break;
    case METH_VARARGS | METH_KEYWORDS:
        __Pyx_CyFunction_func_vectorcall(op) = NULL;
        break;
    default:
        PyErr_SetString(PyExc_SystemError, "Bad call flags for CyFunction");
        Py_DECREF(op);
        return NULL;
    }
#endif
    return (PyObject *) op;
}
static int
__Pyx_CyFunction_clear(__pyx_CyFunctionObject *m)
{
    Py_CLEAR(m->func_closure);
#if CYTHON_COMPILING_IN_LIMITED_API
    Py_CLEAR(m->func);
#else
    Py_CLEAR(((PyCFunctionObject*)m)->m_module);
#endif
#if PY_VERSION_HEX < 0x030C0000 || CYTHON_COMPILING_IN_LIMITED_API
    Py_CLEAR(m->func_dict);
#elif PY_VERSION_HEX < 0x030d0000
    _PyObject_ClearManagedDict((PyObject*)m);
#else
    PyObject_ClearManagedDict((PyObject*)m);
#endif
    Py_CLEAR(m->func_name);
    Py_CLEAR(m->func_qualname);
    Py_CLEAR(m->func_doc);
    Py_CLEAR(m->func_globals);
    Py_CLEAR(m->func_code);
#if !CYTHON_COMPILING_IN_LIMITED_API
#if PY_VERSION_HEX < 0x030900B1
    Py_CLEAR(__Pyx_CyFunction_GetClassObj(m));
#else
    {
        PyObject *cls = (PyObject*) ((PyCMethodObject *) (m))->mm_class;
        ((PyCMethodObject *) (m))->mm_class = NULL;
        Py_XDECREF(cls);
    }
#endif
#endif
    Py_CLEAR(m->defaults_tuple);
    Py_CLEAR(m->defaults_kwdict);
    Py_CLEAR(m->func_annotations);
    Py_CLEAR(m->func_is_coroutine);
    Py_CLEAR(m->defaults);
    return 0;
}
static void __Pyx__CyFunction_dealloc(__pyx_CyFunctionObject *m)
{
    if (__Pyx_CyFunction_weakreflist(m) != NULL)
        PyObject_ClearWeakRefs((PyObject *) m);
    __Pyx_CyFunction_clear(m);
    __Pyx_PyHeapTypeObject_GC_Del(m);
}
static void __Pyx_CyFunction_dealloc(__pyx_CyFunctionObject *m)
{
    PyObject_GC_UnTrack(m);
    __Pyx__CyFunction_dealloc(m);
}
static int __Pyx_CyFunction_traverse(__pyx_CyFunctionObject *m, visitproc visit, void *arg)
{
    {
        int e = __Pyx_call_type_traverse((PyObject*)m, 1, visit, arg);
        if (e) return e;
    }
    Py_VISIT(m->func_closure);
#if CYTHON_COMPILING_IN_LIMITED_API
    Py_VISIT(m->func);
#else
    Py_VISIT(((PyCFunctionObject*)m)->m_module);
#endif
#if PY_VERSION_HEX < 0x030C0000 || CYTHON_COMPILING_IN_LIMITED_API
    Py_VISIT(m->func_dict);
#else
    {
        int e =
#if PY_VERSION_HEX < 0x030d0000
            _PyObject_VisitManagedDict
#else
            PyObject_VisitManagedDict
#endif
                ((PyObject*)m, visit, arg);
        if (e != 0) return e;
    }
#endif
    __Pyx_VISIT_CONST(m->func_name);
    __Pyx_VISIT_CONST(m->func_qualname);
    Py_VISIT(m->func_doc);
    Py_VISIT(m->func_globals);
    __Pyx_VISIT_CONST(m->func_code);
#if !CYTHON_COMPILING_IN_LIMITED_API
    Py_VISIT(__Pyx_CyFunction_GetClassObj(m));
#endif
    Py_VISIT(m->defaults_tuple);
    Py_VISIT(m->defaults_kwdict);
    Py_VISIT(m->func_is_coroutine);
    Py_VISIT(m->defaults);
    return 0;
}
static PyObject*
__Pyx_CyFunction_repr(__pyx_CyFunctionObject *op)
{
    PyObject *repr;
    __Pyx_BEGIN_CRITICAL_SECTION(op);
    repr = PyUnicode_FromFormat("<cyfunction %U at %p>",
                                op->func_qualname, (void *)op);
    __Pyx_END_CRITICAL_SECTION();
    return repr;
}
static PyObject * __Pyx_CyFunction_CallMethod(PyObject *func, PyObject *self, PyObject *arg, PyObject *kw) {
#if CYTHON_COMPILING_IN_LIMITED_API
    PyObject *f = ((__pyx_CyFunctionObject*)func)->func;
    PyCFunction meth;
    int flags;
    meth = PyCFunction_GetFunction(f);
    if (unlikely(!meth)) return NULL;
    flags = PyCFunction_GetFlags(f);
    if (unlikely(flags < 0)) return NULL;
#else
    PyCFunctionObject* f = (PyCFunctionObject*)func;
    PyCFunction meth = f->m_ml->ml_meth;
    int flags = f->m_ml->ml_flags;
#endif
    Py_ssize_t size;
    switch (flags & (METH_VARARGS | METH_KEYWORDS | METH_NOARGS | METH_O)) {
    case METH_VARARGS:
        if (likely(kw == NULL || PyDict_Size(kw) == 0))
            return (*meth)(self, arg);
        break;
    case METH_VARARGS | METH_KEYWORDS:
        return (*(PyCFunctionWithKeywords)(void(*)(void))meth)(self, arg, kw);
    case METH_NOARGS:
        if (likely(kw == NULL || PyDict_Size(kw) == 0)) {
#if CYTHON_ASSUME_SAFE_SIZE
            size = PyTuple_GET_SIZE(arg);
#else
            size = PyTuple_Size(arg);
            if (unlikely(size < 0)) return NULL;
#endif
            if (likely(size == 0))
                return (*meth)(self, NULL);
            __Pyx_CyFunction_raise_argument_count_error(
                (__pyx_CyFunctionObject*)func,
                "takes no arguments", size);
            return NULL;
        }
        break;
    case METH_O:
        if (likely(kw == NULL || PyDict_Size(kw) == 0)) {
#if CYTHON_ASSUME_SAFE_SIZE
            size = PyTuple_GET_SIZE(arg);
#else
            size = PyTuple_Size(arg);
            if (unlikely(size < 0)) return NULL;
#endif
            if (likely(size == 1)) {
                PyObject *result, *arg0;
                #if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
                arg0 = PyTuple_GET_ITEM(arg, 0);
                #else
                arg0 = __Pyx_PySequence_ITEM(arg, 0); if (unlikely(!arg0)) return NULL;
                #endif
                result = (*meth)(self, arg0);
                #if !(CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS)
                Py_DECREF(arg0);
                #endif
                return result;
            }
            __Pyx_CyFunction_raise_argument_count_error(
                (__pyx_CyFunctionObject*)func,
                "takes exactly one argument", size);
            return NULL;
        }
        break;
    default:
        PyErr_SetString(PyExc_SystemError, "Bad call flags for CyFunction");
        return NULL;
    }
    __Pyx_CyFunction_raise_type_error(
        (__pyx_CyFunctionObject*)func, "takes no keyword arguments");
    return NULL;
}
static CYTHON_INLINE PyObject *__Pyx_CyFunction_Call(PyObject *func, PyObject *arg, PyObject *kw) {
    PyObject *self, *result;
#if CYTHON_COMPILING_IN_LIMITED_API
    self = PyCFunction_GetSelf(((__pyx_CyFunctionObject*)func)->func);
    if (unlikely(!self) && PyErr_Occurred()) return NULL;
#else
    self = ((PyCFunctionObject*)func)->m_self;
#endif
    result = __Pyx_CyFunction_CallMethod(func, self, arg, kw);
    return result;
}
static PyObject *__Pyx_CyFunction_CallAsMethod(PyObject *func, PyObject *args, PyObject *kw) {
    PyObject *result;
    __pyx_CyFunctionObject *cyfunc = (__pyx_CyFunctionObject *) func;
#if CYTHON_METH_FASTCALL && CYTHON_VECTORCALL
     __pyx_vectorcallfunc vc = __Pyx_CyFunction_func_vectorcall(cyfunc);
    if (vc) {
#if CYTHON_ASSUME_SAFE_MACROS && CYTHON_ASSUME_SAFE_SIZE
        return __Pyx_PyVectorcall_FastCallDict(func, vc, &PyTuple_GET_ITEM(args, 0), (size_t)PyTuple_GET_SIZE(args), kw);
#else
        (void) &__Pyx_PyVectorcall_FastCallDict;
        return PyVectorcall_Call(func, args, kw);
#endif
    }
#endif
    if ((cyfunc->flags & __Pyx_CYFUNCTION_CCLASS) && !(cyfunc->flags & __Pyx_CYFUNCTION_STATICMETHOD)) {
        Py_ssize_t argc;
        PyObject *new_args;
        PyObject *self;
#if CYTHON_ASSUME_SAFE_SIZE
        argc = PyTuple_GET_SIZE(args);
#else
        argc = PyTuple_Size(args);
        if (unlikely(argc < 0)) return NULL;
#endif
        new_args = PyTuple_GetSlice(args, 1, argc);
        if (unlikely(!new_args))
            return NULL;
        self = PyTuple_GetItem(args, 0);
        if (unlikely(!self)) {
            Py_DECREF(new_args);
            PyErr_Format(PyExc_TypeError,
                         "unbound method %.200S() needs an argument",
                         cyfunc->func_qualname);
            return NULL;
        }
        result = __Pyx_CyFunction_CallMethod(func, self, new_args, kw);
        Py_DECREF(new_args);
    } else {
        result = __Pyx_CyFunction_Call(func, args, kw);
    }
    return result;
}
#if CYTHON_METH_FASTCALL && CYTHON_VECTORCALL
static CYTHON_INLINE int __Pyx_CyFunction_Vectorcall_CheckArgs(__pyx_CyFunctionObject *cyfunc, Py_ssize_t nargs, PyObject *kwnames)
{
    int ret = 0;
    if ((cyfunc->flags & __Pyx_CYFUNCTION_CCLASS) && !(cyfunc->flags & __Pyx_CYFUNCTION_STATICMETHOD)) {
        if (unlikely(nargs < 1)) {
            __Pyx_CyFunction_raise_type_error(
                cyfunc, "needs an argument");
            return -1;
        }
        ret = 1;
    }
    if (unlikely(kwnames) && unlikely(__Pyx_PyTuple_GET_SIZE(kwnames))) {
        __Pyx_CyFunction_raise_type_error(
            cyfunc, "takes no keyword arguments");
        return -1;
    }
    return ret;
}
static PyObject * __Pyx_CyFunction_Vectorcall_NOARGS(PyObject *func, PyObject *const *args, size_t nargsf, PyObject *kwnames)
{
    __pyx_CyFunctionObject *cyfunc = (__pyx_CyFunctionObject *)func;
    Py_ssize_t nargs = PyVectorcall_NARGS(nargsf);
    PyObject *self;
#if CYTHON_COMPILING_IN_LIMITED_API
    PyCFunction meth = PyCFunction_GetFunction(cyfunc->func);
    if (unlikely(!meth)) return NULL;
#else
    PyCFunction meth = ((PyCFunctionObject*)cyfunc)->m_ml->ml_meth;
#endif
    switch (__Pyx_CyFunction_Vectorcall_CheckArgs(cyfunc, nargs, kwnames)) {
    case 1:
        self = args[0];
        args += 1;
        nargs -= 1;
        break;
    case 0:
#if CYTHON_COMPILING_IN_LIMITED_API
        self = PyCFunction_GetSelf(((__pyx_CyFunctionObject*)cyfunc)->func);
        if (unlikely(!self) && PyErr_Occurred()) return NULL;
#else
        self = ((PyCFunctionObject*)cyfunc)->m_self;
#endif
        break;
    default:
        return NULL;
    }
    if (unlikely(nargs != 0)) {
        __Pyx_CyFunction_raise_argument_count_error(
            cyfunc, "takes no arguments", nargs);
        return NULL;
    }
    return meth(self, NULL);
}
static PyObject * __Pyx_CyFunction_Vectorcall_O(PyObject *func, PyObject *const *args, size_t nargsf, PyObject *kwnames)
{
    __pyx_CyFunctionObject *cyfunc = (__pyx_CyFunctionObject *)func;
    Py_ssize_t nargs = PyVectorcall_NARGS(nargsf);
    PyObject *self;
#if CYTHON_COMPILING_IN_LIMITED_API
    PyCFunction meth = PyCFunction_GetFunction(cyfunc->func);
    if (unlikely(!meth)) return NULL;
#else
    PyCFunction meth = ((PyCFunctionObject*)cyfunc)->m_ml->ml_meth;
#endif
    switch (__Pyx_CyFunction_Vectorcall_CheckArgs(cyfunc, nargs, kwnames)) {
    case 1:
        self = args[0];
        args += 1;
        nargs -= 1;
        break;
    case 0:
#if CYTHON_COMPILING_IN_LIMITED_API
        self = PyCFunction_GetSelf(((__pyx_CyFunctionObject*)cyfunc)->func);
        if (unlikely(!self) && PyErr_Occurred()) return NULL;
#else
        self = ((PyCFunctionObject*)cyfunc)->m_self;
#endif
        break;
    default:
        return NULL;
    }
    if (unlikely(nargs != 1)) {
        __Pyx_CyFunction_raise_argument_count_error(
            cyfunc, "takes exactly one argument", nargs);
        return NULL;
    }
    return meth(self, args[0]);
}
static PyObject * __Pyx_CyFunction_Vectorcall_FASTCALL_KEYWORDS(PyObject *func, PyObject *const *args, size_t nargsf, PyObject *kwnames)
{
    __pyx_CyFunctionObject *cyfunc = (__pyx_CyFunctionObject *)func;
    Py_ssize_t nargs = PyVectorcall_NARGS(nargsf);
    PyObject *self;
#if CYTHON_COMPILING_IN_LIMITED_API
    PyCFunction meth = PyCFunction_GetFunction(cyfunc->func);
    if (unlikely(!meth)) return NULL;
#else
    PyCFunction meth = ((PyCFunctionObject*)cyfunc)->m_ml->ml_meth;
#endif
    switch (__Pyx_CyFunction_Vectorcall_CheckArgs(cyfunc, nargs, NULL)) {
    case 1:
        self = args[0];
        args += 1;
        nargs -= 1;
        break;
    case 0:
#if CYTHON_COMPILING_IN_LIMITED_API
        self = PyCFunction_GetSelf(((__pyx_CyFunctionObject*)cyfunc)->func);
        if (unlikely(!self) && PyErr_Occurred()) return NULL;
#else
        self = ((PyCFunctionObject*)cyfunc)->m_self;
#endif
        break;
    default:
        return NULL;
    }
    return ((__Pyx_PyCFunctionFastWithKeywords)(void(*)(void))meth)(self, args, nargs, kwnames);
}
static PyObject * __Pyx_CyFunction_Vectorcall_FASTCALL_KEYWORDS_METHOD(PyObject *func, PyObject *const *args, size_t nargsf, PyObject *kwnames)
{
    __pyx_CyFunctionObject *cyfunc = (__pyx_CyFunctionObject *)func;
    PyTypeObject *cls = (PyTypeObject *) __Pyx_CyFunction_GetClassObj(cyfunc);
    Py_ssize_t nargs = PyVectorcall_NARGS(nargsf);
    PyObject *self;
#if CYTHON_COMPILING_IN_LIMITED_API
    PyCFunction meth = PyCFunction_GetFunction(cyfunc->func);
    if (unlikely(!meth)) return NULL;
#else
    PyCFunction meth = ((PyCFunctionObject*)cyfunc)->m_ml->ml_meth;
#endif
    switch (__Pyx_CyFunction_Vectorcall_CheckArgs(cyfunc, nargs, NULL)) {
    case 1:
        self = args[0];
        args += 1;
        nargs -= 1;
        break;
    case 0:
#if CYTHON_COMPILING_IN_LIMITED_API
        self = PyCFunction_GetSelf(((__pyx_CyFunctionObject*)cyfunc)->func);
        if (unlikely(!self) && PyErr_Occurred()) return NULL;
#else
        self = ((PyCFunctionObject*)cyfunc)->m_self;
#endif
        break;
    default:
        return NULL;
    }
    #if PY_VERSION_HEX < 0x030e00A6
    size_t nargs_value = (size_t) nargs;
    #else
    Py_ssize_t nargs_value = nargs;
    #endif
    return ((__Pyx_PyCMethod)(void(*)(void))meth)(self, cls, args, nargs_value, kwnames);
}
#endif
static PyType_Slot __pyx_CyFunctionType_slots[] = {
    {Py_tp_dealloc, (void *)__Pyx_CyFunction_dealloc},
    {Py_tp_repr, (void *)__Pyx_CyFunction_repr},
    {Py_tp_call, (void *)__Pyx_CyFunction_CallAsMethod},
    {Py_tp_traverse, (void *)__Pyx_CyFunction_traverse},
    {Py_tp_clear, (void *)__Pyx_CyFunction_clear},
    {Py_tp_methods, (void *)__pyx_CyFunction_methods},
    {Py_tp_members, (void *)__pyx_CyFunction_members},
    {Py_tp_getset, (void *)__pyx_CyFunction_getsets},
    {Py_tp_descr_get, (void *)__Pyx_PyMethod_New},
    {0, 0},
};
static PyType_Spec __pyx_CyFunctionType_spec = {
    __PYX_TYPE_MODULE_PREFIX "cython_function_or_method",
    sizeof(__pyx_CyFunctionObject),
    0,
#ifdef Py_TPFLAGS_METHOD_DESCRIPTOR
    Py_TPFLAGS_METHOD_DESCRIPTOR |
#endif
#if CYTHON_METH_FASTCALL
#if defined(Py_TPFLAGS_HAVE_VECTORCALL)
    Py_TPFLAGS_HAVE_VECTORCALL |
#elif defined(_Py_TPFLAGS_HAVE_VECTORCALL)
    _Py_TPFLAGS_HAVE_VECTORCALL |
#endif
#endif
#if PY_VERSION_HEX >= 0x030C0000 && !CYTHON_COMPILING_IN_LIMITED_API
    Py_TPFLAGS_MANAGED_DICT |
#endif
    Py_TPFLAGS_IMMUTABLETYPE | Py_TPFLAGS_DISALLOW_INSTANTIATION |
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC | Py_TPFLAGS_BASETYPE,
    __pyx_CyFunctionType_slots
};
static int __pyx_CyFunction_init(PyObject *module) {
    __pyx_mstatetype *mstate = __Pyx_PyModule_GetState(module);
    mstate->__pyx_CyFunctionType = __Pyx_FetchCommonTypeFromSpec(
        mstate->__pyx_CommonTypesMetaclassType, module, &__pyx_CyFunctionType_spec, NULL);
    if (unlikely(mstate->__pyx_CyFunctionType == NULL)) {
        return -1;
    }
    return 0;
}
static CYTHON_INLINE PyObject *__Pyx_CyFunction_InitDefaults(PyObject *func, PyTypeObject *defaults_type) {
    __pyx_CyFunctionObject *m = (__pyx_CyFunctionObject *) func;
    m->defaults = PyObject_CallObject((PyObject*)defaults_type, NULL);
    if (unlikely(!m->defaults))
        return NULL;
    return m->defaults;
}
static CYTHON_INLINE void __Pyx_CyFunction_SetDefaultsTuple(PyObject *func, PyObject *tuple) {
    __pyx_CyFunctionObject *m = (__pyx_CyFunctionObject *) func;
    m->defaults_tuple = tuple;
    Py_INCREF(tuple);
}
static CYTHON_INLINE void __Pyx_CyFunction_SetDefaultsKwDict(PyObject *func, PyObject *dict) {
    __pyx_CyFunctionObject *m = (__pyx_CyFunctionObject *) func;
    m->defaults_kwdict = dict;
    Py_INCREF(dict);
}
static CYTHON_INLINE void __Pyx_CyFunction_SetAnnotationsDict(PyObject *func, PyObject *dict) {
    __pyx_CyFunctionObject *m = (__pyx_CyFunctionObject *) func;
    m->func_annotations = dict;
    Py_INCREF(dict);
}

static PyObject *__Pyx_CyFunction_New(PyMethodDef *ml, int flags, PyObject* qualname,
                                      PyObject *closure, PyObject *module, PyObject* globals, PyObject* code) {
    PyObject *op = __Pyx_CyFunction_Init(
        PyObject_GC_New(__pyx_CyFunctionObject, __pyx_mstate_global->__pyx_CyFunctionType),
        ml, flags, qualname, closure, module, globals, code
    );
    if (likely(op)) {
        PyObject_GC_Track(op);
    }
    return op;
}

#if CYTHON_CLINE_IN_TRACEBACK && CYTHON_CLINE_IN_TRACEBACK_RUNTIME
#if CYTHON_COMPILING_IN_LIMITED_API && __PYX_LIMITED_VERSION_HEX < 0x030A0000
#define __Pyx_PyProbablyModule_GetDict(o) __Pyx_XNewRef(PyModule_GetDict(o))
#elif !CYTHON_COMPILING_IN_CPYTHON || CYTHON_COMPILING_IN_CPYTHON_FREETHREADING
#define __Pyx_PyProbablyModule_GetDict(o) PyObject_GenericGetDict(o, NULL);
#else
PyObject* __Pyx_PyProbablyModule_GetDict(PyObject *o) {
    PyObject **dict_ptr = _PyObject_GetDictPtr(o);
    return dict_ptr ? __Pyx_XNewRef(*dict_ptr) : NULL;
}
#endif
static int __Pyx_CLineForTraceback(PyThreadState *tstate, int c_line) {
    PyObject *use_cline = NULL;
    PyObject *ptype, *pvalue, *ptraceback;
    PyObject *cython_runtime_dict;
    CYTHON_MAYBE_UNUSED_VAR(tstate);
    if (unlikely(!__pyx_mstate_global->__pyx_cython_runtime)) {
        return c_line;
    }
    __Pyx_ErrFetchInState(tstate, &ptype, &pvalue, &ptraceback);
    cython_runtime_dict = __Pyx_PyProbablyModule_GetDict(__pyx_mstate_global->__pyx_cython_runtime);
    if (likely(cython_runtime_dict)) {
        __PYX_PY_DICT_LOOKUP_IF_MODIFIED(
            use_cline, cython_runtime_dict,
            __Pyx_PyDict_SetDefault(cython_runtime_dict, __pyx_mstate_global->__pyx_n_u_cline_in_traceback, Py_False))
    }
    if (use_cline == NULL || use_cline == Py_False || (use_cline != Py_True && PyObject_Not(use_cline) != 0)) {
        c_line = 0;
    }
    Py_XDECREF(use_cline);
    Py_XDECREF(cython_runtime_dict);
    __Pyx_ErrRestoreInState(tstate, ptype, pvalue, ptraceback);
    return c_line;
}
#endif

static int __pyx_bisect_code_objects(__Pyx_CodeObjectCacheEntry* entries, int count, int code_line) {
    int start = 0, mid = 0, end = count - 1;
    if (end >= 0 && code_line > entries[end].code_line) {
        return count;
    }
    while (start < end) {
        mid = start + (end - start) / 2;
        if (code_line < entries[mid].code_line) {
            end = mid;
        } else if (code_line > entries[mid].code_line) {
             start = mid + 1;
        } else {
            return mid;
        }
    }
    if (code_line <= entries[mid].code_line) {
        return mid;
    } else {
        return mid + 1;
    }
}
static __Pyx_CachedCodeObjectType *__pyx__find_code_object(struct __Pyx_CodeObjectCache *code_cache, int code_line) {
    __Pyx_CachedCodeObjectType* code_object;
    int pos;
    if (unlikely(!code_line) || unlikely(!code_cache->entries)) {
        return NULL;
    }
    pos = __pyx_bisect_code_objects(code_cache->entries, code_cache->count, code_line);
    if (unlikely(pos >= code_cache->count) || unlikely(code_cache->entries[pos].code_line != code_line)) {
        return NULL;
    }
    code_object = code_cache->entries[pos].code_object;
    Py_INCREF(code_object);
    return code_object;
}
static __Pyx_CachedCodeObjectType *__pyx_find_code_object(int code_line) {
#if CYTHON_COMPILING_IN_CPYTHON_FREETHREADING && !CYTHON_ATOMICS
    (void)__pyx__find_code_object;
    return NULL;
#else
    struct __Pyx_CodeObjectCache *code_cache = &__pyx_mstate_global->__pyx_code_cache;
#if CYTHON_COMPILING_IN_CPYTHON_FREETHREADING
    __pyx_nonatomic_int_type old_count = __pyx_atomic_incr_acq_rel(&code_cache->accessor_count);
    if (old_count < 0) {
        __pyx_atomic_decr_acq_rel(&code_cache->accessor_count);
        return NULL;
    }
#endif
    __Pyx_CachedCodeObjectType *result = __pyx__find_code_object(code_cache, code_line);
#if CYTHON_COMPILING_IN_CPYTHON_FREETHREADING
    __pyx_atomic_decr_acq_rel(&code_cache->accessor_count);
#endif
    return result;
#endif
}
static void __pyx__insert_code_object(struct __Pyx_CodeObjectCache *code_cache, int code_line, __Pyx_CachedCodeObjectType* code_object)
{
    int pos, i;
    __Pyx_CodeObjectCacheEntry* entries = code_cache->entries;
    if (unlikely(!code_line)) {
        return;
    }
    if (unlikely(!entries)) {
        entries = (__Pyx_CodeObjectCacheEntry*)PyMem_Malloc(64*sizeof(__Pyx_CodeObjectCacheEntry));
        if (likely(entries)) {
            code_cache->entries = entries;
            code_cache->max_count = 64;
            code_cache->count = 1;
            entries[0].code_line = code_line;
            entries[0].code_object = code_object;
            Py_INCREF(code_object);
        }
        return;
    }
    pos = __pyx_bisect_code_objects(code_cache->entries, code_cache->count, code_line);
    if ((pos < code_cache->count) && unlikely(code_cache->entries[pos].code_line == code_line)) {
        __Pyx_CachedCodeObjectType* tmp = entries[pos].code_object;
        entries[pos].code_object = code_object;
        Py_INCREF(code_object);
        Py_DECREF(tmp);
        return;
    }
    if (code_cache->count == code_cache->max_count) {
        int new_max = code_cache->max_count + 64;
        entries = (__Pyx_CodeObjectCacheEntry*)PyMem_Realloc(
            code_cache->entries, ((size_t)new_max) * sizeof(__Pyx_CodeObjectCacheEntry));
        if (unlikely(!entries)) {
            return;
        }
        code_cache->entries = entries;
        code_cache->max_count = new_max;
    }
    for (i=code_cache->count; i>pos; i--) {
        entries[i] = entries[i-1];
    }
    entries[pos].code_line = code_line;
    entries[pos].code_object = code_object;
    code_cache->count++;
    Py_INCREF(code_object);
}
static void __pyx_insert_code_object(int code_line, __Pyx_CachedCodeObjectType* code_object) {
#if CYTHON_COMPILING_IN_CPYTHON_FREETHREADING && !CYTHON_ATOMICS
    (void)__pyx__insert_code_object;
    return;
#else
    struct __Pyx_CodeObjectCache *code_cache = &__pyx_mstate_global->__pyx_code_cache;
#if CYTHON_COMPILING_IN_CPYTHON_FREETHREADING
    __pyx_nonatomic_int_type expected = 0;
    if (!__pyx_atomic_int_cmp_exchange(&code_cache->accessor_count, &expected, INT_MIN)) {
        return;
    }
#endif
    __pyx__insert_code_object(code_cache, code_line, code_object);
#if CYTHON_COMPILING_IN_CPYTHON_FREETHREADING
    __pyx_atomic_sub(&code_cache->accessor_count, INT_MIN);
#endif
#endif
}

#include "compile.h"
#include "frameobject.h"
#include "traceback.h"
#if PY_VERSION_HEX >= 0x030b00a6 && !CYTHON_COMPILING_IN_LIMITED_API && !defined(PYPY_VERSION)
  #ifndef Py_BUILD_CORE
    #define Py_BUILD_CORE 1
  #endif
  #include "internal/pycore_frame.h"
#endif
#if CYTHON_COMPILING_IN_LIMITED_API
static PyObject *__Pyx_PyCode_Replace_For_AddTraceback(PyObject *code, PyObject *scratch_dict,
                                                       PyObject *firstlineno, PyObject *name) {
    PyObject *replace = NULL;
    if (unlikely(PyDict_SetItemString(scratch_dict, "co_firstlineno", firstlineno))) return NULL;
    if (unlikely(PyDict_SetItemString(scratch_dict, "co_name", name))) return NULL;
    replace = PyObject_GetAttrString(code, "replace");
    if (likely(replace)) {
        PyObject *result = PyObject_Call(replace, __pyx_mstate_global->__pyx_empty_tuple, scratch_dict);
        Py_DECREF(replace);
        return result;
    }
    PyErr_Clear();
    return NULL;
}
static void __Pyx_AddTraceback(const char *funcname, int c_line,
                               int py_line, const char *filename) {
    PyObject *code_object = NULL, *py_py_line = NULL, *py_funcname = NULL, *dict = NULL;
    PyObject *replace = NULL, *getframe = NULL, *frame = NULL;
    PyObject *exc_type, *exc_value, *exc_traceback;
    int success = 0;
    if (c_line) {
        c_line = __Pyx_CLineForTraceback(__Pyx_PyThreadState_Current, c_line);
    }
    PyErr_Fetch(&exc_type, &exc_value, &exc_traceback);
    code_object = __pyx_find_code_object(c_line ? -c_line : py_line);
    if (!code_object) {
        code_object = Py_CompileString("_getframe()", filename, Py_eval_input);
        if (unlikely(!code_object)) goto bad;
        py_py_line = PyLong_FromLong(py_line);
        if (unlikely(!py_py_line)) goto bad;
        if (c_line) {
            py_funcname = PyUnicode_FromFormat( "%s (%s:%d)", funcname, __pyx_cfilenm, c_line);
        } else {
            py_funcname = PyUnicode_FromString(funcname);
        }
        if (unlikely(!py_funcname)) goto bad;
        dict = PyDict_New();
        if (unlikely(!dict)) goto bad;
        {
            PyObject *old_code_object = code_object;
            code_object = __Pyx_PyCode_Replace_For_AddTraceback(code_object, dict, py_py_line, py_funcname);
            Py_DECREF(old_code_object);
        }
        if (unlikely(!code_object)) goto bad;
        __pyx_insert_code_object(c_line ? -c_line : py_line, code_object);
    } else {
        dict = PyDict_New();
    }
    getframe = PySys_GetObject("_getframe");
    if (unlikely(!getframe)) goto bad;
    if (unlikely(PyDict_SetItemString(dict, "_getframe", getframe))) goto bad;
    frame = PyEval_EvalCode(code_object, dict, dict);
    if (unlikely(!frame) || frame == Py_None) goto bad;
    success = 1;
  bad:
    PyErr_Restore(exc_type, exc_value, exc_traceback);
    Py_XDECREF(code_object);
    Py_XDECREF(py_py_line);
    Py_XDECREF(py_funcname);
    Py_XDECREF(dict);
    Py_XDECREF(replace);
    if (success) {
        PyTraceBack_Here(
            (struct _frame*)frame);
    }
    Py_XDECREF(frame);
}
#else
static PyCodeObject* __Pyx_CreateCodeObjectForTraceback(
            const char *funcname, int c_line,
            int py_line, const char *filename) {
    PyCodeObject *py_code = NULL;
    PyObject *py_funcname = NULL;
    if (c_line) {
        py_funcname = PyUnicode_FromFormat( "%s (%s:%d)", funcname, __pyx_cfilenm, c_line);
        if (!py_funcname) goto bad;
        funcname = PyUnicode_AsUTF8(py_funcname);
        if (!funcname) goto bad;
    }
    py_code = PyCode_NewEmpty(filename, funcname, py_line);
    Py_XDECREF(py_funcname);
    return py_code;
bad:
    Py_XDECREF(py_funcname);
    return NULL;
}
static void __Pyx_AddTraceback(const char *funcname, int c_line,
                               int py_line, const char *filename) {
    PyCodeObject *py_code = 0;
    PyFrameObject *py_frame = 0;
    PyThreadState *tstate = __Pyx_PyThreadState_Current;
    PyObject *ptype, *pvalue, *ptraceback;
    if (c_line) {
        c_line = __Pyx_CLineForTraceback(tstate, c_line);
    }
    py_code = __pyx_find_code_object(c_line ? -c_line : py_line);
    if (!py_code) {
        __Pyx_ErrFetchInState(tstate, &ptype, &pvalue, &ptraceback);
        py_code = __Pyx_CreateCodeObjectForTraceback(
            funcname, c_line, py_line, filename);
        if (!py_code) {

            Py_XDECREF(ptype);
            Py_XDECREF(pvalue);
            Py_XDECREF(ptraceback);
            goto bad;
        }
        __Pyx_ErrRestoreInState(tstate, ptype, pvalue, ptraceback);
        __pyx_insert_code_object(c_line ? -c_line : py_line, py_code);
    }
    py_frame = PyFrame_New(
        tstate,
        py_code,
        __pyx_mstate_global->__pyx_d,
        0
    );
    if (!py_frame) goto bad;
    __Pyx_PyFrame_SetLineNumber(py_frame, py_line);
    PyTraceBack_Here(py_frame);
bad:
    Py_XDECREF(py_code);
    Py_XDECREF(py_frame);
}
#endif

#define __PYX_VERIFY_RETURN_INT(target_type, func_type, func_value)\
    __PYX__VERIFY_RETURN_INT(target_type, func_type, func_value, 0)
#define __PYX_VERIFY_RETURN_INT_EXC(target_type, func_type, func_value)\
    __PYX__VERIFY_RETURN_INT(target_type, func_type, func_value, 1)
#define __PYX__VERIFY_RETURN_INT(target_type, func_type, func_value, exc)\
    {\
        func_type value = func_value;\
        if (sizeof(target_type) < sizeof(func_type)) {\
            if (unlikely(value != (func_type) (target_type) value)) {\
                func_type zero = 0;\
                if (exc && unlikely(value == (func_type)-1 && PyErr_Occurred()))\
                    return (target_type) -1;\
                if (is_unsigned && unlikely(value < zero))\
                    goto raise_neg_overflow;\
                else\
                    goto raise_overflow;\
            }\
        }\
        return (target_type) value;\
    }

static CYTHON_INLINE int __Pyx_PyLong_As_int(PyObject *x) {
#ifdef __Pyx_HAS_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif
    const int neg_one = (int) -1, const_zero = (int) 0;
#ifdef __Pyx_HAS_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif
    const int is_unsigned = neg_one > const_zero;
    if (unlikely(!PyLong_Check(x))) {
        int val;
        PyObject *tmp = __Pyx_PyNumber_Long(x);
        if (!tmp) return (int) -1;
        val = __Pyx_PyLong_As_int(tmp);
        Py_DECREF(tmp);
        return val;
    }
    if (is_unsigned) {
#if CYTHON_USE_PYLONG_INTERNALS
        if (unlikely(__Pyx_PyLong_IsNeg(x))) {
            goto raise_neg_overflow;
        } else if (__Pyx_PyLong_IsCompact(x)) {
            __PYX_VERIFY_RETURN_INT(int, __Pyx_compact_upylong, __Pyx_PyLong_CompactValueUnsigned(x))
        } else {
            const digit* digits = __Pyx_PyLong_Digits(x);
            assert(__Pyx_PyLong_DigitCount(x) > 1);
            switch (__Pyx_PyLong_DigitCount(x)) {
                case 2:
                    if ((8 * sizeof(int) > 1 * PyLong_SHIFT)) {
                        if ((8 * sizeof(unsigned long) > 2 * PyLong_SHIFT)) {
                            __PYX_VERIFY_RETURN_INT(int, unsigned long, (((((unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0])))
                        } else if ((8 * sizeof(int) >= 2 * PyLong_SHIFT)) {
                            return (int) (((((int)digits[1]) << PyLong_SHIFT) | (int)digits[0]));
                        }
                    }
                    break;
                case 3:
                    if ((8 * sizeof(int) > 2 * PyLong_SHIFT)) {
                        if ((8 * sizeof(unsigned long) > 3 * PyLong_SHIFT)) {
                            __PYX_VERIFY_RETURN_INT(int, unsigned long, (((((((unsigned long)digits[2]) << PyLong_SHIFT) | (unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0])))
                        } else if ((8 * sizeof(int) >= 3 * PyLong_SHIFT)) {
                            return (int) (((((((int)digits[2]) << PyLong_SHIFT) | (int)digits[1]) << PyLong_SHIFT) | (int)digits[0]));
                        }
                    }
                    break;
                case 4:
                    if ((8 * sizeof(int) > 3 * PyLong_SHIFT)) {
                        if ((8 * sizeof(unsigned long) > 4 * PyLong_SHIFT)) {
                            __PYX_VERIFY_RETURN_INT(int, unsigned long, (((((((((unsigned long)digits[3]) << PyLong_SHIFT) | (unsigned long)digits[2]) << PyLong_SHIFT) | (unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0])))
                        } else if ((8 * sizeof(int) >= 4 * PyLong_SHIFT)) {
                            return (int) (((((((((int)digits[3]) << PyLong_SHIFT) | (int)digits[2]) << PyLong_SHIFT) | (int)digits[1]) << PyLong_SHIFT) | (int)digits[0]));
                        }
                    }
                    break;
            }
        }
#endif
#if CYTHON_COMPILING_IN_CPYTHON && PY_VERSION_HEX < 0x030C00A7
        if (unlikely(Py_SIZE(x) < 0)) {
            goto raise_neg_overflow;
        }
#else
        {
            int result = PyObject_RichCompareBool(x, Py_False, Py_LT);
            if (unlikely(result < 0))
                return (int) -1;
            if (unlikely(result == 1))
                goto raise_neg_overflow;
        }
#endif
        if ((sizeof(int) <= sizeof(unsigned long))) {
            __PYX_VERIFY_RETURN_INT_EXC(int, unsigned long, PyLong_AsUnsignedLong(x))
        } else if ((sizeof(int) <= sizeof(unsigned PY_LONG_LONG))) {
            __PYX_VERIFY_RETURN_INT_EXC(int, unsigned PY_LONG_LONG, PyLong_AsUnsignedLongLong(x))
        }
    } else {
#if CYTHON_USE_PYLONG_INTERNALS
        if (__Pyx_PyLong_IsCompact(x)) {
            __PYX_VERIFY_RETURN_INT(int, __Pyx_compact_pylong, __Pyx_PyLong_CompactValue(x))
        } else {
            const digit* digits = __Pyx_PyLong_Digits(x);
            assert(__Pyx_PyLong_DigitCount(x) > 1);
            switch (__Pyx_PyLong_SignedDigitCount(x)) {
                case -2:
                    if ((8 * sizeof(int) - 1 > 1 * PyLong_SHIFT)) {
                        if ((8 * sizeof(unsigned long) > 2 * PyLong_SHIFT)) {
                            __PYX_VERIFY_RETURN_INT(int, long, -(long) (((((unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0])))
                        } else if ((8 * sizeof(int) - 1 > 2 * PyLong_SHIFT)) {
                            return (int) (((int)-1)*(((((int)digits[1]) << PyLong_SHIFT) | (int)digits[0])));
                        }
                    }
                    break;
                case 2:
                    if ((8 * sizeof(int) > 1 * PyLong_SHIFT)) {
                        if ((8 * sizeof(unsigned long) > 2 * PyLong_SHIFT)) {
                            __PYX_VERIFY_RETURN_INT(int, unsigned long, (((((unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0])))
                        } else if ((8 * sizeof(int) - 1 > 2 * PyLong_SHIFT)) {
                            return (int) ((((((int)digits[1]) << PyLong_SHIFT) | (int)digits[0])));
                        }
                    }
                    break;
                case -3:
                    if ((8 * sizeof(int) - 1 > 2 * PyLong_SHIFT)) {
                        if ((8 * sizeof(unsigned long) > 3 * PyLong_SHIFT)) {
                            __PYX_VERIFY_RETURN_INT(int, long, -(long) (((((((unsigned long)digits[2]) << PyLong_SHIFT) | (unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0])))
                        } else if ((8 * sizeof(int) - 1 > 3 * PyLong_SHIFT)) {
                            return (int) (((int)-1)*(((((((int)digits[2]) << PyLong_SHIFT) | (int)digits[1]) << PyLong_SHIFT) | (int)digits[0])));
                        }
                    }
                    break;
                case 3:
                    if ((8 * sizeof(int) > 2 * PyLong_SHIFT)) {
                        if ((8 * sizeof(unsigned long) > 3 * PyLong_SHIFT)) {
                            __PYX_VERIFY_RETURN_INT(int, unsigned long, (((((((unsigned long)digits[2]) << PyLong_SHIFT) | (unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0])))
                        } else if ((8 * sizeof(int) - 1 > 3 * PyLong_SHIFT)) {
                            return (int) ((((((((int)digits[2]) << PyLong_SHIFT) | (int)digits[1]) << PyLong_SHIFT) | (int)digits[0])));
                        }
                    }
                    break;
                case -4:
                    if ((8 * sizeof(int) - 1 > 3 * PyLong_SHIFT)) {
                        if ((8 * sizeof(unsigned long) > 4 * PyLong_SHIFT)) {
                            __PYX_VERIFY_RETURN_INT(int, long, -(long) (((((((((unsigned long)digits[3]) << PyLong_SHIFT) | (unsigned long)digits[2]) << PyLong_SHIFT) | (unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0])))
                        } else if ((8 * sizeof(int) - 1 > 4 * PyLong_SHIFT)) {
                            return (int) (((int)-1)*(((((((((int)digits[3]) << PyLong_SHIFT) | (int)digits[2]) << PyLong_SHIFT) | (int)digits[1]) << PyLong_SHIFT) | (int)digits[0])));
                        }
                    }
                    break;
                case 4:
                    if ((8 * sizeof(int) > 3 * PyLong_SHIFT)) {
                        if ((8 * sizeof(unsigned long) > 4 * PyLong_SHIFT)) {
                            __PYX_VERIFY_RETURN_INT(int, unsigned long, (((((((((unsigned long)digits[3]) << PyLong_SHIFT) | (unsigned long)digits[2]) << PyLong_SHIFT) | (unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0])))
                        } else if ((8 * sizeof(int) - 1 > 4 * PyLong_SHIFT)) {
                            return (int) ((((((((((int)digits[3]) << PyLong_SHIFT) | (int)digits[2]) << PyLong_SHIFT) | (int)digits[1]) << PyLong_SHIFT) | (int)digits[0])));
                        }
                    }
                    break;
            }
        }
#endif
        if ((sizeof(int) <= sizeof(long))) {
            __PYX_VERIFY_RETURN_INT_EXC(int, long, PyLong_AsLong(x))
        } else if ((sizeof(int) <= sizeof(PY_LONG_LONG))) {
            __PYX_VERIFY_RETURN_INT_EXC(int, PY_LONG_LONG, PyLong_AsLongLong(x))
        }
    }
    {
        int val;
        int ret = -1;
#if PY_VERSION_HEX >= 0x030d00A6 && !CYTHON_COMPILING_IN_LIMITED_API
        Py_ssize_t bytes_copied = PyLong_AsNativeBytes(
            x, &val, sizeof(val), Py_ASNATIVEBYTES_NATIVE_ENDIAN | (is_unsigned ? Py_ASNATIVEBYTES_UNSIGNED_BUFFER | Py_ASNATIVEBYTES_REJECT_NEGATIVE : 0));
        if (unlikely(bytes_copied == -1)) {
        } else if (unlikely(bytes_copied > (Py_ssize_t) sizeof(val))) {
            goto raise_overflow;
        } else {
            ret = 0;
        }
#elif PY_VERSION_HEX < 0x030d0000 && !(CYTHON_COMPILING_IN_PYPY || CYTHON_COMPILING_IN_LIMITED_API) || defined(_PyLong_AsByteArray)
        int one = 1; int is_little = (int)*(unsigned char *)&one;
        unsigned char *bytes = (unsigned char *)&val;
        ret = _PyLong_AsByteArray((PyLongObject *)x,
                                    bytes, sizeof(val),
                                    is_little, !is_unsigned);
#else
        PyObject *v;
        PyObject *stepval = NULL, *mask = NULL, *shift = NULL;
        int bits, remaining_bits, is_negative = 0;
        int chunk_size = (sizeof(long) < 8) ? 30 : 62;
        if (likely(PyLong_CheckExact(x))) {
            v = __Pyx_NewRef(x);
        } else {
            v = PyNumber_Long(x);
            if (unlikely(!v)) return (int) -1;
            assert(PyLong_CheckExact(v));
        }
        {
            int result = PyObject_RichCompareBool(v, Py_False, Py_LT);
            if (unlikely(result < 0)) {
                Py_DECREF(v);
                return (int) -1;
            }
            is_negative = result == 1;
        }
        if (is_unsigned && unlikely(is_negative)) {
            Py_DECREF(v);
            goto raise_neg_overflow;
        } else if (is_negative) {
            stepval = PyNumber_Invert(v);
            Py_DECREF(v);
            if (unlikely(!stepval))
                return (int) -1;
        } else {
            stepval = v;
        }
        v = NULL;
        val = (int) 0;
        mask = PyLong_FromLong((1L << chunk_size) - 1); if (unlikely(!mask)) goto done;
        shift = PyLong_FromLong(chunk_size); if (unlikely(!shift)) goto done;
        for (bits = 0; bits < (int) sizeof(int) * 8 - chunk_size; bits += chunk_size) {
            PyObject *tmp, *digit;
            long idigit;
            digit = PyNumber_And(stepval, mask);
            if (unlikely(!digit)) goto done;
            idigit = PyLong_AsLong(digit);
            Py_DECREF(digit);
            if (unlikely(idigit < 0)) goto done;
            val |= ((int) idigit) << bits;
            tmp = PyNumber_Rshift(stepval, shift);
            if (unlikely(!tmp)) goto done;
            Py_DECREF(stepval); stepval = tmp;
        }
        Py_DECREF(shift); shift = NULL;
        Py_DECREF(mask); mask = NULL;
        {
            long idigit = PyLong_AsLong(stepval);
            if (unlikely(idigit < 0)) goto done;
            remaining_bits = ((int) sizeof(int) * 8) - bits - (is_unsigned ? 0 : 1);
            if (unlikely(idigit >= (1L << remaining_bits)))
                goto raise_overflow;
            val |= ((int) idigit) << bits;
        }
        if (!is_unsigned) {
            if (unlikely(val & (((int) 1) << (sizeof(int) * 8 - 1))))
                goto raise_overflow;
            if (is_negative)
                val = ~val;
        }
        ret = 0;
    done:
        Py_XDECREF(shift);
        Py_XDECREF(mask);
        Py_XDECREF(stepval);
#endif
        if (unlikely(ret))
            return (int) -1;
        return val;
    }
raise_overflow:
    PyErr_SetString(PyExc_OverflowError,
        "value too large to convert to int");
    return (int) -1;
raise_neg_overflow:
    PyErr_SetString(PyExc_OverflowError,
        "can't convert negative value to int");
    return (int) -1;
}

#if CYTHON_VECTORCALL
static int __Pyx_VectorcallBuilder_AddArg(PyObject *key, PyObject *value, PyObject *builder, PyObject **args, int n) {
    (void)__Pyx_PyObject_FastCallDict;
    if (__Pyx_PyTuple_SET_ITEM(builder, n, key) != (0)) return -1;
    Py_INCREF(key);
    args[n] = value;
    return 0;
}
CYTHON_UNUSED static int __Pyx_VectorcallBuilder_AddArg_Check(PyObject *key, PyObject *value, PyObject *builder, PyObject **args, int n) {
    (void)__Pyx_VectorcallBuilder_AddArgStr;
    if (unlikely(!PyUnicode_Check(key))) {
        PyErr_SetString(PyExc_TypeError, "keywords must be strings");
        return -1;
    }
    return __Pyx_VectorcallBuilder_AddArg(key, value, builder, args, n);
}
static int __Pyx_VectorcallBuilder_AddArgStr(const char *key, PyObject *value, PyObject *builder, PyObject **args, int n) {
    PyObject *pyKey = PyUnicode_FromString(key);
    if (!pyKey) return -1;
    return __Pyx_VectorcallBuilder_AddArg(pyKey, value, builder, args, n);
}
#else
CYTHON_UNUSED static int __Pyx_VectorcallBuilder_AddArg_Check(PyObject *key, PyObject *value, PyObject *builder, CYTHON_UNUSED PyObject **args, CYTHON_UNUSED int n) {
    if (unlikely(!PyUnicode_Check(key))) {
        PyErr_SetString(PyExc_TypeError, "keywords must be strings");
        return -1;
    }
    return PyDict_SetItem(builder, key, value);
}
#endif

static CYTHON_INLINE PyObject* __Pyx_PyLong_From_long(long value) {
#ifdef __Pyx_HAS_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif
    const long neg_one = (long) -1, const_zero = (long) 0;
#ifdef __Pyx_HAS_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif
    const int is_unsigned = neg_one > const_zero;
    if (is_unsigned) {
        if (sizeof(long) < sizeof(long)) {
            return PyLong_FromLong((long) value);
        } else if (sizeof(long) <= sizeof(unsigned long)) {
            return PyLong_FromUnsignedLong((unsigned long) value);
#if !CYTHON_COMPILING_IN_PYPY
        } else if (sizeof(long) <= sizeof(unsigned PY_LONG_LONG)) {
            return PyLong_FromUnsignedLongLong((unsigned PY_LONG_LONG) value);
#endif
        }
    } else {
        if (sizeof(long) <= sizeof(long)) {
            return PyLong_FromLong((long) value);
        } else if (sizeof(long) <= sizeof(PY_LONG_LONG)) {
            return PyLong_FromLongLong((PY_LONG_LONG) value);
        }
    }
    {
        unsigned char *bytes = (unsigned char *)&value;
#if !CYTHON_COMPILING_IN_LIMITED_API && PY_VERSION_HEX >= 0x030d00A4
        if (is_unsigned) {
            return PyLong_FromUnsignedNativeBytes(bytes, sizeof(value), -1);
        } else {
            return PyLong_FromNativeBytes(bytes, sizeof(value), -1);
        }
#elif !CYTHON_COMPILING_IN_LIMITED_API && PY_VERSION_HEX < 0x030d0000
        int one = 1; int little = (int)*(unsigned char *)&one;
        return _PyLong_FromByteArray(bytes, sizeof(long),
                                     little, !is_unsigned);
#else
        int one = 1; int little = (int)*(unsigned char *)&one;
        PyObject *from_bytes, *result = NULL, *kwds = NULL;
        PyObject *py_bytes = NULL, *order_str = NULL;
        from_bytes = PyObject_GetAttrString((PyObject*)&PyLong_Type, "from_bytes");
        if (!from_bytes) return NULL;
        py_bytes = PyBytes_FromStringAndSize((char*)bytes, sizeof(long));
        if (!py_bytes) goto limited_bad;
        order_str = PyUnicode_FromString(little ? "little" : "big");
        if (!order_str) goto limited_bad;
        {
            PyObject *args[3+(CYTHON_VECTORCALL ? 1 : 0)] = { NULL, py_bytes, order_str };
            if (!is_unsigned) {
                kwds = __Pyx_MakeVectorcallBuilderKwds(1);
                if (!kwds) goto limited_bad;
                if (__Pyx_VectorcallBuilder_AddArgStr("signed", __Pyx_NewRef(Py_True), kwds, args+3, 0) < 0) goto limited_bad;
            }
            result = __Pyx_Object_Vectorcall_CallFromBuilder(from_bytes, args+1, 2 | __Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET, kwds);
        }
        limited_bad:
        Py_XDECREF(kwds);
        Py_XDECREF(order_str);
        Py_XDECREF(py_bytes);
        Py_XDECREF(from_bytes);
        return result;
#endif
    }
}

static CYTHON_INLINE PyObject* __Pyx_PyLong_From_int(int value) {
#ifdef __Pyx_HAS_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif
    const int neg_one = (int) -1, const_zero = (int) 0;
#ifdef __Pyx_HAS_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif
    const int is_unsigned = neg_one > const_zero;
    if (is_unsigned) {
        if (sizeof(int) < sizeof(long)) {
            return PyLong_FromLong((long) value);
        } else if (sizeof(int) <= sizeof(unsigned long)) {
            return PyLong_FromUnsignedLong((unsigned long) value);
#if !CYTHON_COMPILING_IN_PYPY
        } else if (sizeof(int) <= sizeof(unsigned PY_LONG_LONG)) {
            return PyLong_FromUnsignedLongLong((unsigned PY_LONG_LONG) value);
#endif
        }
    } else {
        if (sizeof(int) <= sizeof(long)) {
            return PyLong_FromLong((long) value);
        } else if (sizeof(int) <= sizeof(PY_LONG_LONG)) {
            return PyLong_FromLongLong((PY_LONG_LONG) value);
        }
    }
    {
        unsigned char *bytes = (unsigned char *)&value;
#if !CYTHON_COMPILING_IN_LIMITED_API && PY_VERSION_HEX >= 0x030d00A4
        if (is_unsigned) {
            return PyLong_FromUnsignedNativeBytes(bytes, sizeof(value), -1);
        } else {
            return PyLong_FromNativeBytes(bytes, sizeof(value), -1);
        }
#elif !CYTHON_COMPILING_IN_LIMITED_API && PY_VERSION_HEX < 0x030d0000
        int one = 1; int little = (int)*(unsigned char *)&one;
        return _PyLong_FromByteArray(bytes, sizeof(int),
                                     little, !is_unsigned);
#else
        int one = 1; int little = (int)*(unsigned char *)&one;
        PyObject *from_bytes, *result = NULL, *kwds = NULL;
        PyObject *py_bytes = NULL, *order_str = NULL;
        from_bytes = PyObject_GetAttrString((PyObject*)&PyLong_Type, "from_bytes");
        if (!from_bytes) return NULL;
        py_bytes = PyBytes_FromStringAndSize((char*)bytes, sizeof(int));
        if (!py_bytes) goto limited_bad;
        order_str = PyUnicode_FromString(little ? "little" : "big");
        if (!order_str) goto limited_bad;
        {
            PyObject *args[3+(CYTHON_VECTORCALL ? 1 : 0)] = { NULL, py_bytes, order_str };
            if (!is_unsigned) {
                kwds = __Pyx_MakeVectorcallBuilderKwds(1);
                if (!kwds) goto limited_bad;
                if (__Pyx_VectorcallBuilder_AddArgStr("signed", __Pyx_NewRef(Py_True), kwds, args+3, 0) < 0) goto limited_bad;
            }
            result = __Pyx_Object_Vectorcall_CallFromBuilder(from_bytes, args+1, 2 | __Pyx_PY_VECTORCALL_ARGUMENTS_OFFSET, kwds);
        }
        limited_bad:
        Py_XDECREF(kwds);
        Py_XDECREF(order_str);
        Py_XDECREF(py_bytes);
        Py_XDECREF(from_bytes);
        return result;
#endif
    }
}

#if CYTHON_COMPILING_IN_LIMITED_API && __PYX_LIMITED_VERSION_HEX < 0x030d0000
static __Pyx_TypeName
__Pyx_PyType_GetFullyQualifiedName(PyTypeObject* tp)
{
    PyObject *module = NULL, *name = NULL, *result = NULL;
    #if __PYX_LIMITED_VERSION_HEX < 0x030b0000
    name = __Pyx_PyObject_GetAttrStr((PyObject *)tp,
                                               __pyx_mstate_global->__pyx_n_u_qualname);
    #else
    name = PyType_GetQualName(tp);
    #endif
    if (unlikely(name == NULL) || unlikely(!PyUnicode_Check(name))) goto bad;
    module = __Pyx_PyObject_GetAttrStr((PyObject *)tp,
                                               __pyx_mstate_global->__pyx_n_u_module);
    if (unlikely(module == NULL) || unlikely(!PyUnicode_Check(module))) goto bad;
    if (PyUnicode_CompareWithASCIIString(module, "builtins") == 0) {
        result = name;
        name = NULL;
        goto done;
    }
    result = PyUnicode_FromFormat("%U.%U", module, name);
    if (unlikely(result == NULL)) goto bad;
  done:
    Py_XDECREF(name);
    Py_XDECREF(module);
    return result;
  bad:
    PyErr_Clear();
    if (name) {
        result = name;
        name = NULL;
    } else {
        result = __Pyx_NewRef(__pyx_mstate_global->__pyx_kp_u__6);
    }
    goto done;
}
#endif

static CYTHON_INLINE long __Pyx_PyLong_As_long(PyObject *x) {
#ifdef __Pyx_HAS_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif
    const long neg_one = (long) -1, const_zero = (long) 0;
#ifdef __Pyx_HAS_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif
    const int is_unsigned = neg_one > const_zero;
    if (unlikely(!PyLong_Check(x))) {
        long val;
        PyObject *tmp = __Pyx_PyNumber_Long(x);
        if (!tmp) return (long) -1;
        val = __Pyx_PyLong_As_long(tmp);
        Py_DECREF(tmp);
        return val;
    }
    if (is_unsigned) {
#if CYTHON_USE_PYLONG_INTERNALS
        if (unlikely(__Pyx_PyLong_IsNeg(x))) {
            goto raise_neg_overflow;
        } else if (__Pyx_PyLong_IsCompact(x)) {
            __PYX_VERIFY_RETURN_INT(long, __Pyx_compact_upylong, __Pyx_PyLong_CompactValueUnsigned(x))
        } else {
            const digit* digits = __Pyx_PyLong_Digits(x);
            assert(__Pyx_PyLong_DigitCount(x) > 1);
            switch (__Pyx_PyLong_DigitCount(x)) {
                case 2:
                    if ((8 * sizeof(long) > 1 * PyLong_SHIFT)) {
                        if ((8 * sizeof(unsigned long) > 2 * PyLong_SHIFT)) {
                            __PYX_VERIFY_RETURN_INT(long, unsigned long, (((((unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0])))
                        } else if ((8 * sizeof(long) >= 2 * PyLong_SHIFT)) {
                            return (long) (((((long)digits[1]) << PyLong_SHIFT) | (long)digits[0]));
                        }
                    }
                    break;
                case 3:
                    if ((8 * sizeof(long) > 2 * PyLong_SHIFT)) {
                        if ((8 * sizeof(unsigned long) > 3 * PyLong_SHIFT)) {
                            __PYX_VERIFY_RETURN_INT(long, unsigned long, (((((((unsigned long)digits[2]) << PyLong_SHIFT) | (unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0])))
                        } else if ((8 * sizeof(long) >= 3 * PyLong_SHIFT)) {
                            return (long) (((((((long)digits[2]) << PyLong_SHIFT) | (long)digits[1]) << PyLong_SHIFT) | (long)digits[0]));
                        }
                    }
                    break;
                case 4:
                    if ((8 * sizeof(long) > 3 * PyLong_SHIFT)) {
                        if ((8 * sizeof(unsigned long) > 4 * PyLong_SHIFT)) {
                            __PYX_VERIFY_RETURN_INT(long, unsigned long, (((((((((unsigned long)digits[3]) << PyLong_SHIFT) | (unsigned long)digits[2]) << PyLong_SHIFT) | (unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0])))
                        } else if ((8 * sizeof(long) >= 4 * PyLong_SHIFT)) {
                            return (long) (((((((((long)digits[3]) << PyLong_SHIFT) | (long)digits[2]) << PyLong_SHIFT) | (long)digits[1]) << PyLong_SHIFT) | (long)digits[0]));
                        }
                    }
                    break;
            }
        }
#endif
#if CYTHON_COMPILING_IN_CPYTHON && PY_VERSION_HEX < 0x030C00A7
        if (unlikely(Py_SIZE(x) < 0)) {
            goto raise_neg_overflow;
        }
#else
        {
            int result = PyObject_RichCompareBool(x, Py_False, Py_LT);
            if (unlikely(result < 0))
                return (long) -1;
            if (unlikely(result == 1))
                goto raise_neg_overflow;
        }
#endif
        if ((sizeof(long) <= sizeof(unsigned long))) {
            __PYX_VERIFY_RETURN_INT_EXC(long, unsigned long, PyLong_AsUnsignedLong(x))
        } else if ((sizeof(long) <= sizeof(unsigned PY_LONG_LONG))) {
            __PYX_VERIFY_RETURN_INT_EXC(long, unsigned PY_LONG_LONG, PyLong_AsUnsignedLongLong(x))
        }
    } else {
#if CYTHON_USE_PYLONG_INTERNALS
        if (__Pyx_PyLong_IsCompact(x)) {
            __PYX_VERIFY_RETURN_INT(long, __Pyx_compact_pylong, __Pyx_PyLong_CompactValue(x))
        } else {
            const digit* digits = __Pyx_PyLong_Digits(x);
            assert(__Pyx_PyLong_DigitCount(x) > 1);
            switch (__Pyx_PyLong_SignedDigitCount(x)) {
                case -2:
                    if ((8 * sizeof(long) - 1 > 1 * PyLong_SHIFT)) {
                        if ((8 * sizeof(unsigned long) > 2 * PyLong_SHIFT)) {
                            __PYX_VERIFY_RETURN_INT(long, long, -(long) (((((unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0])))
                        } else if ((8 * sizeof(long) - 1 > 2 * PyLong_SHIFT)) {
                            return (long) (((long)-1)*(((((long)digits[1]) << PyLong_SHIFT) | (long)digits[0])));
                        }
                    }
                    break;
                case 2:
                    if ((8 * sizeof(long) > 1 * PyLong_SHIFT)) {
                        if ((8 * sizeof(unsigned long) > 2 * PyLong_SHIFT)) {
                            __PYX_VERIFY_RETURN_INT(long, unsigned long, (((((unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0])))
                        } else if ((8 * sizeof(long) - 1 > 2 * PyLong_SHIFT)) {
                            return (long) ((((((long)digits[1]) << PyLong_SHIFT) | (long)digits[0])));
                        }
                    }
                    break;
                case -3:
                    if ((8 * sizeof(long) - 1 > 2 * PyLong_SHIFT)) {
                        if ((8 * sizeof(unsigned long) > 3 * PyLong_SHIFT)) {
                            __PYX_VERIFY_RETURN_INT(long, long, -(long) (((((((unsigned long)digits[2]) << PyLong_SHIFT) | (unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0])))
                        } else if ((8 * sizeof(long) - 1 > 3 * PyLong_SHIFT)) {
                            return (long) (((long)-1)*(((((((long)digits[2]) << PyLong_SHIFT) | (long)digits[1]) << PyLong_SHIFT) | (long)digits[0])));
                        }
                    }
                    break;
                case 3:
                    if ((8 * sizeof(long) > 2 * PyLong_SHIFT)) {
                        if ((8 * sizeof(unsigned long) > 3 * PyLong_SHIFT)) {
                            __PYX_VERIFY_RETURN_INT(long, unsigned long, (((((((unsigned long)digits[2]) << PyLong_SHIFT) | (unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0])))
                        } else if ((8 * sizeof(long) - 1 > 3 * PyLong_SHIFT)) {
                            return (long) ((((((((long)digits[2]) << PyLong_SHIFT) | (long)digits[1]) << PyLong_SHIFT) | (long)digits[0])));
                        }
                    }
                    break;
                case -4:
                    if ((8 * sizeof(long) - 1 > 3 * PyLong_SHIFT)) {
                        if ((8 * sizeof(unsigned long) > 4 * PyLong_SHIFT)) {
                            __PYX_VERIFY_RETURN_INT(long, long, -(long) (((((((((unsigned long)digits[3]) << PyLong_SHIFT) | (unsigned long)digits[2]) << PyLong_SHIFT) | (unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0])))
                        } else if ((8 * sizeof(long) - 1 > 4 * PyLong_SHIFT)) {
                            return (long) (((long)-1)*(((((((((long)digits[3]) << PyLong_SHIFT) | (long)digits[2]) << PyLong_SHIFT) | (long)digits[1]) << PyLong_SHIFT) | (long)digits[0])));
                        }
                    }
                    break;
                case 4:
                    if ((8 * sizeof(long) > 3 * PyLong_SHIFT)) {
                        if ((8 * sizeof(unsigned long) > 4 * PyLong_SHIFT)) {
                            __PYX_VERIFY_RETURN_INT(long, unsigned long, (((((((((unsigned long)digits[3]) << PyLong_SHIFT) | (unsigned long)digits[2]) << PyLong_SHIFT) | (unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0])))
                        } else if ((8 * sizeof(long) - 1 > 4 * PyLong_SHIFT)) {
                            return (long) ((((((((((long)digits[3]) << PyLong_SHIFT) | (long)digits[2]) << PyLong_SHIFT) | (long)digits[1]) << PyLong_SHIFT) | (long)digits[0])));
                        }
                    }
                    break;
            }
        }
#endif
        if ((sizeof(long) <= sizeof(long))) {
            __PYX_VERIFY_RETURN_INT_EXC(long, long, PyLong_AsLong(x))
        } else if ((sizeof(long) <= sizeof(PY_LONG_LONG))) {
            __PYX_VERIFY_RETURN_INT_EXC(long, PY_LONG_LONG, PyLong_AsLongLong(x))
        }
    }
    {
        long val;
        int ret = -1;
#if PY_VERSION_HEX >= 0x030d00A6 && !CYTHON_COMPILING_IN_LIMITED_API
        Py_ssize_t bytes_copied = PyLong_AsNativeBytes(
            x, &val, sizeof(val), Py_ASNATIVEBYTES_NATIVE_ENDIAN | (is_unsigned ? Py_ASNATIVEBYTES_UNSIGNED_BUFFER | Py_ASNATIVEBYTES_REJECT_NEGATIVE : 0));
        if (unlikely(bytes_copied == -1)) {
        } else if (unlikely(bytes_copied > (Py_ssize_t) sizeof(val))) {
            goto raise_overflow;
        } else {
            ret = 0;
        }
#elif PY_VERSION_HEX < 0x030d0000 && !(CYTHON_COMPILING_IN_PYPY || CYTHON_COMPILING_IN_LIMITED_API) || defined(_PyLong_AsByteArray)
        int one = 1; int is_little = (int)*(unsigned char *)&one;
        unsigned char *bytes = (unsigned char *)&val;
        ret = _PyLong_AsByteArray((PyLongObject *)x,
                                    bytes, sizeof(val),
                                    is_little, !is_unsigned);
#else
        PyObject *v;
        PyObject *stepval = NULL, *mask = NULL, *shift = NULL;
        int bits, remaining_bits, is_negative = 0;
        int chunk_size = (sizeof(long) < 8) ? 30 : 62;
        if (likely(PyLong_CheckExact(x))) {
            v = __Pyx_NewRef(x);
        } else {
            v = PyNumber_Long(x);
            if (unlikely(!v)) return (long) -1;
            assert(PyLong_CheckExact(v));
        }
        {
            int result = PyObject_RichCompareBool(v, Py_False, Py_LT);
            if (unlikely(result < 0)) {
                Py_DECREF(v);
                return (long) -1;
            }
            is_negative = result == 1;
        }
        if (is_unsigned && unlikely(is_negative)) {
            Py_DECREF(v);
            goto raise_neg_overflow;
        } else if (is_negative) {
            stepval = PyNumber_Invert(v);
            Py_DECREF(v);
            if (unlikely(!stepval))
                return (long) -1;
        } else {
            stepval = v;
        }
        v = NULL;
        val = (long) 0;
        mask = PyLong_FromLong((1L << chunk_size) - 1); if (unlikely(!mask)) goto done;
        shift = PyLong_FromLong(chunk_size); if (unlikely(!shift)) goto done;
        for (bits = 0; bits < (int) sizeof(long) * 8 - chunk_size; bits += chunk_size) {
            PyObject *tmp, *digit;
            long idigit;
            digit = PyNumber_And(stepval, mask);
            if (unlikely(!digit)) goto done;
            idigit = PyLong_AsLong(digit);
            Py_DECREF(digit);
            if (unlikely(idigit < 0)) goto done;
            val |= ((long) idigit) << bits;
            tmp = PyNumber_Rshift(stepval, shift);
            if (unlikely(!tmp)) goto done;
            Py_DECREF(stepval); stepval = tmp;
        }
        Py_DECREF(shift); shift = NULL;
        Py_DECREF(mask); mask = NULL;
        {
            long idigit = PyLong_AsLong(stepval);
            if (unlikely(idigit < 0)) goto done;
            remaining_bits = ((int) sizeof(long) * 8) - bits - (is_unsigned ? 0 : 1);
            if (unlikely(idigit >= (1L << remaining_bits)))
                goto raise_overflow;
            val |= ((long) idigit) << bits;
        }
        if (!is_unsigned) {
            if (unlikely(val & (((long) 1) << (sizeof(long) * 8 - 1))))
                goto raise_overflow;
            if (is_negative)
                val = ~val;
        }
        ret = 0;
    done:
        Py_XDECREF(shift);
        Py_XDECREF(mask);
        Py_XDECREF(stepval);
#endif
        if (unlikely(ret))
            return (long) -1;
        return val;
    }
raise_overflow:
    PyErr_SetString(PyExc_OverflowError,
        "value too large to convert to long");
    return (long) -1;
raise_neg_overflow:
    PyErr_SetString(PyExc_OverflowError,
        "can't convert negative value to long");
    return (long) -1;
}

#if __PYX_LIMITED_VERSION_HEX < 0x030b0000
void __Pyx_init_runtime_version(void) {
    if (__Pyx_cached_runtime_version == 0) {
        const char* rt_version = Py_GetVersion();
        unsigned long version = 0;
        unsigned long factor = 0x01000000UL;
        unsigned int digit = 0;
        int i = 0;
        while (factor) {
            while ('0' <= rt_version[i] && rt_version[i] <= '9') {
                digit = digit * 10 + (unsigned int) (rt_version[i] - '0');
                ++i;
            }
            version += factor * digit;
            if (rt_version[i] != '.')
                break;
            digit = 0;
            factor >>= 8;
            ++i;
        }
        __Pyx_cached_runtime_version = version;
    }
}
#endif
static unsigned long __Pyx_get_runtime_version(void) {
#if __PYX_LIMITED_VERSION_HEX >= 0x030b0000
    return Py_Version & ~0xFFUL;
#else
    return __Pyx_cached_runtime_version;
#endif
}

static int __Pyx_check_binary_version(unsigned long ct_version, unsigned long rt_version, int allow_newer) {
    const unsigned long MAJOR_MINOR = 0xFFFF0000UL;
    if ((rt_version & MAJOR_MINOR) == (ct_version & MAJOR_MINOR))
        return 0;
    if (likely(allow_newer && (rt_version & MAJOR_MINOR) > (ct_version & MAJOR_MINOR)))
        return 1;
    {
        char message[200];
        PyOS_snprintf(message, sizeof(message),
                      "compile time Python version %d.%d "
                      "of module '%.100s' "
                      "%s "
                      "runtime version %d.%d",
                       (int) (ct_version >> 24), (int) ((ct_version >> 16) & 0xFF),
                       __Pyx_MODULE_NAME,
                       (allow_newer) ? "was newer than" : "does not match",
                       (int) (rt_version >> 24), (int) ((rt_version >> 16) & 0xFF)
       );
        return PyErr_WarnEx(NULL, message, 1);
    }
}

#if CYTHON_COMPILING_IN_LIMITED_API
    static PyObject* __Pyx__PyCode_New(int a, int p, int k, int l, int s, int f,
                                       PyObject *code, PyObject *c, PyObject* n, PyObject *v,
                                       PyObject *fv, PyObject *cell, PyObject* fn,
                                       PyObject *name, int fline, PyObject *lnos) {
        PyObject *exception_table = NULL;
        PyObject *types_module=NULL, *code_type=NULL, *result=NULL;
        #if __PYX_LIMITED_VERSION_HEX < 0x030b0000
        PyObject *version_info;
        PyObject *py_minor_version = NULL;
        #endif
        long minor_version = 0;
        PyObject *type, *value, *traceback;
        PyErr_Fetch(&type, &value, &traceback);
        #if __PYX_LIMITED_VERSION_HEX >= 0x030b0000
        minor_version = 11;
        #else
        if (!(version_info = PySys_GetObject("version_info"))) goto end;
        if (!(py_minor_version = PySequence_GetItem(version_info, 1))) goto end;
        minor_version = PyLong_AsLong(py_minor_version);
        Py_DECREF(py_minor_version);
        if (minor_version == -1 && PyErr_Occurred()) goto end;
        #endif
        if (!(types_module = PyImport_ImportModule("types"))) goto end;
        if (!(code_type = PyObject_GetAttrString(types_module, "CodeType"))) goto end;
        if (minor_version <= 7) {
            (void)p;
            result = PyObject_CallFunction(code_type, "iiiiiOOOOOOiOOO", a, k, l, s, f, code,
                          c, n, v, fn, name, fline, lnos, fv, cell);
        } else if (minor_version <= 10) {
            result = PyObject_CallFunction(code_type, "iiiiiiOOOOOOiOOO", a,p, k, l, s, f, code,
                          c, n, v, fn, name, fline, lnos, fv, cell);
        } else {
            if (!(exception_table = PyBytes_FromStringAndSize(NULL, 0))) goto end;
            result = PyObject_CallFunction(code_type, "iiiiiiOOOOOOOiOOOO", a,p, k, l, s, f, code,
                          c, n, v, fn, name, name, fline, lnos, exception_table, fv, cell);
        }
    end:
        Py_XDECREF(code_type);
        Py_XDECREF(exception_table);
        Py_XDECREF(types_module);
        if (type) {
            PyErr_Restore(type, value, traceback);
        }
        return result;
    }
#elif PY_VERSION_HEX >= 0x030B0000
  static PyCodeObject* __Pyx__PyCode_New(int a, int p, int k, int l, int s, int f,
                                         PyObject *code, PyObject *c, PyObject* n, PyObject *v,
                                         PyObject *fv, PyObject *cell, PyObject* fn,
                                         PyObject *name, int fline, PyObject *lnos) {
    PyCodeObject *result;
    result =
      #if PY_VERSION_HEX >= 0x030C0000
        PyUnstable_Code_NewWithPosOnlyArgs
      #else
        PyCode_NewWithPosOnlyArgs
      #endif
        (a, p, k, l, s, f, code, c, n, v, fv, cell, fn, name, name, fline, lnos, __pyx_mstate_global->__pyx_empty_bytes);
    #if CYTHON_COMPILING_IN_CPYTHON && PY_VERSION_HEX >= 0x030c00A1
    if (likely(result))
        result->_co_firsttraceable = 0;
    #endif
    return result;
  }
#elif !CYTHON_COMPILING_IN_PYPY
  #define __Pyx__PyCode_New(a, p, k, l, s, f, code, c, n, v, fv, cell, fn, name, fline, lnos)\
          PyCode_NewWithPosOnlyArgs(a, p, k, l, s, f, code, c, n, v, fv, cell, fn, name, fline, lnos)
#else
  #define __Pyx__PyCode_New(a, p, k, l, s, f, code, c, n, v, fv, cell, fn, name, fline, lnos)\
          PyCode_New(a, k, l, s, f, code, c, n, v, fv, cell, fn, name, fline, lnos)
#endif
static PyObject* __Pyx_PyCode_New(
        const __Pyx_PyCode_New_function_description descr,
        PyObject * const *varnames,
        PyObject *filename,
        PyObject *funcname,
        PyObject *line_table,
        PyObject *tuple_dedup_map
) {
    PyObject *code_obj = NULL, *varnames_tuple_dedup = NULL, *code_bytes = NULL;
    Py_ssize_t var_count = (Py_ssize_t) descr.nlocals;
    PyObject *varnames_tuple = PyTuple_New(var_count);
    if (unlikely(!varnames_tuple)) return NULL;
    for (Py_ssize_t i=0; i < var_count; i++) {
        Py_INCREF(varnames[i]);
        if (__Pyx_PyTuple_SET_ITEM(varnames_tuple, i, varnames[i]) != (0)) goto done;
    }
    #if CYTHON_COMPILING_IN_LIMITED_API
    varnames_tuple_dedup = PyDict_GetItem(tuple_dedup_map, varnames_tuple);
    if (!varnames_tuple_dedup) {
        if (unlikely(PyDict_SetItem(tuple_dedup_map, varnames_tuple, varnames_tuple) < 0)) goto done;
        varnames_tuple_dedup = varnames_tuple;
    }
    #else
    varnames_tuple_dedup = PyDict_SetDefault(tuple_dedup_map, varnames_tuple, varnames_tuple);
    if (unlikely(!varnames_tuple_dedup)) goto done;
    #endif
    #if CYTHON_AVOID_BORROWED_REFS
    Py_INCREF(varnames_tuple_dedup);
    #endif
    if (__PYX_LIMITED_VERSION_HEX >= (0x030b0000) && line_table != NULL && !CYTHON_COMPILING_IN_GRAAL) {
        Py_ssize_t line_table_length = __Pyx_PyBytes_GET_SIZE(line_table);
        #if !CYTHON_ASSUME_SAFE_SIZE
        if (unlikely(line_table_length == -1)) goto done;
        #endif
        Py_ssize_t code_len = (line_table_length * 2 + 4) & ~3LL;
        code_bytes = PyBytes_FromStringAndSize(NULL, code_len);
        if (unlikely(!code_bytes)) goto done;
        char* c_code_bytes = PyBytes_AsString(code_bytes);
        if (unlikely(!c_code_bytes)) goto done;
        memset(c_code_bytes, 0, (size_t) code_len);
    }
    code_obj = (PyObject*) __Pyx__PyCode_New(
        (int) descr.argcount,
        (int) descr.num_posonly_args,
        (int) descr.num_kwonly_args,
        (int) descr.nlocals,
        0,
        (int) descr.flags,
        code_bytes ? code_bytes : __pyx_mstate_global->__pyx_empty_bytes,
        __pyx_mstate_global->__pyx_empty_tuple,
        __pyx_mstate_global->__pyx_empty_tuple,
        varnames_tuple_dedup,
        __pyx_mstate_global->__pyx_empty_tuple,
        __pyx_mstate_global->__pyx_empty_tuple,
        filename,
        funcname,
        (int) descr.first_line,
        (__PYX_LIMITED_VERSION_HEX >= (0x030b0000) && line_table) ? line_table : __pyx_mstate_global->__pyx_empty_bytes
    );
done:
    Py_XDECREF(code_bytes);
    #if CYTHON_AVOID_BORROWED_REFS
    Py_XDECREF(varnames_tuple_dedup);
    #endif
    Py_DECREF(varnames_tuple);
    return code_obj;
}

static PyObject *__Pyx_DecompressString(const char *s, Py_ssize_t length, int algo) {
    PyObject *module = NULL, *decompress, *compressed_bytes, *decompressed;
    const char* module_name = algo == 3 ? "compression.zstd" : algo == 2 ? "bz2" : "zlib";
    PyObject *methodname = PyUnicode_FromString("decompress");
    if (unlikely(!methodname)) return NULL;
    #if __PYX_LIMITED_VERSION_HEX >= 0x030e0000
    if (algo == 3) {
        PyObject *fromlist = Py_BuildValue("[O]", methodname);
        if (unlikely(!fromlist)) goto bad;
        module = PyImport_ImportModuleLevel("compression.zstd", NULL, NULL, fromlist, 0);
        Py_DECREF(fromlist);
    } else
    #endif
        module = PyImport_ImportModule(module_name);
    if (unlikely(!module)) goto import_failed;
    decompress = PyObject_GetAttr(module, methodname);
    if (unlikely(!decompress)) goto import_failed;
    {
        #ifdef __cplusplus
            char *memview_bytes = const_cast<char*>(s);
        #else
            #if defined(__clang__)
              #pragma clang diagnostic push
              #pragma clang diagnostic ignored "-Wcast-qual"
            #elif !defined(__INTEL_COMPILER) && defined(__GNUC__)
              #pragma GCC diagnostic push
              #pragma GCC diagnostic ignored "-Wcast-qual"
            #endif
            char *memview_bytes = (char*) s;
            #if defined(__clang__)
              #pragma clang diagnostic pop
            #elif !defined(__INTEL_COMPILER) && defined(__GNUC__)
              #pragma GCC diagnostic pop
            #endif
        #endif
        #if CYTHON_COMPILING_IN_LIMITED_API && !defined(PyBUF_READ)
        int memview_flags = 0x100;
        #else
        int memview_flags = PyBUF_READ;
        #endif
        compressed_bytes = PyMemoryView_FromMemory(memview_bytes, length, memview_flags);
    }
    if (unlikely(!compressed_bytes)) {
        Py_DECREF(decompress);
        goto bad;
    }
    decompressed = PyObject_CallFunctionObjArgs(decompress, compressed_bytes, NULL);
    Py_DECREF(compressed_bytes);
    Py_DECREF(decompress);
    Py_DECREF(module);
    Py_DECREF(methodname);
    return decompressed;
import_failed:
    PyErr_Format(PyExc_ImportError,
        "Failed to import '%.20s.decompress' - cannot initialise module strings. "
        "String compression was configured with the C macro 'CYTHON_COMPRESS_STRINGS=%d'.",
        module_name, algo);
bad:
    Py_XDECREF(module);
    Py_DECREF(methodname);
    return NULL;
}

#include <string.h>
static CYTHON_INLINE Py_ssize_t __Pyx_ssize_strlen(const char *s) {
    size_t len = strlen(s);
    if (unlikely(len > (size_t) PY_SSIZE_T_MAX)) {
        PyErr_SetString(PyExc_OverflowError, "byte string is too long");
        return -1;
    }
    return (Py_ssize_t) len;
}
static CYTHON_INLINE PyObject* __Pyx_PyUnicode_FromString(const char* c_str) {
    Py_ssize_t len = __Pyx_ssize_strlen(c_str);
    if (unlikely(len < 0)) return NULL;
    return __Pyx_PyUnicode_FromStringAndSize(c_str, len);
}
static CYTHON_INLINE PyObject* __Pyx_PyByteArray_FromString(const char* c_str) {
    Py_ssize_t len = __Pyx_ssize_strlen(c_str);
    if (unlikely(len < 0)) return NULL;
    return PyByteArray_FromStringAndSize(c_str, len);
}
static CYTHON_INLINE const char* __Pyx_PyObject_AsString(PyObject* o) {
    Py_ssize_t ignore;
    return __Pyx_PyObject_AsStringAndSize(o, &ignore);
}
#if __PYX_DEFAULT_STRING_ENCODING_IS_ASCII || __PYX_DEFAULT_STRING_ENCODING_IS_UTF8
static CYTHON_INLINE const char* __Pyx_PyUnicode_AsStringAndSize(PyObject* o, Py_ssize_t *length) {
    if (unlikely(__Pyx_PyUnicode_READY(o) == -1)) return NULL;
#if CYTHON_COMPILING_IN_LIMITED_API
    {
        const char* result;
        Py_ssize_t unicode_length;
        CYTHON_MAYBE_UNUSED_VAR(unicode_length);
        #if __PYX_LIMITED_VERSION_HEX < 0x030A0000
        if (unlikely(PyArg_Parse(o, "s#", &result, length) < 0)) return NULL;
        #else
        result = PyUnicode_AsUTF8AndSize(o, length);
        #endif
        #if __PYX_DEFAULT_STRING_ENCODING_IS_ASCII
        unicode_length = PyUnicode_GetLength(o);
        if (unlikely(unicode_length < 0)) return NULL;
        if (unlikely(unicode_length != *length)) {
            PyUnicode_AsASCIIString(o);
            return NULL;
        }
        #endif
        return result;
    }
#else
#if __PYX_DEFAULT_STRING_ENCODING_IS_ASCII
    if (likely(PyUnicode_IS_ASCII(o))) {
        *length = PyUnicode_GET_LENGTH(o);
        return PyUnicode_AsUTF8(o);
    } else {
        PyUnicode_AsASCIIString(o);
        return NULL;
    }
#else
    return PyUnicode_AsUTF8AndSize(o, length);
#endif
#endif
}
#endif
static CYTHON_INLINE const char* __Pyx_PyObject_AsStringAndSize(PyObject* o, Py_ssize_t *length) {
#if __PYX_DEFAULT_STRING_ENCODING_IS_ASCII || __PYX_DEFAULT_STRING_ENCODING_IS_UTF8
    if (PyUnicode_Check(o)) {
        return __Pyx_PyUnicode_AsStringAndSize(o, length);
    } else
#endif
    if (PyByteArray_Check(o)) {
#if (CYTHON_ASSUME_SAFE_SIZE && CYTHON_ASSUME_SAFE_MACROS) || (CYTHON_COMPILING_IN_PYPY && (defined(PyByteArray_AS_STRING) && defined(PyByteArray_GET_SIZE)))
        *length = PyByteArray_GET_SIZE(o);
        return PyByteArray_AS_STRING(o);
#else
        *length = PyByteArray_Size(o);
        if (*length == -1) return NULL;
        return PyByteArray_AsString(o);
#endif
    } else
    {
        char* result;
        int r = PyBytes_AsStringAndSize(o, &result, length);
        if (unlikely(r < 0)) {
            return NULL;
        } else {
            return result;
        }
    }
}
static CYTHON_INLINE int __Pyx_PyObject_IsTrue(PyObject* x) {
   int is_true = x == Py_True;
   if (is_true | (x == Py_False) | (x == Py_None)) return is_true;
   else return PyObject_IsTrue(x);
}
static CYTHON_INLINE int __Pyx_PyObject_IsTrueAndDecref(PyObject* x) {
    int retval;
    if (unlikely(!x)) return -1;
    retval = __Pyx_PyObject_IsTrue(x);
    Py_DECREF(x);
    return retval;
}
static PyObject* __Pyx_PyNumber_LongWrongResultType(PyObject* result) {
    __Pyx_TypeName result_type_name = __Pyx_PyType_GetFullyQualifiedName(Py_TYPE(result));
    if (PyLong_Check(result)) {
        if (PyErr_WarnFormat(PyExc_DeprecationWarning, 1,
                "__int__ returned non-int (type " __Pyx_FMT_TYPENAME ").  "
                "The ability to return an instance of a strict subclass of int is deprecated, "
                "and may be removed in a future version of Python.",
                result_type_name)) {
            __Pyx_DECREF_TypeName(result_type_name);
            Py_DECREF(result);
            return NULL;
        }
        __Pyx_DECREF_TypeName(result_type_name);
        return result;
    }
    PyErr_Format(PyExc_TypeError,
                 "__int__ returned non-int (type " __Pyx_FMT_TYPENAME ")",
                 result_type_name);
    __Pyx_DECREF_TypeName(result_type_name);
    Py_DECREF(result);
    return NULL;
}
static CYTHON_INLINE PyObject* __Pyx_PyNumber_Long(PyObject* x) {
#if CYTHON_USE_TYPE_SLOTS
  PyNumberMethods *m;
#endif
  PyObject *res = NULL;
  if (likely(PyLong_Check(x)))
      return __Pyx_NewRef(x);
#if CYTHON_USE_TYPE_SLOTS
  m = Py_TYPE(x)->tp_as_number;
  if (likely(m && m->nb_int)) {
      res = m->nb_int(x);
  }
#else
  if (!PyBytes_CheckExact(x) && !PyUnicode_CheckExact(x)) {
      res = PyNumber_Long(x);
  }
#endif
  if (likely(res)) {
      if (unlikely(!PyLong_CheckExact(res))) {
          return __Pyx_PyNumber_LongWrongResultType(res);
      }
  }
  else if (!PyErr_Occurred()) {
      PyErr_SetString(PyExc_TypeError,
                      "an integer is required");
  }
  return res;
}
static CYTHON_INLINE Py_ssize_t __Pyx_PyIndex_AsSsize_t(PyObject* b) {
  Py_ssize_t ival;
  PyObject *x;
  if (likely(PyLong_CheckExact(b))) {
    #if CYTHON_USE_PYLONG_INTERNALS
    if (likely(__Pyx_PyLong_IsCompact(b))) {
        return __Pyx_PyLong_CompactValue(b);
    } else {
      const digit* digits = __Pyx_PyLong_Digits(b);
      const Py_ssize_t size = __Pyx_PyLong_SignedDigitCount(b);
      switch (size) {
         case 2:
           if (8 * sizeof(Py_ssize_t) > 2 * PyLong_SHIFT) {
             return (Py_ssize_t) (((((size_t)digits[1]) << PyLong_SHIFT) | (size_t)digits[0]));
           }
           break;
         case -2:
           if (8 * sizeof(Py_ssize_t) > 2 * PyLong_SHIFT) {
             return -(Py_ssize_t) (((((size_t)digits[1]) << PyLong_SHIFT) | (size_t)digits[0]));
           }
           break;
         case 3:
           if (8 * sizeof(Py_ssize_t) > 3 * PyLong_SHIFT) {
             return (Py_ssize_t) (((((((size_t)digits[2]) << PyLong_SHIFT) | (size_t)digits[1]) << PyLong_SHIFT) | (size_t)digits[0]));
           }
           break;
         case -3:
           if (8 * sizeof(Py_ssize_t) > 3 * PyLong_SHIFT) {
             return -(Py_ssize_t) (((((((size_t)digits[2]) << PyLong_SHIFT) | (size_t)digits[1]) << PyLong_SHIFT) | (size_t)digits[0]));
           }
           break;
         case 4:
           if (8 * sizeof(Py_ssize_t) > 4 * PyLong_SHIFT) {
             return (Py_ssize_t) (((((((((size_t)digits[3]) << PyLong_SHIFT) | (size_t)digits[2]) << PyLong_SHIFT) | (size_t)digits[1]) << PyLong_SHIFT) | (size_t)digits[0]));
           }
           break;
         case -4:
           if (8 * sizeof(Py_ssize_t) > 4 * PyLong_SHIFT) {
             return -(Py_ssize_t) (((((((((size_t)digits[3]) << PyLong_SHIFT) | (size_t)digits[2]) << PyLong_SHIFT) | (size_t)digits[1]) << PyLong_SHIFT) | (size_t)digits[0]));
           }
           break;
      }
    }
    #endif
    return PyLong_AsSsize_t(b);
  }
  x = PyNumber_Index(b);
  if (!x) return -1;
  ival = PyLong_AsSsize_t(x);
  Py_DECREF(x);
  return ival;
}
static CYTHON_INLINE Py_hash_t __Pyx_PyIndex_AsHash_t(PyObject* o) {
  if (sizeof(Py_hash_t) == sizeof(Py_ssize_t)) {
    return (Py_hash_t) __Pyx_PyIndex_AsSsize_t(o);
  } else {
    Py_ssize_t ival;
    PyObject *x;
    x = PyNumber_Index(o);
    if (!x) return -1;
    ival = PyLong_AsLong(x);
    Py_DECREF(x);
    return ival;
  }
}
static CYTHON_INLINE PyObject *__Pyx_Owned_Py_None(int b) {
    CYTHON_UNUSED_VAR(b);
    return __Pyx_NewRef(Py_None);
}
static CYTHON_INLINE PyObject * __Pyx_PyBool_FromLong(long b) {
  return __Pyx_NewRef(b ? Py_True: Py_False);
}
static CYTHON_INLINE PyObject * __Pyx_PyLong_FromSize_t(size_t ival) {
    return PyLong_FromSize_t(ival);
}

#if CYTHON_PEP489_MULTI_PHASE_INIT && CYTHON_USE_MODULE_STATE
#ifndef CYTHON_MODULE_STATE_LOOKUP_THREAD_SAFE
#if (CYTHON_COMPILING_IN_LIMITED_API || PY_VERSION_HEX >= 0x030C0000)
  #define CYTHON_MODULE_STATE_LOOKUP_THREAD_SAFE 1
#else
  #define CYTHON_MODULE_STATE_LOOKUP_THREAD_SAFE 0
#endif
#endif
#if CYTHON_MODULE_STATE_LOOKUP_THREAD_SAFE && !CYTHON_ATOMICS
#error "Module state with PEP489 requires atomics. Currently that's one of\
 C11, C++11, gcc atomic intrinsics or MSVC atomic intrinsics"
#endif
#if !CYTHON_MODULE_STATE_LOOKUP_THREAD_SAFE
#define __Pyx_ModuleStateLookup_Lock()
#define __Pyx_ModuleStateLookup_Unlock()
#elif !CYTHON_COMPILING_IN_LIMITED_API && PY_VERSION_HEX >= 0x030d0000
static PyMutex __Pyx_ModuleStateLookup_mutex = {0};
#define __Pyx_ModuleStateLookup_Lock() PyMutex_Lock(&__Pyx_ModuleStateLookup_mutex)
#define __Pyx_ModuleStateLookup_Unlock() PyMutex_Unlock(&__Pyx_ModuleStateLookup_mutex)
#elif defined(__cplusplus) && __cplusplus >= 201103L
#include <mutex>
static std::mutex __Pyx_ModuleStateLookup_mutex;
#define __Pyx_ModuleStateLookup_Lock() __Pyx_ModuleStateLookup_mutex.lock()
#define __Pyx_ModuleStateLookup_Unlock() __Pyx_ModuleStateLookup_mutex.unlock()
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ > 201112L) && !defined(__STDC_NO_THREADS__)
#include <threads.h>
static mtx_t __Pyx_ModuleStateLookup_mutex;
static once_flag __Pyx_ModuleStateLookup_mutex_once_flag = ONCE_FLAG_INIT;
static void __Pyx_ModuleStateLookup_initialize_mutex(void) {
    mtx_init(&__Pyx_ModuleStateLookup_mutex, mtx_plain);
}
#define __Pyx_ModuleStateLookup_Lock()\
  call_once(&__Pyx_ModuleStateLookup_mutex_once_flag, __Pyx_ModuleStateLookup_initialize_mutex);\
  mtx_lock(&__Pyx_ModuleStateLookup_mutex)
#define __Pyx_ModuleStateLookup_Unlock() mtx_unlock(&__Pyx_ModuleStateLookup_mutex)
#elif defined(HAVE_PTHREAD_H)
#include <pthread.h>
static pthread_mutex_t __Pyx_ModuleStateLookup_mutex = PTHREAD_MUTEX_INITIALIZER;
#define __Pyx_ModuleStateLookup_Lock() pthread_mutex_lock(&__Pyx_ModuleStateLookup_mutex)
#define __Pyx_ModuleStateLookup_Unlock() pthread_mutex_unlock(&__Pyx_ModuleStateLookup_mutex)
#elif defined(_WIN32)
#include <Windows.h>
static SRWLOCK __Pyx_ModuleStateLookup_mutex = SRWLOCK_INIT;
#define __Pyx_ModuleStateLookup_Lock() AcquireSRWLockExclusive(&__Pyx_ModuleStateLookup_mutex)
#define __Pyx_ModuleStateLookup_Unlock() ReleaseSRWLockExclusive(&__Pyx_ModuleStateLookup_mutex)
#else
#error "No suitable lock available for CYTHON_MODULE_STATE_LOOKUP_THREAD_SAFE.\
 Requires C standard >= C11, or C++ standard >= C++11,\
 or pthreads, or the Windows 32 API, or Python >= 3.13."
#endif
typedef struct {
    int64_t id;
    PyObject *module;
} __Pyx_InterpreterIdAndModule;
typedef struct {
    char interpreter_id_as_index;
    Py_ssize_t count;
    Py_ssize_t allocated;
    __Pyx_InterpreterIdAndModule table[1];
} __Pyx_ModuleStateLookupData;
#define __PYX_MODULE_STATE_LOOKUP_SMALL_SIZE 32
#if CYTHON_MODULE_STATE_LOOKUP_THREAD_SAFE
static __pyx_atomic_int_type __Pyx_ModuleStateLookup_read_counter = 0;
#endif
#if CYTHON_MODULE_STATE_LOOKUP_THREAD_SAFE
static __pyx_atomic_ptr_type __Pyx_ModuleStateLookup_data = 0;
#else
static __Pyx_ModuleStateLookupData* __Pyx_ModuleStateLookup_data = NULL;
#endif
static __Pyx_InterpreterIdAndModule* __Pyx_State_FindModuleStateLookupTableLowerBound(
        __Pyx_InterpreterIdAndModule* table,
        Py_ssize_t count,
        int64_t interpreterId) {
    __Pyx_InterpreterIdAndModule* begin = table;
    __Pyx_InterpreterIdAndModule* end = begin + count;
    if (begin->id == interpreterId) {
        return begin;
    }
    while ((end - begin) > __PYX_MODULE_STATE_LOOKUP_SMALL_SIZE) {
        __Pyx_InterpreterIdAndModule* halfway = begin + (end - begin)/2;
        if (halfway->id == interpreterId) {
            return halfway;
        }
        if (halfway->id < interpreterId) {
            begin = halfway;
        } else {
            end = halfway;
        }
    }
    for (; begin < end; ++begin) {
        if (begin->id >= interpreterId) return begin;
    }
    return begin;
}
static PyObject *__Pyx_State_FindModule(CYTHON_UNUSED void* dummy) {
    int64_t interpreter_id = PyInterpreterState_GetID(__Pyx_PyInterpreterState_Get());
    if (interpreter_id == -1) return NULL;
#if CYTHON_MODULE_STATE_LOOKUP_THREAD_SAFE
    __Pyx_ModuleStateLookupData* data = (__Pyx_ModuleStateLookupData*)__pyx_atomic_pointer_load_relaxed(&__Pyx_ModuleStateLookup_data);
    {
        __pyx_atomic_incr_acq_rel(&__Pyx_ModuleStateLookup_read_counter);
        if (likely(data)) {
            __Pyx_ModuleStateLookupData* new_data = (__Pyx_ModuleStateLookupData*)__pyx_atomic_pointer_load_acquire(&__Pyx_ModuleStateLookup_data);
            if (likely(data == new_data)) {
                goto read_finished;
            }
        }
        __pyx_atomic_decr_acq_rel(&__Pyx_ModuleStateLookup_read_counter);
        __Pyx_ModuleStateLookup_Lock();
        __pyx_atomic_incr_relaxed(&__Pyx_ModuleStateLookup_read_counter);
        data = (__Pyx_ModuleStateLookupData*)__pyx_atomic_pointer_load_relaxed(&__Pyx_ModuleStateLookup_data);
        __Pyx_ModuleStateLookup_Unlock();
    }
  read_finished:;
#else
    __Pyx_ModuleStateLookupData* data = __Pyx_ModuleStateLookup_data;
#endif
    __Pyx_InterpreterIdAndModule* found = NULL;
    if (unlikely(!data)) goto end;
    if (data->interpreter_id_as_index) {
        if (interpreter_id < data->count) {
            found = data->table+interpreter_id;
        }
    } else {
        found = __Pyx_State_FindModuleStateLookupTableLowerBound(
            data->table, data->count, interpreter_id);
    }
  end:
    {
        PyObject *result=NULL;
        if (found && found->id == interpreter_id) {
            result = found->module;
        }
#if CYTHON_MODULE_STATE_LOOKUP_THREAD_SAFE
        __pyx_atomic_decr_acq_rel(&__Pyx_ModuleStateLookup_read_counter);
#endif
        return result;
    }
}
#if CYTHON_MODULE_STATE_LOOKUP_THREAD_SAFE
static void __Pyx_ModuleStateLookup_wait_until_no_readers(void) {
    while (__pyx_atomic_load(&__Pyx_ModuleStateLookup_read_counter) != 0);
}
#else
#define __Pyx_ModuleStateLookup_wait_until_no_readers()
#endif
static int __Pyx_State_AddModuleInterpIdAsIndex(__Pyx_ModuleStateLookupData **old_data, PyObject* module, int64_t interpreter_id) {
    Py_ssize_t to_allocate = (*old_data)->allocated;
    while (to_allocate <= interpreter_id) {
        if (to_allocate == 0) to_allocate = 1;
        else to_allocate *= 2;
    }
    __Pyx_ModuleStateLookupData *new_data = *old_data;
    if (to_allocate != (*old_data)->allocated) {
         new_data = (__Pyx_ModuleStateLookupData *)realloc(
            *old_data,
            sizeof(__Pyx_ModuleStateLookupData)+(to_allocate-1)*sizeof(__Pyx_InterpreterIdAndModule));
        if (!new_data) {
            PyErr_NoMemory();
            return -1;
        }
        for (Py_ssize_t i = new_data->allocated; i < to_allocate; ++i) {
            new_data->table[i].id = i;
            new_data->table[i].module = NULL;
        }
        new_data->allocated = to_allocate;
    }
    new_data->table[interpreter_id].module = module;
    if (new_data->count < interpreter_id+1) {
        new_data->count = interpreter_id+1;
    }
    *old_data = new_data;
    return 0;
}
static void __Pyx_State_ConvertFromInterpIdAsIndex(__Pyx_ModuleStateLookupData *data) {
    __Pyx_InterpreterIdAndModule *read = data->table;
    __Pyx_InterpreterIdAndModule *write = data->table;
    __Pyx_InterpreterIdAndModule *end = read + data->count;
    for (; read<end; ++read) {
        if (read->module) {
            write->id = read->id;
            write->module = read->module;
            ++write;
        }
    }
    data->count = write - data->table;
    for (; write<end; ++write) {
        write->id = 0;
        write->module = NULL;
    }
    data->interpreter_id_as_index = 0;
}
static int __Pyx_State_AddModule(PyObject* module, CYTHON_UNUSED void* dummy) {
    int64_t interpreter_id = PyInterpreterState_GetID(__Pyx_PyInterpreterState_Get());
    if (interpreter_id == -1) return -1;
    int result = 0;
    __Pyx_ModuleStateLookup_Lock();
#if CYTHON_MODULE_STATE_LOOKUP_THREAD_SAFE
    __Pyx_ModuleStateLookupData *old_data = (__Pyx_ModuleStateLookupData *)
            __pyx_atomic_pointer_exchange(&__Pyx_ModuleStateLookup_data, 0);
#else
    __Pyx_ModuleStateLookupData *old_data = __Pyx_ModuleStateLookup_data;
#endif
    __Pyx_ModuleStateLookupData *new_data = old_data;
    if (!new_data) {
        new_data = (__Pyx_ModuleStateLookupData *)calloc(1, sizeof(__Pyx_ModuleStateLookupData));
        if (!new_data) {
            result = -1;
            PyErr_NoMemory();
            goto end;
        }
        new_data->allocated = 1;
        new_data->interpreter_id_as_index = 1;
    }
    __Pyx_ModuleStateLookup_wait_until_no_readers();
    if (new_data->interpreter_id_as_index) {
        if (interpreter_id < __PYX_MODULE_STATE_LOOKUP_SMALL_SIZE) {
            result = __Pyx_State_AddModuleInterpIdAsIndex(&new_data, module, interpreter_id);
            goto end;
        }
        __Pyx_State_ConvertFromInterpIdAsIndex(new_data);
    }
    {
        Py_ssize_t insert_at = 0;
        {
            __Pyx_InterpreterIdAndModule* lower_bound = __Pyx_State_FindModuleStateLookupTableLowerBound(
                new_data->table, new_data->count, interpreter_id);
            assert(lower_bound);
            insert_at = lower_bound - new_data->table;
            if (unlikely(insert_at < new_data->count && lower_bound->id == interpreter_id)) {
                lower_bound->module = module;
                goto end;
            }
        }
        if (new_data->count+1 >= new_data->allocated) {
            Py_ssize_t to_allocate = (new_data->count+1)*2;
            new_data =
                (__Pyx_ModuleStateLookupData*)realloc(
                    new_data,
                    sizeof(__Pyx_ModuleStateLookupData) +
                    (to_allocate-1)*sizeof(__Pyx_InterpreterIdAndModule));
            if (!new_data) {
                result = -1;
                new_data = old_data;
                PyErr_NoMemory();
                goto end;
            }
            new_data->allocated = to_allocate;
        }
        ++new_data->count;
        int64_t last_id = interpreter_id;
        PyObject *last_module = module;
        for (Py_ssize_t i=insert_at; i<new_data->count; ++i) {
            int64_t current_id = new_data->table[i].id;
            new_data->table[i].id = last_id;
            last_id = current_id;
            PyObject *current_module = new_data->table[i].module;
            new_data->table[i].module = last_module;
            last_module = current_module;
        }
    }
  end:
#if CYTHON_MODULE_STATE_LOOKUP_THREAD_SAFE
    __pyx_atomic_pointer_exchange(&__Pyx_ModuleStateLookup_data, new_data);
#else
    __Pyx_ModuleStateLookup_data = new_data;
#endif
    __Pyx_ModuleStateLookup_Unlock();
    return result;
}
static int __Pyx_State_RemoveModule(CYTHON_UNUSED void* dummy) {
    int64_t interpreter_id = PyInterpreterState_GetID(__Pyx_PyInterpreterState_Get());
    if (interpreter_id == -1) return -1;
    __Pyx_ModuleStateLookup_Lock();
#if CYTHON_MODULE_STATE_LOOKUP_THREAD_SAFE
    __Pyx_ModuleStateLookupData *data = (__Pyx_ModuleStateLookupData *)
            __pyx_atomic_pointer_exchange(&__Pyx_ModuleStateLookup_data, 0);
#else
    __Pyx_ModuleStateLookupData *data = __Pyx_ModuleStateLookup_data;
#endif
    if (data->interpreter_id_as_index) {
        if (interpreter_id < data->count) {
            data->table[interpreter_id].module = NULL;
        }
        goto done;
    }
    {
        __Pyx_ModuleStateLookup_wait_until_no_readers();
        __Pyx_InterpreterIdAndModule* lower_bound = __Pyx_State_FindModuleStateLookupTableLowerBound(
            data->table, data->count, interpreter_id);
        if (!lower_bound) goto done;
        if (lower_bound->id != interpreter_id) goto done;
        __Pyx_InterpreterIdAndModule *end = data->table+data->count;
        for (;lower_bound<end-1; ++lower_bound) {
            lower_bound->id = (lower_bound+1)->id;
            lower_bound->module = (lower_bound+1)->module;
        }
    }
    --data->count;
    if (data->count == 0) {
        free(data);
        data = NULL;
    }
  done:
#if CYTHON_MODULE_STATE_LOOKUP_THREAD_SAFE
    __pyx_atomic_pointer_exchange(&__Pyx_ModuleStateLookup_data, data);
#else
    __Pyx_ModuleStateLookup_data = data;
#endif
    __Pyx_ModuleStateLookup_Unlock();
    return 0;
}
#endif

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
