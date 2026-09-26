#ifndef MXCPPCHECKCONFIGDIALOG_H
#define MXCPPCHECKCONFIGDIALOG_H

#include "mxCommonConfigControls.h"
#include "widgetDisabler.h"

class wxListBox;
class wxPanel;
class wxCheckBox;
class wxTextCtrl;
class wxNotebook;
class cppcheck_configuration;

class mxCppCheckConfigDialog : public mxDialog {
private:
	wxListBox *sources_in, *sources_out;
	wxTextCtrl *config_d;
	wxTextCtrl *config_u;
	wxTextCtrl *style;
	wxTextCtrl *platform;
	wxTextCtrl *standard;
	wxTextCtrl *suppress_file;
	wxTextCtrl *suppress_ids;
	wxTextCtrl *additional_files;
	wxCheckBox *exclude_headers;
	widgetBinder m_binder;
	cppcheck_configuration *project_cppcheck_config;
protected:
public:
	mxCppCheckConfigDialog(wxWindow *parent);
	wxPanel *CreateGeneralPanel(wxNotebook *notebook);
	wxPanel *CreateFilesPanel(wxNotebook *notebook);
	
//	void OnCheckCopyConfig(wxCommandEvent &evt);
	void OnButtonConfigD(wxCommandEvent &evt);
	void OnButtonConfigU(wxCommandEvent &evt);
	void OnButtonStyle(wxCommandEvent &evt);
	void OnButtonPlatform(wxCommandEvent &evt);
	void OnButtonStandard(wxCommandEvent &evt);
	void OnButtonSuppressFile(wxCommandEvent &evt);
	void OnButtonSuppressIds(wxCommandEvent &evt);
	void OnButtonIncludeFile(wxCommandEvent &evt);
	void OnButtonExcludeFile(wxCommandEvent &evt);
	void OnButtonAdditionalFiles(wxCommandEvent &evt);
	void OnButtonExcludeHeaders(wxCommandEvent &evt);
	void OnButtonOk(wxCommandEvent &evt);
	void OnButtonCancel(wxCommandEvent &evt);
	void OnButtonHelp(wxCommandEvent &evt);

	DECLARE_EVENT_TABLE();
};

#endif

