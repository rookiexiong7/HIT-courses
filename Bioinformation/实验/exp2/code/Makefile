CC = g++
CFLAGS = -std=c++17
SRCS = main.cpp LFindex.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC = exp2

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)
