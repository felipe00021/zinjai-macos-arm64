#include <wx/sizer.h>
#include "IniFile.h"
#include "ProjectManager.h"
#include "mxSizers.h"
#include "mxMessageDialog.h"
#include "mxTemplateCombination.h"
#include "mxNewWizard.h"
#include "ConfigManager.h"
#include "CodeHelper.h"
#include "Language.h"
#include "mxHelpWindow.h"

BEGIN_EVENT_TABLE(mxTemplateCombination,wxDialog)
	EVT_CHOICE(wxID_ANY,mxTemplateCombination::OnCombo)
	EVT_BUTTON(wxID_OK,mxTemplateCombination::OnButtonOk)
	EVT_BUTTON(wxID_CANCEL,mxTemplateCombination::OnButtonCancel)
	EVT_BUTTON(mxID_HELP_BUTTON,mxTemplateCombination::OnButtonHelp)
END_EVENT_TABLE()

mxTemplateCombination::mxTemplateCombination(wxWindow *parent, wxString other_zpr) 
	: mxDialog(parent,LANG(TEMPLATECOMB_CAPTION,"Combinar Plantillas")), m_zpr_path(other_zpr)
{
	CreateSizer sizer(this);
	
	CreateHorizontalSizer hor_sizer(this);
	
	m_sizer_profiles = new wxFlexGridSizer(1,3,15,5);
	m_sizer_profiles->AddGrowableCol(0,1);
	m_sizer_profiles->AddGrowableCol(2,1);
	m_sizer_profiles->Add(new wxStaticText(this,wxID_ANY,project->project_name,wxDefaultPosition,wxDefaultSize,wxALIGN_CENTRE),sizers->Exp0);
	m_sizer_profiles->Add(new wxStaticText(this,wxID_ANY,LANG(TEMPLATECOMB_ACTION,"Acción"),wxDefaultPosition,wxDefaultSize,wxALIGN_CENTRE),sizers->Exp0);
	m_sizer_profiles->Add(new wxStaticText(this,wxID_ANY,GetName(other_zpr),wxDefaultPosition,wxDefaultSize,wxALIGN_CENTRE),sizers->Exp0);
	
	int pc = GetProfiles(other_zpr,m_other_profiles);
	if (pc<=0) mxMessageDialog(parent,LANG(TEMPLATECOMB_ERROR,"Error al abrir el archivo de proyecto")).Title(other_zpr).IconError().Run();
	for(int i=0;i<project->configurations_count;i++) AddProfile(project->configurations[i]->name/*,true*/);
	wxCommandEvent cmd_evt; OnCombo(cmd_evt);
//	sizer.BeginButton("Agregar perfil").EndButton();
	
	CreateSizer opts_sizer(this);
	hor_sizer.GetSizer()->Add(m_sizer_profiles,sizers->Exp1);
	
	opts_sizer.BeginSection(LANG(TEMPLATECOMB_PROFILE_FIELDS,"Campos a combinar/reemplazar por perfil:"))
		.BeginCheck(LANG(PROJECTCONFIG_GENERAL_ENV_VARS,"Variables de entorno")).Value(true).EndCheck(m_merge_env_vars)
		.BeginCheck(LANG(PROJECTCONFIG_COMPILING_EXTRA_ARGS,"Parámetros extra para la compilación")).Value(true).EndCheck(m_merge_comp_extra)
		.BeginCheck(LANG(PROJECTCONFIG_COMPILING_MACROS,"Constantes de preprocesador")).Value(true).EndCheck(m_merge_macros)
		.BeginCheck(LANG(PROJECTCONFIG_COMPILING_EXTRA_PATHS,"Directorios adicionales para buscar cabeceras")).Value(true).EndCheck(m_merge_headers)
		.BeginCheck(LANG(PROJECTCONFIG_LINKING_EXTRA_ARGS,"Parámetros extra para el enlazado")).Value(true).EndCheck(m_merge_link_extra)
		.BeginCheck(LANG(PROJECTCONFIG_LINKING_EXTRA_PATHS,"Directorios adicionales para buscar bibliotecas")).Value(true).EndCheck(m_merge_lib_dirs)
		.BeginCheck(LANG(PROJECTCONFIG_LINKING_EXTRA_LIBS,"Bibliotecas a enlazar")).Value(true).EndCheck(m_merge_libraries)
		.BeginCheck(LANG(PROJECTCONFIG_LIBS_TO_USE,"Bibliotecas del sistema a utilizar")).Value(true).EndCheck(m_merge_libs_to_use)
#ifdef _ZINJAI_DEBUG
		.BeginCheck(LANG(TEMPLATECOMB_CUSTOM_STEPS,"Pasos de compilación personalizados")).Value(true).EndCheck(m_merge_custom_steps)
#endif
		.EndSection();
	opts_sizer.BeginSection(LANG(TEMPLATECOMB_GENERAL_FIELDS,"Opciones generales a combinar:"))
		.BeginCheck(LANG(PROJECTGENERAL_AUTOCOMP_EXTRA,"Indices de autocompletado adicionales")).Value(true).EndCheck(m_merge_autocomp)
		.BeginCheck(LANG(PROJECTGENERAL_AUTOIMPROVE_TEMPLATES," Mejora de inspecciones según tipo ")).Value(true).EndCheck(m_merge_inspections)
#ifdef _ZINJAI_DEBUG
		.BeginCheck(LANG(MENUITEM_TOOLS_CUSTOM_TOOLS,"Herramientas Personalizables")).Value(true).EndCheck(m_merge_custom_tools)
#endif
		.EndSection();
	hor_sizer.GetSizer()->Add(opts_sizer.GetSizer(),sizers->Exp0);
	
	sizer.Add(hor_sizer,sizers->Exp1);
	
	sizer.BeginBottom().Help().Ok().Cancel().EndBottom(this);
	sizer.SetAndFit();
}

