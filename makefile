compile:
	g++ go-points.cpp `wx-config --cppflags` `wx-config --libs` -o go-points
clean:
	rm -f go-points
