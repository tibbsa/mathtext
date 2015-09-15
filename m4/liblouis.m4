#
# SYNOPSIS
#
#   CHECK_LIB_LOUIS([ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
#
# DESCRIPTION
#
#   Test for the liblouis libraries (assuming they are required).
#
#   If no path to the installed library is given the macro searches under
#   /usr, /usr/local, /opt and /opt/local.
#
#   This macro calls:
#
#     AC_SUBST(LOUIS_CFLAGS) AC_SUBST(LOUIS_CPPFLAGS) / 
#     AC_SUBST(LOUIS_LDFLAGS)
#
#   And sets:
#
#     HAVE_LIBLOUIS
#
# LICENSE
#
#   Copyright (c) 2015 Anthony Tibbs <anthony@tibbs.ca>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 1

AC_DEFUN([CHECK_LIB_LOUIS],
[
AC_ARG_WITH([louis],
    [AS_HELP_STRING([--with-louis@<:@=ARG@:>@],
        [use liblouis from a standard location (ARG=yes),
         from the specified location (ARG=<path>)
         @<:@ARG=yes@:>@ ])],
    [
     if test "x$withval" != "xyes"; then
        ac_liblouis_path="$withval"
     fi
    ],
)

AC_ARG_WITH([liblouis-include],
    [AS_HELP_STRING([--with-liblouis-include=INCLUDE_DIR],
        [Force given directory for liblouis include path. Note that this will overwrite general library path setting.])
    ], [
        if test -d "$withval"; then
            ac_liblouis_include_path="$withval"
        else
            AC_MSG_ERROR([--with-liblouis-include expected directory name])
        fi
    ],
    [ac_liblouis_include_path=""]
)

AC_ARG_WITH([liblouis-lib],
    [AS_HELP_STRING([--with-liblouis-lib=LIB_DIR],
        [Force given directory for liblouis libraries. Note that this will overwrite general library path setting.])
    ], [
        if test -d "$withval"; then
            ac_liblouis_lib_path="$withval"
        else
            AC_MSG_ERROR([--with-liblouis-lib expected directory name])
        fi
    ],
    [ac_liblouis_lib_path=""]
)

    succeeded=no

    dnl On x86_64 systems check for system libraries in both lib64 and lib.
    dnl The former is specified by FHS, but e.g. Debian does not adhere to
    dnl this (as it rises problems for generic multi-arch support).
    dnl The last entry in the list is chosen by default when no libraries
    dnl are found, e.g. when only header-only libraries are installed!
    libsubdirs="lib"
    if test `uname -m` = x86_64; then
        libsubdirs="lib lib64"
    fi
 
    if test "x$ac_liblouis_path" != "x"; then
        for libsubdir in $libsubdirs ; do
            if ls "$ac_liblouis_path/$libsubdir/liblouis"* >/dev/null 2>&1 ; then
	        LOUIS_LIBPATH="$ac_liblouis_path/$libsubdir"
                LOUIS_LDFLAGS="-L$ac_liblouis_path/$libsubdir"
                LOUIS_CFLAGS="-I$ac_liblouis_path/include"
		LOUIS_CPPFLAGS="-I$ac_liblouis_path/include"
                break;
            fi
        done
    else
        for ac_liblouis_path_tmp in /usr /usr/local /opt /opt/local ; do
            for libsubdir in $libsubdirs ; do
                if ls "$ac_liblouis_path_tmp/$libsubdir/liblouis"* >/dev/null 2>&1 ; then
		    LOUIS_LIBPATH="$ac_liblouis_path_tmp/$libsubdir"
                    LOUIS_LDFLAGS="-L$ac_liblouis_path_tmp/$libsubdir"
                    LOUIS_CFLAGS="-I$ac_liblouis_path_tmp/include"
		    LOUIS_CPPFLAGS="-I$ac_liblouis_path_tmp/include"
                    break;
                fi
            done
        done
    fi

    if test "x$ac_liblouis_include_path" != "x" ; then
        LOUIS_CFLAGS="-I$ac_liblouis_include_path"
        LOUIS_CPPFLAGS="-I$ac_liblouis_include_path"
    fi

    if test "x$ac_liblouis_lib_path" != "x" ; then
        LOUIS_LDFLAGS="-L$ac_liblouis_lib_path"
    fi

    CFLAGS_SAVED="$CFLAGS"
    CFLAGS="$CFLAGS $LOUIS_CFLAGS"
    export CFLAGS

    CPPFLAGS_SAVED="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS $LOUIS_CPPFLAGS"
    export CPPFLAGS

    LDFLAGS_SAVED="$LDFLAGS"
    LDFLAGS="$LDFLAGS $LOUIS_LDFLAGS"
    export LDFLAGS

    AC_CHECK_LIB([louis], [lou_translateString], [
    	LOUIS_LDFLAGS="$LOUIS_LDFLAGS"
	LOUIS_STATIC_LIB="$LOUIS_LIBPATH/liblouis.a"
        succeeded=yes
    ], [
        AC_MSG_ERROR([Could not find working liblouis library.])
        ax_cv_liblouis_link=no
    ])

    if test "x$succeeded" = "xyes"; then
        AC_SUBST(LOUIS_CFLAGS)
	AC_SUBST(LOUIS_CPPFLAGS)
        AC_SUBST(LOUIS_LDFLAGS)
	AC_SUBST(LOUIS_STATIC_LIB)

        AC_DEFINE(HAVE_LIBLOUIS, , [define if liblouis is available])
        ifelse([$1], , :, [$1])
    else
        AC_MSG_NOTICE([[Could not detect the liblouis installation. Please make sure you have a working liblouis installation and specify its path with --with-liblouis option.]])
        ifelse([$2], , :, [$2])
    fi

    CFLAGS="$CFLAGS_SAVED"
    export CFLAGS

    CPPFLAGS="$CPPFLAGS_SAVED"
    export CPPFLAGS

    LDFLAGS="$LDFLAGS_SAVED"
    export LDFLAGS


])
