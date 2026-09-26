#ifndef MXOUTPUTVIEW_H
#define MXOUTPUTVIEW_H
#include <wx/dialog.h>
#include <wx/process.h>
#include <wx/timer.h>
#include <wx/frame.h>
#include "mxValgrindOuput.h"

class wxTextCtrl;
class wxProcess;
class wxTimer;
class wxStaticText;
class mxBitmapButton;
class wxFile;
class wxGauge;

enum mxOVmode {
	mxOV_EXTRA_NULL,
	mxOV_EXTRA_COMMAND,
	mxOV_EXTRA_URL,
};

class mxOutputView : public wxFrame {
private:
	wxTextCtrl *ctrl_std;
	wxTextCtrl *ctrl_err;
	wxStaticText *state;
	wxGauge *progress_bar;
	wxProcess *process;
	wxTimer *timer;
	int pid;
	mxOVmode extra_mode;
	wxString extra_command;
	wxString extra_label;
	wxString win_caption;
	mxBitmapButton *extra_button;
	mxBitmapButton *close_button;
	bool working;
	mxVOmode output_mode;
	wxString output_file;
	wxFile *textfile;
	wxString second_path;    ///< if a second command must be run after first one finishes, here's the workdir
	wxString second_command; ///< if a second command must be run after first one finishes, here's the command
public:
	mxOutputView(wxString caption, mxOVmode extra_mode=mxOV_EXTRA_NULL, wxString extra_button="",wxString extra_command="", mxVOmode output_mode=mxVO_NULL, wxString output_file="");
	void Launch(wxString path, wxString command, wxString second_path="", wxString second_command="");
	void Launched(wxProcess *_process, int _pid);
	void GetProcessOutput();
	void OnOkButton(wxCommandEvent &evt);
	void OnExtraButton(wxCommandEvent &evt);
	void OnClose(wxCloseEvent &evt);
	void OnProcessTerminateCommon(int exit_code);
	void OnProcessTerminate(wxProcessEvent &evt);
	void OnTimer(wxTimerEvent &evt);
	~mxOutputView();
	DECLARE_EVENT_TABLE();
};

#endif

