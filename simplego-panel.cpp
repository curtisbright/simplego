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

// Mark all area around the (x, y) cell as owned by colour
void SimpleGoPanel::SpreadArea(char board[21][21], int x, int y, int colour)
{	if(board[x][y]==EMPTY)
	{	board[x][y] = AREA(colour);
		SpreadArea(board, x-1, y, colour);
		SpreadArea(board, x, y-1, colour);
		SpreadArea(board, x+1, y, colour);
		SpreadArea(board, x, y+1, colour);
	}
	else if((board[x][y]==AREA(BLACK)&&colour==WHITE)||(board[x][y]==AREA(WHITE)&&colour==BLACK))
	{	board[x][y] = MIXEDAREA;
		SpreadArea(board, x-1, y, colour);
		SpreadArea(board, x, y-1, colour);
		SpreadArea(board, x+1, y, colour);
		SpreadArea(board, x, y+1, colour);
	}
}

// Calculate the score of the given board and update the status bar
void SimpleGoPanel::ScoreGame(char board[21][21])
{	if(gnugoscore)
	{	int score = -6;
		for(int i=1; i<=boardsize; i++)
			for(int j=1; j<=boardsize; j++)
				if(gnugoboard[i][j]==BLACK)
					score++;
				else if(gnugoboard[i][j]==WHITE)
					score--;
		
		frame->SetStatusText(wxString::Format("%s%c+%d.5", boardsize>12 ? "S: " : "", score>0 ? 'B' : 'W', score>0 ? score-1 : -score), 2);
	}
	else
	{	char temp[21][21];
		memcpy(temp, board, BOARDMEMORYLEN);
		ScoreArea(temp);
		
		int whitescore = 0, blackscore = 0;
		for(int i=1; i<=boardsize; i++)
			for(int j=1; j<=boardsize; j++)
				if(temp[i][j]==BLACK)
					blackscore++;
				else if(temp[i][j]==WHITE)
					whitescore++;
		
		frame->SetStatusText(wxString::Format("%s%d-%d", boardsize>12 ? "S: " : "", blackscore, whitescore), 2);
	}
}

// Update turn and move info on status bar
void SimpleGoPanel::UpdateStatus()
{	frame->SetStatusText(wxString::Format("%s%s", boardsize>12 ? "T: " : "", curmove%2==0 ? "Black" : "White"), 0);
	frame->SetStatusText(wxString::Format("%s%d", boardsize>12 ? "M: " : "", curmove), 1);
}

// Remove the group on the given board containing cell (x, y)
void SimpleGoPanel::RemoveGroup(char board[21][21], int x, int y)
{	int colour = board[x][y];
	board[x][y] = EMPTY;
	if(board[x-1][y]==colour)
		RemoveGroup(board, x-1, y);
	if(board[x+1][y]==colour)
		RemoveGroup(board, x+1, y);
	if(board[x][y-1]==colour)
		RemoveGroup(board, x, y-1);
	if(board[x][y+1]==colour)
		RemoveGroup(board, x, y+1);
}

// Determine if the group on the given board containing cell (x, y) has liberties
// Stones in the group are marked as they are examined
bool SimpleGoPanel::HasLibertiesRec(char board[21][21], int x, int y)
{	if((board[x-1][y]==EMPTY) || (board[x][y-1]==EMPTY) || (board[x+1][y]==EMPTY) || (board[x][y+1]==EMPTY))
		return true;
	int colour = board[x][y];
	board[x][y] = BORDER;
	if((board[x-1][y]==colour && HasLibertiesRec(board, x-1, y))
	 ||(board[x][y-1]==colour && HasLibertiesRec(board, x, y-1))
	 ||(board[x+1][y]==colour && HasLibertiesRec(board, x+1, y))
	 ||(board[x][y+1]==colour && HasLibertiesRec(board, x, y+1)))
		return true;
	return false;
}

