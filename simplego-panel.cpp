#include <wx/wx.h>
#include "simplego-frame.h"
#include "simplego-panel.h"

SimpleGoPanel::SimpleGoPanel(SimpleGoFrame* parent) : wxPanel(parent, wxID_ANY, wxPoint(0, 0), wxSize(320, 320))
{	
	frame = parent;
	Connect(wxEVT_PAINT, wxPaintEventHandler(SimpleGoPanel::Paint));
	Connect(wxEVT_LEFT_UP, wxMouseEventHandler(SimpleGoPanel::LMouseUp));
	Connect(wxEVT_IDLE, wxIdleEventHandler(SimpleGoPanel::OnIdle), NULL, this);
	Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(SimpleGoPanel::onKeyDown));

	timer = new wxTimer();
	timer->SetOwner(this);
	//timer->Start(1);
}

void SimpleGoPanel::OnIdle(wxIdleEvent& event)
{	if(!frame->play_menu->IsChecked(ID_RANDOM))
		return;

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
		if(validmove(x, y, temp))
		{	makemove(x, y);
			return;
		}
		count++;
		attempts[x][y] = 1;
	}
	if(curmove>=1 && memcmp(history[curmove], history[curmove-1], BOARDMEMORYLEN)==0)
	{	frame->play_menu->Check(ID_RANDOM, false);
		timer->Stop();
	}
	makepass();
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

void SimpleGoPanel::updateboard()
{	memcpy(board, history[curmove], BOARDMEMORYLEN);
	wxClientDC dc(this);
	DrawBoard(dc, board);
	updateturn();
	scoregame(board);
}

