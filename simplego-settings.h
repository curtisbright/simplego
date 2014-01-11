class SimpleGoSettingsDialog : public wxDialog
{
private:
	enum {ID_OK=wxID_HIGHEST+1};
	SimpleGoFrame* frame;
	wxTextCtrl* blackname;
	wxTextCtrl* whitename;
	wxChoice* blacklevel;
	wxChoice* whitelevel;
	wxTextCtrl* komi;
	wxTextCtrl* timeout;
	wxCheckBox* suicide;
	void OKClick(wxCommandEvent& event);
	void KeyDown(wxKeyEvent& event);
	void UpdateSettings();
public:
	SimpleGoSettingsDialog(SimpleGoFrame* parent);
};