// Determine if the group on the given board which contains cell (x, y) has liberties
// The given board is not modified
bool SimpleGoPanel::HasLiberties(char board[21][21], int x, int y)
{	char temp[21][21];
	memcpy(temp, board, BOARDMEMORYLEN);
	return HasLibertiesRec(temp, x, y);
}

// Determine if a move at cell (x, y) is valid on the given board
bool SimpleGoPanel::ValidMove(char board[21][21], int x, int y, bool checkdupes)
{	if(board[x][y]!=EMPTY)
		return false;
		
	board[x][y] = curmove%2+1;
	int colour = board[x][y];
	int oppcolour = OPP(colour);

	if(board[x-1][y]==oppcolour && !HasLiberties(board, x-1, y))
		RemoveGroup(board, x-1, y);
	if(board[x+1][y]==oppcolour && !HasLiberties(board, x+1, y))
		RemoveGroup(board, x+1, y);
	if(board[x][y+1]==oppcolour && !HasLiberties(board, x, y+1))
		RemoveGroup(board, x, y+1);
	if(board[x][y-1]==oppcolour && !HasLiberties(board, x, y-1))
		RemoveGroup(board, x, y-1);
	if(!frame->gamemenu->IsChecked(ID_SUICIDE) && !HasLiberties(board, x, y))
		return false;
	else if(!HasLiberties(board, x, y))
		RemoveGroup(board, x, y);
		
	bool dupe = false;
	if(checkdupes)
	{	for(int i=0; i<=curmove; i++)
			if(memcmp(board, history[i], BOARDMEMORYLEN)==0)
				dupe = true;
	}
	
	return !dupe;
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
	else if(colour==AREA(BLACK))
	{	dc.SetPen(*wxBLACK_PEN);
		dc.SetBrush(*wxBLACK_BRUSH);
		dc.DrawCircle(wxPoint(16*x,16*y), 3);
	}
	else if(colour==AREA(WHITE))
	{	dc.SetBrush(*wxWHITE_BRUSH);
		dc.SetPen(*wxBLACK_PEN);
		dc.DrawCircle(wxPoint(16*x,16*y), 3);
	}
}

