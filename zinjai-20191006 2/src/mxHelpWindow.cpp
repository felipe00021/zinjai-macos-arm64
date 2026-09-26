#include <wx/textfile.h>
#include "mxHelpWindow.h"
#include "ConfigManager.h"
#include <wx/treectrl.h>
#include "mxMessageDialog.h"
#include "mxMainWindow.h"

#define ERROR_PAGE(page) wxString("<I>ERROR</I>: La pagina \"")<<page<<"\" no se encuentra. <br><br> La ayuda de <I>ZinjaI</I> aún está en contrucción."
#define _index "index.html"
#include "ids.h"
#include "mxSizers.h"
#include "mxReferenceWindow.h"
#include "ProjectManager.h"

mxHelpWindow *mxHelpWindow::instance=nullptr;

void mxHelpWindow::PopulateIndex(wxTreeItemId node, wxString dir) {
	wxString index_file = mxUT::WichOne(
			DIR_PLUS_FILE(dir,"index_"+config->Init.language_file),
			DIR_PLUS_FILE(dir,"index_spanish"),
			DIR_PLUS_FILE(dir,"index") );
	wxTextFile fil(index_file);
	if (fil.Exists()) {
		fil.Open();
		int tabs_prev = -1;
		for ( wxString str = fil.GetFirstLine(); !fil.Eof(); str = fil.GetNextLine() ) {
			if (str.IsEmpty()||str[0]=='#') continue;
			int tabs_cur = 0; while (tabs_cur<int(str.Len()) && str[tabs_cur]=='\t') tabs_cur++;
			if (int(str.Len())>tabs_cur+3) {
				if (tabs_cur==tabs_prev) {
					node = tree->AppendItem(tree->GetItemParent(node),_ZS(str.Mid(tabs_cur+2).AfterFirst(' ')),str[tabs_cur]-'0');
				} else if (tabs_cur>tabs_prev) {
					node = tree->AppendItem(node,_ZS(str.Mid(tabs_cur+2).AfterFirst(' ')),str[tabs_cur]-'0');
				} else {
					for (int j=0;j<tabs_prev-tabs_cur;j++)
						node = tree->GetItemParent(node);
					node = tree->AppendItem(tree->GetItemParent(node),_ZS(str.Mid(tabs_cur+2).AfterFirst(' ')),str[tabs_cur]-'0');
				}
				items[DIR_PLUS_FILE(dir,str.Mid(tabs_cur+2).BeforeFirst(' '))] = node;
				tabs_prev=tabs_cur;
			}
		}
		fil.Close();
	}
}

mxHelpWindow::mxHelpWindow(wxString file) : mxGenericHelpWindow(LANG(HELPW_CAPTION,"Ayuda de ZinjaI"),true) {
	current_dir = DIR_PLUS_FILE(config->zinjai_dir,"guihelp");
	ignore_tree_event=false;
	// populate index tree
	wxTreeItemId root_node = tree->AddRoot("Temas de Ayuda",0);
	PopulateIndex(root_node,current_dir);
	
	wxTreeItemId complements_node = tree->AppendItem(root_node,LANG(HELPW_COMPLEMENTS_SECTION,"Ayudas de Complementos"),0);
	items[DIR_PLUS_FILE(current_dir,"complements_section.html")] = complements_node;
	wxArrayString complements;
	wxString complements_dir = config->GetZinjaiComplementsPath("guihelp");
	if (mxUT::GetFilesFromDir(complements,complements_dir,false)) {
		for(unsigned int i=0;i<complements.GetCount();i++) {
			PopulateIndex(complements_node, DIR_PLUS_FILE(complements_dir,complements[i]));
		}
	}
		
	{ // drepecated method, I'll remove this code in the future
		wxArrayString complements;
		wxString complements_dir = config->GetZinjaiComplementsPath("guihelp");
		if (mxUT::GetFilesFromDir(complements,complements_dir)) {
			for(unsigned int i=0;i<complements.GetCount();i++) {
				wxString page_name = complements[i].BeforeLast('.');
				wxString file = DIR_PLUS_FILE(complements_dir,complements[i]);
				items[file] = tree->AppendItem(complements_node,page_name,1);
			}
		}
	}
	
	// expand all first level items
	wxTreeItemIdValue cokkie;
	wxTreeItemId node = tree->GetFirstChild(root_node,cokkie);
	while (node.IsOk()) {
		tree->Expand(node);
		node = tree->GetNextSibling(node);
	}
	
	if (!file.Len()) file="index"; 
	LoadPage(file);
	
	wxBoxSizer *forum_sizer = new wxBoxSizer(wxHORIZONTAL); wxButton *button;
	forum_sizer->Add(new wxStaticText(this,wxID_ANY,LANG(HELPW_FORUM_TEXT,"¿Lo que buscas no está en la ayuda, está desactualizado, erróneo o incompleto? ")),sizers->Center);
	forum_sizer->Add(button=new wxButton(this,mxID_HELPW_FORUM,LANG(HELPW_FORUM_BUTTON,"accede al Foro..."),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT),sizers->Center);
	button->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(mxHelpWindow::OnForum),nullptr,this);
	general_sizer->Add(forum_sizer,sizers->Right);
	Layout();
}

