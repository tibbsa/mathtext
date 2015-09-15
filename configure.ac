AC_INIT([mathtext], [0.10], [anthony@tibbs.ca])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([foreign -Wall -Werror -Wno-portability subdir-objects])
AM_SILENT_RULES([yes])

# Checks for programs.
AC_PROG_CXX
AC_PROG_RANLIB

# Checks for libraries.
AX_PTHREAD

AX_BOOST_BASE([1.58], , [AC_MSG_ERROR([MathText needs Boost, but it was not found in your system])])
AX_BOOST_THREAD
AX_BOOST_PROGRAM_OPTIONS
AX_BOOST_LOG
AX_BOOST_SYSTEM
AX_BOOST_FILESYSTEM
AX_BOOST_REGEX

CHECK_LIB_LOUIS(, [AC_MSG_ERROR([MathText needs the liblouis library, but it was not found in your system.])])

# Checks for header files.
AC_CHECK_HEADERS([stdint.h stdlib.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([gettimeofday memset strerror])

# Output files.
AC_CONFIG_FILES([
  Makefile 
  src/Makefile 
  cli/Makefile 
  test/Makefile
  doc/Makefile
  gui/Makefile])
AC_OUTPUT
