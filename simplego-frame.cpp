#include <wx/wx.h>
#include <wx/aboutdlg.h>
#include <wx/textfile.h>
#include "simplego-frame.h"
#include "simplego-panel.h"

// Frame constructor - create the panel and add menus and status bar to it
SimpleGoFrame::SimpleGoFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(NULL, -1, title, pos, size, style)
{	panel = new SimpleGoPanel(this);
	
	wxMenuBar* menubar = new wxMenuBar;
	gamemenu = new wxMenu;
	playmenu = new wxMenu;
	
	gamemenu->Append(ID_NEW_GAME, wxT("&New game"));
	gamemenu->Append(ID_BOARD_SIZE, wxT("&Board size..."));
	playmenu->Append(ID_PASS, wxT("&Pass"));
	playmenu->Append(ID_GO_TO_MOVE, wxT("&Go to move..."));
	playmenu->Append(ID_RANDOM, wxT("&Random!"), "", wxITEM_CHECK);
	gamemenu->Append(ID_SUICIDE, wxT("&Allow suicide"), "", wxITEM_CHECK);
	gamemenu->Append(ID_SAVE_GAME, wxT("&Save game..."), "");
	gamemenu->AppendSeparator();
	gamemenu->Append(ID_ABOUT, wxT("Ab&out..."), "");
	
	Connect(ID_NEW_GAME, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SimpleGoFrame::NewGame));
	Connect(ID_BOARD_SIZE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SimpleGoFrame::SetBoard));
	Connect(ID_PASS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SimpleGoFrame::Pass));
	Connect(ID_GO_TO_MOVE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SimpleGoFrame::GoToMove));
	Connect(ID_SAVE_GAME, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SimpleGoFrame::SaveGame));
	Connect(ID_ABOUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SimpleGoFrame::About));
	Connect(wxEVT_MENU_HIGHLIGHT, wxMenuEventHandler(SimpleGoFrame::Nothing));
	
	menubar->Append(gamemenu, wxT("&Game"));
	menubar->Append(playmenu, wxT("&Play"));
	SetMenuBar(menubar);
	
	int styles[3] = {wxSB_SUNKEN, wxSB_SUNKEN, wxSB_SUNKEN};
	CreateStatusBar(3)->SetStatusStyles(3, styles);
	SetClientSize(320, 320);
	panel->InitGame();
	panel->UpdateBoard();
}

// New game menu command
void SimpleGoFrame::NewGame(wxCommandEvent& WXUNUSED(event))
{	panel->InitGame();
	panel->UpdateBoard();
}

// Board size... menu command
void SimpleGoFrame::SetBoard(wxCommandEvent& WXUNUSED(event))
{	int num = wxAtoi(wxGetTextFromUser("Enter the new board size, between 2 and 19:", "New board size", ""));
	if(num>=2&&num<=19)
	{	panel->boardsize = num;
		panel->InitGame();
		panel->UpdateBoard();
	}
}

// Pass menu command
void SimpleGoFrame::Pass(wxCommandEvent& WXUNUSED(event))
{	panel->MakePass();
}

// Go to move... menu command
void SimpleGoFrame::GoToMove(wxCommandEvent& WXUNUSED(event))
{	long num;
	if(wxGetTextFromUser(wxString::Format("Enter the move number to go to, between 0 and %d:", panel->totmove), "Go to move", "").ToLong(&num, 10)
	   && num>=0 && num<=panel->totmove)
	{	panel->curmove = num;
		panel->UpdateBoard();
	}
}

// Save game menu command
void SimpleGoFrame::SaveGame(wxCommandEvent& WXUNUSED(event))
{	char str[15];
	time_t rawtime;
	time(&rawtime);
	strftime(str, 15, "%y%m%d-1.sgf", localtime(&rawtime));
	for(int i=2; wxFileExists(str); i++)
		sprintf(str+6, "-%d.sgf", i);
	
	wxFileDialog SaveDialog(this, "Save Game", "", str, "*.sgf", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	
	if(SaveDialog.ShowModal()==wxID_OK)
	{	wxTextFile file(SaveDialog.GetPath());
		file.AddLine(wxString::Format("(;FF[4]GM[1]SZ[%d]", panel->boardsize));
		for(int i=0; i<panel->totmove; i++)
		{	if(panel->movelist[i].x==0&&panel->movelist[i].y==0)
				file.AddLine(wxString::Format(";%c[]", i%2 ? 'W' : 'B'));
			else
				file.AddLine(wxString::Format(";%c[%c%c]", i%2 ? 'W' : 'B', panel->movelist[i].x+'a'-1, panel->movelist[i].y+'a'-1));
		}
		file.AddLine(")");
		file.Write();
		file.Close();
	}
}

// Menu about event
void SimpleGoFrame::About(wxCommandEvent& WXUNUSED(event))
{	wxAboutDialogInfo info;
	info.SetName("Simple Go");
	info.SetDescription("A simple implementation of the game Go\nby Curtis Bright");
	info.SetWebSite("http://www.curtisbright.com/simplego/");
	wxAboutBox(info);
}

// Menu highlight event
void SimpleGoFrame::Nothing(wxMenuEvent& WXUNUSED(event))
{	// Don't clear the status bar
}
