#include "wx/wx.h"

int movenum;
int boardsize;
int randomon = 0;
char turn;
char board[21][21];
char (*history)[21][21];
wxMenu* play_menu;

//wxBitmap* boardbmp;

//wxButton* PassButton;
//wxButton* RandomButton;
//wxButton* NewButton;
//wxStaticText* TurnText;
//wxStaticText* MoveText;
//wxStaticText* ScoreText;
//wxTextCtrl* boardsizeInput;
//wxCheckBox* suicide;

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

	if(haslib || (!lefthaslib && (*board)[x-1][y]!=colour) || (!righthaslib && (*board)[x+1][y]!=colour) || (!downhaslib && (*board)[x][y+1]!=colour) || (!uphaslib && (*board)[x][y-1]!=colour))
	{	if(!lefthaslib && (*board)[x-1][y]!=colour)
			removegroup(*board, x-1, y);
		if(!righthaslib && (*board)[x+1][y]!=colour)
			removegroup(*board, x+1, y);
		if(!downhaslib && (*board)[x][y+1]!=colour)
			removegroup(*board, x, y+1);
		if(!uphaslib && (*board)[x][y-1]!=colour)
			removegroup(*board, x, y-1);
			
		bool dupe = false;
		for(int i=0; i<=movenum; i++)
			if(memcmp(*board, history[i], sizeof(*board))==0)
				dupe = true;
				
		return !dupe;
	}
	return false;
}

class MainPanel : public wxPanel
{
public:
	void LMouseUp(wxMouseEvent& event);
	void Paint(wxPaintEvent& evt);
	void DrawStone(wxDC& dc, int x, int y, int colour);
	void DrawBoard(wxDC& dc);
	void makemove(int x, int y);
	void OnIdle(wxIdleEvent& event);
	wxFrame* frame;
	MainPanel(wxFrame* parent);
};

MainPanel* panel;

MainPanel::MainPanel(wxFrame* parent) : wxPanel(parent, wxID_ANY, wxPoint(0, 0), wxSize(321, 321), wxTAB_TRAVERSAL, _T("panel"))
{	
	frame = parent;
	Connect(wxEVT_PAINT, wxPaintEventHandler(MainPanel::Paint));
	Connect(wxEVT_LEFT_UP, wxMouseEventHandler(MainPanel::LMouseUp));
	Connect(wxEVT_IDLE, wxIdleEventHandler(MainPanel::OnIdle), NULL, this);
}

void MainPanel::OnIdle(wxIdleEvent& event)
{	if(!play_menu->IsChecked(wxID_HIGHEST+6))
		return;
	
	int x = 1+rand()%19, y = 1+rand()%19;
	char attempts[21][21];
	memset(attempts, 0, sizeof(attempts));
	char temp[21][21];
	int count = 0;
	while(count<boardsize*boardsize)
	{	while(attempts[x][y]==1)
		{	x = 1+rand()%19;
			y = 1+rand()%19;
		}
		memcpy(temp, board, sizeof(temp));
		if(validmove(x, y, &temp))
		{	makemove(x, y);
			event.RequestMore();
			return;
		}
		count++;
		attempts[x][y] = 1;
	}
	turn = (turn+1)%2;

	event.RequestMore();
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
		if((x==4||x==10||x==16)&&(y==4||y==10||y==16))
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
		dc.DrawCircle(wxPoint(16*x,16*y), 6);
		dc.DrawLine(16*x-(x>1?8:0),16*y,16*x-(x>1?6:0),16*y);
		dc.DrawLine(16*x,16*y-(y>1?8:0),16*x,16*y-(y>1?6:0));
	}
}

void MainPanel::DrawBoard(wxDC& dc)
{	int i, j;

	// Fix for Ubuntu 12.04 Classic
	frame->SetClientSize(321, 321);
	
	//dc.Clear();

	/*dc.SetPen(*wxRED_PEN);
	dc.SetBrush(*wxRED_BRUSH);
	wxColour col;
	dc.GetPixel(0, 0, &col);
	dc.FloodFill(0, 0, col);*/

	/*dc.SetPen(*wxBLACK_PEN);
	dc.SetBrush(*wxWHITE_BRUSH);

	for(i=1; i<19; i++)
		for(j=1; j<19; j++)
			dc.DrawRectangle(16*i, 16*j, 17, 17);
			
	dc.SetBrush(*wxBLACK_BRUSH);
	dc.DrawRectangle(16*4-1, 16*4-1, 3, 3);
	dc.DrawRectangle(16*4-1, 16*10-1, 3, 3);
	dc.DrawRectangle(16*4-1, 16*16-1, 3, 3);
	dc.DrawRectangle(16*10-1, 16*4-1, 3, 3);
	dc.DrawRectangle(16*10-1, 16*10-1, 3, 3);
	dc.DrawRectangle(16*10-1, 16*16-1, 3, 3);
	dc.DrawRectangle(16*16-1, 16*4-1, 3, 3);
	dc.DrawRectangle(16*16-1, 16*10-1, 3, 3);
	dc.DrawRectangle(16*16-1, 16*16-1, 3, 3);*/

	for(i=0; i<21; i++)
		for(j=0; j<21; j++)
			DrawStone(dc, i, j, board[i][j]);
}

