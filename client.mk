CXX=gcc
CXX_OPTS= -g -Wall

client.o: client.c
	$(CXX) $(CXX_OPTS) -c -o $@ $<

client: client.o
	$(CXX) $(CXX_OPTS) -o $@ $^ -lgd