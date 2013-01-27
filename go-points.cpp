#include "wx/wx.h"

//int x, y;
int turn;
int move;
int board[19][19];
int (*history)[19][19];

wxButton* PassButton;

/*wxString pointname()
{	wxString output = "";
	output += wxString::Format("%c", x>=8 ? 'A'+x+1 : 'A'+x);
	output += wxString::Format("%d", 19-y);
	return output;
}*/

void printboard(int board[19][19], int x, int y)
{	for(int j=0; j<19; j++)
	{	for(int i=0; i<19; i++)
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

void removegroup(int board[19][19], int x, int y)
{	if(x<0 || x>18 || y<0 || y>18)
		return;
	int colour = board[x][y];
	if(colour==0)
		return;
	board[x][y] = 0;
	if(x>0 && board[x-1][y]==colour)
		removegroup(board, x-1, y);
	if(x<18 && board[x+1][y]==colour)
		removegroup(board, x+1, y);
	if(y>0 && board[x][y-1]==colour)
		removegroup(board, x, y-1);
	if(y<18 && board[x][y+1]==colour)
		removegroup(board, x, y+1);
}

bool haslibertiesrec(int board[19][19], int x, int y)
{	if((x>0 && board[x-1][y]==0) || (y>0 && board[x][y-1]==0) || (x<18 && board[x+1][y]==0) || (y<18 && board[x][y+1]==0))
		return true;
	int colour = board[x][y];
	board[x][y] = 3;
	bool result = false;
	result |= ((x>0 && colour==board[x-1][y]) ? haslibertiesrec(board, x-1, y) : false);
	result |= ((y>0 && colour==board[x][y-1]) ? haslibertiesrec(board, x, y-1) : false);
	result |= ((x<18 && colour==board[x+1][y]) ? haslibertiesrec(board, x+1, y) : false);
	result |= ((y<18 && colour==board[x][y+1]) ? haslibertiesrec(board, x, y+1) : false);
	return result;
}

bool hasliberties(int board[19][19], int x, int y)
{	if(x<0 || x>18 || y<0 || y>18)
		return false;
	int temp[19][19];
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

MainPanel::MainPanel(wxFrame* parent) : wxPanel(parent, wxID_ANY, wxPoint(0, 0), wxSize(325, 325), wxTAB_TRAVERSAL, _T("panel"))
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
	int origmove = move;
	int count = 0;
	while(origmove==move)
	{	x = rand()%19;
		y = rand()%19;
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
{	if(colour==0)
		return;
	else if(colour==1)
		dc.SetBrush(*wxBLACK_BRUSH);
	else if(colour==2)
		dc.SetBrush(*wxWHITE_BRUSH);
	else
	{	printboard(board, x, y);
		assert(false);
	}
	dc.DrawCircle(wxPoint(16*(x+1),16*(y+1)), 6);
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

	for(i=0; i<19; i++)
		for(j=0; j<19; j++)
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

	PassButton = new wxButton(this, wxID_HIGHEST+1, _T("Pass"), wxPoint(125, 325));
	Connect(wxID_HIGHEST+1, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::Pass));
	
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
{	//printf("makemove(%d, %d)\n", x, y);
	if(x<0 || x>18 || y<0 || y>18)
		return;
	wxClientDC dc(this);
	int temp[19][19];
	memcpy(temp, board, sizeof(temp));
	if(board[x][y]==0)
	{	temp[x][y] = (turn+1)%2+1;
		int colour = temp[x][y];
		bool haslib = hasliberties(temp, x, y);
		bool lefthaslib = hasliberties(temp, x-1, y);
		bool righthaslib = hasliberties(temp, x+1, y);
		bool downhaslib = hasliberties(temp, x, y-1);
		bool uphaslib = hasliberties(temp, x, y+1);
	
		if(haslib || (x>0 && !lefthaslib && temp[x-1][y]!=colour) || (x<18 && !righthaslib && temp[x+1][y]!=colour) || (y>0 && !downhaslib && temp[x][y-1]!=colour) || (y<18 && !uphaslib && temp[x][y+1]!=colour))
		{	if(!lefthaslib && temp[x-1][y]!=colour)
				removegroup(temp, x-1, y);
			if(!righthaslib && temp[x+1][y]!=colour)
				removegroup(temp, x+1, y);
			if(!downhaslib && temp[x][y-1]!=colour)
				removegroup(temp, x, y-1);
			if(!uphaslib && temp[x][y+1]!=colour)
				removegroup(temp, x, y+1);
				
			bool dupe = false;
			for(int i=0; i<move; i++)
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
				history = (int(*)[19][19])realloc(history, (move+1)*sizeof(int[19][19]));
				memcpy(history[move], board, sizeof(history[move]));
				move++;
				printf("Move %d:\n", move);
				printboard(board, x, y);
			}
		}
	}
}

void MainPanel::LMouseUp(wxMouseEvent& event)
{	int x, y;
	x = (event.GetPosition().x-8)/16;
	y = (event.GetPosition().y-8)/16;
	makemove(x, y);
}

bool MyApp::OnInit()
{
	turn = 1;
	move = 0;
	history = NULL;
	for(int i=0; i<19; i++)
		for(int j=0; j<19; j++)
			board[i][j] = 0;
	
	frame = new MainFrame(wxT("Go Points"), wxPoint(-1, -1), wxSize(350, 400));
	
	frame->Show();
	SetTopWindow(frame);
	
	return true;
} 

IMPLEMENT_APP(MyApp)
