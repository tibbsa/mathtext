CXX = $(PTHREAD_CXX)

bin_PROGRAMS = mttest

mttest_SOURCES = mttest.cpp \
	interpreting/Int_Numbers.cpp \
	interpreting/Int_Exponents.cpp \
	interpreting/Int_Subscripts.cpp \
	interpreting/Int_Comparators.cpp \
	rendering/ueb/UEB_item_detection.cpp \
	rendering/ueb/UEB_ItemNumbers.cpp \
	rendering/ueb/UEB_Symbols.cpp \
	rendering/ueb/UEB_Subscripts.cpp \
	rendering/ueb/UEB_Modifiers.cpp \
	rendering/ueb/UEB_Fractions.cpp \
	rendering/ueb/UEB_GreekLetters.cpp \
	rendering/ueb/UEB_Comparators.cpp \
	rendering/ueb/UEB_Roots.cpp \
	rendering/ueb/UEB_Exponents.cpp \
	rendering/ueb/UEB_Numbers.cpp \
	rendering/ueb/UEB_Operators.cpp \
	rendering/ueb/UEB_Summation.cpp \
	rendering/ueb/UEB_examples.cpp


mttest_CPPFLAGS = -DBOOST_LOG_DYN_LINK -I$(top_srcdir)/include \
	-I$(srcdir)/include $(BOOST_CPPFLAGS) $(PTHREAD_CPPFLAGS) $(LOUIS_CPPFLAGS)

mttest_LDFLAGS = $(BOOST_LDFLAGS) $(PTHREAD_CPPFLAGS) $(LOUIS_LDFLAGS) \
	-L$(top_srcdir)/src 

mttest_LDADD = \
	../src/libmathtext.a \
	$(BOOST_LOG_LIB) \
	$(BOOST_THREAD_LIB) \
	$(BOOST_PROGRAM_OPTIONS_LIB) \
	$(BOOST_FILESYSTEM_LIB) \
	$(BOOST_SYSTEM_LIB) \
	$(BOOST_REGEX_LIB) \
	$(LOUIS_STATIC_LIB)
