CC = g++
CFLAGS = -std=c++17

SRCS = main.cpp sort.cpp C2B.cpp BWT_easy_vers.cpp
OBJS = $(SRCS:.cpp=.o)
HEADERS = sort.h C2B.h BWT_easy_vers.h
EXEC = exp1

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)

