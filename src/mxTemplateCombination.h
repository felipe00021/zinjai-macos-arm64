#ifndef MXTEMPLATECOMBINATION_H
#define MXTEMPLATECOMBINATION_H
#include "mxCommonConfigControls.h"
#include <vector>

class wxFlexGridSizer;

class mxTemplateCombination : public mxDialog {
	wxArrayString m_other_profiles;
	wxFlexGridSizer *m_sizer_profiles;
	wxCheckBox *m_merge_env_vars;
	wxCheckBox *m_merge_comp_extra;
	wxCheckBox *m_merge_link_extra;
	wxCheckBox *m_merge_headers;
	wxCheckBox *m_merge_lib_dirs;
	wxCheckBox *m_merge_libraries;
	wxCheckBox *m_merge_libs_to_use;
	wxCheckBox *m_merge_macros;
	wxCheckBox *m_merge_custom_steps;
	wxCheckBox *m_merge_custom_tools;
	wxCheckBox *m_merge_autocomp;
	wxCheckBox *m_merge_inspections;
	std::vector<wxStaticText*> m_names_this;
	std::vector<wxChoice*> m_actions;
	std::vector<wxChoice*> m_names_other;
	wxString m_zpr_path;
public:
	mxTemplateCombination(wxWindow *parent, wxString other_zpr);
	void AddProfile(wxString name/*, bool original*/);
	static void Run(wxWindow *parent);
	static int MatchName(const wxString &name, const wxArrayString &candidates);

	struct CombineOptsProf {
		bool env_vars;
		bool comp_extra;
		bool link_extra;
		bool headers;
		bool lib_dirs;
		bool libraries;
		bool macros;
		bool custom_steps;
		bool libs_to_use;
	};
	struct CombineOptsGeneral {
		bool custom_tools;
		bool autocomp;
		bool inspections;
	};
	static void CombineProfile(wxString dest_pname, wxString zpr_path, wxString src_pname, CombineOptsProf opts, bool replace=false);
	static void CombineGeneralOpts(wxString zpr_path, CombineOptsGeneral opts);

private:
	static wxString GetName(const wxString zpr_path);
	static int GetProfiles(const wxString zpr_path, wxArrayString &profiles_names);
	void OnCombo(wxCommandEvent &evt);
	void OnButtonOk(wxCommandEvent &evt);
	void OnButtonCancel(wxCommandEvent &evt);
	void OnButtonHelp(wxCommandEvent &evt);
	DECLARE_EVENT_TABLE();
};

#endif

