WXDIR = #~/wxWidgets-3.0.0/
WXDIRMSW = ~/wxWidgets-3.0.0-msw/
DATE = $(shell date +%y.%m.%d)
simplego: simplego-app.o simplego-frame.o simplego-panel.o simplego-settings.o simplego-statusbar.o
	g++ simplego-app.o simplego-frame.o simplego-panel.o simplego-settings.o simplego-statusbar.o `$(WXDIR)wx-config --libs` -o simplego
simplego-app.o: simplego-app.cpp simplego-app.h simplego-frame.h
	g++ simplego-app.cpp -c `$(WXDIR)wx-config --cxxflags`
simplego-frame.o: simplego-frame.cpp simplego-frame.h simplego-panel.h simplego-statusbar.h
	g++ simplego-frame.cpp -c `$(WXDIR)wx-config --cxxflags` -DVERSION=\"$(DATE)\"
simplego-panel.o: simplego-panel.cpp simplego-frame.h simplego-panel.h simplego-settings.h simplego-statusbar.h
	g++ simplego-panel.cpp -c `$(WXDIR)wx-config --cxxflags`
simplego-settings.o: simplego-settings.cpp simplego-frame.h
	g++ simplego-settings.cpp -c `$(WXDIR)wx-config --cxxflags`
simplego-statusbar.o: simplego-statusbar.cpp simplego-frame.h
	g++ simplego-statusbar.cpp -c `$(WXDIR)wx-config --cxxflags`
simplego.exe: simplego-app-msw.o simplego-frame-msw.o simplego-panel-msw.o simplego-settings-msw.o simplego-statusbar-msw.o
	i586-mingw32msvc-g++ simplego-app-msw.o simplego-frame-msw.o simplego-panel-msw.o simplego-settings-msw.o simplego-statusbar-msw.o `$(WXDIRMSW)wx-config --libs` -o simplego.exe
simplego-app-msw.o: simplego-app.cpp simplego-app.h simplego-frame.h
	i586-mingw32msvc-g++ simplego-app.cpp -c `$(WXDIRMSW)wx-config --cxxflags` -o simplego-app-msw.o
simplego-frame-msw.o: simplego-frame.cpp simplego-frame.h simplego-panel.h simplego-statusbar.h
	i586-mingw32msvc-g++ simplego-frame.cpp -c `$(WXDIRMSW)wx-config --cxxflags` -DVERSION=\"$(DATE)\" -o simplego-frame-msw.o
simplego-panel-msw.o: simplego-panel.cpp simplego-frame.h simplego-panel.h simplego-settings.h simplego-statusbar.h
	i586-mingw32msvc-g++ simplego-panel.cpp -c `$(WXDIRMSW)wx-config --cxxflags` -o simplego-panel-msw.o
simplego-settings-msw.o: simplego-settings.cpp simplego-frame.h
	i586-mingw32msvc-g++ simplego-settings.cpp -c `$(WXDIRMSW)wx-config --cxxflags` -o simplego-settings-msw.o
simplego-statusbar-msw.o: simplego-statusbar.cpp simplego-frame.h
	i586-mingw32msvc-g++ simplego-statusbar.cpp -c `$(WXDIRMSW)wx-config --cxxflags` -o simplego-statusbar-msw.o
clean:
	rm -f simplego-app.o simplego-frame.o simplego-panel.o simplego-settings.o simplego-statusbar.o simplego-app-msw.o simplego-frame-msw.o simplego-panel-msw.o simplego-settings-msw.o simplego-statusbar-msw.o
clear: clean
	rm -f simplego simplego.exe
