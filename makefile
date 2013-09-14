WXDIR = #~/wxWidgets-2.9.5/
simplego.exe: simplego-app.o simplego-frame.o simplego-panel.o
	i586-mingw32msvc-g++ simplego-app.o simplego-frame.o simplego-panel.o `$(WXDIR)wx-config --libs` -o simplego.exe
simplego-app.o: simplego-app.cpp simplego-app.h simplego-frame.h
	i586-mingw32msvc-g++ simplego-app.cpp -c `$(WXDIR)wx-config --cxxflags`
simplego-frame.o: simplego-frame.cpp simplego-frame.h simplego-panel.h
	i586-mingw32msvc-g++ simplego-frame.cpp -c `$(WXDIR)wx-config --cxxflags`
simplego-panel.o: simplego-panel.cpp simplego-frame.h simplego-panel.h
	i586-mingw32msvc-g++ simplego-panel.cpp -c `$(WXDIR)wx-config --cxxflags`
clean:
	rm -f simplego-app.o simplego-frame.o simplego-panel.o
clear: clean
	rm -f simplego.exe
