CXX     = g++
SRCS    = $(wildcard *.cpp)
OBJS    = $(SRCS:.cpp=.o)
TARGET  = client

all : $(TARGET)
	$(CXX) -o $(TARGET) $(OBJS) -lpthread
$(TARGET) : $(SRCS)
	$(CXX) -c $(SRCS)
clean :
	rm -f $(TARGET)
	rm -f *.o
