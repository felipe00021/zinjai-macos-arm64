#ifndef MXSOURCEUNDOHISTORY_H
#define MXSOURCEUNDOHISTORY_H
#include <wx/scrolbar.h>
#include <wx/panel.h>

class mxSource;

class mxSourceUndoHistory : public wxPanel {
	wxScrollBar *m_scrollbar;
public:
	mxSourceUndoHistory(wxWindow *parent, mxSource *src);
	~mxSourceUndoHistory();
	void OnSourceClose(); // mxSource uses this to tell this class when its being destroyed
private:
	mxSource *m_src;
	int m_redo_count, m_undo_count, m_original_pos;
	void Set(mxSource *src);
	void OnScroll(wxScrollEvent &evt);
	void OnButtonReset(wxCommandEvent &evt);
	void OnButtonKeep(wxCommandEvent &evt);
	void Goto(int pos);
	DECLARE_EVENT_TABLE();
};

#endif

