#include <wx/wx.h>
#include <wx/aboutdlg.h>
#include <wx/textfile.h>
#ifndef __WXMSW__
#include <sys/resource.h>
#endif
#include "simplego-frame.h"
#include "simplego-panel.h"
#include "simplego-settings.h"

// Frame constructor - create the panel and add menus and status bar to it
SimpleGoFrame::SimpleGoFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(NULL, -1, title, pos, size, style)
{	panel = new SimpleGoPanel(this);
	
	wxMenuBar* menubar = new wxMenuBar;
	gamemenu = new wxMenu;
	playmenu = new wxMenu;
	
	gamemenu->Append(ID_NEW_GAME, wxT("&New game"));
	gamemenu->Append(ID_BOARD_SIZE, wxT("&Board size..."));
	gamemenu->Append(ID_SETTINGS, wxT("&Settings..."));
	playmenu->Append(ID_PASS, wxT("&Pass"));
	playmenu->Append(ID_GO_TO_MOVE, wxT("&Go to move..."));
	playmenu->Append(ID_GNUGO, wxT("&Make GNU Go move"));
	playmenu->Append(ID_RANDOM, wxT("&Random!"), "", wxITEM_CHECK);
	gamemenu->Append(ID_GNUGO_WHITE, wxT("GNU Go plays &White"), "", wxITEM_CHECK);
	gamemenu->Append(ID_SCORE_GAME, wxT("S&core game"));
	gamemenu->Append(ID_LOAD_GAME, wxT("&Load game..."));
	gamemenu->Append(ID_SAVE_GAME, wxT("&Save game..."));
	gamemenu->AppendSeparator();
	gamemenu->Append(ID_ABOUT, wxT("Ab&out..."));
	
	#ifdef __WXMSW__
	playmenu->Enable(ID_GNUGO, false);
	gamemenu->Enable(ID_GNUGO_WHITE, false);
	gamemenu->Enable(ID_SCORE_GAME, false);
	#endif
	
	Connect(ID_NEW_GAME, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SimpleGoFrame::NewGame));
	Connect(ID_BOARD_SIZE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SimpleGoFrame::GetBoard));
	Connect(ID_SETTINGS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SimpleGoFrame::Settings));
	Connect(ID_PASS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SimpleGoFrame::Pass));
	Connect(ID_GO_TO_MOVE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SimpleGoFrame::GoToMove));
	Connect(ID_GNUGO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SimpleGoFrame::GNUGoMove));
	Connect(ID_GNUGO_WHITE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SimpleGoFrame::GNUGoWhite));
	Connect(ID_SCORE_GAME, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SimpleGoFrame::ScoreGame));
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

// Run GNU Go and have it play the next move
void SimpleGoFrame::MakeGNUGoMove()
{	int in[2], out[2], n;
	char buf[256];
	char data[256] = {0};
	char str[256], strtime[32], strkomi[32], strlevel[32];

	#ifndef __WXMSW__
	pipe(in);
	pipe(out);

	if(fork()==0)
	{	close(STDIN_FILENO);
		close(STDOUT_FILENO);
		dup2(in[0], STDIN_FILENO);
		dup2(out[1], STDOUT_FILENO);
		close(in[1]);
		close(out[0]);
		sprintf(str, "--%s-suicide", suicide ? "allow" : "forbid");
		sprintf(strtime, "%d", timeout);
		sprintf(strkomi, "%.1f", komi);
		sprintf(strlevel, "%d", panel->curmove%2 ? whitelevel : blacklevel);
		execlp("gnugo", "gnugo", "--mode", "gtp", "--chinese-rules", "--no-ko", str, "--level", strlevel[0]=='0' ? "10" : strlevel, "--never-resign", "--komi", strkomi, "--clock", strtime, NULL);
	}
	
	close(in[0]);
	close(out[1]);

	sprintf(str, "boardsize %d\n", panel->boardsize);
	write(in[1], str, strlen(str));
	while(read(out[0], buf, 1) && buf[0] != '=');

	for(int i=0; i<panel->curmove; i++)
	{	if(panel->movelist[i].x==0&&panel->movelist[i].y==0)
			sprintf(str, "play %s pass\n", i%2 ? "white" : "black");
		else
			sprintf(str, "play %s %c%d\n", i%2 ? "white" : "black", panel->movelist[i].x+'A'-1+(panel->movelist[i].x>8 ? 1 : 0), (panel->boardsize+1)-panel->movelist[i].y);
		write(in[1], str, strlen(str));
		while(read(out[0], buf, 1) && buf[0] != '=');
	}

	sprintf(str, "genmove %s\n", panel->curmove%2 ? "white" : "black");
	write(in[1], str, strlen(str));

	close(in[1]);

	while(read(out[0], buf, 1))
		if(buf[0] == '=')
		{	while(n = read(out[0], buf, 255))
			{	buf[n] = '\0';
				strcat(data, buf);
			}
			if(data[1]>='A' && data[1]<='T' && data[2]>='1' && data[2]<='9')
			{	int x = data[1]-'A'+1-(data[1]>'H' ? 1 : 0);
				int y = (panel->boardsize+1)-atoi(data+2);
				panel->MakeMove(x, y);
			}
			else
				panel->MakePass();
		}
	
	close(out[0]);
	#endif
}

