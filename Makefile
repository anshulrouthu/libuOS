
OBJS:=$(patsubst %.cpp, %.o, $(wildcard *.cpp))
OBJS+=$(patsubst %.c, %.o, $(wildcard *.c))
CFLAGS:=-g

.PHONY: all
all: main

main: $(OBJS)
	@echo "Linking... $@"
	@$(CC) $(CFLAGS) $^ -o $@

%.o: %.cpp
	  @echo "[CXX] $@"
	  @$(CXX) $(CFLAGS) $(INC) -c $< -o $@

%.o: %.c
	 @echo "[CXX] $@"
	 @$(CC) $(CFLAGS) $(INC) -c $< -o $@

.PHONY: clean
clean:
	rm -rf *.o main