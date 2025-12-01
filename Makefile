OBJS = Hogwarts.o View.o Controller.o Model.o
INCLUDES = -I../include
LIBS = -L../lib
LDFLAGS = -lglad -lglfw3
CFLAGS = -g -std=c++11
PROGRAM = Hogwarts

LDFLAGS += -lopengl32 -lgdi32
PROGRAM :=$(addsuffix .exe,$(PROGRAM))
COMPILER = g++

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
	
RM = rm	-f
RM := del

clean: 
	$(RM) $(OBJS) $(PROGRAM)
    