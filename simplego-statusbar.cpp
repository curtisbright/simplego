#include <wx/wx.h>
#include "simplego-frame.h"
#include "simplego-statusbar.h"

// Application initialization
SimpleGoStatusBar::SimpleGoStatusBar(SimpleGoFrame* parent) : wxStatusBar(parent, wxID_ANY, wxFULL_REPAINT_ON_RESIZE)
{	movenumtext = new wxStaticText(this, wxID_ANY, "");
	turntext = new wxStaticText(this, wxID_ANY, "");
	scoretext = new wxStaticText(this, wxID_ANY, "");
	Connect(wxEVT_SIZE, wxSizeEventHandler(SimpleGoStatusBar::OnSize));
} 

void SimpleGoStatusBar::OnSize(wxSizeEvent& event)
{	GetFieldRect(0, fieldrect);
	turntext->Move(fieldrect.x + 3, fieldrect.y + (fieldrect.height - turnsize.y + 1)/2);
	movenumtext->Move(fieldrect.x + (fieldrect.width - movenumsize.x) - 3, fieldrect.y + (fieldrect.height - movenumsize.y + 1)/2);
	scoretext->Move(fieldrect.x + turnsize.x + (fieldrect.width - turnsize.x - movenumsize.x - scoresize.x)/2, fieldrect.y + (fieldrect.height - scoresize.y + 1)/2);
	event.Skip();
}

void SimpleGoStatusBar::SetMoveNum(wxString text, wxString tooltip)
{	movenumtext->SetToolTip(tooltip);
	movenumtext->SetLabel(text);
	movenumsize = movenumtext->GetSize();
	movenumtext->Move(fieldrect.x + (fieldrect.width - movenumsize.x) - 3, fieldrect.y + (fieldrect.height - movenumsize.y + 1)/2);
}

void SimpleGoStatusBar::SetTurn(wxString text, wxString tooltip)
{	turntext->SetToolTip(tooltip);
	turntext->SetLabel(text);
	turnsize = turntext->GetSize();
	turntext->Move(fieldrect.x + 3, fieldrect.y + (fieldrect.height - turnsize.y + 1)/2);
}

void SimpleGoStatusBar::SetScore(wxString text, wxString tooltip)
{	scoretext->SetToolTip(tooltip);
	scoretext->SetLabel(text);
	scoresize = scoretext->GetSize();
	scoretext->Move(fieldrect.x + turnsize.x + (fieldrect.width - turnsize.x - movenumsize.x - scoresize.x)/2, fieldrect.y + (fieldrect.height - scoresize.y + 1)/2);
}
