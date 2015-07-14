BUILD_NUMBER_FILE := makefile.bld
CORE_CPP_FILES := $(wildcard ../common/*.cpp) $(wildcard ../common/elements/*.cpp) $(wildcard ../common/renderers/*.cpp) 
CORE_OBJ_FILES := $(addprefix ../build/obj/,$(notdir $(CORE_CPP_FILES:.cpp=.o)))
MT_CPP_FILES := main.cpp
MT_OBJ_FILES := ../build/obj/main.o
TEST_CPP_FILES := mttest.cpp
TEST_OBJ_FILES := ../build/obj/mttest.o

BOOST_PATH := ../../boost_1_58_0
LIBLOUIS_PATH := ../../liblouis
LD_FLAGS := -g -L$(BOOST_PATH)/stage/lib -L$(LIBLOUIS_PATH)/lib
LD_LIBS := -pthread -Wl,-Bstatic -llouis -lboost_thread -lboost_program_options -lboost_filesystem -lboost_system -lboost_log -lboost_regex -Wl,-Bdynamic
CC_FLAGS := -Wall -g -I../common/h -I$(BOOST_PATH) -I$(LIBLOUIS_PATH)/include
CC_FLAGS += -DMATHTEXT_VERSION_BUILD=$(shell cat makefile.bld)
CC_FLAGS += -MMD
VPATH=.:../common:../common/elements:../common/renderers

all: mathtext mttest

mttest: $(TEST_OBJ_FILES) $(CORE_OBJ_FILES)
	g++ $(LD_FLAGS) -o $@ $^ $(LD_LIBS)

mathtext: $(MT_OBJ_FILES) $(CORE_OBJ_FILES)
	g++ $(LD_FLAGS) -o $@ $^ $(LD_LIBS)

../build/obj/%.o: %.cpp
	g++ $(CC_FLAGS) -c -o $@ $<

../build/obj/main.o: $(BUILD_NUMBER_FILE)

$(BUILD_NUMBER_FILE): $(CPP_FILES)
	@if ! test -f $(BUILD_NUMBER_FILE); then echo 0 > $(BUILD_NUMBER_FILE); fi
	@echo $$(($$(cat $(BUILD_NUMBER_FILE)) + 1)) > $(BUILD_NUMBER_FILE)

clean: 
	rm ../build/obj/*
	rm mathtext
	rm mttest

-include $(OBJ_FILES:.o=.d)

