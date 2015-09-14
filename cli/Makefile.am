CXX = $(PTHREAD_CXX)

bin_PROGRAMS = mathtext

mathtext_SOURCES = main.cpp

mathtext_CPPFLAGS = -DBOOST_LOG_DYN_LINK -I$(top_srcdir)/include \
	$(BOOST_CPPFLAGS) $(PTHREAD_CPPFLAGS)

mathtext_LDFLAGS = $(BOOST_LDFLAGS) $(PTHREAD_CPPFLAGS) \
	-L$(top_srcdir)/src

mathtext_LDADD = \
	../src/libmathtext.a \
	$(BOOST_LOG_LIB) \
	$(BOOST_THREAD_LIB) \
	$(BOOST_PROGRAM_OPTIONS_LIB) \
	$(BOOST_FILESYSTEM_LIB) \
	$(BOOST_SYSTEM_LIB) \
	$(BOOST_REGEX_LIB)
