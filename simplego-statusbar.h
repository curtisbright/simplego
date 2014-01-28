class SimpleGoStatusBar : public wxStatusBar
{
private:
	wxRect fieldrect;
	wxSize turnsize;
	wxSize movenumsize;
	wxSize scoresize;
	wxStaticText* turntext;
	wxStaticText* movenumtext;
	wxStaticText* scoretext;
	void OnSize(wxSizeEvent& event);
public:
	SimpleGoStatusBar(SimpleGoFrame* parent);
	void SetTurn(wxString text, wxString tooltip);
	void SetMoveNum(wxString text, wxString tooltip);
	void SetScore(wxString text, wxString tooltip);
};
