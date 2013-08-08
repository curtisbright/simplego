#include <wx/wx.h>
#include "simplego-app.h"
#include "simplego-frame.h"
#include "simplego-panel.h"

bool SimpleGoApp::OnInit()
{	
	SimpleGoFrame* frame = new SimpleGoFrame(wxT("Simple Go"), wxPoint(-1, -1), wxSize(-1, -1), wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER & ~wxMAXIMIZE_BOX);
	
	frame->Show();
	SetTopWindow(frame);
	
	frame->panel->boardsize = 19;
	frame->panel->initgame();
	
	return true;
} 

IMPLEMENT_APP(SimpleGoApp)
