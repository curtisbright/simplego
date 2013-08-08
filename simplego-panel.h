#define EMPTY 0
#define BLACK 1
#define WHITE 2
#define OPP(x) (3-x)
#define BORDER 3
#define AREAOFFSET 3
#define BLACKAREA 4
#define WHITEAREA 5
#define MIXEDAREA 6
#define BOARDMEMORYLEN sizeof(char[21][21])

enum {
	ID_PASS = wxID_HIGHEST+1,
	ID_GO_TO_MOVE = wxID_HIGHEST+2,
	ID_NEW_GAME = wxID_HIGHEST+3,
	ID_BOARD_SIZE = wxID_HIGHEST+4,
	ID_RANDOM = wxID_HIGHEST+5,
	ID_SUICIDE = wxID_HIGHEST+6
};

class SimpleGoFrame;

class SimpleGoPanel : public wxPanel
{
public:
	int curmove;
	int totmove;
	int boardsize;
	char board[21][21];
	char (*history)[21][21];
	SimpleGoFrame* frame;

	void updateboard();
	void spreadarea(char board[21][21], int x, int y, int colour);
	void scoregame(char board[21][21]);
	void updateturn();
	void removegroup(char board[21][21], int x, int y);
	bool haslibertiesrec(char board[21][21], int x, int y);
	bool hasliberties(char board[21][21], int x, int y);
	bool validmove(int x, int y, char board[21][21]);
	void makemove(int x, int y);
	void makepass();
	void initgame();

	wxTimer* timer;
	void LMouseUp(wxMouseEvent& event);
	void Paint(wxPaintEvent& evt);
	void DrawStone(wxDC& dc, int x, int y, int colour);
	void DrawBoard(wxDC& dc, char board[21][21]);
	void OnIdle(wxIdleEvent& event);
	void onKeyDown(wxKeyEvent& aEvent);
	SimpleGoPanel(SimpleGoFrame* parent);
};
