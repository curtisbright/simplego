#include <wx/wx.h>
#include "simplego-settings.h"

// Settings dialog constructor
SimpleGoSettingsDialog::SimpleGoSettingsDialog(wxWindow* parent) : wxDialog(parent, wxID_ANY, wxT("Settings"))
{	wxBoxSizer* vsizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(vsizer);
	wxBoxSizer* blackhsizer = new wxStaticBoxSizer(wxHORIZONTAL, this, "Black");
	wxBoxSizer* whitehsizer = new wxStaticBoxSizer(wxHORIZONTAL, this, "White");
	vsizer->Add(blackhsizer, 0, wxALL | wxALIGN_CENTRE_VERTICAL, 5);
	vsizer->Add(whitehsizer, 0, wxALL | wxALIGN_CENTRE_VERTICAL, 5);
	wxStaticText* blackprompt = new wxStaticText(this, wxID_ANY, "Name:");
	wxStaticText* whiteprompt = new wxStaticText(this, wxID_ANY, "Name:");
	blackhsizer->Add(blackprompt, 0, wxALL | wxALIGN_CENTRE_VERTICAL, 5);
	whitehsizer->Add(whiteprompt, 0, wxALL | wxALIGN_CENTRE_VERTICAL, 5);
	wxTextCtrl* blackname = new wxTextCtrl(this, wxID_ANY);
	wxTextCtrl* whitename = new wxTextCtrl(this, wxID_ANY);
	blackhsizer->Add(blackname, 0, wxALL | wxALIGN_CENTRE_VERTICAL, 5);
	whitehsizer->Add(whitename, 0, wxALL | wxALIGN_CENTRE_VERTICAL, 5);
	wxCheckBox* blackgnugo = new wxCheckBox(this, wxID_ANY, "GNU Go");
	wxCheckBox* whitegnugo = new wxCheckBox(this, wxID_ANY, "GNU Go");
	blackhsizer->Add(blackgnugo, 0, wxALL | wxALIGN_CENTRE_VERTICAL, 5);
	whitehsizer->Add(whitegnugo, 0, wxALL | wxALIGN_CENTRE_VERTICAL, 5);
	wxString levels[11] = {"Default level", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};        
	wxArrayString* str = new wxArrayString(11, levels);
	wxChoice* blacklevel = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, *str);
	wxChoice* whitelevel = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, *str);
	blacklevel->SetSelection(0);
	whitelevel->SetSelection(0);
	blackhsizer->Add(blacklevel, 0, wxALL | wxALIGN_CENTRE_VERTICAL, 5);
	whitehsizer->Add(whitelevel, 0, wxALL | wxALIGN_CENTRE_VERTICAL, 5);
	vsizer->Fit(this);
}
