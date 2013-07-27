#include "wx/wx.h"

#define EMPTY 0
#define BLACK 1
#define WHITE 2
#define BORDER 3
#define CHECKED 4
#define BLACKAREA 4
#define WHITEAREA 5
#define MIXEDAREA 6

int curmove;
int totmove;
int boardsize;
char board[21][21];
char (*history)[21][21];
wxMenu* game_menu;
wxMenu* play_menu;
wxFrame* frame;

enum {
	ID_PASS = wxID_HIGHEST+1,
	ID_GO_TO_MOVE = wxID_HIGHEST+2,
	ID_NEW_GAME = wxID_HIGHEST+3,
	ID_BOARD_SIZE = wxID_HIGHEST+4,
	ID_RANDOM = wxID_HIGHEST+5,
	ID_SUICIDE = wxID_HIGHEST+6
};

class MainPanel : public wxPanel
{
public:
	wxTimer* timer;
	void LMouseUp(wxMouseEvent& event);
	void Paint(wxPaintEvent& evt);
	void DrawStone(wxDC& dc, int x, int y, int colour);
	void DrawBoard(wxDC& dc, char board[21][21]);
	void OnIdle(wxIdleEvent& event);
	MainPanel(wxFrame* parent);
};

MainPanel* panel;

void spreadarea(char board[21][21], int x, int y, int colour)
{	if(board[x][y]==EMPTY)
	{	board[x][y] = colour+3;
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

void scoregame(char board[21][21])
{	char temp[21][21];
	memcpy(temp, board, sizeof(temp));
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
	
	/*wxClientDC dc(panel);
	panel->DrawBoard(dc, temp);*/
}

void removegroup(char board[21][21], int x, int y)
{	int colour = board[x][y];
	if(colour==EMPTY)
		return;
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

bool haslibertiesrec(char board[21][21], int x, int y)
{	if((board[x-1][y]==EMPTY) || (board[x][y-1]==EMPTY) || (board[x+1][y]==EMPTY) || (board[x][y+1]==EMPTY))
		return true;
	int colour = board[x][y];
	board[x][y] = CHECKED;
	bool result = false;
	result |= ((colour==board[x-1][y]) ? haslibertiesrec(board, x-1, y) : false);
	result |= ((colour==board[x][y-1]) ? haslibertiesrec(board, x, y-1) : false);
	result |= ((colour==board[x+1][y]) ? haslibertiesrec(board, x+1, y) : false);
	result |= ((colour==board[x][y+1]) ? haslibertiesrec(board, x, y+1) : false);
	return result;
}

bool hasliberties(char board[21][21], int x, int y)
{	char temp[21][21];
	memcpy(temp, board, sizeof(temp));
	return haslibertiesrec(temp, x, y);
}

bool validmove(int x, int y, char (*board)[21][21])
{	if((*board)[x][y]!=EMPTY)
		return false;
		
	(*board)[x][y] = curmove%2+1;
	int colour = (*board)[x][y];
	bool haslib = hasliberties(*board, x, y);
	bool lefthaslib = hasliberties(*board, x-1, y);
	bool righthaslib = hasliberties(*board, x+1, y);
	bool downhaslib = hasliberties(*board, x, y+1);
	bool uphaslib = hasliberties(*board, x, y-1);

	if(game_menu->IsChecked(ID_SUICIDE) || haslib || (!lefthaslib && (*board)[x-1][y]!=colour) || (!righthaslib && (*board)[x+1][y]!=colour) || (!downhaslib && (*board)[x][y+1]!=colour) || (!uphaslib && (*board)[x][y-1]!=colour))
	{	if(!lefthaslib && (*board)[x-1][y]!=colour)
			removegroup(*board, x-1, y);
		if(!righthaslib && (*board)[x+1][y]!=colour)
			removegroup(*board, x+1, y);
		if(!downhaslib && (*board)[x][y+1]!=colour)
			removegroup(*board, x, y+1);
		if(!uphaslib && (*board)[x][y-1]!=colour)
			removegroup(*board, x, y-1);
		if(!hasliberties(*board, x, y))
			removegroup(*board, x, y);
			
		bool dupe = false;
		for(int i=0; i<=curmove; i++)
			if(memcmp(*board, history[i], sizeof(*board))==0)
				dupe = true;
				
		return !dupe;
	}
	return false;
}

void makemove(int x, int y)
{	if(x==0 || y==0 || x>boardsize || y>boardsize)
		return;
	char temp[21][21];
	memcpy(temp, board, sizeof(temp));
	
	if(validmove(x, y, &temp))
	{	board[x][y] = curmove%2+1;
		wxClientDC dc(panel);
		panel->DrawStone(dc, x, y, curmove%2+1);
		if(memcmp(temp, board, sizeof(temp)))
		{	//printf("updating board...\n");
			for(int i=0; i<21; i++)
				for(int j=0; j<21; j++)
					if(temp[i][j]!=board[i][j])
						panel->DrawStone(dc, i, j, temp[i][j]);
			memcpy(board, temp, sizeof(temp));
			//DrawBoard(dc);
		}
		
		history = (char(*)[21][21])realloc(history, (curmove+2)*sizeof(char[21][21]));
		memcpy(history[curmove+1], board, sizeof(history[curmove+1]));
		curmove++;
		totmove = curmove;

		if(curmove%2==0)
			frame->SetStatusText(wxT("Turn: Black"), 0);
		else
			frame->SetStatusText(wxT("Turn: White"), 0);
		frame->SetStatusText(wxString::Format("Move: %d", curmove), 1);
		scoregame(board);
	}
}

void makepass()
{	history = (char(*)[21][21])realloc(history, (curmove+2)*sizeof(char[21][21]));
	memcpy(history[curmove+1], board, sizeof(history[curmove+1]));
	curmove++;
	totmove = curmove;
	if(curmove%2==0)
		frame->SetStatusText(wxT("Turn: Black"), 0);
	else
		frame->SetStatusText(wxT("Turn: White"), 0);
	frame->SetStatusText(wxString::Format("Move: %d", curmove), 1);
}

void initgame()
{	curmove = 0;
	totmove = 0;
	if(history!=NULL)
		free(history);
	history = (char(*)[21][21])malloc(sizeof(char[21][21]));
	for(int i=0; i<21; i++)
		for(int j=0; j<21; j++)
			board[i][j] = EMPTY;
	for(int i=0; i<21; i++)
		{	board[i][0] = BORDER;
			board[i][boardsize+1] = BORDER;
			board[0][i] = BORDER;
			board[boardsize+1][i] = BORDER;
		}
	memcpy(history[0], board, sizeof(history[0]));
	frame->SetStatusText(wxT("Turn: Black"), 0);
	frame->SetStatusText(wxT("Move: 0"), 1);
	scoregame(board);
}

MainPanel::MainPanel(wxFrame* parent) : wxPanel(parent, wxID_ANY, wxPoint(0, 0), wxSize(320, 320), wxTAB_TRAVERSAL, _T("panel"))
{	
	frame = parent;
	Connect(wxEVT_PAINT, wxPaintEventHandler(MainPanel::Paint));
	Connect(wxEVT_LEFT_UP, wxMouseEventHandler(MainPanel::LMouseUp));
	Connect(wxEVT_IDLE, wxIdleEventHandler(MainPanel::OnIdle), NULL, this);

	timer = new wxTimer();
	timer->SetOwner(this);
	//timer->Start(1);
}

void MainPanel::OnIdle(wxIdleEvent& event)
{	if(!play_menu->IsChecked(ID_RANDOM))
		return;

	int x = 1+rand()%boardsize, y = 1+rand()%boardsize;
	char attempts[21][21];
	memset(attempts, 0, sizeof(attempts));
	char temp[21][21];
	int count = 0;
	while(count<boardsize*boardsize)
	{	while(attempts[x][y]==1)
		{	x = 1+rand()%boardsize;
			y = 1+rand()%boardsize;
		}
		memcpy(temp, board, sizeof(temp));
		if(validmove(x, y, &temp))
		{	makemove(x, y);
			return;
		}
		count++;
		attempts[x][y] = 1;
	}
	if(curmove>=1 && memcmp(history[curmove], history[curmove-1], sizeof(history[curmove]))==0)
	{	play_menu->Check(ID_RANDOM, false);
		panel->timer->Stop();
	}
	makepass();
}

void MainPanel::DrawStone(wxDC& dc, int x, int y, int colour)
{	if(colour==EMPTY/*||colour==MIXEDAREA*/)
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
	/*else if(colour==BLACKAREA)
	{	dc.SetPen(*wxBLACK_PEN);
		dc.DrawLine(16*x-(x>1?8:0),16*y,16*x+(x<boardsize?8:0),16*y);
		dc.DrawLine(16*x,16*y-(y>1?8:1),16*x,16*y+(y<boardsize?8:1));
		dc.SetBrush(*wxBLACK_BRUSH);
		dc.DrawCircle(wxPoint(16*x,16*y), 3);
	}
	else if(colour==WHITEAREA)
	{	dc.SetBrush(*wxWHITE_BRUSH);
		dc.SetPen(*wxBLACK_PEN);
		dc.DrawLine(16*x-(x>1?8:0),16*y,16*x-(x>1?6:0),16*y);
		dc.DrawLine(16*x,16*y-(y>1?8:0),16*x,16*y-(y>1?6:0));
		dc.DrawLine(16*x+(x<boardsize?6:0),16*y,16*x+(x<boardsize?8:0),16*y);
		dc.DrawLine(16*x,16*y+(y<boardsize?6:0),16*x,16*y+(y<boardsize?8:0));
		dc.DrawCircle(wxPoint(16*x,16*y), 3);
	}*/
}

void MainPanel::DrawBoard(wxDC& dc, char board[21][21])
{	int i, j;

	// Fix for Ubuntu 12.04 Classic
	frame->SetClientSize(320, 320);

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

void MainPanel::Paint(wxPaintEvent& event)
{	wxPaintDC dc(this);

	DrawBoard(dc, board);
}
 
class MyApp : public wxApp
{	bool OnInit();
};

class MainFrame : public wxFrame
{	
private:
	void Pass(wxCommandEvent& event);
	void Random(wxCommandEvent& event);
	void Nothing(wxMenuEvent& event);
	void OnEnter(wxCommandEvent& event);
	void GoToMove(wxCommandEvent& event);
	void NewGame(wxCommandEvent& event);
	void SetBoard(wxCommandEvent& event);
	void onKeyDown(wxKeyEvent& aEvent);

public:
	MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style);
};

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(NULL, -1, title, pos, size, style)
{	panel = new MainPanel(this);
	panel->SetBackgroundColour(*wxWHITE);
	
	wxMenuBar* menu_bar = new wxMenuBar;
	game_menu = new wxMenu;
	play_menu = new wxMenu;
	
	game_menu->Append(ID_NEW_GAME, wxT("&New game"));
	game_menu->Append(ID_BOARD_SIZE, wxT("&Board size..."));
	play_menu->Append(ID_PASS, wxT("&Pass"));
	play_menu->Append(ID_GO_TO_MOVE, wxT("&Go to move..."));
	play_menu->Append(ID_RANDOM, wxT("&Random!"), "", wxITEM_CHECK);
	game_menu->Append(ID_SUICIDE, wxT("&Allow suicide"), "", wxITEM_CHECK);
	
	Connect(ID_PASS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::Pass));
	Connect(ID_GO_TO_MOVE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::GoToMove));
	Connect(ID_NEW_GAME, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::NewGame));
	Connect(ID_BOARD_SIZE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::SetBoard));
	Connect(ID_RANDOM, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::Random));
	Connect(wxEVT_MENU_HIGHLIGHT, wxMenuEventHandler(MainFrame::Nothing));
	Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(MainFrame::onKeyDown));
	
	menu_bar->Append(game_menu, wxT("&Game"));
	menu_bar->Append(play_menu, wxT("&Play"));
	SetMenuBar(menu_bar);
	
	CreateStatusBar(3);
	SetClientSize(320, 320);
}