mxHelpWindow::~mxHelpWindow() { instance=nullptr; }

void mxHelpWindow::OnSearch(wxString value) {
	wxArrayString results_body, results_title, keywords;
	mxUT::Split(value.MakeUpper(),keywords,true,false); // palabras clave a buscar
	if (keywords.GetCount()==0) {
		mxMessageDialog(this,LANG(HELPW_SEARCH_ERROR_EMPTY,"Debe introducir al menos una palabra clave para buscar"))
			.Title(LANG(GENERAL_ERROR,"Error")).IconWarning().Run();
		return;
	}
	unsigned char *bfound = new unsigned char[keywords.GetCount()]; // para marcar las palabras encontradas al buscar en el contenido linea por linea
	html->SetPage(wxString("<HTML><HEAD></HEAD><BODY><I><B>")<<LANG(HELPW_SEARCH_SEARCHING,"Buscando...")<<"</B></I></BODY></HTML>");
	wxArrayString already_searched; // archivos ya examinados, porque pueden aparecer más de una vez por estar en el indice con diferentes anchors
	for(HashStringTreeItem::iterator it = items.begin(), ed = items.end(); it!=ed; ++it) {
		wxString title = tree->GetItemText(it->second), fname = it->first;
		if (fname.Find('#')!=wxNOT_FOUND) fname = fname.BeforeFirst('#');
		if (already_searched.Index(fname)!=wxNOT_FOUND) continue; 
		else already_searched.Add(fname);
		wxString result_line = wxString("<!--")<<_ZS(title)<<"--><LI><A href=\""<<it->first<<"\">"<<_ZW(title)<<"</A></LI>";
		// ver si coincide en el título del ítem
		bool title_matches = true;
		wxString utitle = title.MakeUpper();
		for (unsigned int ik=0;ik<keywords.GetCount();ik++) {
			if (!utitle.Contains(keywords[ik])) {
			    title_matches = false; break;
			}
		}
		if (title_matches) {
			results_title.Add(result_line);
		} else {
			// ver si coincide dentro del contenido
			fname = GetRealFName(fname);
			if (fname.IsEmpty()) continue; // some help pages might be missing
			memset(bfound,0,keywords.GetCount());
			wxTextFile fil(fname);
			if (!fil.Open()) continue;
			unsigned int fc=0;
			for ( wxString str = fil.GetFirstLine(); !fil.Eof(); str = fil.GetNextLine() ) {
				for (unsigned int ik=0;ik<keywords.GetCount();ik++) {
					if (bfound[ik]==0 && str.MakeUpper().Contains(keywords[ik])) {
						fc++; bfound[ik]=1;
					}
				}
				if (fc==keywords.GetCount()) {
					results_body.Add(result_line);
					break;
				}
			}
			fil.Close();
		}
	}
	wxString result;
	result << "<HTML><HEAD></HEAD><BODY>";
	if (results_title.GetCount()) {
		results_title.Sort();
		result << wxString("<I><B>")<<LANG(HELPW_SEARCH_RESULTS_TITLE,"Coincidencias en el título:")<<"</B></I><UL>";
		for (unsigned int i=0;i<results_title.GetCount();i++)
			result<<results_title[i];
		result<<"</UL>";
	}
	if (results_body.GetCount()&&results_title.GetCount()) result<<"<BR>"<<"<BR>";
	if (results_body.GetCount()) {
		results_body.Sort();
		result << wxString("<I><B>")<<LANG(HELPW_SEARCH_RESULTS_BODY,"Coincidencias dentro del contenido:")<<"</B></I><UL>";
		for (unsigned int i=0;i<results_body.GetCount();i++)
			result<<results_body[i];
		result<<"</UL>";
	}
	result<<"</BODY></HTML>";
	if (results_body.GetCount()||results_title.GetCount()) 
		html->SetPage(result);
	else
		html->SetPage(wxString("<HTML><HEAD></HEAD><BODY><B>")<<LANG(HELPW_SEARCH_NO_RESULTS_FOR,"No se encontraron coincidencias para \"")<<value<<"\".</B></BODY></HTML>");
	delete [] bfound;
}

void mxHelpWindow::ShowIndex() {
	LoadPage(_index);
}

