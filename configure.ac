AC_INIT([mathtext], [dev], [anthony@tibbs.ca])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIRS([m4])
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

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

# Output files.
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile cli/Makefile])
AC_OUTPUT
