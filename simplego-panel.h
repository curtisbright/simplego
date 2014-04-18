// Board cell type constants
#define EMPTY 0
#define BLACK 1
#define WHITE 2
#define BORDER 3
// Opposite colour
#define OPP(x) (3-x)
// 'Area' cell types used in scoring process
#define AREA(x) (x+3)
#define MIXEDAREA AREA(BORDER)
// Amount of memory used to store board
#define BOARDMEMORYLEN sizeof(char[21][21])

class SimpleGoFrame;

class SimpleGoPanel : public wxPanel
{
private:
	SimpleGoFrame* frame;
	char (*history)[21][21];	// All previous board positions
	struct pos {int x, y;};
	void SpreadArea(char board[21][21], int x, int y, int colour);
	void RemoveGroup(char board[21][21], int x, int y);
	bool HasLibertiesRec(char board[21][21], int x, int y);
	bool HasLiberties(char board[21][21], int x, int y);
	bool ValidMove(char board[21][21], int x, int y);
	void Paint(wxPaintEvent& event);
	void DrawStone(wxDC& dc, int x, int y, int colour);
	void DrawBoard(wxDC& dc, char board[21][21]);
	void Idle(wxIdleEvent& event);
	void LMouseUp(wxMouseEvent& event);
	void KeyDown(wxKeyEvent& event);
	
public:
	int curmove;				// Current move number
	int totmove;				// Total number of moves
	int boardsize;				// Size of board
	pos* movelist;				// All previous moves
	char board[21][21];			// Current board position
	char gnugoboard[21][21];	// GNU Go board report
	bool gnugoscore;
	bool gnugopause;
	void ScoreArea(char board[21][21]);
	void UpdateStatus();
	void UpdateBoard();
	void MakePass();
	void MakeMove(int x, int y);
	void MakePassSGF();
	void MakeMoveSGF(int x, int y);
	void InitGame();
	SimpleGoPanel(SimpleGoFrame* parent);
};
