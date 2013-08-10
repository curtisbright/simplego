CPPFILES = $(wildcard *.cpp)
OBJFILES = $(CPPFILES:.cpp=.o)
simplego: $(OBJFILES)
	g++ $(OBJFILES) `wx-config --libs` -o simplego
%.o: %.cpp %.h simplego-panel.h simplego-frame.h
	g++ $< -c `wx-config --cxxflags`
clean:
	rm -f simplego *.o
