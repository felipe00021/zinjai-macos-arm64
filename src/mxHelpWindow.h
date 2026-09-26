#ifndef MXHELPWINDOW_H
#define MXHELPWINDOW_H
#include "mxGenericHelpWindow.h"
#include "Cpp11.h"

class wxDialog;

class mxHelpWindow : public mxGenericHelpWindow {
	
	static mxHelpWindow *instance;
	mxHelpWindow(wxString file="");
	HashStringTreeItem items;
	
	wxString current_dir;
	wxString FixURL(wxString url, bool set_dir, bool select_tree, bool keep_args);
	wxString GetRealFName(wxString url);
	bool LoadPage(wxString url);
	
//	wxString GetHelpFile(wxString file);
	
	void PopulateIndex(wxTreeItemId root, wxString dir);
	
public:
	static mxHelpWindow *ShowHelp(wxString page="", wxDialog *from_modal=nullptr);
	
//	void LoadPage(wxString file);
	
	void ShowIndex();
	void OnTree(wxTreeItemId item);
	void OnSearch(wxString value);
	bool OnLink(wxString href);
//	bool CurrentPageIsHome() override;
	
	void OnForum(wxCommandEvent &event);
	
//	/// @brief Wrapper for wxHtmlWindow::LoadPage that fix href path (it enters relative to current page, but current page's path is not current working directory)
//	void FixLoadPage(const wxString &href); 
	
private:
	bool ignore_tree_event;
public:
	/// @brief Selects an iden in the index tree skipping its selecion event callback
	void SelectTreeItem(const wxString &fname);
	
	static void DeleteInstance() { if (instance) instance->Destroy(); }
	
	~mxHelpWindow();
};

#endif
