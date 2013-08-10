#include <wx/wx.h>
#include "simplego-panel.h"
#include "simplego-frame.h"

SimpleGoFrame::SimpleGoFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(NULL, -1, title, pos, size, style)
{	panel = new SimpleGoPanel(this);
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
	
	Connect(ID_PASS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SimpleGoFrame::Pass));
	Connect(ID_GO_TO_MOVE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SimpleGoFrame::GoToMove));
	Connect(ID_NEW_GAME, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SimpleGoFrame::NewGame));
	Connect(ID_BOARD_SIZE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SimpleGoFrame::SetBoard));
	Connect(ID_RANDOM, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SimpleGoFrame::Random));
	Connect(wxEVT_MENU_HIGHLIGHT, wxMenuEventHandler(SimpleGoFrame::Nothing));
	
	menu_bar->Append(game_menu, wxT("&Game"));
	menu_bar->Append(play_menu, wxT("&Play"));
	SetMenuBar(menu_bar);
	
	CreateStatusBar(3);
	SetClientSize(320, 320);
	panel->InitGame();
}

void SimpleGoFrame::Pass(wxCommandEvent& WXUNUSED(event))
{	panel->MakePass();
}

void SimpleGoFrame::Random(wxCommandEvent& WXUNUSED(event))
{	panel->timer->Start(1);
}

void SimpleGoFrame::Nothing(wxMenuEvent& WXUNUSED(event))
{	// Don't clear the status bar
}

void SimpleGoFrame::GoToMove(wxCommandEvent& WXUNUSED(event))
{	int num = wxAtoi(wxGetTextFromUser(wxString::Format("Enter the move number to go to, between 0 and %d:", panel->totmove), "Go to move", ""));
	if(num>=0 && num<=panel->totmove)
	{	panel->curmove = num;
		panel->UpdateBoard();
	}
}

void SimpleGoFrame::NewGame(wxCommandEvent& WXUNUSED(event))
{	panel->InitGame();
	panel->UpdateBoard();
}

void SimpleGoFrame::SetBoard(wxCommandEvent& WXUNUSED(event))
{	int num = wxAtoi(wxGetTextFromUser("Enter the new board size, between 2 and 19:", "New board size", ""));
	if(num>=2&&num<=19)
	{	panel->boardsize = num;
		panel->InitGame();
		panel->UpdateBoard();
	}
}
