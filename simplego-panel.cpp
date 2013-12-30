#include <wx/wx.h>
#include "simplego-panel.h"
#include "simplego-frame.h"

// Panel constructor
SimpleGoPanel::SimpleGoPanel(SimpleGoFrame* parent) : wxPanel(parent)
{	frame = parent;
	boardsize = 19;
	history = NULL;
	movelist = NULL;
	SetBackgroundColour(*wxWHITE);
	Connect(wxEVT_PAINT, wxPaintEventHandler(SimpleGoPanel::Paint));
	Connect(wxEVT_IDLE, wxIdleEventHandler(SimpleGoPanel::Idle), NULL, this);
	Connect(wxEVT_LEFT_UP, wxMouseEventHandler(SimpleGoPanel::LMouseUp));
	Connect(wxEVT_CHAR_HOOK, wxKeyEventHandler(SimpleGoPanel::KeyDown));
}

// Update turn and move info on status bar
void SimpleGoPanel::UpdateStatus()
{	if(capturehist[curmove][0]>=5 || capturehist[curmove][1]>=5 || HasPente(board))
		frame->SetStatusText(wxString::Format("Winner: %s", curmove%2==1 ? "Black" : "White"), 0);
	else
		frame->SetStatusText(wxString::Format("Turn: %s", curmove%2==0 ? "Black" : "White"), 0);
	frame->SetStatusText(wxString::Format("Move: %d", curmove), 1);
	frame->SetStatusText(wxString::Format("Captures: %d-%d", capturehist[curmove][0], capturehist[curmove][1]), 2);
}

// Determine if a move at cell (x, y) is valid on the given board
bool SimpleGoPanel::ValidMove(char board[21][21], int x, int y, int& captures)
{	if(board[x][y]!=EMPTY || capturehist[curmove][0]>=5 || capturehist[curmove][1]>=5 || HasPente(board))
		return false;
		
	board[x][y] = curmove%2+1;
	int colour = board[x][y];
	int oppcolour = OPP(colour);
	for(int i=-1; i<=1; i++)
		for(int j=-1; j<=1; j++)
			if(x+3*i>=1 && y+3*j>=1 && x+3*i<=boardsize && y+3*j<=boardsize)
			{	if(board[x+3*i][y+3*j]==colour && board[x+2*i][y+2*j]==oppcolour && board[x+i][y+j]==oppcolour)
				{	board[x+2*i][y+2*j] = EMPTY;
					board[x+i][y+j] = EMPTY;
					captures++;
				}
			}
			
	return true;
}

// Process paint event by redrawing the board
void SimpleGoPanel::Paint(wxPaintEvent& event)
{	wxPaintDC dc(this);
	DrawBoard(dc, board);
}

// Draw the stone type of colour in cell (x, y) using the provided device context
void SimpleGoPanel::DrawStone(wxDC& dc, int x, int y, int colour)
{	if(colour==EMPTY)
	{	dc.SetBrush(*wxWHITE_BRUSH);
		dc.SetPen(*wxWHITE_PEN);
		dc.DrawRectangle(16*x-8, 16*y-8, 8, 8);
		dc.DrawRectangle(16*x-8, 16*y+1, 8, 7);
		dc.DrawRectangle(16*x+1, 16*y-8, 7, 8);
		dc.DrawRectangle(16*x+1, 16*y+1, 7, 7);
		if(x==1)
			dc.DrawLine(16*x-8, 16*y, 16*x, 16*y);
		else if(x==boardsize)
			dc.DrawLine(16*x+1, 16*y, 16*x+8, 16*y);
		if(y==1)
			dc.DrawLine(16*x, 16*y-8, 16*x, 16*y);
		else if(y==boardsize)
			dc.DrawLine(16*x, 16*y+1, 16*x, 16*y+8);
		dc.SetPen(*wxBLACK_PEN);
		dc.DrawLine(16*x-(x>1?8:0),16*y,16*x+(x<boardsize?8:1),16*y);
		dc.DrawLine(16*x,16*y-(y>1?8:0),16*x,16*y+(y<boardsize?8:1));
		dc.SetBrush(*wxBLACK_BRUSH);
		if((boardsize==7&&(x==3||x==boardsize-2)&&(y==3||y==boardsize-2))
			||(boardsize>7&&boardsize<13&&(x==3||x==boardsize-2||2*x==boardsize+1)&&(y==3||y==boardsize-2||2*y==boardsize+1))
			||(boardsize>=13&&(x==4||x==boardsize-3||2*x==boardsize+1)&&(y==4||y==boardsize-3||2*y==boardsize+1)))
			dc.DrawRectangle(16*x-1, 16*y-1, 3, 3);
	}
	else if(colour==BLACK)
	{	dc.SetPen(*wxBLACK_PEN);
		dc.DrawLine(16*x-(x>1?8:0),16*y,16*x+(x<boardsize?8:0),16*y);
		dc.DrawLine(16*x,16*y-(y>1?8:1),16*x,16*y+(y<boardsize?8:1));
		dc.SetBrush(*wxBLACK_BRUSH);
		dc.DrawCircle(wxPoint(16*x,16*y), 6);
	}
	else if(colour==WHITE)
	{	dc.SetBrush(*wxWHITE_BRUSH);
		dc.SetPen(*wxBLACK_PEN);
		dc.DrawLine(16*x-(x>1?8:0),16*y,16*x-(x>1?6:0),16*y);
		dc.DrawLine(16*x,16*y-(y>1?8:0),16*x,16*y-(y>1?6:0));
		dc.DrawLine(16*x+(x<boardsize?6:0),16*y,16*x+(x<boardsize?8:0),16*y);
		dc.DrawLine(16*x,16*y+(y<boardsize?6:0),16*x,16*y+(y<boardsize?8:0));
		dc.DrawCircle(wxPoint(16*x,16*y), 6);
	}
}

