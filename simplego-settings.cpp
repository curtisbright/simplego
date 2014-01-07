#include <wx/wx.h>
#include "simplego-settings.h"

// Settings dialog constructor
SimpleGoSettingsDialog::SimpleGoSettingsDialog(wxWindow* parent) : wxDialog(parent, wxID_ANY, wxT("Settings"))
{	wxGridSizer* sizer = new wxGridSizer(2, 0, 0);
	SetSizer(sizer);
	wxStaticText* blackprompt = new wxStaticText(this, wxID_ANY, "Black Name:");
	wxStaticText* whiteprompt = new wxStaticText(this, wxID_ANY, "White Name:");
	wxTextCtrl* blackname = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(125, -1));
	wxTextCtrl* whitename = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(125, -1));
	sizer->Add(blackprompt, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_RIGHT, 2);
	sizer->Add(blackname, 0, wxALL | wxALIGN_CENTRE_VERTICAL, 2);
	sizer->Add(whiteprompt, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_RIGHT, 2);
	sizer->Add(whitename, 0, wxALL | wxALIGN_CENTRE_VERTICAL, 2);
	wxStaticText* blackgnugoprompt = new wxStaticText(this, wxID_ANY, "Black GNU Go:");
	wxStaticText* whitegnugoprompt = new wxStaticText(this, wxID_ANY, "White GNU Go:");
	wxString levels[11] = {"Off", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};        
	wxArrayString* str = new wxArrayString(11, levels);
	wxChoice* blacklevel = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, *str);
	wxChoice* whitelevel = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, *str);
	blacklevel->SetSelection(0);
	whitelevel->SetSelection(0);
	sizer->Add(blackgnugoprompt, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_RIGHT, 2);
	sizer->Add(blacklevel, 0, wxALL | wxALIGN_CENTRE_VERTICAL, 2);
	sizer->Add(whitegnugoprompt, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_RIGHT, 2);
	sizer->Add(whitelevel, 0, wxALL | wxALIGN_CENTRE_VERTICAL, 2);
	wxStaticText* komiprompt = new wxStaticText(this, wxID_ANY, "Komi:");
	wxTextCtrl* komi = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(125, -1));
	wxStaticText* gnugotimeoutprompt = new wxStaticText(this, wxID_ANY, "GNU Go Time:");
	wxTextCtrl* gnugotimeout = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(125, -1));
	wxCheckBox* suicide = new wxCheckBox(this, wxID_ANY, "Allow suicide");
	wxButton* okbutton = new wxButton(this, wxID_OK, "OK");
	sizer->Add(komiprompt, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_RIGHT, 2);
	sizer->Add(komi, 0, wxALL | wxALIGN_CENTRE_VERTICAL, 2);
	sizer->Add(gnugotimeoutprompt, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_RIGHT, 2);
	sizer->Add(gnugotimeout, 0, wxALL | wxALIGN_CENTRE_VERTICAL, 2);
	sizer->Add(suicide, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_CENTRE, 2);
	sizer->Add(okbutton, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_CENTRE, 2);
	sizer->Fit(this);
}
