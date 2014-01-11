class SimpleGoSettingsDialog : public wxDialog
{
private:
	enum {ID_OK=wxID_HIGHEST+1,ID_CANCEL};
	SimpleGoFrame* frame;
	wxTextCtrl* blackname;
	wxTextCtrl* whitename;
	wxChoice* blacklevel;
	wxChoice* whitelevel;
	wxTextCtrl* timeout;
	wxTextCtrl* komi;
	wxChoice* suicide;
	void CancelClick(wxCommandEvent& event);
	void OKClick(wxCommandEvent& event);
	void KeyDown(wxKeyEvent& event);
	void UpdateSettings();
public:
	SimpleGoSettingsDialog(SimpleGoFrame* parent);
};
