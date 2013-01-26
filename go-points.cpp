#include "wx/wx.h"

//int x, y;
int turn;
int board[19][19];

wxButton* AIButton;
wxTextCtrl* Depth1Text;

/*wxString pointname()
{	wxString output = "";
	output += wxString::Format("%c", x>=8 ? 'A'+x+1 : 'A'+x);
	output += wxString::Format("%d", 19-y);
	return output;
}*/

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

MainPanel::MainPanel(wxFrame* parent) : wxPanel(parent, wxID_ANY, wxPoint(0, 0), wxSize(336, 336), wxTAB_TRAVERSAL, _T("panel"))
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
	//void RunAI(wxCommandEvent& event);
	void OnEnter(wxCommandEvent& event);

public:
	MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
};

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size) : wxFrame(NULL, -1, title, pos, size)
{	panel = new MainPanel(this);

	//AIButton = new wxButton(this, wxID_HIGHEST+1, _T("OK"), wxPoint(150, 350));
	//Connect(wxID_HIGHEST+1, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::RunAI));
	
	Depth1Text = new wxTextCtrl(this, wxID_HIGHEST+2, _T(""), wxPoint(25, 350), wxDefaultSize, wxTE_PROCESS_ENTER);
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

/*void MainFrame::RunAI(wxCommandEvent& WXUNUSED(event))
{	//wxMessageBox(_("This is a wxWidgets Hello world sample"), _("About Hello World"), wxOK | wxICON_INFORMATION, this);

	//long depth1, depth2;
	//Depth1Text->GetValue().ToLong(&depth1);
	//Depth2Text->GetValue().ToLong(&depth2);
	
	if(Depth1Text->GetValue().Cmp(pointname())!=0)
		wxMessageBox(pointname(), _("Wrong"), wxOK | wxICON_INFORMATION, this);
	
	x = rand()%19;
	y = rand()%19;
	Depth1Text->SetValue("");
	
	Refresh();
}*/

void MainPanel::LMouseUp(wxMouseEvent& event)
{	int x, y;
	x = (event.GetPosition().x-8)/16;
	y = (event.GetPosition().y-8)/16;
	wxClientDC dc(this);
	if(board[x][y]==0)
	{	turn = (turn+1)%2;
		board[x][y] = turn+1;
		DrawStone(dc, x, y, turn+1);
	}
}

bool MyApp::OnInit()
{
	frame = new MainFrame(wxT("Go Points"), wxPoint(-1, -1), wxSize(700, 700));
	/*x = 0;
	y = 18;
	x = rand()%19;
	y = rand()%19;*/
	turn = 1;
	
	frame->Show();
	SetTopWindow(frame);
	
	return true;
} 

IMPLEMENT_APP(MyApp)
