#include "mxSourceUndoHistory.h"
#include "Cpp11.h"
#include "mxSource.h"
#include <wx/sizer.h>
#include "mxSizers.h"
#include <wx/bmpbuttn.h>
#include "mxArt.h"
#include "mxAUI.h"
#include "mxMainWindow.h"
#include "mxMessageDialog.h"

BEGIN_EVENT_TABLE(mxSourceUndoHistory,wxPanel)
	EVT_COMMAND_SCROLL_CHANGED(wxID_ANY,mxSourceUndoHistory::OnScroll)
	EVT_COMMAND_SCROLL_THUMBTRACK(wxID_ANY,mxSourceUndoHistory::OnScroll)
	EVT_BUTTON(wxID_CANCEL,mxSourceUndoHistory::OnButtonReset)
	EVT_BUTTON(wxID_OK,mxSourceUndoHistory::OnButtonKeep)
END_EVENT_TABLE()

mxSourceUndoHistory::mxSourceUndoHistory(wxWindow *parent, mxSource *src) 
	: wxPanel(parent,wxID_ANY), m_src(nullptr)
{
	wxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	m_scrollbar = new wxScrollBar(this,wxID_ANY);
	sizer->Add(m_scrollbar,sizers->BA10_Exp1);
	wxBitmapButton *btn_keep = new wxBitmapButton(this,wxID_OK,*bitmaps->buttons.ok);
	wxBitmapButton *btn_back = new wxBitmapButton(this,wxID_CANCEL,*bitmaps->buttons.cancel);
	sizer->Add(btn_back,sizers->BA5_Center);
	sizer->Add(btn_keep,sizers->BA5_Center);
	Set(src);
	this->SetSizerAndFit(sizer);
}

void mxSourceUndoHistory::Set (mxSource * src) {
	if (src==m_src) return;
	if (src) { 
		src->m_undo_history_panel = nullptr;
		src->SetReadOnlyMode(ROM_SPECIAL,false);
	}
	m_src = src; if (!src) return;
	src->m_undo_history_panel = this;
	m_src->Freeze();
	m_undo_count = m_redo_count = 0;
	// go forward as much as we can
	for(m_redo_count=0; m_src->CanRedo(); m_redo_count++) m_src->Redo();
	// undo the redone stuff
	for(int i=0;i<m_redo_count;i++) m_src->Undo();
	// go backwards as much as we can
	for(m_undo_count=0; m_src->CanUndo(); m_undo_count++) m_src->Undo();
	// redo the undone stuff
	for(int i=0;i<m_undo_count;i++) m_src->Redo();
	// we are back at the state we started
	m_src->Thaw(); m_original_pos = m_undo_count;
	m_src->SetReadOnlyMode(ROM_SPECIAL,true);
	m_scrollbar->SetScrollbar(m_undo_count,1,m_redo_count+m_undo_count+1,(m_redo_count+m_undo_count)/10,true);
}

void mxSourceUndoHistory::OnScroll (wxScrollEvent & evt) {
	evt.Skip(); Goto(m_scrollbar->GetThumbPosition());
}

mxSourceUndoHistory::~mxSourceUndoHistory ( ) {
	if (m_src) {
		m_src->m_undo_history_panel = nullptr;
		m_src->SetReadOnlyMode(ROM_SPECIAL,false);
	}
}

void mxSourceUndoHistory::OnButtonReset (wxCommandEvent & evt) {
	Goto(m_original_pos); OnButtonKeep(evt);
}

void mxSourceUndoHistory::OnButtonKeep (wxCommandEvent & evt) {
	main_window->m_aui->Close(this);
}

void mxSourceUndoHistory::Goto (int pos) {
	mxSource::TemporaryDisableReadOnly rom_guard(m_src);
	while (pos>m_undo_count) {
		m_src->Redo(); m_undo_count++; m_redo_count--;
	}
	while (pos<m_undo_count) {
		m_src->Undo(); m_undo_count--; m_redo_count++;
	}	
}

void mxSourceUndoHistory::OnSourceClose ( ) {
	Goto(m_original_pos); main_window->m_aui->Close(this);
}