// Draw the given board using the provided device context
void SimpleGoPanel::DrawBoard(wxDC& dc, char board[21][21])
{	for(int i=0; i<21; i++)
		for(int j=0; j<21; j++)
		{	if(i>=1&&j>=1&&i<=boardsize&&j<=boardsize)
			{	DrawStone(dc, i, j, board[i][j]);
				if(gnugoscore && board[i][j]!=gnugoboard[i][j])
					DrawStone(dc, i, j, AREA(gnugoboard[i][j]));
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
		if(ValidMove(temp, x, y, true))
		{	MakeMove(x, y);
			#ifdef __WXMSW__
			event.RequestMore();
			#endif
			return;
		}
		count++;
		attempts[x][y] = 1;
	}

	MakePass();
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
// left/right moves through history; 'p' passes the turn
void SimpleGoPanel::KeyDown(wxKeyEvent& event)
{	if(event.GetKeyCode()==WXK_LEFT&&curmove>0)
	{	gnugoscore = false;
		curmove--;
		UpdateBoard();
	}
	else if(event.GetKeyCode()==WXK_RIGHT&&curmove<totmove)
	{	gnugoscore = false;
		curmove++;
		UpdateBoard();
	}
	else if(event.GetKeyCode()=='P'&&!event.AltDown())
		MakePass();
	event.Skip();
}

// Fill empty cells on the given board with the colour who owns that area
void SimpleGoPanel::ScoreArea(char board[21][21])
{	for(int i=1; i<=boardsize; i++)
		for(int j=1; j<=boardsize; j++)
			if(board[i][j]==BLACK||board[i][j]==WHITE)
			{	SpreadArea(board, i-1, j, board[i][j]);
				SpreadArea(board, i, j-1, board[i][j]);
				SpreadArea(board, i+1, j, board[i][j]);
				SpreadArea(board, i, j+1, board[i][j]);
			}
	for(int i=1; i<=boardsize; i++)
		for(int j=1; j<=boardsize; j++)
			if(board[i][j]==AREA(BLACK))
				board[i][j] = BLACK;
			else if(board[i][j]==AREA(WHITE))
				board[i][j] = WHITE;
}

// Redraw the current board and update status bar info
void SimpleGoPanel::UpdateBoard()
{	memcpy(board, history[curmove], BOARDMEMORYLEN);
	wxClientDC dc(this);
	DrawBoard(dc, board);
	UpdateStatus();
	ScoreGame(board);
}

// Pass the current turn and update the status bar and history
void SimpleGoPanel::MakePass()
{	if(gnugoscore)
	{	gnugoscore = false;
		wxClientDC dc(this);
		DrawBoard(dc, board);
	}
	curmove++;
	history = (char(*)[21][21])realloc(history, (curmove+1)*BOARDMEMORYLEN);
	memcpy(history[curmove], board, BOARDMEMORYLEN);
	movelist = (pos*)realloc(movelist, curmove*sizeof(pos));
	movelist[curmove-1].x = 0;
	movelist[curmove-1].y = 0;
	totmove = curmove;
	UpdateStatus();
	if(curmove>=2 && movelist[curmove-2].x==0 && movelist[curmove-2].y==0)
	{	frame->playmenu->Check(ID_RANDOM, false);
		frame->MakeGNUGoScore();
	}
	else if(curmove%2==1 && frame->gamemenu->IsChecked(ID_GNUGO_WHITE))
		frame->MakeGNUGoMove();
}

// Make a move on cell (x, y) if legal, and update the current board info and history
void SimpleGoPanel::MakeMove(int x, int y)
{	if(x<=0 || y<=0 || x>boardsize || y>boardsize)
		return;
	char temp[21][21];
	memcpy(temp, board, BOARDMEMORYLEN);
	
	if(ValidMove(temp, x, y, true))
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
		
		if(gnugoscore)
		{	gnugoscore = false;
			DrawBoard(dc, board);
		}
		curmove++;
		history = (char(*)[21][21])realloc(history, (curmove+1)*BOARDMEMORYLEN);
		memcpy(history[curmove], board, BOARDMEMORYLEN);
		movelist = (pos*)realloc(movelist, curmove*sizeof(pos));
		movelist[curmove-1].x = x;
		movelist[curmove-1].y = y;
		totmove = curmove;
		UpdateStatus();
		ScoreGame(board);
		if(curmove%2==1 && frame->gamemenu->IsChecked(ID_GNUGO_WHITE))
			frame->MakeGNUGoMove();
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

	if(board[x-1][y]==oppcolour && !HasLiberties(board, x-1, y))
		RemoveGroup(board, x-1, y);
	if(board[x+1][y]==oppcolour && !HasLiberties(board, x+1, y))
		RemoveGroup(board, x+1, y);
	if(board[x][y+1]==oppcolour && !HasLiberties(board, x, y+1))
		RemoveGroup(board, x, y+1);
	if(board[x][y-1]==oppcolour && !HasLiberties(board, x, y-1))
		RemoveGroup(board, x, y-1);
	if(!HasLiberties(board, x, y))
		RemoveGroup(board, x, y);
	
	curmove++;
	history = (char(*)[21][21])realloc(history, (curmove+1)*BOARDMEMORYLEN);
	memcpy(history[curmove], board, BOARDMEMORYLEN);
	movelist = (pos*)realloc(movelist, curmove*sizeof(pos));
	movelist[curmove-1].x = x;
	movelist[curmove-1].y = y;
	totmove = curmove;
}

// Initialize the current board and history variables
void SimpleGoPanel::InitGame()
{	gnugoscore = false;
	curmove = 0;
	totmove = 0;
	if(history!=NULL)
		free(history);
	if(movelist!=NULL)
		free(movelist);
	history = (char(*)[21][21])malloc(BOARDMEMORYLEN);
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
