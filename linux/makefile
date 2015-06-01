BUILD_NUMBER_FILE := makefile.bld
CPP_FILES := $(wildcard *.cpp)
OBJ_FILES := $(addprefix ../build/obj/,$(notdir $(CPP_FILES:.cpp=.o)))
LD_FLAGS := 
CC_FLAGS := -I../common/h
CC_FLAGS += -DMATHTEXT_VERSION_BUILD=$(shell cat makefile.bld)

mathtext: $(OBJ_FILES)
	g++ $(LD_FLAGS) -o $@ $^

../build/obj/%.o: %.cpp
	g++ $(CC_FLAGS) -c -o $@ $<

../build/obj/main.o: $(BUILD_NUMBER_FILE)

$(BUILD_NUMBER_FILE): $(CPP_FILES)
	@if ! test -f $(BUILD_NUMBER_FILE); then echo 0 > $(BUILD_NUMBER_FILE); fi
	@echo $$(($$(cat $(BUILD_NUMBER_FILE)) + 1)) > $(BUILD_NUMBER_FILE)

CC_FLAGS += -MMD
-include $(OBJFILES:.o=.d)

