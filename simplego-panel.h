// Board cell type constants
#define EMPTY 0
#define BLACK 1
#define WHITE 2
#define BORDER 3
// Opposite colour
#define OPP(x) (3-x)
// Amount of memory used to store board
#define BOARDMEMORYLEN sizeof(char[21][21])

class SimpleGoFrame;

class SimpleGoPanel : public wxPanel
{
private:
	SimpleGoFrame* frame;
	char (*history)[21][21];	// All previous board positions
	struct pos {int x, y;};
	void UpdateStatus();
	bool ValidMove(char board[21][21], int x, int y, int& captures);
	void Paint(wxPaintEvent& event);
	void DrawStone(wxDC& dc, int x, int y, int colour);
	void DrawBoard(wxDC& dc, char board[21][21]);
	void Idle(wxIdleEvent& event);
	void LMouseUp(wxMouseEvent& event);
	void KeyDown(wxKeyEvent& event);
	bool HasNInARow(char board[21][21], int n, int x, int y, int xd, int yd, int colour);
	bool HasPente(char board[21][21]);
	int (*capturehist)[2];
	
public:
	int curmove;				// Current move number
	int totmove;				// Total number of moves
	bool finishedgame;
	int boardsize;				// Size of board
	pos* movelist;				// All previous moves
	char board[21][21];			// Current board position
	void UpdateBoard();
	void MakeMove(int x, int y);
	void MakeMoveSGF(int x, int y);
	void InitGame();
	SimpleGoPanel(SimpleGoFrame* parent);
};
