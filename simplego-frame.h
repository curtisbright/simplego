#ifndef VERSION
#define VERSION ""
#endif

// Menu id constants
enum {
	ID_NEW_GAME = wxID_HIGHEST+1,
	ID_BOARD_SIZE,
	ID_PASS,
	ID_GO_TO_MOVE,
	ID_RANDOM,
	ID_SAVE_GAME,
	ID_ABOUT,
	ID_GNUGO,
	ID_GNUGO_WHITE,
	ID_LOAD_GAME,
	ID_SCORE_GAME,
	ID_SETTINGS
};

class SimpleGoPanel;

class SimpleGoFrame : public wxFrame
{	
private:
	void NewGame(wxCommandEvent& event);
	void GetBoard(wxCommandEvent& event);
	void Settings(wxCommandEvent& event);
	void Pass(wxCommandEvent& event);
	void GoToMove(wxCommandEvent& event);
	void GNUGoMove(wxCommandEvent& event);
	void GNUGoWhite(wxCommandEvent& event);
	void ScoreGame(wxCommandEvent& event);
	void LoadGame(wxCommandEvent& event);
	void SaveGame(wxCommandEvent& event);
	void About(wxCommandEvent& event);
	void Nothing(wxMenuEvent& event);
	void SetSize(int boardsize);

public:
	wxMenu* gamemenu;
	wxMenu* playmenu;
	wxString blackname;
	wxString whitename;
	int blacklevel;
	int whitelevel;
	double komi;
	int timeout;
	bool suicide;
	void MakeGNUGoMove();
	void MakeGNUGoScore();
	SimpleGoPanel* panel;
	SimpleGoFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style);
};
