#include <wx/wx.h>
#include <wx/aboutdlg.h>
#include <wx/textfile.h>
#ifndef __WXMSW__
#include <sys/resource.h>
#endif
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
	playmenu->Append(ID_GO_TO_MOVE, wxT("&Go to move..."));
	playmenu->Append(ID_RANDOM, wxT("&Random!"), "", wxITEM_CHECK);
	gamemenu->Append(ID_LOAD_GAME, wxT("&Load game..."));
	gamemenu->Append(ID_SAVE_GAME, wxT("&Save game..."));
	gamemenu->AppendSeparator();
	gamemenu->Append(ID_ABOUT, wxT("Ab&out..."));
	
	Connect(ID_NEW_GAME, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SimpleGoFrame::NewGame));
	Connect(ID_BOARD_SIZE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SimpleGoFrame::GetBoard));
	Connect(ID_GO_TO_MOVE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SimpleGoFrame::GoToMove));
	Connect(ID_LOAD_GAME, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SimpleGoFrame::LoadGame));
	Connect(ID_SAVE_GAME, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SimpleGoFrame::SaveGame));
	Connect(ID_ABOUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SimpleGoFrame::About));
	Connect(wxEVT_MENU_HIGHLIGHT, wxMenuEventHandler(SimpleGoFrame::Nothing));
	
	menubar->Append(gamemenu, wxT("&Game"));
	menubar->Append(playmenu, wxT("&Play"));
	SetMenuBar(menubar);
	
	int styles[3] = {wxSB_SUNKEN, wxSB_SUNKEN, wxSB_SUNKEN};
	CreateStatusBar(3, wxSTB_SHOW_TIPS)->SetStatusStyles(3, styles);
	SetClientSize(16*(panel->boardsize+1), 16*(panel->boardsize+1));
	panel->InitGame();
	panel->UpdateBoard();
}

// New game menu command
void SimpleGoFrame::NewGame(wxCommandEvent& event)
{	panel->InitGame();
	panel->UpdateBoard();
}

// Board size... menu command
void SimpleGoFrame::GetBoard(wxCommandEvent& event)
{	int num = wxAtoi(wxGetTextFromUser("Enter the new board size, between 2 and 19:", "New board size", ""));
	if(num>=2&&num<=19)
	{	panel->boardsize = num;
		panel->InitGame();
		SetSize(num);
		panel->UpdateBoard();
	}
}

// Go to move... menu command
void SimpleGoFrame::GoToMove(wxCommandEvent& event)
{	int num = wxAtoi(wxGetTextFromUser(wxString::Format("Enter the move number to go to, between 0 and %d:", panel->totmove), "Go to move", ""));
	if(num>=0 && num<=panel->totmove)
	{	panel->curmove = num;
		panel->UpdateBoard();
	}
}

// Load game... menu command
void SimpleGoFrame::LoadGame(wxCommandEvent& event)
{	wxFileDialog LoadDialog(this, "Load Game", "", "", "*.sgf", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	
	if(LoadDialog.ShowModal()==wxID_OK)
	{	wxFile file(LoadDialog.GetPath());
		bool sizeset = false;
		wxString str;
		file.ReadAll(&str);
		int j=0;
		for(int i=0; i<str.Len(); i++)
			if(strchr(" \t\r\n\v\f", str.GetChar(i))==NULL)
				str.SetChar(j++, str.GetChar(i));
		str.Remove(j);
		for(int i=0; i<str.Len(); i++)
		{	wxString substr = str.Mid(i, 3);
			if(substr.Cmp("SZ[")==0)
			{	int num = wxAtoi(str.Mid(i+3, 2));
				if(num>=2&&num<=19)
				{	panel->boardsize = num;
					panel->InitGame();
				}
				sizeset = true;
			}
			else if((substr.Find("B[")==1 || substr.Find("W[")==1) && !(substr.GetChar(0)>='A' && substr.GetChar(0)<='Z'))
			{	if(!sizeset)
				{	panel->boardsize = 19;
					panel->InitGame();
				}
				if(str.Mid(i+3, 1).Cmp("a")>=0 && str.Mid(i+3, 1).Cmp("s")<=0 && str.Mid(i+4, 1).Cmp("a")>=0 && str.Mid(i+4, 1).Cmp("s")<=0)
				{	int x = str.Mid(i+3, 1).GetChar(0) - 'a' + 1;
					int y = str.Mid(i+4, 1).GetChar(0) - 'a' + 1;
					panel->MakeMoveSGF(x, y);
				}
			}
		}
		file.Close();
		SetSize(panel->boardsize);
		panel->UpdateBoard();
	}
}

// Save game... menu command
void SimpleGoFrame::SaveGame(wxCommandEvent& event)
{	char str[21];
	time_t rawtime;
	time(&rawtime);
	strftime(str, 21, "pente-%y%m%d-1.sgf", localtime(&rawtime));
	for(int i=2; wxFileExists(str); i++)
		sprintf(str+6, "-%d.sgf", i);
	
	wxFileDialog SaveDialog(this, "Save Game", "", str, "*.sgf", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	
	if(SaveDialog.ShowModal()==wxID_OK)
	{	wxFile file(SaveDialog.GetPath(), wxFile::write);
		file.Write(wxString::Format("(;FF[4]GM[41]SZ[%d]AP[Simple Pente:%s]", panel->boardsize, VERSION));
		for(int i=0; i<panel->totmove; i++)
		{	file.Write(wxString::Format(";%c[%c%c]", i%2 ? 'W' : 'B', panel->movelist[i].x+'a'-1, panel->movelist[i].y+'a'-1));
		}
		file.Write(")");
		file.Close();
	}
}

// About... menu command
void SimpleGoFrame::About(wxCommandEvent& event)
{	wxAboutDialogInfo info;
	info.SetName("Simple Pente");
	info.SetVersion(VERSION);
	info.SetDescription("A simple implementation of the game Pente\nby Curtis Bright");
	info.SetWebSite("http://www.curtisbright.com/simplego/");
	wxAboutBox(info);
}

// Menu highlight event
void SimpleGoFrame::Nothing(wxMenuEvent& event)
{	// Don't clear the status bar
}

// Set the window size
void SimpleGoFrame::SetSize(int boardsize)
{	if(boardsize>9)
		SetClientSize(16*(boardsize+1), 16*(boardsize+1));
	else
		SetClientSize(160, 160);
}
