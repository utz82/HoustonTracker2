CXXFLAGS	= -Wall -g -o
LDFLAGS		= `wx-config --cflags --libs`

ht2util-gui: wxht2util.cpp ht2util-gui.h
	g++ $(CXXFLAGS) ht2util-gui wxht2util.cpp $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf ht2util-gui
