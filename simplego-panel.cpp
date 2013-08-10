#include <wx/wx.h>
#include "simplego-frame.h"
#include "simplego-panel.h"

SimpleGoPanel::SimpleGoPanel(SimpleGoFrame* parent) : wxPanel(parent, wxID_ANY, wxPoint(0, 0), wxSize(320, 320))
{	frame = parent;
	timer = new wxTimer();
	Connect(wxEVT_PAINT, wxPaintEventHandler(SimpleGoPanel::Paint));
	Connect(wxEVT_LEFT_UP, wxMouseEventHandler(SimpleGoPanel::LMouseUp));
	Connect(wxEVT_IDLE, wxIdleEventHandler(SimpleGoPanel::OnIdle), NULL, this);
	Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(SimpleGoPanel::OnKeyDown));
	boardsize = 19;
}

void SimpleGoPanel::SpreadArea(char board[21][21], int x, int y, int colour)
{	if(board[x][y]==EMPTY)
	{	board[x][y] = colour+AREAOFFSET;
		SpreadArea(board, x-1, y, colour);
		SpreadArea(board, x, y-1, colour);
		SpreadArea(board, x+1, y, colour);
		SpreadArea(board, x, y+1, colour);
	}
	else if((board[x][y]==BLACKAREA&&colour==WHITE)||(board[x][y]==WHITEAREA&&colour==BLACK))
	{	board[x][y] = MIXEDAREA;
		SpreadArea(board, x-1, y, colour);
		SpreadArea(board, x, y-1, colour);
		SpreadArea(board, x+1, y, colour);
		SpreadArea(board, x, y+1, colour);
	}
}

void SimpleGoPanel::ScoreGame(char board[21][21])
{	char temp[21][21];
	memcpy(temp, board, BOARDMEMORYLEN);
	for(int i=1; i<=boardsize; i++)
		for(int j=1; j<=boardsize; j++)
			if(temp[i][j]==BLACK||temp[i][j]==WHITE)
			{	SpreadArea(temp, i-1, j, temp[i][j]);
				SpreadArea(temp, i, j-1, temp[i][j]);
				SpreadArea(temp, i+1, j, temp[i][j]);
				SpreadArea(temp, i, j+1, temp[i][j]);
			}
	
	int whitescore = 0, blackscore = 0;
	
	for(int i=1; i<=boardsize; i++)
		for(int j=1; j<=boardsize; j++)
			if(temp[i][j]==BLACK||temp[i][j]==BLACKAREA)
				blackscore++;
			else if(temp[i][j]==WHITE||temp[i][j]==WHITEAREA)
				whitescore++;
				
	frame->SetStatusText(wxString::Format("Area: %d-%d", blackscore, whitescore), 2);
}

void SimpleGoPanel::UpdateTurn()
{	if(curmove%2==0)
		frame->SetStatusText(wxT("Turn: Black"), 0);
	else
		frame->SetStatusText(wxT("Turn: White"), 0);
	frame->SetStatusText(wxString::Format("Move: %d", curmove), 1);
}

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

bool SimpleGoPanel::HasLiberties(char board[21][21], int x, int y)
{	char temp[21][21];
	memcpy(temp, board, BOARDMEMORYLEN);
	return HasLibertiesRec(temp, x, y);
}

bool SimpleGoPanel::ValidMove(int x, int y, char board[21][21])
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
	if(!frame->game_menu->IsChecked(ID_SUICIDE) && !HasLiberties(board, x, y))
		return false;
	else if(!HasLiberties(board, x, y))
		RemoveGroup(board, x, y);
		
	bool dupe = false;
	for(int i=0; i<=curmove; i++)
		if(memcmp(board, history[i], BOARDMEMORYLEN)==0)
			dupe = true;
	
	return !dupe;
}

void SimpleGoPanel::MakeMove(int x, int y)
{	if(x==0 || y==0 || x>boardsize || y>boardsize)
		return;
	char temp[21][21];
	memcpy(temp, board, BOARDMEMORYLEN);
	
	if(ValidMove(x, y, temp))
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
		history = (char(*)[21][21])realloc(history, (curmove+1)*sizeof(char[21][21]));
		memcpy(history[curmove], board, BOARDMEMORYLEN);
		totmove = curmove;
		UpdateTurn();
		ScoreGame(board);
	}
}

void SimpleGoPanel::Paint(wxPaintEvent& event)
{	wxPaintDC dc(this);
	DrawBoard(dc, board);
}

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

void SimpleGoPanel::DrawBoard(wxDC& dc, char board[21][21])
{	int i, j;

	for(i=1; i<20; i++)
		for(j=1; j<20; j++)
		{	if(i<=boardsize&&j<=boardsize)
				DrawStone(dc, i, j, board[i][j]);
			else
			{	dc.SetBrush(*wxWHITE_BRUSH);
				dc.SetPen(*wxWHITE_PEN);
				dc.DrawRectangle(16*i-8, 16*j-8, 16, 16);
			}
		}
}

void SimpleGoPanel::OnIdle(wxIdleEvent& event)
{	if(!frame->play_menu->IsChecked(ID_RANDOM))
	{	timer->Stop();
		return;
	}

	int x = 1+rand()%boardsize, y = 1+rand()%boardsize;
	char attempts[21][21];
	memset(attempts, 0, BOARDMEMORYLEN);
	char temp[21][21];
	int count = 0;
	while(count<boardsize*boardsize)
	{	while(attempts[x][y]==1)
		{	x = 1+rand()%boardsize;
			y = 1+rand()%boardsize;
		}
		memcpy(temp, board, BOARDMEMORYLEN);
		if(ValidMove(x, y, temp))
		{	MakeMove(x, y);
			return;
		}
		count++;
		attempts[x][y] = 1;
	}
	if(curmove>=1 && memcmp(history[curmove], history[curmove-1], BOARDMEMORYLEN)==0)
		frame->play_menu->Check(ID_RANDOM, false);

	MakePass();
}

void SimpleGoPanel::LMouseUp(wxMouseEvent& event)
{	int x, y;
	x = (event.GetPosition().x+8)/16;
	y = (event.GetPosition().y+8)/16;
	MakeMove(x, y);
}

void SimpleGoPanel::OnKeyDown(wxKeyEvent& event)
{	if(event.GetKeyCode()==WXK_LEFT&&curmove>0)
	{	curmove--;
		UpdateBoard();
	}
	else if(event.GetKeyCode()==WXK_RIGHT&&curmove<totmove)
	{	curmove++;
		UpdateBoard();
	}
	else if(event.GetKeyCode()=='P')
		MakePass();
}

void SimpleGoPanel::UpdateBoard()
{	memcpy(board, history[curmove], BOARDMEMORYLEN);
	wxClientDC dc(this);
	DrawBoard(dc, board);
	UpdateTurn();
	ScoreGame(board);
}

void SimpleGoPanel::MakePass()
{	curmove++;
	history = (char(*)[21][21])realloc(history, (curmove+1)*BOARDMEMORYLEN);
	memcpy(history[curmove], board, BOARDMEMORYLEN);
	totmove = curmove;
	UpdateTurn();
}

void SimpleGoPanel::InitGame()
{	curmove = 0;
	totmove = 0;
	if(history!=NULL)
		free(history);
	history = (char(*)[21][21])malloc(BOARDMEMORYLEN);
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
	UpdateTurn();
	ScoreGame(board);
}
