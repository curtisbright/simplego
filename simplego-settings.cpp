#include <wx/wx.h>
#include "simplego-frame.h"
#include "simplego-settings.h"

// Settings dialog constructor
SimpleGoSettingsDialog::SimpleGoSettingsDialog(SimpleGoFrame* parent) : wxDialog(parent, wxID_ANY, wxT("Settings"))
{	frame = parent;
	wxGridSizer* sizer = new wxGridSizer(2, 0, 0);
	SetSizer(sizer);
	
	wxStaticText* blacknameprompt = new wxStaticText(this, wxID_ANY, "Black Name:");
	wxStaticText* whitenameprompt = new wxStaticText(this, wxID_ANY, "White Name:");
	wxStaticText* blacklevelprompt = new wxStaticText(this, wxID_ANY, "Black GNU Go:");
	wxStaticText* whitelevelprompt = new wxStaticText(this, wxID_ANY, "White GNU Go:");
	wxStaticText* timeoutprompt = new wxStaticText(this, wxID_ANY, "GNU Go Time:");
	wxStaticText* komiprompt = new wxStaticText(this, wxID_ANY, "Komi:");
	wxStaticText* suicideprompt = new wxStaticText(this, wxID_ANY, "Suicide:");
	wxButton* cancelbutton = new wxButton(this, ID_CANCEL, "Cancel");
	wxButton* okbutton = new wxButton(this, ID_OK, "OK");

	wxString levels[11] = {"Off", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};        
	wxString offon[2] = {"Off", "On"};
	wxArrayString* arrlevels = new wxArrayString(11, levels);
	wxArrayString* arroffon = new wxArrayString(2, offon);
	
	blackname = new wxTextCtrl(this, wxID_ANY, frame->blackname, wxDefaultPosition, wxSize(100, -1));
	whitename = new wxTextCtrl(this, wxID_ANY, frame->whitename, wxDefaultPosition, wxSize(100, -1));
	blacklevel = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(100, -1), *arrlevels);
	whitelevel = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(100, -1), *arrlevels);
	timeout = new wxTextCtrl(this, wxID_ANY, wxString::FromDouble(frame->timeout), wxDefaultPosition, wxSize(100, -1));
	komi = new wxTextCtrl(this, wxID_ANY, wxString::FromDouble(frame->komi), wxDefaultPosition, wxSize(100, -1));
	suicide = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(100, -1), *arroffon);
	
	blacklevel->SetSelection(frame->blacklevel);
	whitelevel->SetSelection(frame->whitelevel);
	suicide->SetSelection(frame->suicide);
	
	sizer->Add(blacknameprompt, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_RIGHT, 2);
	sizer->Add(blackname, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_CENTRE, 2);
	sizer->Add(whitenameprompt, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_RIGHT, 2);
	sizer->Add(whitename, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_CENTRE, 2);
	sizer->Add(blacklevelprompt, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_RIGHT, 2);
	sizer->Add(blacklevel, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_CENTRE, 2);
	sizer->Add(whitelevelprompt, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_RIGHT, 2);
	sizer->Add(whitelevel, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_CENTRE, 2);
	sizer->Add(timeoutprompt, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_RIGHT, 2);
	sizer->Add(timeout, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_CENTRE, 2);
	sizer->Add(komiprompt, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_RIGHT, 2);
	sizer->Add(komi, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_CENTRE, 2);
	sizer->Add(suicideprompt, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_RIGHT, 2);
	sizer->Add(suicide, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_CENTRE, 2);
	sizer->Add(cancelbutton, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_CENTRE, 2);
	sizer->Add(okbutton, 0, wxALL | wxALIGN_CENTRE_VERTICAL | wxALIGN_CENTRE, 2);
	sizer->Fit(this);
	
	#ifdef __WXMSW__
	blacklevel->Enable(false);
	whitelevel->Enable(false);
	timeout->Enable(false);
	#endif
	
	Connect(ID_CANCEL, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SimpleGoSettingsDialog::CancelClick));
	Connect(ID_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SimpleGoSettingsDialog::OKClick));
	Connect(wxEVT_CHAR_HOOK, wxKeyEventHandler(SimpleGoSettingsDialog::KeyDown));
}

// Process a click on OK by closing the dialog
void SimpleGoSettingsDialog::CancelClick(wxCommandEvent& event)
{	EndModal(0);
}

// Process a click on OK by updating settings and closing the dialog
void SimpleGoSettingsDialog::OKClick(wxCommandEvent& event)
{	UpdateSettings();
	EndModal(1);
}

// Process a key press
void SimpleGoSettingsDialog::KeyDown(wxKeyEvent& event)
{	if(event.GetKeyCode()==WXK_RETURN||event.GetKeyCode()==WXK_NUMPAD_ENTER)
	{	UpdateSettings();
		EndModal(1);
	}
	else if(event.GetKeyCode()==WXK_ESCAPE)
		EndModal(0);
	event.Skip();
}

// Update settings
void SimpleGoSettingsDialog::UpdateSettings()
{	frame->blackname = blackname->GetValue();
	frame->whitename = whitename->GetValue();
	frame->blacklevel = blacklevel->GetSelection();
	frame->whitelevel = whitelevel->GetSelection();
	frame->komi = wxAtof(wxString::Format("%.1f", wxAtof(komi->GetValue())));
	frame->timeout = wxAtoi(timeout->GetValue());
	frame->suicide = suicide->GetSelection();
}
