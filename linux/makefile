BUILD_NUMBER_FILE := makefile.bld
CORE_CPP_FILES := $(wildcard ../common/*.cpp) $(wildcard ../common/elements/*.cpp) $(wildcard ../common/renderers/*.cpp) $(wildcard ../common/interpreters/*.cpp)
CORE_OBJ_FILES := $(addprefix ../build/obj/,$(notdir $(CORE_CPP_FILES:.cpp=.o)))
MT_CPP_FILES := main.cpp
MT_OBJ_FILES := ../build/obj/main.o
OBJ_FILES := $(MT_OBJ_FILES) $(CORE_OBJ_FILES)
BOOST_PATH := ../../boost_1_58_0
LIBLOUIS_PATH := ../../liblouis
LD_FLAGS := -g -L$(BOOST_PATH)/stage/lib -L$(LIBLOUIS_PATH)/lib
LD_LIBS := -pthread -Wl,-Bstatic -llouis -lboost_thread -lboost_program_options -lboost_filesystem -lboost_system -lboost_log -lboost_regex -Wl,-Bdynamic
CC_FLAGS := -Wall -g -I../common/h -I$(BOOST_PATH) -I$(LIBLOUIS_PATH)/include
CC_FLAGS += -DMATHTEXT_VERSION_BUILD=$(shell cat makefile.bld)
CC_FLAGS += -MMD
#CC_FLAGS += -DMT_LOG_TO_CONSOLE
VPATH=.:../common:../common/elements:../common/renderers:../common/interpreters

all: pch mathtext

pch: ../common/h/common-includes.h.gch

mathtext: $(OBJ_FILES)
	@echo Linking MathText
	@g++ $(LD_FLAGS) -o $@ $^ $(LD_LIBS)

../common/h/common-includes.h.gch: ../common/h/common-includes.h
	@echo Precompiling common header files:
	@g++ $(CC_FLAGS) -c -o $@ $<

../build/obj/%.o: %.cpp
	@echo Compiling $<
	@g++ $(CC_FLAGS) -c -o $@ $<

../build/obj/main.o: $(BUILD_NUMBER_FILE)

$(BUILD_NUMBER_FILE): $(CPP_FILES)
	@if ! test -f $(BUILD_NUMBER_FILE); then echo 0 > $(BUILD_NUMBER_FILE); fi
	@echo $$(($$(cat $(BUILD_NUMBER_FILE)) + 1)) > $(BUILD_NUMBER_FILE)

clean-pch:
	rm ../common/h/common-includes.h.gch

clean: 
	rm ../common/h/common-includes.h.gch
	rm ../build/obj/*
	rm mathtext

-include $(OBJ_FILES:.o=.d)

