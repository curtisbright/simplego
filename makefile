WXDIR = #~/wxWidgets-2.9.5/
DATE = $(shell date +%y.%m.%d)
simplego: simplego-app.o simplego-frame.o simplego-panel.o
	g++ simplego-app.o simplego-frame.o simplego-panel.o `$(WXDIR)wx-config --libs` -o simplego
simplego-app.o: simplego-app.cpp simplego-app.h simplego-frame.h
	g++ simplego-app.cpp -c `$(WXDIR)wx-config --cxxflags`
simplego-frame.o: simplego-frame.cpp simplego-frame.h simplego-panel.h
	g++ simplego-frame.cpp -c `$(WXDIR)wx-config --cxxflags` -DVERSION=\"$(DATE)\"
simplego-panel.o: simplego-panel.cpp simplego-frame.h simplego-panel.h
	g++ simplego-panel.cpp -c `$(WXDIR)wx-config --cxxflags`
clean:
	rm -f simplego-app.o simplego-frame.o simplego-panel.o
clear: clean
	rm -f simplego
