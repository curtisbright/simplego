class SimpleGoPanel;

class SimpleGoFrame : public wxFrame
{	
private:
	void Pass(wxCommandEvent& event);
	void Random(wxCommandEvent& event);
	void Nothing(wxMenuEvent& event);
	void GoToMove(wxCommandEvent& event);
	void NewGame(wxCommandEvent& event);
	void SetBoard(wxCommandEvent& event);

public:
	wxMenu* game_menu;
	wxMenu* play_menu;
	SimpleGoPanel* panel;
	SimpleGoFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style);
};
