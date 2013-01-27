compile: go-points.cpp
	g++ go-points.cpp `wx-config --cppflags` `wx-config --libs` -O3 -o go-points
debug: go-points.cpp
	g++ go-points.cpp `wx-config --cppflags` `wx-config --libs` -o go-points -g -pg
clean:
	rm -f go-points