// Draw the given board using the provided device context
void SimpleGoPanel::DrawBoard(wxDC& dc, char board[21][21])
{	for(int i=0; i<21; i++)
		for(int j=0; j<21; j++)
		{	if(i>=1&&j>=1&&i<=boardsize&&j<=boardsize)
			{	DrawStone(dc, i, j, board[i][j]);
			}
			else
			{	dc.SetBrush(*wxWHITE_BRUSH);
				dc.SetPen(*wxWHITE_PEN);
				dc.DrawRectangle(16*i-8, 16*j-8, 16, 16);
			}
		}
}

// During idle time make random moves if Random! has been selected
void SimpleGoPanel::Idle(wxIdleEvent& event)
{	if(!frame->playmenu->IsChecked(ID_RANDOM))
		return;

	int x = 1+rand()%boardsize, y = 1+rand()%boardsize;
	char attempts[21][21], temp[21][21];
	memset(attempts, 0, BOARDMEMORYLEN);
	int count = 0;
	while(count<boardsize*boardsize)
	{	while(attempts[x][y]==1)
		{	x = 1+rand()%boardsize;
			y = 1+rand()%boardsize;
		}
		memcpy(temp, board, BOARDMEMORYLEN);
		int captures = 0;
		if(ValidMove(temp, x, y, captures))
		{	MakeMove(x, y);
			#ifdef __WXMSW__
			event.RequestMore();
			#endif
			return;
		}
		count++;
		attempts[x][y] = 1;
	}

	frame->playmenu->Check(ID_RANDOM, false);
	#ifdef __WXMSW__
	event.RequestMore();
	#endif
}

// Process a left mouse click by making a move attempt at the appropriate cell
void SimpleGoPanel::LMouseUp(wxMouseEvent& event)
{	int x, y;
	x = (event.GetPosition().x+8)/16;
	y = (event.GetPosition().y+8)/16;
	MakeMove(x, y);
}

// Process a key press
// left/right moves through history
void SimpleGoPanel::KeyDown(wxKeyEvent& event)
{	if(event.GetKeyCode()==WXK_LEFT&&curmove>0)
	{	curmove--;
		UpdateBoard();
	}
	else if(event.GetKeyCode()==WXK_RIGHT&&curmove<totmove)
	{	curmove++;
		UpdateBoard();
	}
	event.Skip();
}

// Check if the given board contains n in a row with given position, direction, and colour
bool SimpleGoPanel::HasNInARow(char board[21][21], int n, int x, int y, int xd, int yd, int colour)
{	if(!(x>=1 && y>=1 && x<=boardsize && y<=boardsize) || board[x][y]!=colour)
		return false;
	if(n==1)
		return true;
	return HasNInARow(board, n-1, x+xd, y+yd, xd, yd, colour);
}

// Check if the given board contains a Pente (five in a row of same colour)
bool SimpleGoPanel::HasPente(char board[21][21])
{	for(int i=1; i<=boardsize; i++)
		for(int j=1; j<=boardsize; j++)
			for(int xd=-1; xd<=1; xd++)
				for(int yd=-1; yd<=1; yd++)
					if((xd!=0||yd!=0) && board[i][j]!=EMPTY && HasNInARow(board, 5, i, j, xd, yd, board[i][j]))
						return true;
	return false;
}