void MainPanel::Paint(wxPaintEvent& event)
{	wxPaintDC dc(this);

	DrawBoard(dc);
}
 
class MyApp : public wxApp
{	bool OnInit();
	wxFrame* frame;
	//MainPanel* drawPane;
};

class MainFrame : public wxFrame
{	
private:
	void Pass(wxCommandEvent& event);
	void OnEnter(wxCommandEvent& event);
	void GoToMove(wxCommandEvent& event);
	void NewGame(wxCommandEvent& event);
	void SetBoard(wxCommandEvent& event);

public:
	MainPanel* panel;
	MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style);
};

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(NULL, -1, title, pos, size, style)
{	panel = new MainPanel(this);
	//wxFlexGridSizer* goSizer = new wxFlexGridSizer(1, 2, 0, 0);
	//wxBoxSizer* descriptionSizer = new wxBoxSizer(wxVERTICAL);
	
	//goSizer->Add(descriptionSizer, 1, wxBOTTOM|wxRIGHT, 5);
	//goSizer->Add(panel, 1, wxBOTTOM|wxRIGHT, 5);
	
	//TurnText = new wxStaticText(this, wxID_ANY, _T("Turn: Black"));
	//MoveText = new wxStaticText(this, wxID_ANY, _T("Move: 0"));
	//ScoreText = new wxStaticText(this, wxID_ANY, _T("Score: 0-0"));
	//PassButton = new wxButton(this, wxID_HIGHEST+1, _T("Pass"));
	
	//descriptionSizer->Add(TurnText, 0, wxALIGN_TOP|wxLEFT|wxTOP, 5);
	//descriptionSizer->Add(MoveText, 0, wxALIGN_TOP|wxLEFT|wxTOP, 5);
	//descriptionSizer->Add(ScoreText, 0, wxALIGN_TOP|wxLEFT|wxTOP, 5);
	//descriptionSizer->Add(PassButton, 0, wxALIGN_TOP|wxLEFT|wxTOP, 5);
	//this->SetSizer(goSizer);
	//goSizer->Fit(this);
	
	wxMenuBar* menu_bar = new wxMenuBar;
	wxMenu* game_menu = new wxMenu;
	play_menu = new wxMenu;
	
	game_menu->Append(wxID_HIGHEST+4, wxT("&New game"));
	game_menu->Append(wxID_HIGHEST+5, wxT("&Board size..."));
	play_menu->Append(wxID_HIGHEST+1, wxT("&Pass"));
	play_menu->Append(wxID_HIGHEST+2, wxT("&Go to move..."));
	play_menu->Append(wxID_HIGHEST+6, wxT("&Random!"), "", wxITEM_CHECK);
	game_menu->Append(wxID_HIGHEST+7, wxT("&Allow suicide"), "", wxITEM_CHECK);
	
	Connect(wxID_HIGHEST+1, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::Pass));
	Connect(wxID_HIGHEST+2, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::GoToMove));
	Connect(wxID_HIGHEST+4, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::NewGame));
	Connect(wxID_HIGHEST+5, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::SetBoard));
	
	menu_bar->Append(game_menu, wxT("&Game"));
	menu_bar->Append(play_menu, wxT("&Play"));
	SetMenuBar(menu_bar);
	
	CreateStatusBar(3);
	SetStatusText(wxT("Turn: Black"), 0);
	SetStatusText(wxT("Move: 0"), 1);
	SetStatusText(wxT("Score: 0-0"), 2);
	
	//wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	//sizer->Add(panel);
	//this->SetSizer(sizer);
	//sizer->Fit(this);
	
	SetClientSize(panel->GetSize());
	
	//panel->SetSize( this->GetClientSize( ) ); 
	
	//goSizer->Fit(this);
	
	//Depth1Text = new wxTextCtrl(this, wxID_HIGHEST+2, _T(""), wxPoint(25, 350), wxDefaultSize, wxTE_PROCESS_ENTER);
	//Connect(wxID_HIGHEST+2, wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(MainFrame::OnEnter));
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
{	wxGetTextFromUser(wxString::Format("Enter the move number to go to, between 0 and %d:", movenum), "Go to move", wxString::Format("%d", movenum));
}

