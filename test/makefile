CORE_CPP_FILES := $(wildcard ../common/*.cpp) $(wildcard ../common/elements/*.cpp) $(wildcard ../common/renderers/*.cpp) 
CORE_OBJ_FILES := $(addprefix ../build/test-obj/,$(notdir $(CORE_CPP_FILES:.cpp=.o)))
TEST_CPP_FILES := mttest.cpp $(wildcard rendering/*.cpp) $(wildcard rendering/ueb/*.cpp)
TEST_OBJ_FILES := $(addprefix ../build/test-obj/,$(notdir $(TEST_CPP_FILES:.cpp=.o)))
BOOST_PATH := ../../boost_1_58_0
LIBLOUIS_PATH := ../../liblouis
LD_FLAGS := -g -L$(BOOST_PATH)/stage/lib -L$(LIBLOUIS_PATH)/lib
LD_LIBS := -pthread -Wl,-Bstatic -llouis -lboost_thread -lboost_program_options -lboost_filesystem -lboost_system -lboost_log -lboost_regex -Wl,-Bdynamic
CC_FLAGS := -DMT_NO_CONSOLE_LOG -MMD -DTEST -Wall -g -Ih -I../common/h -I$(BOOST_PATH) -I$(LIBLOUIS_PATH)/include
VPATH=.:../common:../common/elements:../common/renderers:rendering:rendering/ueb

mttest: $(TEST_OBJ_FILES) $(CORE_OBJ_FILES)
	g++ $(LD_FLAGS) -o $@ $^ $(LD_LIBS)

../build/test-obj/%.o: %.cpp
	g++ $(CC_FLAGS) -c -o $@ $<

clean: 
	rm ../build/test-obj/*
	rm mttest

-include $(OBJ_FILES:.o=.d)

