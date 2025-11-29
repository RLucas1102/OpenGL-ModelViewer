src := ./src
inc := ./include
imgui := ./imgui
obj := ./obj
bin := ./bin
libGLFW := ./pckgs/libglfw3.a
libASSIMP := ./pckgs/assimp/bin
program := $(bin)/renderer.out

CPP_SOURCES := $(wildcard $(src)/*.cpp)
CPP_OBJECTS := $(CPP_SOURCES:$(src)/%.cpp=$(obj)/%.o)

GUI_SOURCES := #$(wildcard $(imgui)/*.cpp)
GUI_OBJECTS := #$(GUI_SOURCES:$(imgui)/%.cpp=$(obj)/%.o)

C_SOURCES := $(wildcard $(src)/*.c)
C_OBJECTS := $(C_SOURCES:$(src)/%.c=$(obj)/%.o)

CC = g++
CFLAGS = -g -I $(inc)
GUIFLAGS = -I $(inc)/imgui
LFLAGS = $(libGLFW) -L $(libASSIMP) -lassimpmodel

$(program) : $(CPP_OBJECTS) $(GUI_OBJECTS) $(C_OBJECTS)
	$(CC) $^ $(LFLAGS) -o $@

$(CPP_OBJECTS) : $(obj)/%.o : $(src)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(C_OBJECTS) : $(obj)/%.o : $(src)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(GUI_OBJECTS) : $(obj)/%.o : $(imgui)/%.cpp
	$(CC) $(GUIFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(CPP_OBJECTS)
	rm -f $(GUI_OBJECTS)
	rm -f $(C_OBJECTS)
	rm -f $(program)