void SimpleGoPanel::onKeyDown(wxKeyEvent& event)
{	if(event.GetKeyCode()==WXK_LEFT&&curmove>0)
	{	curmove--;
		updateboard();
	}
	else if(event.GetKeyCode()==WXK_RIGHT&&curmove<totmove)
	{	curmove++;
		updateboard();
	}
	else if(event.GetKeyCode()=='P')
		makepass();
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

void SimpleGoPanel::Paint(wxPaintEvent& event)
{	wxPaintDC dc(this);
	DrawBoard(dc, board);
}

void SimpleGoPanel::spreadarea(char board[21][21], int x, int y, int colour)
{	if(board[x][y]==EMPTY)
	{	board[x][y] = colour+AREAOFFSET;
		spreadarea(board, x-1, y, colour);
		spreadarea(board, x, y-1, colour);
		spreadarea(board, x+1, y, colour);
		spreadarea(board, x, y+1, colour);
	}
	else if((board[x][y]==BLACKAREA&&colour==WHITE)||(board[x][y]==WHITEAREA&&colour==BLACK))
	{	board[x][y] = MIXEDAREA;
		spreadarea(board, x-1, y, colour);
		spreadarea(board, x, y-1, colour);
		spreadarea(board, x+1, y, colour);
		spreadarea(board, x, y+1, colour);
	}
}

void SimpleGoPanel::scoregame(char board[21][21])
{	char temp[21][21];
	memcpy(temp, board, BOARDMEMORYLEN);
	for(int i=1; i<=boardsize; i++)
		for(int j=1; j<=boardsize; j++)
			if(temp[i][j]==BLACK||temp[i][j]==WHITE)
			{	spreadarea(temp, i-1, j, temp[i][j]);
				spreadarea(temp, i, j-1, temp[i][j]);
				spreadarea(temp, i+1, j, temp[i][j]);
				spreadarea(temp, i, j+1, temp[i][j]);
			}
	
	int whitescore = 0;
	int blackscore = 0;
	
	for(int i=1; i<=boardsize; i++)
		for(int j=1; j<=boardsize; j++)
			if(temp[i][j]==BLACK||temp[i][j]==BLACKAREA)
				blackscore++;
			else if(temp[i][j]==WHITE||temp[i][j]==WHITEAREA)
				whitescore++;
				
	frame->SetStatusText(wxString::Format("Area: %d-%d", blackscore, whitescore), 2);
}

void SimpleGoPanel::updateturn()
{	if(curmove%2==0)
		frame->SetStatusText(wxT("Turn: Black"), 0);
	else
		frame->SetStatusText(wxT("Turn: White"), 0);
	frame->SetStatusText(wxString::Format("Move: %d", curmove), 1);
}

void SimpleGoPanel::removegroup(char board[21][21], int x, int y)
{	int colour = board[x][y];
	board[x][y] = EMPTY;
	if(board[x-1][y]==colour)
		removegroup(board, x-1, y);
	if(board[x+1][y]==colour)
		removegroup(board, x+1, y);
	if(board[x][y-1]==colour)
		removegroup(board, x, y-1);
	if(board[x][y+1]==colour)
		removegroup(board, x, y+1);
}

bool SimpleGoPanel::haslibertiesrec(char board[21][21], int x, int y)
{	if((board[x-1][y]==EMPTY) || (board[x][y-1]==EMPTY) || (board[x+1][y]==EMPTY) || (board[x][y+1]==EMPTY))
		return true;
	int colour = board[x][y];
	board[x][y] = BORDER;
	if((board[x-1][y]==colour && haslibertiesrec(board, x-1, y))
	 ||(board[x][y-1]==colour && haslibertiesrec(board, x, y-1))
	 ||(board[x+1][y]==colour && haslibertiesrec(board, x+1, y))
	 ||(board[x][y+1]==colour && haslibertiesrec(board, x, y+1)))
		return true;
	return false;
}

bool SimpleGoPanel::hasliberties(char board[21][21], int x, int y)
{	char temp[21][21];
	memcpy(temp, board, BOARDMEMORYLEN);
	return haslibertiesrec(temp, x, y);
}

bool SimpleGoPanel::validmove(int x, int y, char board[21][21])
{	if(board[x][y]!=EMPTY)
		return false;
		
	board[x][y] = curmove%2+1;
	int colour = board[x][y];
	int oppcolour = OPP(colour);

	if(board[x-1][y]==oppcolour && !hasliberties(board, x-1, y))
		removegroup(board, x-1, y);
	if(board[x+1][y]==oppcolour && !hasliberties(board, x+1, y))
		removegroup(board, x+1, y);
	if(board[x][y+1]==oppcolour && !hasliberties(board, x, y+1))
		removegroup(board, x, y+1);
	if(board[x][y-1]==oppcolour && !hasliberties(board, x, y-1))
		removegroup(board, x, y-1);
	if(!frame->game_menu->IsChecked(ID_SUICIDE) && !hasliberties(board, x, y))
		return false;
	else if(!hasliberties(board, x, y))
		removegroup(board, x, y);
		
	bool dupe = false;
	for(int i=0; i<=curmove; i++)
		if(memcmp(board, history[i], BOARDMEMORYLEN)==0)
			dupe = true;
	
	return !dupe;
}

void SimpleGoPanel::makemove(int x, int y)
{	if(x==0 || y==0 || x>boardsize || y>boardsize)
		return;
	char temp[21][21];
	memcpy(temp, board, BOARDMEMORYLEN);
	
	if(validmove(x, y, temp))
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
		
		history = (char(*)[21][21])realloc(history, (curmove+2)*sizeof(char[21][21]));
		memcpy(history[curmove+1], board, BOARDMEMORYLEN);
		curmove++;
		totmove = curmove;

		updateturn();
		scoregame(board);
	}
}

void SimpleGoPanel::makepass()
{	history = (char(*)[21][21])realloc(history, (curmove+2)*BOARDMEMORYLEN);
	memcpy(history[curmove+1], board, BOARDMEMORYLEN);
	curmove++;
	totmove = curmove;
	updateturn();
}

void SimpleGoPanel::initgame()
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
	updateturn();
	scoregame(board);
}

void SimpleGoPanel::LMouseUp(wxMouseEvent& event)
{	int x, y;
	x = (event.GetPosition().x+8)/16;
	y = (event.GetPosition().y+8)/16;
	makemove(x, y);
}