void mxTemplateCombination::AddProfile (wxString name/*, bool original*/) {
	wxStaticText *text_orig = new wxStaticText(this,wxID_ANY,name);
//	/*if (original) */text_orig->SetEditable(false);
	m_sizer_profiles->Add(text_orig,sizers->BA5_Center);
	wxArrayString opts; 
//	if (original) {
		opts.Add(LANG(TEMPLATECOMB_ACTION_NONE,"No modificar"));
		opts.Add(LANG(TEMPLATECOMB_ACTION_COMBINE,"Combinar con"));
		opts.Add(LANG(TEMPLATECOMB_ACTION_REPLACE,"Reemplazar con"));
//	} else 
//		opts.Add("Copiar de");
	wxChoice *combo_actions = new wxChoice(this,wxID_ANY,wxDefaultPosition,wxDefaultSize,opts);
	m_sizer_profiles->Add(combo_actions);
	wxChoice *combo_other = new wxChoice(this,wxID_ANY,wxDefaultPosition,wxDefaultSize,m_other_profiles);
	int sel = MatchName(name,m_other_profiles);
	combo_actions->SetSelection(sel==wxNOT_FOUND?0:1);
	combo_other->SetSelection(sel==wxNOT_FOUND?0:sel);
	m_sizer_profiles->Add(combo_other,sizers->Exp1);
	m_names_this.push_back(text_orig);
	m_actions.push_back(combo_actions);
	m_names_other.push_back(combo_other);
}

wxString mxTemplateCombination::GetName (const wxString zpr_path) {
	IniFileReader fil(zpr_path);
	if (fil.IsOk()) {
		for ( wxString section = fil.GetNextSection(); !section.IsEmpty(); section = fil.GetNextSection() ) {
			if (section=="general") {
				for( IniFileReader::Pair p = fil.GetNextPair(); p.IsOk(); p = fil.GetNextPair() ) {
					if (p.Key()=="project_name") return p.AsString();
				}
			}
		}
	}
	return wxFileName(zpr_path).GetName();
}

int mxTemplateCombination::GetProfiles (const wxString zpr_path, wxArrayString &profiles_names) {
	IniFileReader fil(zpr_path);
	if (!fil.IsOk()) return -1;
	for ( wxString section = fil.GetNextSection(); !section.IsEmpty(); section = fil.GetNextSection() ) {
		if (section=="config") {
			for( IniFileReader::Pair p = fil.GetNextPair(); p.IsOk(); p = fil.GetNextPair() ) {
				if (p.Key()=="name") profiles_names.Add(p.AsString());
			}
		}
	}
	return profiles_names.GetCount();
}

void mxTemplateCombination::OnCombo (wxCommandEvent & evt) {
	for(size_t i=0;i<m_names_this.size();i++) { 
		m_names_other[i]->Enable(m_actions[i]->GetSelection()!=0);
	}
}

