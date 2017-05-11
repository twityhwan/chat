CXX     = g++
LIBS	= -lpthread
SRCS    = $(wildcard *.cpp)
OBJS    = $(SRCS:.cpp=.o)
TARGET  = server

all : $(TARGET)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)
$(TARGET) :
	$(CXX) -c $(SRCS)
clean :
	rm -f $(TARGET)
	rm -f *.o
