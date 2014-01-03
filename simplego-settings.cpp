#include <wx/wx.h>
#include "simplego-settings.h"

// Settings dialog constructor
SimpleGoSettingsDialog::SimpleGoSettingsDialog(wxWindow* parent) : wxDialog(parent, wxID_ANY, wxT("Settings"), wxPoint(80,80), wxSize(200,200))
{	wxBoxSizer* vsizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(vsizer);
	wxBoxSizer* blackhsizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* whitehsizer = new wxBoxSizer(wxHORIZONTAL);
	vsizer->Add(blackhsizer);
	vsizer->Add(whitehsizer);
	wxStaticText* blackprompt = new wxStaticText(this, wxID_ANY, "Black Name:");
	wxStaticText* whiteprompt = new wxStaticText(this, wxID_ANY, "White Name:");
	blackhsizer->Add(blackprompt);
	whitehsizer->Add(whiteprompt);
}
