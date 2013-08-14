// Menu id constants
enum {
	ID_NEW_GAME = wxID_HIGHEST+1,
	ID_BOARD_SIZE = wxID_HIGHEST+2,
	ID_PASS = wxID_HIGHEST+3,
	ID_GO_TO_MOVE = wxID_HIGHEST+4,
	ID_RANDOM = wxID_HIGHEST+5,
	ID_SUICIDE = wxID_HIGHEST+6
};

class SimpleGoPanel;

class SimpleGoFrame : public wxFrame
{	
private:
	void NewGame(wxCommandEvent& event);
	void SetBoard(wxCommandEvent& event);
	void Pass(wxCommandEvent& event);
	void GoToMove(wxCommandEvent& event);
	void Random(wxCommandEvent& event);
	void Nothing(wxMenuEvent& event);

public:
	wxMenu* gamemenu;
	wxMenu* playmenu;
	SimpleGoPanel* panel;
	SimpleGoFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style);
};
