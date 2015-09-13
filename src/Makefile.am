AM_CPPFLAGS = -I$(top_srcdir)/include \
	$(BOOST_CPPFLAGS)

AM_LDFLAGS = $(BOOST_LDFLAGS)

noinst_LIBRARIES = libmathtext.a
libmathtext_a_SOURCES = *.cpp renderers/*.cpp elements/*.cpp interpreters/*.cpp
libmathtext_a_LIBADD = $(BOOST_THREAD_LIB) \
			$(BOOST_PROGRAM_OPTIONS_LIB) \
			$(BOOST_FILESYSTEM_LIB) \
			$(BOOST_SYSTEM_LIB) \
			$(BOOST_LOG_LIB) \
			$(BOOST_REGEX_LIB)