void MainFrame::onKeyDown(wxKeyEvent& event)
{	if(event.GetKeyCode()==WXK_LEFT&&curmove>0)
	{	curmove--;
		if(curmove%2==0)
			frame->SetStatusText(wxT("Turn: Black"), 0);
		else
			frame->SetStatusText(wxT("Turn: White"), 0);
		memcpy(board, history[curmove], sizeof(board));
		wxClientDC dc(panel);
		panel->DrawBoard(dc, board);
		frame->SetStatusText(wxString::Format("Move: %d", curmove), 1);
		scoregame(board);
	}
	else if(event.GetKeyCode()==WXK_RIGHT&&curmove<totmove)
	{	curmove++;
		if(curmove%2==0)
			frame->SetStatusText(wxT("Turn: Black"), 0);
		else
			frame->SetStatusText(wxT("Turn: White"), 0);
		memcpy(board, history[curmove], sizeof(board));
		wxClientDC dc(panel);
		panel->DrawBoard(dc, board);
		frame->SetStatusText(wxString::Format("Move: %d", curmove), 1);
		scoregame(board);
	}
}

void MainFrame::GoToMove(wxCommandEvent& WXUNUSED(event))
{	int num = wxAtoi(wxGetTextFromUser(wxString::Format("Enter the move number to go to, between 0 and %d:", totmove), "Go to move", ""));
	if(num>=0 && num<=totmove)
	{	curmove = num;
		if(curmove%2==0)
			frame->SetStatusText(wxT("Turn: Black"), 0);
		else
			frame->SetStatusText(wxT("Turn: White"), 0);
		memcpy(board, history[curmove], sizeof(board));
		wxClientDC dc(panel);
		panel->DrawBoard(dc, board);
		frame->SetStatusText(wxString::Format("Move: %d", curmove), 1);
		scoregame(board);
	}
}

