CORE_CPP_FILES := $(wildcard ../common/*.cpp) $(wildcard ../common/elements/*.cpp) $(wildcard ../common/renderers/*.cpp) $(wildcard ../common/interpreters/*.cpp)
CORE_OBJ_FILES := $(addprefix ../build/test-obj/,$(notdir $(CORE_CPP_FILES:.cpp=.o))) 
TEST_CPP_FILES := mttest.cpp $(wildcard rendering/*.cpp) $(wildcard rendering/ueb/*.cpp)
TEST_OBJ_FILES := $(addprefix ../build/test-obj/,$(notdir $(TEST_CPP_FILES:.cpp=.o)))
OBJ_FILES := $(CORE_OBJ_FILES) $(TEST_OBJ_FILES)
BOOST_PATH := ../../boost_1_58_0
LIBLOUIS_PATH := ../../liblouis
LD_FLAGS := -g -L$(BOOST_PATH)/stage/lib -L$(LIBLOUIS_PATH)/lib
LD_LIBS := -pthread -Wl,-Bstatic -llouis -lboost_thread -lboost_program_options -lboost_filesystem -lboost_system -lboost_log -lboost_regex -Wl,-Bdynamic
CC_FLAGS := -DMT_NO_CONSOLE_LOG -MMD -DTEST -Wall -g -Ih -I../common/h -I$(BOOST_PATH) -I$(LIBLOUIS_PATH)/include
VPATH=.:../common:../common/elements:../common/renderers:../common/interpreters:rendering:rendering/ueb

all: pch mttest

pch: ../common/h/common-includes.h.gch

mttest: $(OBJ_FILES)
	@echo Linking
	@g++ $(LD_FLAGS) -o $@ $^ $(LD_LIBS)

../common/h/common-includes.h.gch: ../common/h/common-includes.h
	@echo Precompiling common header files:
	@g++ $(CC_FLAGS) -c -o $@ $<

../build/test-obj/%.o: %.cpp
	@echo $<: 
	@g++ $(CC_FLAGS) -c -o $@ $<

clean-pch:
	rm ../common/h/common-includes.h.gch

clean: 
	rm ../common/h/common-includes.h.gch
	rm ../build/test-obj/*
	rm mttest

-include $(OBJ_FILES:.o=.d)

