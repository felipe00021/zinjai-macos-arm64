#include "mxExternCompilerOutput.h"
#include "mxMainWindow.h"
#include "ids.h"
#include "mxAUI.h"
#include <wx/imaglist.h>
#include "mxHidenPanel.h"
#include "ConfigManager.h"
#include "mxArt.h"

BEGIN_EVENT_TABLE(mxExternCompilerOutput,wxTreeCtrl)
	EVT_TREE_ITEM_ACTIVATED(wxID_ANY,mxExternCompilerOutput::OnDClick)
//	EVT_MENU(mxID_EXTERN_COMPILER_OUTPUT,mxExternCompilerOutput::OnOnlyErrors)
	EVT_MENU(mxID_COMPILER_POPUP_FULL,mxExternCompilerOutput::OnViewFullOutput)
	EVT_RIGHT_DOWN(mxExternCompilerOutput::OnPopup)
END_EVENT_TABLE();

mxExternCompilerOutput::mxExternCompilerOutput(wxWindow *compiler_panel)
	: wxTreeCtrl(compiler_panel,mxID_EXTERN_COMPILER_OUTPUT,wxDefaultPosition,wxDefaultSize,wxTR_DEFAULT_STYLE | wxNO_BORDER | wxTR_HIDE_ROOT) 
{
	
	wxString tdir = config->HighDPI()?"trees/24/":"trees/16/";
	int tsize = config->HighDPI()?24:16;
	wxImageList* imglist = new wxImageList(tsize,tsize, true, 2);
	imglist->Add(bitmaps->GetBitmap(tdir+"co_info.png"));
	imglist->Add(bitmaps->GetBitmap(tdir+"co_folder.png"));
	imglist->Add(bitmaps->GetBitmap(tdir+"co_out.png"));
	imglist->Add(bitmaps->GetBitmap(tdir+"co_error.png"));
	wxTreeCtrl::AssignImageList(imglist);
	
	m_last_cmd = m_root = wxTreeCtrl::AddRoot("Extern Toolchain Root");
	m_status = wxTreeCtrl::AppendItem(m_root,LANG(CAPTION_COMPILER_OUTPUT,"Resultados de la Compilación"),Status);
}

void mxExternCompilerOutput::AddLine (mxExternCompilerOutput::Type t, const wxString &message) {
	wxString pre = t==Status ? "= " : (t==Cmd ? "> " : (t==Std ? "< " : (t==Err ? "!! " : "??? ") ) );
	m_full_output.Add(pre+message);
	switch (t) {
	case Status:
		wxTreeCtrl::SetItemText(m_status,message);
		wxTreeCtrl::EnsureVisible(m_status);
		break;
	case Cmd:
		m_last_cmd = wxTreeCtrl::AppendItem(m_root,message,Cmd);
		break;
	default:
		wxTreeItemId item = wxTreeCtrl::AppendItem(m_last_cmd,message,t);
		wxTreeCtrl::EnsureVisible(item);
	}
}

void mxExternCompilerOutput::OnDClick(wxTreeEvent &evt) {
	evt.Skip(); SelectError(wxTreeCtrl::GetSelection());
}

void mxExternCompilerOutput::OnPopup (wxMouseEvent & evt) {
	mxHidenPanelIgnoreGuard ignore_autohide;
	wxMenu menu("");
	menu.Append(mxID_COMPILER_POPUP_FULL, "Ver salida completa");
//	menu.AppendCheckItem(mxID_EXTERN_COMPILER_OUTPUT, "Mostrar solo errores")->Check(showing_only_errors);
	PopupMenu(&menu);
}

//void mxExternCompilerOutput::OnOnlyErrors (wxCommandEvent & evt) {
//	if (showing_only_errors) {
//		showing_only_errors = false;
//		wxListBox::Clear();
//		for(unsigned int i=0;i<lines.GetCount();i++) {
//			Append(lines[i]);
//		}
//	} else {
//		PopulateArray();
//		for(unsigned int i=0;i<GetCount();i++) {
//			while (i<GetCount() && !IsErrorLine(i))
//				Delete(i);
//		}
//		showing_only_errors = true;
//	}
//}
//
void mxExternCompilerOutput::OnViewFullOutput (wxCommandEvent & evt) {
//	PopulateArray();
	main_window->ShowSpecilaUnnamedSource("<ultima_compilacion>",m_full_output);
}
//
//
//void mxExternCompilerOutput::PopulateArray ( ) {
//	if(lines.GetCount()) return;
//	for(unsigned int i=0;i<GetCount();i++) 
//		lines.Add(GetString(i));
//}

void mxExternCompilerOutput::Clear ( ) {
	m_full_output.Clear();
	wxTreeCtrl::SetItemText(m_status,LANG(CAPTION_COMPILER_OUTPUT,"Resultados de la Compilación"));
	while(true) {
		wxTreeItemIdValue cookie;
		wxTreeCtrl::GetFirstChild(m_root,cookie);
		wxTreeItemId item = wxTreeCtrl::GetNextChild(m_root,cookie);
		if (!item.IsOk()) return;
		wxTreeCtrl::Delete(item);
	}
}

void mxExternCompilerOutput::OnErrorNext ( ) {
	wxTreeItemId sel = wxTreeCtrl::GetSelection();
	if (!sel.IsOk()) sel = m_status;
	wxTreeItemId sel_orig = sel;
	do {
		if (wxTreeCtrl::GetChildrenCount(sel)) {
			wxTreeItemIdValue cookie;
			sel = wxTreeCtrl::GetFirstChild(sel,cookie);
		} else {
			while(true) {
				wxTreeItemId next = wxTreeCtrl::GetNextSibling(sel);
				if (next.IsOk()) { sel = next; break; }
				sel = wxTreeCtrl::GetItemParent(sel);
				if (sel==m_root) { sel = m_status; break; }
			}
		}
	} while (sel!=sel_orig && wxTreeCtrl::GetItemImage(sel)!=Err);
	
	if (sel!=sel_orig) { wxTreeCtrl::SelectItem(sel); SelectError(sel); }
}

void mxExternCompilerOutput::OnErrorPrev ( ) {
	wxTreeItemId sel = wxTreeCtrl::GetSelection();
	if (!sel.IsOk()) sel = m_status;
	wxTreeItemId sel_orig = sel;
	do {
		wxTreeItemId parent = wxTreeCtrl::GetItemParent(sel);
		wxTreeItemIdValue cookie;
		if (sel == wxTreeCtrl::GetFirstChild(parent,cookie)) {
			sel = parent;
		} else {
			sel = wxTreeCtrl::GetPrevSibling(sel);
		}
		if (sel==m_root) {
			while (wxTreeCtrl::GetChildrenCount(sel))
				sel = wxTreeCtrl::GetLastChild(sel);
		}
	} while (sel!=sel_orig && wxTreeCtrl::GetItemImage(sel)!=Err);
	
	if (sel!=sel_orig) { wxTreeCtrl::SelectItem(sel); SelectError(sel); }
}

void mxExternCompilerOutput::SelectError (wxTreeItemId sel) {
	if (!sel.IsOk()) return;
	wxString str = wxTreeCtrl::GetItemText(sel);
	if (str.Len()) main_window->OnSelectErrorCommon(str,"",true);
}
