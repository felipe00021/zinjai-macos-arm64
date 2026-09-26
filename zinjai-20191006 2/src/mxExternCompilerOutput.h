#ifndef MXEXTERNCOMPILEROUTPUT_H
#define MXEXTERNCOMPILEROUTPUT_H
#include <wx/listbox.h>
#include <wx/treectrl.h>

class mxExternCompilerOutput : public wxTreeCtrl {
	wxArrayString m_full_output;
	wxTreeItemId m_root;
	wxTreeItemId m_status;
	wxTreeItemId m_last_cmd;
	void SelectError(wxTreeItemId sel);
public:
	enum Type { Status=0,Cmd=1,Std=2,Err=3,TypeCount=4 };
	mxExternCompilerOutput(wxWindow *compiler_panel);
	void OnDClick(wxTreeEvent &evt);
	void OnPopup(wxMouseEvent &evt);
//	void OnOnlyErrors(wxCommandEvent &evt);
	void OnViewFullOutput(wxCommandEvent &evt);
	void AddLine(Type t, const wxString &message);
	void Clear();
	void OnErrorNext();
	void OnErrorPrev();
	
	DECLARE_EVENT_TABLE();
};

#endif

