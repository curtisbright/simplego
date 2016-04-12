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
	ID_LOAD_GAME,
	ID_SCORE_GAME,
	ID_SETTINGS
};

class SimpleGoPanel;
class SimpleGoStatusBar;

class SimpleGoFrame : public wxFrame
{	
private:
	void NewGame(wxCommandEvent& event);
	void GetBoard(wxCommandEvent& event);
	void Settings(wxCommandEvent& event);
	void Pass(wxCommandEvent& event);
	void GoToMove(wxCommandEvent& event);
	void GNUGoMove(wxCommandEvent& event);
	void ScoreGame(wxCommandEvent& event);
	void LoadGame(wxCommandEvent& event);
	void SaveGame(wxCommandEvent& event);
	void About(wxCommandEvent& event);
	void SetSize(int boardsize);
	~SimpleGoFrame();

public:
	SimpleGoPanel* panel;
	SimpleGoStatusBar* statusbar;
	wxMenu* gamemenu;
	wxMenu* playmenu;
	wxString blackname;
	wxString whitename;
	int blacklevel;
	int whitelevel;
	double komi;
	int timeout;
	bool suicide;
	bool madesuicide;
	wxString score;
	wxString scoretooltip;
	bool sgfload;
	wxString prevblackname;
	wxString prevwhitename;
	double prevkomi;
	void MakeGNUGoMove();
	void MakeGNUGoScore();
	void PlaySGF(wxString filename);
	SimpleGoFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style);
};