void MainFrame::Pass(wxCommandEvent& WXUNUSED(event))
{	turn = (turn+1)%2;
	if(turn==0)
		//TurnText->SetLabel("Turn: White");
		SetStatusText(wxT("Turn: White"), 0);
	else
		//TurnText->SetLabel("Turn: Black");
		SetStatusText(wxT("Turn: Black"), 0);
	history = (char(*)[21][21])realloc(history, (movenum+2)*sizeof(char[21][21]));
	memcpy(history[movenum+1], board, sizeof(history[movenum+1]));
	movenum++;
	//MoveText->SetLabel("Move: " + wxString::Format("%d", movenum));
	SetStatusText(wxString::Format("Move: %d", movenum), 1);
	/*int x, y, count;
	for(int i=0; i<25; i++)
	{	int origmove = move;
		count = 0;
		while(origmove==move)
		{	x = rand()%19;
			y = rand()%19;
			panel->makemove(x, y);
			count++;
			if(count>500)
				break;
		}
		printf("attempts: %d\n", count);
	}*/
}

void MainFrame::SetBoard(wxCommandEvent& WXUNUSED(event))
{	wxGetTextFromUser("Enter the new board size, between 2 and 19:", "New board size", "19");
}

void MainFrame::NewGame(wxCommandEvent& WXUNUSED(event))
{	turn = 1;
	movenum = 0;
	boardsize = 19;
	if(history!=NULL)
		free(history);
	history = (char(*)[21][21])malloc(sizeof(char[21][21]));
	for(int i=0; i<21; i++)
		for(int j=0; j<21; j++)
			board[i][j] = 0;
	memcpy(history[movenum], board, sizeof(history[movenum]));
			
	for(int i=0; i<21; i++)
	{	board[i][0] = 3;
		board[i][boardsize+1] = 3;
		board[0][i] = 3;
		board[boardsize+1][i] = 3;
	}
	
	wxClientDC dc(panel);
	panel->DrawBoard(dc);
}

void MainPanel::makemove(int x, int y)
{	if(x==0 || y==0 || x>boardsize || y>boardsize)
		return;
	wxClientDC dc(this);
	char temp[21][21];
	memcpy(temp, board, sizeof(temp));
	
	if(validmove(x, y, &temp))
	{	turn = (turn+1)%2;
		board[x][y] = turn+1;
		DrawStone(dc, x, y, turn+1);
		if(memcmp(temp, board, sizeof(temp)))
		{	//printf("updating board...\n");
			for(int i=0; i<21; i++)
				for(int j=0; j<21; j++)
					if(temp[i][j]!=board[i][j])
						DrawStone(dc, i, j, temp[i][j]);
			memcpy(board, temp, sizeof(temp));
			//DrawBoard(dc);
		}
		
		history = (char(*)[21][21])realloc(history, (movenum+2)*sizeof(char[21][21]));
		memcpy(history[movenum+1], board, sizeof(history[movenum+1]));
		movenum++;
		//printf("Move %d:\n", movenum);
		//printboard(board, x, y);
		//MoveText->SetLabel("Move: " + wxString::Format("%d", movenum));
		frame->SetStatusText(wxString::Format("Move: %d", movenum), 1);
		
		if(turn==0)
			//TurnText->SetLabel("Turn: White");
			frame->SetStatusText("Turn: White", 0);
		else
			//TurnText->SetLabel("Turn: Black");
			frame->SetStatusText("Turn: Black", 0);
	}
}

void MainPanel::LMouseUp(wxMouseEvent& event)
{	int x, y;
	x = (event.GetPosition().x+8)/16;
	y = (event.GetPosition().y+8)/16;
	makemove(x, y);
}

bool MyApp::OnInit()
{
	//boardbmp = new wxBitmap(321, 321);
	
	turn = 1;
	movenum = 0;
	boardsize = 19;
	history = (char(*)[21][21])malloc(sizeof(char[21][21]));
	for(int i=0; i<21; i++)
		for(int j=0; j<21; j++)
			board[i][j] = 0;
	memcpy(history[movenum], board, sizeof(history[movenum]));
			
	for(int i=0; i<21; i++)
	{	board[i][0] = 3;
		board[i][boardsize+1] = 3;
		board[0][i] = 3;
		board[boardsize+1][i] = 3;
	}
	
	frame = new MainFrame(wxT("Simple Go"), wxPoint(-1, -1), wxSize(-1, -1), wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER & ~wxMAXIMIZE_BOX);
	
	frame->Show();
	SetTopWindow(frame);
	
	return true;
} 

IMPLEMENT_APP(MyApp)
