#include <wx/wx.h>
#include "simplego-frame.h"
#include "simplego-settings.h"

// Settings dialog constructor
SimpleGoSettingsDialog::SimpleGoSettingsDialog(SimpleGoFrame* parent) : wxDialog(parent, wxID_ANY, wxT("Settings"))
{	frame = parent;
	wxGridSizer* sizer = new wxGridSizer(2, 0, 0);
	SetSizer(sizer);
	wxStaticText* blackprompt = new wxStaticText(this, wxID_ANY, "Black Name:");
	wxStaticText* whiteprompt = new wxStaticText(this, wxID_ANY, "White Name:");
	blackname = new wxTextCtrl(this, wxID_ANY, frame->blackname, wxDefaultPosition, wxSize(100, -1));
	whitename = new wxTextCtrl(this, wxID_ANY, frame->whitename, wxDefaultPosition, wxSize(100, -1));
	sizer->Add(blackprompt, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_RIGHT, 2);
	sizer->Add(blackname, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_CENTRE, 2);
	sizer->Add(whiteprompt, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_RIGHT, 2);
	sizer->Add(whitename, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_CENTRE, 2);
	wxStaticText* blackgnugoprompt = new wxStaticText(this, wxID_ANY, "Black GNU Go:");
	wxStaticText* whitegnugoprompt = new wxStaticText(this, wxID_ANY, "White GNU Go:");
	wxString levels[11] = {"Off", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};        
	wxArrayString* str = new wxArrayString(11, levels);
	blacklevel = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(100, -1), *str);
	whitelevel = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(100, -1), *str);
	blacklevel->SetSelection(frame->blacklevel);
	whitelevel->SetSelection(frame->whitelevel);
	sizer->Add(blackgnugoprompt, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_RIGHT, 2);
	sizer->Add(blacklevel, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_CENTRE, 2);
	sizer->Add(whitegnugoprompt, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_RIGHT, 2);
	sizer->Add(whitelevel, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_CENTRE, 2);
	wxStaticText* komiprompt = new wxStaticText(this, wxID_ANY, "Komi:");
	komi = new wxTextCtrl(this, wxID_ANY, wxString::FromDouble(frame->komi), wxDefaultPosition, wxSize(100, -1));
	wxStaticText* timeoutprompt = new wxStaticText(this, wxID_ANY, "GNU Go Time:");
	timeout = new wxTextCtrl(this, wxID_ANY, wxString::FromDouble(frame->timeout), wxDefaultPosition, wxSize(100, -1));
	suicide = new wxCheckBox(this, wxID_ANY, "Allow suicide");
	suicide->SetValue(frame->suicide);
	wxButton* okbutton = new wxButton(this, ID_OK, "OK");
	sizer->Add(komiprompt, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_RIGHT, 2);
	sizer->Add(komi, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_CENTRE, 2);
	sizer->Add(timeoutprompt, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_RIGHT, 2);
	sizer->Add(timeout, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_CENTRE, 2);
	sizer->Add(suicide, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_RIGHT, 2);
	sizer->Add(okbutton, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_CENTRE, 2);
	sizer->Fit(this);
	
	Connect(ID_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SimpleGoSettingsDialog::UpdateSettings));
}

// Update settings and close the dialog
void SimpleGoSettingsDialog::UpdateSettings(wxCommandEvent& event)
{	frame->blackname = blackname->GetValue();
	frame->whitename = whitename->GetValue();
	frame->blacklevel = blacklevel->GetSelection();
	frame->whitelevel = whitelevel->GetSelection();
	frame->komi = wxAtof(komi->GetValue());
	frame->timeout = wxAtof(timeout->GetValue());
	frame->suicide = suicide->GetValue();
	EndModal(0);
}