// Redraw the current board and update status bar info
void SimpleGoPanel::UpdateBoard()
{	memcpy(board, history[curmove], BOARDMEMORYLEN);
	wxClientDC dc(this);
	DrawBoard(dc, board);
	UpdateStatus();
}

// Make a move on cell (x, y) if legal, and update the current board info and history
void SimpleGoPanel::MakeMove(int x, int y)
{	if(x<=0 || y<=0 || x>boardsize || y>boardsize)
		return;
	char temp[21][21];
	memcpy(temp, board, BOARDMEMORYLEN);
	int captures = 0;
	
	if(ValidMove(temp, x, y, captures))
	{	board[x][y] = curmove%2+1;
		wxClientDC dc(this);
		DrawStone(dc, x, y, curmove%2+1);
		if(memcmp(temp, board, BOARDMEMORYLEN))
		{	for(int i=0; i<21; i++)
				for(int j=0; j<21; j++)
					if(temp[i][j]!=board[i][j])
						DrawStone(dc, i, j, temp[i][j]);
			memcpy(board, temp, BOARDMEMORYLEN);
		}
		
		curmove++;
		history = (char(*)[21][21])realloc(history, (curmove+1)*BOARDMEMORYLEN);
		memcpy(history[curmove], board, BOARDMEMORYLEN);
		movelist = (pos*)realloc(movelist, curmove*sizeof(pos));
		movelist[curmove-1].x = x;
		movelist[curmove-1].y = y;
		int colour = board[x][y];
		int oppcolour = OPP(colour);
		capturehist = (int(*)[2])realloc(capturehist, (curmove+1)*sizeof(int[2]));
		capturehist[curmove][colour-1] = capturehist[curmove-1][colour-1] + captures;
		capturehist[curmove][oppcolour-1] = capturehist[curmove-1][oppcolour-1];
		totmove = curmove;
		UpdateStatus();
		finishedgame = capturehist[curmove][0]>=5 || capturehist[curmove][1]>=5 || HasPente(board);
	}
}

// Make a move on cell (x, y) according to the SGF move execution rules,
// and update the current board info and history (but not the GUI)
void SimpleGoPanel::MakeMoveSGF(int x, int y)
{	if(x<=0 || y<=0 || x>boardsize || y>boardsize)
		return;
	
	board[x][y] = curmove%2+1;
	int colour = board[x][y];
	int oppcolour = OPP(colour);
	int captures = 0;
	for(int i=-1; i<=1; i++)
		for(int j=-1; j<=1; j++)
			if(x+3*i>=1 && y+3*j>=1 && x+3*i<=boardsize && y+3*j<=boardsize)
			{	if(board[x+3*i][y+3*j]==colour && board[x+2*i][y+2*j]==oppcolour && board[x+i][y+j]==oppcolour)
				{	board[x+2*i][y+2*j] = EMPTY;
					board[x+i][y+j] = EMPTY;
					captures++;
				}
			}
	
	curmove++;
	history = (char(*)[21][21])realloc(history, (curmove+1)*BOARDMEMORYLEN);
	memcpy(history[curmove], board, BOARDMEMORYLEN);
	movelist = (pos*)realloc(movelist, curmove*sizeof(pos));
	movelist[curmove-1].x = x;
	movelist[curmove-1].y = y;
	capturehist = (int(*)[2])realloc(capturehist, (curmove+1)*sizeof(int[2]));
	capturehist[curmove][colour-1] = capturehist[curmove-1][colour-1] + captures;
	capturehist[curmove][oppcolour-1] = capturehist[curmove-1][oppcolour-1];
	totmove = curmove;
	finishedgame = capturehist[curmove][0]>=5 || capturehist[curmove][1]>=5 || HasPente(board);
}

// Initialize the current board and history variables
void SimpleGoPanel::InitGame()
{	curmove = 0;
	totmove = 0;
	finishedgame = false;
	if(history!=NULL)
		free(history);
	if(movelist!=NULL)
		free(movelist);
	if(capturehist!=NULL)
		free(capturehist);
	history = (char(*)[21][21])malloc(BOARDMEMORYLEN);
	capturehist = (int(*)[2])malloc(sizeof(int[2]));
	capturehist[0][0] = 0;
	capturehist[0][1] = 0;
	movelist = (pos*)malloc(0);
	for(int i=0; i<21; i++)
		for(int j=0; j<21; j++)
			board[i][j] = EMPTY;
	for(int i=0; i<21; i++)
		{	board[i][0] = BORDER;
			board[i][boardsize+1] = BORDER;
			board[0][i] = BORDER;
			board[boardsize+1][i] = BORDER;
		}
	memcpy(history[0], board, BOARDMEMORYLEN);
}
