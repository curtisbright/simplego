#include <wx/wx.h>
#include "simplego-app.h"
#include "simplego-frame.h"

bool SimpleGoApp::OnInit()
{	SimpleGoFrame* frame = new SimpleGoFrame(wxT("Simple Go"), wxPoint(-1, -1), wxSize(-1, -1), wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER & ~wxMAXIMIZE_BOX);
	frame->Show();
	return true;
} 

IMPLEMENT_APP(SimpleGoApp)