struct ProfileName {
	wxString name;
	enum OS { UnknownOS, Windows, Linux, Mac } os;
	enum Bits { UnknownBits, i86, x64 } bits;
	enum Build { UnknownBuild, Debug, Release } build;
	ProfileName(Build btype=UnknownBuild) : os(GuessOS()), bits(GuessBits()), build(btype) { }
	static OS GuessOS() {
#if defined(__WIN32__) || defined(__WIN64__)
		return Windows;
#elif defined(__APPLE__)
		return Mac;
#elif defined(__linux__)
		return Linux;
#else 
		return UnknownOs;
#endif
	}
	static Bits GuessBits()  {
#if defined(__WIN64__) || defined(__x86_64__)
		return x64;
#elif defined(__i386__) || defined(__WIN32__)
		return i86;
#else
		return UnknownBits;
#endif
	}
	ProfileName(wxString profile_name) {
		
		name = profile_name.Lower(); 
		name.Replace(" ","",true);
		name.Replace(".","",true);
		name.Replace("_","",true);
		name.Replace("-","",true);
		
		if (name.Contains("64"))      bits = x64;
		else if (name.Contains("32")) bits = i86;
		else if (name.Contains("86")) bits = i86;
		else                          bits = UnknownBits;
		
		if (name.Contains("windows"))    os = Windows;
		else if (name.Contains("linux")) os = Linux;
		else if (name.Contains("apple")) os = Mac;
		else if (name.Contains("wnd"))   os = Windows;
		else if (name.Contains("win"))   os = Windows;
		else if (name.Contains("lin"))   os = Linux;
		else if (name.Contains("lnx"))   os = Linux;
		else if (name.Contains("mac"))   os = Mac;
		else if (name.Contains("osx"))   os = Mac;
		else if (name.Contains("app"))   os = Mac;
		else if (name.Contains("w"))     os = Windows;
		else if (name.Contains("l"))     os = Linux;
		else                             os = UnknownOS;
		
		if (name.Contains("release"))    build = Release;
		else if (name.Contains("debug")) build = Debug;
		else if (name.Contains("rls"))   build = Release;
		else if (name.Contains("rel"))   build = Release;
		else if (name.Contains("deb"))   build = Debug;
		else if (name.Contains("dbg"))   build = Debug;
		else                             build = UnknownBuild;
		
	}
	
	int MatchLevel(ProfileName &o) {
		return
			(name==o.name                       ? 32 : 0) +
			((os!=UnknownOS&&os==o.os)          ? 16 : 0) +
			(build==o.build                     ?  8 : 0) +
			(bits==o.bits                       ?  4 : 0) +
			((os==UnknownOS&&o.os==GuessOS())   ?  2 : 0) +
			((os==UnknownOS&&o.os==UnknownOS)   ?  1 : 0) ;
	}
};


int mxTemplateCombination::MatchName (const wxString & name, const wxArrayString & candidates) {
	ProfileName pt(name);
	int max_lev = 0, sel = -1; 
	for(unsigned int i=0;i<candidates.GetCount();i++) {
		int lev = ProfileName(candidates[i]).MatchLevel(pt);
		if (lev>max_lev) { max_lev = lev; sel = i; }
	}
	return sel;
}

void mxTemplateCombination::OnButtonOk (wxCommandEvent & evt) {
	CombineOptsProf opts_prof;
	opts_prof.env_vars = m_merge_env_vars->GetValue();
	opts_prof.comp_extra = m_merge_comp_extra->GetValue();
	opts_prof.link_extra = m_merge_link_extra->GetValue();
	opts_prof.headers = m_merge_headers->GetValue();
	opts_prof.lib_dirs = m_merge_lib_dirs->GetValue();
	opts_prof.libraries = m_merge_libraries->GetValue();
	opts_prof.libs_to_use = m_merge_libs_to_use->GetValue();
	opts_prof.macros = m_merge_macros->GetValue();
#ifdef _ZINJAI_DEBUG
	opts_prof.custom_steps = m_merge_custom_steps->GetValue();
#endif
	for(size_t i=0;i<m_names_this.size();i++) {
		int iaction = m_actions[i]->GetSelection();
		if (iaction==0) continue;
		wxString this_name = m_names_this[i]->GetLabel();
		wxString other_name = m_names_other[i]->GetStringSelection();
		CombineProfile(this_name, m_zpr_path, other_name, opts_prof,iaction==2);
	}
	
	CombineOptsGeneral opts_general;
	opts_general.autocomp = m_merge_autocomp->GetValue();
#ifdef _ZINJAI_DEBUG
	opts_general.custom_tools = m_merge_custom_tools->GetValue();
#endif
	opts_general.inspections = m_merge_inspections->GetValue();
	CombineGeneralOpts(m_zpr_path,opts_general);
	
	Close();
}

void mxTemplateCombination::OnButtonCancel (wxCommandEvent & evt) {
	Close();
}

