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
	void SetTurn(wxString text);
	void SetMoveNum(wxString text);
	void SetScore(wxString text);
};
