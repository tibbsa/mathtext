BUILD_NUMBER_FILE := makefile.bld
CPP_FILES := $(wildcard *.cpp) $(wildcard ../common/*.cpp) 
OBJ_FILES := $(addprefix ../build/obj/,$(notdir $(CPP_FILES:.cpp=.o)))
BOOST_PATH := ../../boost_1_58_0
LD_FLAGS := -L$(BOOST_PATH)/stage/lib
LD_LIBS := -Wl,-Bstatic -lboost_program_options -lboost_filesystem -lboost_system -Wl,-Bdynamic 
CC_FLAGS := -I../common/h -I$(BOOST_PATH) 
CC_FLAGS += -DMATHTEXT_VERSION_BUILD=$(shell cat makefile.bld)
VPATH=.:../common

mathtext: $(OBJ_FILES)
	g++ $(LD_FLAGS) -o $@ $^ $(LD_LIBS)

../build/obj/%.o: %.cpp
	g++ $(CC_FLAGS) -c -o $@ $<

../build/obj/main.o: $(BUILD_NUMBER_FILE)

$(BUILD_NUMBER_FILE): $(CPP_FILES)
	@if ! test -f $(BUILD_NUMBER_FILE); then echo 0 > $(BUILD_NUMBER_FILE); fi
	@echo $$(($$(cat $(BUILD_NUMBER_FILE)) + 1)) > $(BUILD_NUMBER_FILE)

CC_FLAGS += -MMD
-include $(OBJFILES:.o=.d)

