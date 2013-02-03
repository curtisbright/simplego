#include "wx/wx.h"

int movenum;
int boardsize;
char turn;
char board[21][21];
char (*history)[21][21];

wxButton* PassButton;
wxStaticText* TurnText;

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

class MainPanel : public wxPanel
{
public:
	wxTimer* timer;
	void OnTimer(wxTimerEvent& event);
	void LMouseUp(wxMouseEvent& event);
	void Paint(wxPaintEvent& evt);
	void DrawStone(wxDC& dc, int x, int y, int colour);
	void DrawBoard(wxDC& dc);
	void makemove(int x, int y);
	MainPanel(wxFrame* parent);
};

MainPanel* panel;

MainPanel::MainPanel(wxFrame* parent) : wxPanel(parent, wxID_ANY, wxPoint(0, 50), wxSize(312, 312), wxTAB_TRAVERSAL, _T("panel"))
{		
	Connect(wxEVT_PAINT, wxPaintEventHandler(MainPanel::Paint));
	Connect(wxEVT_LEFT_UP, wxMouseEventHandler(MainPanel::LMouseUp));
	Connect(wxEVT_TIMER, wxTimerEventHandler(MainPanel::OnTimer), NULL, this);
	
	timer = new wxTimer();
	timer->SetOwner(this);
	//timer->Start(5);
}

void MainPanel::OnTimer(wxTimerEvent& event)
{	int x, y;
	int origmove = movenum;
	int count = 0;
	while(origmove==movenum)
	{	x = 1+rand()%19;
		y = 1+rand()%19;
		makemove(x, y);
		count++;
		//printf("attempt %d\n", count);
		if(count>3000)
		{	//timer->Stop();
			turn = (turn+1)%2;
			break;
		}
	}
}

void MainPanel::DrawStone(wxDC& dc, int x, int y, int colour)
{	if(colour!=1 && colour!=2)
		return;
	else if(colour==1)
		dc.SetBrush(*wxBLACK_BRUSH);
	else if(colour==2)
		dc.SetBrush(*wxWHITE_BRUSH);
	dc.DrawCircle(wxPoint(16*x,16*y), 6);
}

void MainPanel::DrawBoard(wxDC& dc)
{	int i, j;

	dc.Clear();

	dc.SetPen(*wxBLACK_PEN);
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
	dc.DrawRectangle(16*16-1, 16*16-1, 3, 3);

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

public:
	MainPanel* panel;
	MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
};

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size) : wxFrame(NULL, -1, title, pos, size)
{	panel = new MainPanel(this);

	PassButton = new wxButton(this, wxID_HIGHEST+1, _T("Pass"), wxPoint(125, 0));
	Connect(wxID_HIGHEST+1, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::Pass));
	
	TurnText = new wxStaticText(this, wxID_ANY, _T("Black to play"), wxPoint(0, 0));
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

void MainFrame::Pass(wxCommandEvent& WXUNUSED(event))
{	turn = (turn+1)%2;
	if(turn==0)
		TurnText->SetLabel("White to play");
	else
		TurnText->SetLabel("Black to play");
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

void MainPanel::makemove(int x, int y)
{	if(x==0 || y==0 || x>boardsize || y>boardsize)
		return;
	wxClientDC dc(this);
	char temp[21][21];
	memcpy(temp, board, sizeof(temp));
	if(board[x][y]==0)
	{	temp[x][y] = (turn+1)%2+1;
		int colour = temp[x][y];
		bool haslib = hasliberties(temp, x, y);
		bool lefthaslib = hasliberties(temp, x-1, y);
		bool righthaslib = hasliberties(temp, x+1, y);
		bool downhaslib = hasliberties(temp, x, y-1);
		bool uphaslib = hasliberties(temp, x, y+1);
	
		if(haslib || (!lefthaslib && temp[x-1][y]!=colour) || (!righthaslib && temp[x+1][y]!=colour) || (!downhaslib && temp[x][y-1]!=colour) || (!uphaslib && temp[x][y+1]!=colour))
		{	if(!lefthaslib && temp[x-1][y]!=colour)
				removegroup(temp, x-1, y);
			if(!righthaslib && temp[x+1][y]!=colour)
				removegroup(temp, x+1, y);
			if(!downhaslib && temp[x][y-1]!=colour)
				removegroup(temp, x, y-1);
			if(!uphaslib && temp[x][y+1]!=colour)
				removegroup(temp, x, y+1);
				
			bool dupe = false;
			for(int i=0; i<movenum; i++)
				if(memcmp(temp, history[i], sizeof(temp))==0)
					dupe = true;
			
			if(dupe==false)
			{	turn = (turn+1)%2;
				board[x][y] = turn+1;
				DrawStone(dc, x, y, turn+1);
				if(memcmp(temp, board, sizeof(temp)))
				{	memcpy(board, temp, sizeof(temp));
					DrawBoard(dc);
				}
				history = (char(*)[21][21])realloc(history, (movenum+1)*sizeof(char[21][21]));
				memcpy(history[movenum], board, sizeof(history[movenum]));
				movenum++;
				printf("Move %d:\n", movenum);
				printboard(board, x, y);
				
				if(turn==0)
					TurnText->SetLabel("White to play");
				else
					TurnText->SetLabel("Black to play");
			}
		}
	}
}

void MainPanel::LMouseUp(wxMouseEvent& event)
{	int x, y;
	x = 1+(event.GetPosition().x-8)/16;
	y = 1+(event.GetPosition().y-8)/16;
	makemove(x, y);
}

bool MyApp::OnInit()
{
	turn = 1;
	movenum = 0;
	boardsize = 19;
	history = NULL;
	for(int i=0; i<21; i++)
		for(int j=0; j<21; j++)
			board[i][j] = 0;
			
	for(int i=0; i<21; i++)
	{	board[i][0] = 3;
		board[i][boardsize+1] = 3;
		board[0][i] = 3;
		board[boardsize+1][i] = 3;
	}
	
	frame = new MainFrame(wxT("Go Points"), wxPoint(-1, -1), wxSize(350, 400));
	
	frame->Show();
	SetTopWindow(frame);
	
	return true;
} 

IMPLEMENT_APP(MyApp)
