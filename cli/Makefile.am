CXX = $(PTHREAD_CXX)

bin_PROGRAMS = mathtext

mathtext_SOURCES = main.cpp

mathtext_CPPFLAGS = -I$(top_srcdir)/include \
	$(BOOST_CPPFLAGS) $(PTHREAD_CPPFLAGS)

mathtext_LDFLAGS = $(BOOST_LDFLAGS) $(PTHREAD_CPPFLAGS) \
	-L$(top_srcdir)/src -Wl,-Bstatic,$(BOOST_THREAD_LIB),$(BOOST_PROGRAM_OPTIONS_LIB),$(BOOST_FILESYSTEM_LIB),$(BOOST_SYSTEM_LIB),$(BOOST_LOG_LIB),$(BOOST_REGEX_LIB),-Bdynamic

mathtext_LDADD = ../src/libmathtext.a

#mathtext_LDADD = \
#	$(BOOST_THREAD_LIB) \
#	$(BOOST_PROGRAM_OPTIONS_LIB) \
#	$(BOOST_FILESYSTEM_LIB) \
#	$(BOOST_SYSTEM_LIB) \
#	$(BOOST_LOG_LIB) \
#	$(BOOST_REGEX_LIB) ../src/libmathtext.a
