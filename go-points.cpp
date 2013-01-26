#include "wx/wx.h"

int x, y;

wxButton* AIButton;
wxTextCtrl* Depth1Text;

wxString pointname()
{	wxString output = "";
	output += wxString::Format("%c", x>=8 ? 'A'+x+1 : 'A'+x);
	output += wxString::Format("%d", 19-y);
	return output;
}

class MainPanel : public wxPanel
{
public:
	void Paint(wxPaintEvent& evt);
	void DrawStone(wxDC& dc, int x, int y);
	void DrawBoard(wxDC& dc);
	MainPanel(wxFrame* parent);
};

MainPanel* panel;

MainPanel::MainPanel(wxFrame* parent) : wxPanel(parent, wxID_ANY, wxPoint(0, 0), wxSize(336, 336), wxTAB_TRAVERSAL, _T("panel"))
{		
	Connect(wxEVT_PAINT, wxPaintEventHandler(MainPanel::Paint));
}

void MainPanel::DrawStone(wxDC& dc, int x, int y)
{	dc.SetBrush(*wxBLACK_BRUSH);
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

	if(x>=0)
		DrawStone(dc, x, y);

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
	Connect(wxID_HIGHEST+2, wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(MainFrame::OnEnter));
}

void MainFrame::OnEnter(wxCommandEvent& WXUNUSED(event))
{	if(Depth1Text->GetValue().CmpNoCase(pointname())!=0)
		wxMessageBox(pointname(), _("Wrong"), wxOK | wxICON_INFORMATION, this);
	
	x = 0;
	y = 18;
	x = rand()%19;
	y = rand()%19;
	
	Depth1Text->SetValue("");
	
	Refresh();
}

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

bool MyApp::OnInit()
{
	frame = new MainFrame(wxT("Go Points"), wxPoint(-1, -1), wxSize(700, 700));
	x = 0;
	y = 18;
	x = rand()%19;
	y = rand()%19;
	
	frame->Show();
	SetTopWindow(frame);
	
	return true;
} 

IMPLEMENT_APP(MyApp)
