#include "wx/wx.h"

//int x, y;
int turn;
int move;
int board[19][19];
int (*history)[19][19];

wxButton* PassButton;
//wxTextCtrl* Depth1Text;

/*wxString pointname()
{	wxString output = "";
	output += wxString::Format("%c", x>=8 ? 'A'+x+1 : 'A'+x);
	output += wxString::Format("%d", 19-y);
	return output;
}*/

void boardcpy(int cpy[19][19], int src[19][19])
{	for(int i=0; i<19; i++)
		for(int j=0; j<19; j++)
			cpy[i][j] = src[i][j];
}

void printboard(int board[19][19])
{	for(int j=0; j<19; j++)
	{	for(int i=0; i<19; i++)
		{	if(board[i][j]==0)
				printf(".");
			else if(board[i][j]==1)
				printf("X");
			else if(board[i][j]==2)
				printf("O");
			else if(board[i][j]==3)
				printf("*");
		}
		printf("\n");
	}
	printf("\n");
}

void removegroup(int board[19][19], int x, int y)
{	int colour = board[x][y];
	board[x][y] = 0;
	if(x>1 && board[x-1][y]==colour)
		removegroup(board, x-1, y);
	if(x<18 && board[x+1][y]==colour)
		removegroup(board, x+1, y);
	if(y>1 && board[x][y-1]==colour)
		removegroup(board, x, y-1);
	if(y<18 && board[x][y+1]==colour)
		removegroup(board, x, y+1);
}

bool hasliberties(int board[19][19], int x, int y)
{	if(x>1 && board[x-1][y]==0)
		return true;
	else if(y>1 && board[x][y-1]==0)
		return true;
	else if(x<18 && board[x+1][y]==0)
		return true;
	else if(y<18 && board[x][y+1]==0)
		return true;
	int colour = board[x][y];
	int temp[19][19], copy[19][19];
	boardcpy(temp, board);
	temp[x][y] = 3;
	boardcpy(copy, temp);
	bool result = false;
	result |= ((x>1 && colour==copy[x-1][y]) ? hasliberties(temp, x-1, y) : false);
	boardcpy(copy, temp);
	result |= ((y>1 && colour==copy[x][y-1]) ? hasliberties(temp, x, y-1) : false);
	boardcpy(copy, temp);
	result |= ((x<18 && colour==copy[x+1][y]) ? hasliberties(temp, x+1, y) : false);
	boardcpy(copy, temp);
	result |= ((y<18 && colour==copy[x][y+1]) ? hasliberties(temp, x, y+1) : false);
	return result;
}

class MainPanel : public wxPanel
{
public:
	void LMouseUp(wxMouseEvent& event);
	void Paint(wxPaintEvent& evt);
	void DrawStone(wxDC& dc, int x, int y, int colour);
	void DrawBoard(wxDC& dc);
	MainPanel(wxFrame* parent);
};

MainPanel* panel;

MainPanel::MainPanel(wxFrame* parent) : wxPanel(parent, wxID_ANY, wxPoint(0, 0), wxSize(325, 325), wxTAB_TRAVERSAL, _T("panel"))
{		
	Connect(wxEVT_PAINT, wxPaintEventHandler(MainPanel::Paint));
	Connect(wxEVT_LEFT_UP, wxMouseEventHandler(MainPanel::LMouseUp));
}

void MainPanel::DrawStone(wxDC& dc, int x, int y, int colour)
{	if(colour==0)
		return;
	else if(colour==1)
		dc.SetBrush(*wxBLACK_BRUSH);
	else if(colour==2)
		dc.SetBrush(*wxWHITE_BRUSH);
	else
		assert(false);
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
			
	//if(x>=0)
	//	DrawStone(dc, x, y);

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
}

void MainPanel::LMouseUp(wxMouseEvent& event)
{	int x, y;
	x = (event.GetPosition().x-8)/16;
	y = (event.GetPosition().y-8)/16;
	if(x<0 || x>18 || y<0 || y>18)
		return;
	wxClientDC dc(this);
	int temp[19][19];
	boardcpy(temp, board);
	if(board[x][y]==0)
	{	temp[x][y] = (turn+1)%2+1;
		if(hasliberties(temp, x, y) || (x>1 && !hasliberties(temp, x-1, y))
		                            || (x<18 && !hasliberties(temp, x+1, y))
		                            || (y>1 && !hasliberties(temp, x, y-1))
		                            || (y<18 && !hasliberties(temp, x, y+1)))
		{	if(x>1 && !hasliberties(temp, x-1, y))
				removegroup(temp, x-1, y);
			if(x<18 && !hasliberties(temp, x+1, y))
				removegroup(temp, x+1, y);
			if(y>1 && !hasliberties(temp, x, y-1))
				removegroup(temp, x, y-1);
			if(y<18 && !hasliberties(temp, x, y+1))
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
				{	boardcpy(board, temp);
					DrawBoard(dc);
				}
				history = (int(*)[19][19])realloc(history, (move+1)*sizeof(int[19][19]));
				boardcpy(history[move], board);
				move++;
				printf("Move %d:\n", move);
				printboard(board);
			}
		}
	}
}

bool MyApp::OnInit()
{
	frame = new MainFrame(wxT("Go Points"), wxPoint(-1, -1), wxSize(350, 400));
	/*x = 0;
	y = 18;
	x = rand()%19;
	y = rand()%19;*/
	turn = 1;
	move = 0;
	history = NULL;
	for(int i=0; i<19; i++)
		for(int j=0; j<19; j++)
			board[i][j] = 0;
	
	frame->Show();
	SetTopWindow(frame);
	
	return true;
} 

IMPLEMENT_APP(MyApp)