void mxTemplateCombination::Run (wxWindow * parent) {
	wxArrayString dirs, names;
	mxNewWizard::GetProjectTemplates(dirs,names);
	const wxString custom_zpr_name = LANG(TEMPLATECOMB_FROM_ZPR,"<Elegir archivo .zpr>");
	names.Add(custom_zpr_name);
	wxString choice = wxGetSingleChoice(LANG(TEMPLATECOMB_CHOOSE_TEMPLATE,"Seleccione una plantilla"),LANG(TEMPLATECOMB_CAPTION,"Combinar plantillas"),names,parent);
	if (choice=="") return; // cancel
	wxString zpr_path;
	if (choice==custom_zpr_name) {
		wxFileDialog dlg (parent, "Abrir Proyecto", project?project->last_dir:config->Files.last_dir, " ", "Any file (*)|*", wxFD_OPEN | wxFD_FILE_MUST_EXIST );
		dlg.SetWildcard("Proyectos ZinjaI|" WILDCARD_PROJECT);
		if (dlg.ShowModal() != wxID_OK) return;
		zpr_path = dlg.GetPath();
	} else {
		wxString fname = dirs[names.Index(choice)];
		zpr_path = DIR_PLUS_FILE( mxUT::WichOne(fname,"templates",false), fname+DOT_PROJECT_EXT );
	}
	( new mxTemplateCombination(parent,zpr_path) )->ShowModal();
}

static void Mix(bool do_mix, bool replace, wxString &var_dest, const wxString &var_orig) {
	if (!do_mix) return;
	if (replace||var_dest.IsEmpty()) var_dest = var_orig;
	else var_dest << " " << var_orig;
}

void mxTemplateCombination::CombineGeneralOpts (wxString zpr_path, CombineOptsGeneral opts) {
	IniFileReader fil(zpr_path);
	for ( wxString section = fil.GetNextSection(); !section.IsEmpty(); section = fil.GetNextSection() ) {
		if (section=="general") {
			for( IniFileReader::Pair p = fil.GetNextPair(); p.IsOk(); p = fil.GetNextPair() ) {
				if (p.Key()=="autocomp_extra") Mix(opts.autocomp, false, project->autocomp_extra, p.AsString());
				else if (p.Key()=="inspection_improving_template") {
					wxString from = p.AsString().BeforeFirst('|'), to = p.AsString().AfterFirst('|');
					int i = project->inspection_improving_template_from.Index(from);
					if (i==wxNOT_FOUND) {
						project->inspection_improving_template_from.Add(from);
						project->inspection_improving_template_to.Add(to);
					} else {
						project->inspection_improving_template_to[i] = to;
					}
				}
//				bool custom_tools;
			}
		}
	}
	if (opts.autocomp) g_code_helper->ReloadIndexes();
}


void mxTemplateCombination::CombineProfile (wxString dest_pname, wxString zpr_path, wxString src_pname, CombineOptsProf opts, bool replace) {
	project_configuration *conf = project->GetConfig(dest_pname); EXPECT(conf!=nullptr);
	IniFileReader fil(zpr_path);
	for ( wxString section = fil.GetNextSection(); !section.IsEmpty(); section = fil.GetNextSection() ) {
		if (section=="config") {
			bool in_desired_section = false;
			for( IniFileReader::Pair p = fil.GetNextPair(); p.IsOk(); p = fil.GetNextPair() ) {
				if (p.Key()=="name" && p.AsString()==src_pname) { in_desired_section =true; }
				else if (!in_desired_section) continue;
				else if (p.Key()=="env_vars")        Mix(opts.env_vars,    replace, conf->env_vars,        p.AsString());
				else if (p.Key()=="compiling_extra") Mix(opts.comp_extra,  replace, conf->compiling_extra, p.AsString());
				else if (p.Key()=="linking_extra")   Mix(opts.link_extra,  replace, conf->linking_extra,   p.AsString());
				else if (p.Key()=="headers_dirs")    Mix(opts.headers,     replace, conf->headers_dirs,    p.AsString());
				else if (p.Key()=="libraries_dirs")  Mix(opts.lib_dirs,    replace, conf->libraries_dirs,  p.AsString());
				else if (p.Key()=="libraries")       Mix(opts.libraries,   replace, conf->libraries,       p.AsString());
				else if (p.Key()=="libs_to_use")     Mix(opts.libs_to_use, replace, conf->libs_to_use,     p.AsString());
				else if (p.Key()=="macros")          Mix(opts.macros,      replace, conf->macros,          p.AsString());
//				else if (p.Key()=="custom_steps") Mix(opts.custom_steps,replace, conf->, p.AsString());
			}
		}
	}
}

void mxTemplateCombination::OnButtonHelp (wxCommandEvent & evt) {
	mxHelpWindow::ShowHelp("templates_combination.html",this);
}

