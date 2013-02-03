simple-go: simple-go.cpp
	g++ simple-go.cpp `wx-config --cppflags` `wx-config --libs` -o simple-go
clean:
	rm -f simple-go