// Run GNU Go and have it score the game
void SimpleGoFrame::MakeGNUGoScore()
{	int in[2], out[2], n;
	char buf[256];
	char data[2048] = {0};
	char str[256];

	#ifndef __WXMSW__
	pipe(in);
	pipe(out);

	if(fork()==0)
	{	close(STDIN_FILENO);
		close(STDOUT_FILENO);
		dup2(in[0], STDIN_FILENO);
		dup2(out[1], STDOUT_FILENO);
		close(in[1]);
		close(out[0]);
		struct rlimit limit;
		limit.rlim_cur = timeout;
		setrlimit(RLIMIT_CPU, &limit);
		sprintf(str, "--%s-suicide", suicide ? "allow" : "forbid");
		execlp("gnugo", "gnugo", "--mode", "gtp", "--chinese-rules", str, NULL);
	}
	
	close(in[0]);
	close(out[1]);

	sprintf(str, "boardsize %d\n", panel->boardsize);
	write(in[1], str, strlen(str));
	while(read(out[0], buf, 1) && buf[0] != '=');

	for(int i=1; i<=panel->boardsize; i++)
		for(int j=1; j<=panel->boardsize; j++)
			if(panel->board[i][j]!=EMPTY)
			{	sprintf(str, "play %s %c%d\n", panel->board[i][j]==WHITE ? "white" : "black", i+'A'-1+(i>8 ? 1 : 0), (panel->boardsize+1)-j);
				write(in[1], str, strlen(str));
				while(read(out[0], buf, 1) && buf[0] != '=');
			}

	sprintf(str, "final_status_list dead\n");
	write(in[1], str, strlen(str));
	
	close(in[1]);

	memcpy(panel->gnugoboard, panel->board, BOARDMEMORYLEN);

	while(read(out[0], buf, 1))
		if(buf[0] == '=')
		{	while(n = read(out[0], buf, 255))
			{	buf[n] = '\0';
				strcat(data, buf);
			}
			for(int i=0; i<(int)strlen(data)-1; i++)
				if(data[i]>='A' && data[i]<='T' && data[i+1]>='1' && data[i+1]<='9')
				{	int x = data[i]-'A'+1-(data[i]>'H' ? 1 : 0);
					int y = (panel->boardsize+1)-atoi(data+i+1);
					panel->gnugoboard[x][y] = OPP(panel->board[x][y]);
				}
		}
	
	close(out[0]);
	
	panel->ScoreArea(panel->gnugoboard);
	panel->gnugoscore = true;
	panel->UpdateBoard();
	#endif
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

// Settings... menu command
void SimpleGoFrame::Settings(wxCommandEvent& event)
{	SimpleGoSettingsDialog dialog(this);
	dialog.ShowModal();
}

// Pass menu command
void SimpleGoFrame::Pass(wxCommandEvent& event)
{	panel->MakePass();
}

// Go to move... menu command
void SimpleGoFrame::GoToMove(wxCommandEvent& event)
{	int num = wxAtoi(wxGetTextFromUser(wxString::Format("Enter the move number to go to, between 0 and %d:", panel->totmove), "Go to move", ""));
	if(num>=0 && num<=panel->totmove)
	{	panel->gnugoscore = false;
		panel->curmove = num;
		panel->UpdateBoard();
	}
}

// GNU Go move menu command
void SimpleGoFrame::GNUGoMove(wxCommandEvent& event)
{	MakeGNUGoMove();
}

// GNU Go plays White menu command
void SimpleGoFrame::GNUGoWhite(wxCommandEvent& event)
{	if(panel->curmove%2==1 && gamemenu->IsChecked(ID_GNUGO_WHITE))
		MakeGNUGoMove();
}

// Score game menu command
void SimpleGoFrame::ScoreGame(wxCommandEvent& event)
{	MakeGNUGoScore();
}

// Load game... menu command
void SimpleGoFrame::LoadGame(wxCommandEvent& event)
{	wxFileDialog LoadDialog(this, "Load Game", "", "", "*.sgf", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	
	if(LoadDialog.ShowModal()==wxID_OK)
	{	wxFile file(LoadDialog.GetPath());
		bool prevgnugoplay = gamemenu->IsChecked(ID_GNUGO_WHITE);
		gamemenu->Check(ID_GNUGO_WHITE, false);
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
				if(str.Mid(i+2, 2).Cmp("[]")==0)
					panel->MakePass();
				else if(str.Mid(i+3, 1).Cmp("a")>=0 && str.Mid(i+3, 1).Cmp("s")<=0 && str.Mid(i+4, 1).Cmp("a")>=0 && str.Mid(i+4, 1).Cmp("s")<=0)
				{	int x = str.Mid(i+3, 1).GetChar(0) - 'a' + 1;
					int y = str.Mid(i+4, 1).GetChar(0) - 'a' + 1;
					panel->MakeMoveSGF(x, y);
				}
			}
		}
		file.Close();
		gamemenu->Check(ID_GNUGO_WHITE, prevgnugoplay);
		SetSize(panel->boardsize);
		panel->UpdateBoard();
	}
}

