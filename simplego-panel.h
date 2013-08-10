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

class SimpleGoFrame;

class SimpleGoPanel : public wxPanel
{
private:
	SimpleGoFrame* frame;
	char board[21][21];
	char (*history)[21][21];
	void SpreadArea(char board[21][21], int x, int y, int colour);
	void ScoreGame(char board[21][21]);
	void UpdateTurn();
	void RemoveGroup(char board[21][21], int x, int y);
	bool HasLibertiesRec(char board[21][21], int x, int y);
	bool HasLiberties(char board[21][21], int x, int y);
	bool ValidMove(int x, int y, char board[21][21]);
	void MakeMove(int x, int y);
	void Paint(wxPaintEvent& event);
	void DrawStone(wxDC& dc, int x, int y, int colour);
	void DrawBoard(wxDC& dc, char board[21][21]);
	void OnIdle(wxIdleEvent& event);
	void LMouseUp(wxMouseEvent& event);
	void OnKeyDown(wxKeyEvent& event);
	
public:
	wxTimer* timer;
	int curmove;
	int totmove;
	int boardsize;
	void UpdateBoard();
	void MakePass();
	void InitGame();
	SimpleGoPanel(SimpleGoFrame* parent);
};
