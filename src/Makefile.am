AM_CPPFLAGS = -DBOOST_LOG_DYN_LINK -I$(top_srcdir)/include \
	$(BOOST_CPPFLAGS) $(PTHREAD_CPPFLAGS) $(LOUIS_CPPFLAGS) 
CXX = $(PTHREAD_CXX)


noinst_LIBRARIES = libmathtext.a

libmathtext_a_SOURCES = \
	liblouis-mt.cpp \
	logging.cpp \
	MathDocument.cpp \
	MathDocumentLine.cpp \
	MathInterpreter.cpp \
	MathInterpreterMsg.cpp \
	MathSourceFile.cpp \
	utility.cpp \
	elements/MDE_Command.cpp \
	elements/MDE_Comparator.cpp \
	elements/MDE.cpp \
	elements/MDE_Exponent.cpp \
	elements/MDE_Fraction.cpp \
	elements/MDE_GenericText.cpp \
	elements/MDE_GreekLetter.cpp \
	elements/MDE_Group.cpp \
	elements/MDE_ItemNumber.cpp \
	elements/MDE_LineBreak.cpp \
	elements/MDE_MathBlock.cpp \
	elements/MDE_ModeMarker.cpp \
	elements/MDE_Modifier.cpp \
	elements/MDE_Number.cpp \
	elements/MDE_Operator.cpp \
	elements/MDE_Root.cpp \
	elements/MDE_SourceLine.cpp \
	elements/MDE_Subscript.cpp \
	elements/MDE_Summation.cpp \
	elements/MDE_Symbol.cpp \
	elements/MDE_TextBlock.cpp \
	interpreters/InterpretCommands.cpp \
	interpreters/InterpretComparators.cpp \
	interpreters/InterpretExponents.cpp \
	interpreters/InterpretFractions.cpp \
	interpreters/InterpretGreek.cpp \
	interpreters/InterpretGroups.cpp \
	interpreters/InterpretItemNumbers.cpp \
	interpreters/InterpretModifiers.cpp \
	interpreters/InterpretNumbers.cpp \
	interpreters/InterpretOperators.cpp \
	interpreters/InterpretRoots.cpp \
	interpreters/InterpretSubscripts.cpp \
	interpreters/InterpretSummations.cpp \
	interpreters/InterpretSymbols.cpp \
	renderers/MathRenderer.cpp \
	renderers/UEBRenderer.cpp \
	renderers/LaTeXRenderer.cpp \
	../include/LaTeXRenderer.h \
	../include/liblouis-mt.h \
	../include/logging.h \
	../include/MathDocumentElements.h \
	../include/MathDocument.h \
	../include/MathDocumentLine.h \
	../include/MathExceptions.h \
	../include/MathInterpreter.h \
	../include/MathInterpreterMsg.h \
	../include/MathRenderer.h \
	../include/MathSourceFile.h \
	../include/mathtext.h \
	../include/UEBRenderer.h \
	../include/utility.h
