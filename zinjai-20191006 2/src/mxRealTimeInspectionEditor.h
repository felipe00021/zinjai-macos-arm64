#ifndef MXREALTIMEINSPECTIONEDITOR_H
#define MXREALTIMEINSPECTIONEDITOR_H
#include <wx/frame.h>
#include <wx/timer.h>
#include <wx/scrolwin.h>
#include <wx/tglbtn.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include "Inspection.h"
#include "mxUtils.h"
#include "Cpp11.h"
#include "raii.h"
#include "asserts.h"

class mxRealTimeInspectionEditor : public wxFrame, public myDIEventHandler, public myDIGlobalEventHandler {
	BoolFlag mask_events;
	wxScrolledWindow *scroll_win;
	wxFlexGridSizer *sizer;
	wxBitmapButton *update_button;
	struct AuxRTIE {
		static wxColour color_ok, color_error, color_modified;
		DebuggerInspection *di;
		int level; bool broken;
		wxStaticText *label;
		enum state_t {st_ok,st_mod,st_err} state;
		enum control_t {ct_button,ct_text, ct_check, ct_combo, ct_null} ctrl_type;
		wxControl *ctrl;
		wxTextCtrl *GetText()  { EXPECT(ctrl_type==ct_text);  return reinterpret_cast<wxTextCtrl*>(ctrl); }
		wxCheckBox *GetCheck() { EXPECT(ctrl_type==ct_check); return reinterpret_cast<wxCheckBox*>(ctrl); }
		wxComboBox *GetCombo() { EXPECT(ctrl_type==ct_combo); return reinterpret_cast<wxComboBox*>(ctrl); }
		wxToggleButton *GetButton() { EXPECT(ctrl_type==ct_button); return reinterpret_cast<wxToggleButton*>(ctrl); }
		void SetText (wxTextCtrl *text)  { EXPECT(ctrl_type==ct_null); ctrl_type=ct_text;  ctrl=text; }
		void SetCombo(wxComboBox *combo) { EXPECT(ctrl_type==ct_null); ctrl_type=ct_combo; ctrl=combo; }
		void SetCheck(wxCheckBox *check) { EXPECT(ctrl_type==ct_null); ctrl_type=ct_check; ctrl=check; }
		void SetButton(wxToggleButton *button) { EXPECT(ctrl_type==ct_null); ctrl_type=ct_button; ctrl=button; }
		bool IsButton() const { return ctrl_type==ct_button; }
		wxString GetValue() {
			switch(ctrl_type) {
			case ct_text:
				return GetText()->GetValue();
			case ct_check:
				return GetCheck()->GetValue()?"true":"false";
			case ct_combo:
				return di->GetValueType()+"::"+GetCombo()->GetValue();
			default:
				DEBUG_BREAK; return "";
			}
		}
		AuxRTIE():state(st_ok),ctrl_type(ct_null) {}
		bool IsOk() const { return state==st_ok; }
		void SetState(state_t new_state) { 
			if (state==new_state) return;
#ifdef __WIN32__
			if (ctrl_type!=ct_combo&&ctrl_type!=ct_text) return;
			switch(state=new_state) {
			case st_mod: ctrl->SetForegroundColour(color_modified); break;
			case st_ok:  ctrl->SetForegroundColour(color_ok); break;
			default:     ctrl->SetForegroundColour(color_error);
			}
			ctrl->Refresh();
#else
			switch(state=new_state) {
			case st_mod: label->SetForegroundColour(color_modified); break;
			case st_ok:  label->SetForegroundColour(color_ok); break;
			default:     label->SetForegroundColour(color_error);
			}
#endif
		}
		void SetValueFromDI();
	};
	SingleList<AuxRTIE> inspections;
	wxTimer m_resize_timer;
public:
	mxRealTimeInspectionEditor(const wxString &inspection);
	~mxRealTimeInspectionEditor();
	
	void Add(int pos, int lev, DebuggerInspection *di);
	void Break(int num);
	void Resize(bool only_grow_h);
	
	// eventos myDIGlobalEventHandler
	void OnDebugStop() override { Destroy(); }
	// eventos myDIEventHandler
	void OnDIError(DebuggerInspection *di) override;
	void OnDIValueChanged(DebuggerInspection *di) override;
	void OnDIOutOfScope(DebuggerInspection *di) override;
	
	// eventos propios
	void OnUpdateValues(wxCommandEvent &evt);
	void OnToggleButton(wxCommandEvent &evt);
	void OnContinueOrRefresh(wxCommandEvent &evt);
	void OnClose(wxCloseEvent &evt);
	void OnResizeTimer(wxTimerEvent &evt);
	void OnText(wxCommandEvent &evt);
	void OnApplyChange(wxCommandEvent &evt);
	
	DECLARE_EVENT_TABLE();
};

#endif

