AC_INIT([mathtext], [dev], [anthony@tibbs.ca])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE([foreign -Wall -Werror subdir-objects])

# Checks for programs.
AC_PROG_CXX
AC_PROG_RANLIB
AM_PROG_AR

# Checks for libraries.
AX_BOOST_BASE([1.58], , [ACMSG_ERROR([MathText needs Boost, but it was not found in your system])])
#TODO: Add AX_BOOST_*

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

# Output files.
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
