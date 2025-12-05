OBJS = Hogwarts.o View.o Controller.o Model.o KDTree.o KDInternalNode.o KDAbstractNode.o
INCLUDES = -I../include
LIBS = -L../lib
LDFLAGS = -lglad -lglfw3
CFLAGS = -g -std=c++11
PROGRAM = Hogwarts

ifeq ($(OS),Windows_NT)     # is Windows_NT on XP, 2000, 7, Vista, 10...
    LDFLAGS += -lopengl32 -lgdi32
    PROGRAM :=$(addsuffix .exe,$(PROGRAM))
	COMPILER = g++
else ifeq ($(shell uname -s),Darwin)     # is MACOSX
    LDFLAGS += -framework Cocoa -framework OpenGL -framework IOKit
	COMPILER = clang++
endif

Hogwarts: $(OBJS)
	$(COMPILER) -o $(PROGRAM) $(OBJS) $(LIBS) $(LDFLAGS)

Hogwarts.o: Hogwarts.cpp
	$(COMPILER) $(INCLUDES) $(CFLAGS) -c Hogwarts.cpp

View.o: View.cpp View.h
	$(COMPILER) $(INCLUDES) $(CFLAGS) -c View.cpp	

Controller.o: Controller.cpp Controller.h
	$(COMPILER) $(INCLUDES) $(CFLAGS) -c Controller.cpp	

Model.o: Model.cpp Model.h
	$(COMPILER) $(INCLUDES) $(CFLAGS) -c Model.cpp		

KDTree.o: kd-tree/KDTree.cpp kd-tree/KDTree.h
	$(COMPILER) $(INCLUDES) $(CFLAGS) -c kd-tree/KDTree.cpp -o KDTree.o

KDInternalNode.o: kd-tree/KDInternalNode.cpp kd-tree/KDInternalNode.h
	$(COMPILER) $(INCLUDES) $(CFLAGS) -c kd-tree/KDInternalNode.cpp -o KDInternalNode.o

KDAbstractNode.o: kd-tree/KDAbstractNode.cpp kd-tree/KDAbstractNode.h
	$(COMPILER) $(INCLUDES) $(CFLAGS) -c kd-tree/KDAbstractNode.cpp -o KDAbstractNode.o
	
RM = rm	-f
RM := del

clean: 
	$(RM) $(OBJS) $(PROGRAM)
    