// Save game... menu command
void SimpleGoFrame::SaveGame(wxCommandEvent& event)
{	char str[15];
	time_t rawtime;
	time(&rawtime);
	strftime(str, 15, "%y%m%d-1.sgf", localtime(&rawtime));
	for(int i=2; wxFileExists(str); i++)
		sprintf(str+6, "-%d.sgf", i);
	
	wxFileDialog SaveDialog(this, "Save Game", "", str, "*.sgf", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	
	if(SaveDialog.ShowModal()==wxID_OK)
	{	wxFile file(SaveDialog.GetPath(), wxFile::write);
		file.Write(wxString::Format("(;FF[4]GM[1]SZ[%d]AP[Simple Go:%s]", panel->boardsize, VERSION));
		if(panel->gnugoscore)
		{	int score = -6;
			for(int i=1; i<=panel->boardsize; i++)
				for(int j=1; j<=panel->boardsize; j++)
					if(panel->gnugoboard[i][j]==BLACK)
						score++;
					else if(panel->gnugoboard[i][j]==WHITE)
						score--;
			file.Write(wxString::Format("KM[6.5]RE[%c+%d.5]", score>0 ? 'B' : 'W', score>0 ? score-1 : -score));
		}
		for(int i=0; i<panel->totmove; i++)
		{	if(panel->movelist[i].x==0&&panel->movelist[i].y==0)
				file.Write(wxString::Format(";%c[]", i%2 ? 'W' : 'B'));
			else
				file.Write(wxString::Format(";%c[%c%c]", i%2 ? 'W' : 'B', panel->movelist[i].x+'a'-1, panel->movelist[i].y+'a'-1));
		}
		file.Write(")");
		file.Close();
	}
}

// About... menu command
void SimpleGoFrame::About(wxCommandEvent& event)
{	wxAboutDialogInfo info;
	info.SetName("Simple Go");
	info.SetVersion(VERSION);
	info.SetDescription("A simple implementation of the game Go\nby Curtis Bright");
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
