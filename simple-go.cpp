#include "wx/wx.h"

bool lastpass;
int movenum;
int boardsize;
int randomon = 0;
char turn;
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
	void DrawBoard(wxDC& dc);
	void OnIdle(wxIdleEvent& event);
	MainPanel(wxFrame* parent);
};

MainPanel* panel;

/*wxString pointname()
{	wxString output = "";
	output += wxString::Format("%c", x>=8 ? 'A'+x+1 : 'A'+x);
	output += wxString::Format("%d", 19-y);
	return output;
}*/

void printboard(char board[21][21], int x, int y)
{	for(int j=0; j<21; j++)
	{	for(int i=0; i<21; i++)
		{	//if(i==x && j==y)
			//	printf("*");
			if(board[i][j]==0)
				printf(".");
			else if(board[i][j]==1)
				printf("X");
			else if(board[i][j]==2)
				printf("O");
			else if(board[i][j]==3)
				printf(" ");
		}
		printf("\n");
	}
	printf("\n");
}

void removegroup(char board[21][21], int x, int y)
{	int colour = board[x][y];
	if(colour==0)
		return;
	board[x][y] = 0;
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
{	if((board[x-1][y]==0) || (board[x][y-1]==0) || (board[x+1][y]==0) || (board[x][y+1]==0))
		return true;
	int colour = board[x][y];
	board[x][y] = 4;
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
{	if((*board)[x][y]!=0)
		return false;
		
	(*board)[x][y] = (turn+1)%2+1;
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
		for(int i=0; i<=movenum; i++)
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
	{	lastpass = false;
		turn = (turn+1)%2;
		board[x][y] = turn+1;
		wxClientDC dc(panel);
		panel->DrawStone(dc, x, y, turn+1);
		if(memcmp(temp, board, sizeof(temp)))
		{	//printf("updating board...\n");
			for(int i=0; i<21; i++)
				for(int j=0; j<21; j++)
					if(temp[i][j]!=board[i][j])
						panel->DrawStone(dc, i, j, temp[i][j]);
			memcpy(board, temp, sizeof(temp));
			//DrawBoard(dc);
		}
		
		history = (char(*)[21][21])realloc(history, (movenum+2)*sizeof(char[21][21]));
		memcpy(history[movenum+1], board, sizeof(history[movenum+1]));
		movenum++;
		//printf("Move %d:\n", movenum);
		//printboard(board, x, y);
		frame->SetStatusText(wxString::Format("Move: %d", movenum), 1);
		
		if(turn==0)
			frame->SetStatusText("Turn: White", 0);
		else
			frame->SetStatusText("Turn: Black", 0);
	}
}

void makepass()
{	lastpass = true;
	turn = (turn+1)%2;
	if(turn==0)
		frame->SetStatusText(wxT("Turn: White"), 0);
	else
		frame->SetStatusText(wxT("Turn: Black"), 0);
	history = (char(*)[21][21])realloc(history, (movenum+2)*sizeof(char[21][21]));
	memcpy(history[movenum+1], board, sizeof(history[movenum+1]));
	movenum++;
	frame->SetStatusText(wxString::Format("Move: %d", movenum), 1);
}

void initgame()
{	turn = 1;
	movenum = 0;
	lastpass = false;
	if(history!=NULL)
		free(history);
	history = (char(*)[21][21])malloc(sizeof(char[21][21]));
	for(int i=0; i<21; i++)
		for(int j=0; j<21; j++)
			board[i][j] = 0;
	for(int i=0; i<21; i++)
		{	board[i][0] = 3;
			board[i][boardsize+1] = 3;
			board[0][i] = 3;
			board[boardsize+1][i] = 3;
		}
	memcpy(history[movenum], board, sizeof(history[movenum]));
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
	if(lastpass)
	{	play_menu->Check(ID_RANDOM, false);
		panel->timer->Stop();
	}
	makepass();
}

void MainPanel::DrawStone(wxDC& dc, int x, int y, int colour)
{	if(colour==0)
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
	else if(colour==1)
	{	dc.SetPen(*wxBLACK_PEN);
		dc.DrawLine(16*x-(x>1?8:0),16*y,16*x+(x<boardsize?8:0),16*y);
		dc.DrawLine(16*x,16*y-(y>1?8:1),16*x,16*y+(y<boardsize?8:1));
		dc.SetBrush(*wxBLACK_BRUSH);
		dc.DrawCircle(wxPoint(16*x,16*y), 6);
	}
	else if(colour==2)
	{	dc.SetBrush(*wxWHITE_BRUSH);
		dc.SetPen(*wxBLACK_PEN);
		dc.DrawLine(16*x-(x>1?8:0),16*y,16*x-(x>1?6:0),16*y);
		dc.DrawLine(16*x,16*y-(y>1?8:0),16*x,16*y-(y>1?6:0));
		dc.DrawLine(16*x+(x<boardsize?6:0),16*y,16*x+(x<boardsize?8:0),16*y);
		dc.DrawLine(16*x,16*y+(y<boardsize?6:0),16*x,16*y+(y<boardsize?8:0));
		dc.DrawCircle(wxPoint(16*x,16*y), 6);
	}
}

void MainPanel::DrawBoard(wxDC& dc)
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

	DrawBoard(dc);
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
	
	menu_bar->Append(game_menu, wxT("&Game"));
	menu_bar->Append(play_menu, wxT("&Play"));
	SetMenuBar(menu_bar);
	
	CreateStatusBar(3);
	SetStatusText(wxT("Turn: Black"), 0);
	SetStatusText(wxT("Move: 0"), 1);
	SetStatusText(wxT("Score: 0-0"), 2);
	
	SetClientSize(320, 320);
}

/*void MainFrame::OnEnter(wxCommandEvent& WXUNUSED(event))
{	if(Depth1Text->GetValue().CmpNoCase(pointname())!=0)
		wxMessageBox(pointname(), _("Wrong"), wxOK | wxICON_INFORMATION, this);
	
	x = 0;
	y = 18;
	x = rand()%19;
	y = rand()%19;
	
	Depth1Text->SetValue("");
	
	Refresh();
}*/

void MainFrame::GoToMove(wxCommandEvent& WXUNUSED(event))
{	wxGetTextFromUser(wxString::Format("Enter the move number to go to, between 0 and %d:", movenum), "Go to move", "");
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
		panel->DrawBoard(dc);
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
	panel->DrawBoard(dc);
}

void MainPanel::LMouseUp(wxMouseEvent& event)
{	int x, y;
	x = (event.GetPosition().x+8)/16;
	y = (event.GetPosition().y+8)/16;
	makemove(x, y);
}

bool MyApp::OnInit()
{	
	boardsize = 19;
	initgame();
	
	frame = new MainFrame(wxT("Simple Go"), wxPoint(-1, -1), wxSize(-1, -1), wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER & ~wxMAXIMIZE_BOX);
	
	frame->Show();
	SetTopWindow(frame);
	
	return true;
} 

IMPLEMENT_APP(MyApp)
