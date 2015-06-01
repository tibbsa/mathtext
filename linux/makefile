CPP_FILES := $(wildcard *.cpp)
OBJ_FILES := $(addprefix ../build/obj/,$(notdir $(CPP_FILES:.cpp=.o)))
LD_FLAGS := 
CC_FLAGS := 

mathtext: $(OBJ_FILES)
	g++ $(LD_FLAGS) -o $@ $^

../build/obj/%.o: %.cpp
	g++ $(CC_FLAGS) -c -o $@ $<

CC_FLAGS += -MMD
-include $(OBJFILES:.o=.d)