mxHelpWindow *mxHelpWindow::ShowHelp(wxString page, wxDialog *from_modal) {
	
	if (from_modal) {
		if ( mxMessageDialog(from_modal,"Se cerrará este cuadro de diálogo (perdiendo los cambios)\n"
										"para poder acceder a la ventana de ayuda.")
				.Title(LANG(GENERAL_WARNING,"Advertencia")).ButtonsOkCancel().IconWarning().Run().ok ) 
		{
			from_modal->EndModal(0);
		} else
			return nullptr;
	}
	
	if (page=="") page=_index;
	if (instance) {
		if (!instance->IsShown()) instance->Show(); 
		else if (instance->IsIconized()) instance->Maximize(false); 
		else instance->Raise();
		instance->LoadPage(page); 
	} else {
		instance=new mxHelpWindow(page);
		instance->Show();
	}
	if (page==_index) instance->search_text->SetFocus();
	else instance->html->SetFocus();
	return instance;
}

bool mxHelpWindow::OnLink (wxString href) {
	if (href=="cppreference:") {
		Close(); mxReferenceWindow::ShowPage();
#ifdef __APPLE__
	} else if (href.StartsWith("action:")) {
		wxString action = href.AfterFirst(':');
		if (action=="gdb_on_mac") {
			wxString command = mxUT::GetCommandForRunningInTerminal(
								"ZinjaI - download and install GDB",
								"sh src_extras/mac-compile_gdb.sh");
			wxExecute(command);
		} else if (action=="keychain_access") {
			wxExecute("\"/Applications/Utilities/Keychain Access.app/Contents/MacOS/Keychain Access\"");
		}
#endif
	} else if (href.StartsWith("explore:")) {
		wxString path = href.AfterFirst(':');
#ifdef __WIN32__
		path.Replace("/","\\",true);
#endif
		mxUT::DirParameterReplace(path,"ZINJAI",config->zinjai_dir,false);
		mxUT::DirParameterReplace(path,"MINGW",current_toolchain.mingw_dir,false);
		if (project) mxUT::DirParameterReplace(path,"PROJECT",project->GetPath(),false);
		mxUT::OpenFolder(path);
	} else if (href.StartsWith("foropen:")) {
		main_window->NewFileFromTemplate(DIR_PLUS_FILE(current_dir,href.AfterFirst(':')),true);
	} else if (href.Contains("://")) {
		mxUT::OpenInBrowser(href);
	} else {
		LoadPage(href);
	}
	return true;
}

void mxHelpWindow::OnTree (wxTreeItemId item) {
	if (ignore_tree_event) return;
	tree->Expand(item);
	HashStringTreeItem::iterator it = items.begin();
	while (it!=items.end() && it->second != item) ++it;
	if (it!=items.end()) LoadPage(it->first);
}

void mxHelpWindow::OnForum (wxCommandEvent & event) {
	mxUT::OpenInBrowser(LANG(HELPW_ADDRESS,"http://zinjai.sourceforge.net/index.php?page=contacto.php"));
}

wxString mxHelpWindow::FixURL(wxString url, bool set_dir, bool select_tree, bool keep_args) {
	if (url.Contains("$")) {
		mxUT::DirParameterReplace(url,"ZINJAI",config->zinjai_dir,false);
		if (project)
			mxUT::DirParameterReplace(url,"PROJECT",project->GetPath(),false);
	}
	// cortar ruta, nombre, ext del archivo y argumentos de la url
	int pos_args = url.Len(), pos_name = 0, pos_ext = -1;
	for(size_t i=0;i<url.size();i++) { 
		if (url[i]=='#') { pos_args = i; break; }
		else if (url[i]=='\\'||url[i]=='/') { pos_name = i+1; pos_ext=-1; }
		else if (url[i]=='.') pos_ext = i;
	}
	wxString args = url.Mid(pos_args);
	wxString ext = pos_ext==-1 ? "" : url.Mid(pos_ext,pos_args-pos_ext);
	wxString name = url.Mid(pos_name, (pos_ext==-1?pos_args:pos_ext)-pos_name);
	wxString path = url.Mid(0,pos_name);
	// corregir y guardar el directorio actual
	path = DIR_PLUS_FILE(current_dir,path);
	if (set_dir) current_dir = path;
	// seleccionar el item en el arbol
	if (select_tree) {
		HashStringTreeItem::iterator it = items.find(path+name+ext+args);
		if (it==items.end()) it = items.find(path+name+ext);
		if (it==items.end()) {
			wxArrayTreeItemIds ta;
			if (tree->GetSelections(ta)) 
				tree->SelectItem(tree->GetSelection(),false);
		} else {
			ignore_tree_event=true;
			tree->SelectItem(it->second);
			ignore_tree_event=false;
		}
	}
	// ver si hay traduccion
	if (!keep_args) args.Clear();
	return config->GetHelpFileEx(path,name,ext,args);
}

bool mxHelpWindow::LoadPage(wxString url) {
	wxString s = FixURL(url,true,true,true);
	if (!s.IsEmpty()) { html->LoadPage(s); return true; }
	else { html->SetPage(ERROR_PAGE(url)); return false; }
}

wxString mxHelpWindow::GetRealFName(wxString url) {
	return FixURL(url,false,false,false);
}


