#ifndef MXMULTIPLECHOICEEDITOR_H
#define MXMULTIPLECHOICEEDITOR_H
#include <map>
#include <wx/arrstr.h>
#include "mxCommonConfigControls.h"

class wxWindow;
class wxTextCtrl;
class wxCheckListBox;

class mxMultipleChoiceEditor : public mxDialog {
public:
	mxMultipleChoiceEditor(wxWindow *parent, wxString title, wxString message, wxTextCtrl *text, const wxArrayString &options_array, bool comma_splits=true);
protected:
	void OnOkButton(wxCommandEvent &evt);
	void OnCancelButton(wxCommandEvent &evt);
	void OnFilter(wxCommandEvent &evt);
	void List2Set();
	void Set2List();
	
	wxCheckListBox *m_check_list;
	wxTextCtrl *m_filter_text;
	wxTextCtrl *m_dst_text;
	typedef std::map<wxString,bool> map_t;
	map_t m_data;
	DECLARE_EVENT_TABLE();
};

#endif