void MainFrame::Pass(wxCommandEvent& WXUNUSED(event))
{	makepass();
}

void MainFrame::Nothing(wxMenuEvent& WXUNUSED(event))
{	
}

void MainFrame::SetBoard(wxCommandEvent& WXUNUSED(event))
{	int num = wxAtoi(wxGetTextFromUser("Enter the new board size, between 2 and 19:", "New board size", ""));
	if(num>=2&&num<=19)
	{	boardsize = num;
		initgame();
		wxClientDC dc(panel);
		panel->DrawBoard(dc, board);
	}
}

void MainFrame::Random(wxCommandEvent& WXUNUSED(event))
{	if(panel->timer->IsRunning())
		panel->timer->Stop();
	else
		panel->timer->Start(1);
}

void MainFrame::NewGame(wxCommandEvent& WXUNUSED(event))
{	initgame();
	
	wxClientDC dc(panel);
	panel->DrawBoard(dc, board);
}

void MainPanel::LMouseUp(wxMouseEvent& event)
{	int x, y;
	x = (event.GetPosition().x+8)/16;
	y = (event.GetPosition().y+8)/16;
	makemove(x, y);
}

bool MyApp::OnInit()
{	
	frame = new MainFrame(wxT("Simple Go"), wxPoint(-1, -1), wxSize(-1, -1), wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER & ~wxMAXIMIZE_BOX);
	
	frame->Show();
	SetTopWindow(frame);
	
	boardsize = 19;
	initgame();
	
	return true;
} 

IMPLEMENT_APP(MyApp)
