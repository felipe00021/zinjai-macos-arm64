#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/tglbtn.h>
#include "mxRealTimeInspectionEditor.h"
#include "mxMessageDialog.h"
#include "mxMainWindow.h"
#include "mxSizers.h"
#include "mxArt.h"
#include "MenusAndToolsConfig.h"

BEGIN_EVENT_TABLE(mxRealTimeInspectionEditor,wxFrame)
	EVT_CLOSE(mxRealTimeInspectionEditor::OnClose)
	EVT_MENU(mxID_DEBUG_RUN,mxRealTimeInspectionEditor::OnContinueOrRefresh)
	EVT_BUTTON(wxID_REFRESH,mxRealTimeInspectionEditor::OnUpdateValues)
	EVT_TOGGLEBUTTON(wxID_ANY,mxRealTimeInspectionEditor::OnToggleButton)
	EVT_TIMER(wxID_ANY,mxRealTimeInspectionEditor::OnResizeTimer)
	EVT_TEXT(wxID_ANY,mxRealTimeInspectionEditor::OnText)
	EVT_TEXT_ENTER(wxID_ANY,mxRealTimeInspectionEditor::OnApplyChange)
	EVT_COMBOBOX(wxID_ANY,mxRealTimeInspectionEditor::OnApplyChange)
	EVT_CHECKBOX(wxID_ANY,mxRealTimeInspectionEditor::OnApplyChange)
END_EVENT_TABLE()
	
wxColour mxRealTimeInspectionEditor::AuxRTIE::color_error;
wxColour mxRealTimeInspectionEditor::AuxRTIE::color_modified;
wxColour mxRealTimeInspectionEditor::AuxRTIE::color_ok;
	
static wxString StripScope(wxString value) {
	if (value.Contains("::")) value=value.AfterFirst(':').Mid(1);
	return value;
}

mxRealTimeInspectionEditor::mxRealTimeInspectionEditor(const wxString &expression)
	: wxFrame(main_window,wxID_ANY,expression,wxGetMousePosition()-wxPoint(25,10),wxDefaultSize,wxDEFAULT_FRAME_STYLE|wxSTAY_ON_TOP)
	, m_resize_timer(GetEventHandler(),wxID_ANY)
{
	SetBackgroundColour(wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ));
	DebuggerInspection *di;
	(di = DebuggerInspection::Create(expression,DIF_DONT_USE_HELPER,this,false))->Init();
	if (di->GetDbiType()!=DIT_VARIABLE_OBJECT) {
		mxMessageDialog(main_window,LANG(REALTIMEINSP_ERROR_CREATING_INSPECTION,"Ha ocurrido un error al registrar la inspección"))
			.Title(LANG(GENERAL_ERROR,"Error")).IconError().Run();
		Destroy(); return;
	}
	
	wxBoxSizer *out_sizer = new wxBoxSizer(wxHORIZONTAL);
	scroll_win = new wxScrolledWindow(this,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxVSCROLL|wxTAB_TRAVERSAL);
	scroll_win->SetScrollRate(0,10); scroll_win->EnableScrolling(false,true);
	out_sizer->Add(scroll_win,sizers->BA5_Exp1);
	
	sizer = new wxFlexGridSizer(2); 
	sizer->AddGrowableCol(1);
	sizer->SetFlexibleDirection(wxBOTH);
	sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_NONE);
	
	Add(0,0,di); if (inspections[0].IsButton()) Break(0);
	scroll_win->SetSizer(sizer); this->SetSizer(out_sizer);
	Layout(); Resize(false);
	
	// usamos un accelerator para F5, que será continuar o actualizar
	pair<int,int> shortcut = _menu_item_2(mnDEBUG,mxID_DEBUG_RUN)->GetFlagAndKeycode();
	wxAcceleratorEntry aentry(shortcut.first,shortcut.second,mxID_DEBUG_RUN);
	if (shortcut.second) this->SetAcceleratorTable( wxAcceleratorTable(1,&aentry) );
	
	update_button->SetFocus();
	Show();
	
	wxColour fore = inspections[0].label->GetForegroundColour(), back = inspections[0].label->GetBackgroundColour();
	AuxRTIE::color_ok = fore;
	AuxRTIE::color_error = mxUT::mix_colors(back,fore,1,0,0);
	AuxRTIE::color_modified = mxUT::mix_colors(back,fore,.6,.4,0);
}

mxRealTimeInspectionEditor::~mxRealTimeInspectionEditor() {
	for(int i=0;i<inspections.GetSize();i++)
		inspections[i].di->Destroy();
	debug->InvalidatePauseEvent(this);
}

void mxRealTimeInspectionEditor::Break (int num) {
	if (!debug->IsPaused()) {
		_DEBUG_LAMBDA_2( lmbBreakInspection, mxRealTimeInspectionEditor,win, int,num, { win->Break(num); } );
		debug->PauseFor(new lmbBreakInspection(this,num));
		return;
	}
	if (inspections[num].broken) {
		int child = num+1;
		while (child<inspections.GetSize() && inspections[child].level > inspections[num].level) {
			AuxRTIE &aux = inspections[child];
			sizer->Detach(aux.label); aux.label->Destroy();
			sizer->Detach(aux.ctrl); aux.ctrl->Destroy();
			DebuggerInspection *di = aux.di;
			di->Destroy();
			inspections.Remove(child);
		}
	} else {
		DebuggerInspection *di = inspections[num].di;
		SingleList<DebuggerInspection*> children;
		if (!di->Break(children,true,true) || children.GetSize()==0) return;
	//	if (inspections[num].button) inspections[num].button->Enable(false);
		for(int i=0;i<children.GetSize();i++) { 
			Add(num+i+1,inspections[num].level+1,children[i]);
		}
	}
	inspections[num].broken = !inspections[num].broken;
	inspections[num].GetButton()->SetValue(inspections[num].broken);
}

void mxRealTimeInspectionEditor::OnClose (wxCloseEvent & evt) {
	Destroy();
}

void mxRealTimeInspectionEditor::Add (int pos, int lev, DebuggerInspection * di) {
	BoolFlagGuard fg(mask_events);
	AuxRTIE aux; aux.di=di; wxString tabs(' ',4*lev); aux.level=lev; aux.broken=false;
	aux.label = new wxStaticText(scroll_win,wxID_ANY,tabs+di->GetShortExpression()+": ");
	sizer->Insert(2*pos,aux.label,0,/*wxALIGN_CENTER_HORIZONTAL|*/wxALIGN_CENTER_VERTICAL);
	if (di->AskGDBIfIsEditable()) {
		if(di->GetValueType()=="bool") {
			aux.SetCheck( new wxCheckBox(scroll_win,wxID_ANY,"") );
			aux.GetCheck()->SetValue(di->GetValue()=="true");
		} else {
			wxArrayString opts;
			if (debug->SendCommand("ptype ",di->GetValueType()).stream.Contains("enum class")) {
				wxString ans = debug->SendCommand("complete whatis ",di->GetValueType()+"::").stream;
				while (ans.Contains("\n")) {
					wxString line=ans.BeforeFirst('\n');
					ans=ans.AfterFirst('\n');
					while (line.Len() && (line.Last()=='\n'||line.Last()=='\r')) line.RemoveLast();
					opts.Add(line.AfterFirst(':').Mid(1));
				}
			}
			if (opts.IsEmpty()) {
				wxString value = di->GetValue(); if (di->IsCString()) value = mxUT::UnEscapeString(value,true);
				aux.SetText( new wxTextCtrl(scroll_win,wxID_ANY,value,wxDefaultPosition,wxDefaultSize,wxTE_PROCESS_ENTER) );
			} else {
				aux.SetCombo( new wxComboBox(scroll_win,wxID_ANY,StripScope(di->GetValue()),wxDefaultPosition,wxDefaultSize,opts,wxTE_PROCESS_ENTER) );
			}
		}
	} else {
		aux.SetButton( new wxToggleButton(scroll_win,wxID_ANY,di->GetValue(),wxDefaultPosition,wxDefaultSize,wxNO_BORDER|wxBU_EXACTFIT) );
	}
	aux.ctrl->SetToolTip(di->GetValueType());
	aux.ctrl->SetMinSize(wxSize(50,aux.ctrl->GetMinSize().GetHeight()));
	
	if (pos==0) {
		wxSizer *aux_sizer = new wxBoxSizer(wxHORIZONTAL);
		aux_sizer->Add(aux.ctrl,sizers->Exp1);
		wxString reload_bmp_path = wxString(config->HighDPI()?"dialogs/b24/":"dialogs/b16/")+"button_reload.png";
		aux_sizer->Add(update_button = new wxBitmapButton(scroll_win,wxID_REFRESH,bitmaps->GetBitmap(reload_bmp_path)));
		sizer->Insert(2*pos+1,aux_sizer,sizers->Exp1);
	} else {
		wxWindow *prev_wid = sizer->GetItem(2*pos-1)->GetWindow();
		sizer->Insert(2*pos+1,aux.ctrl,sizers->Exp1);
		aux.ctrl->MoveAfterInTabOrder(prev_wid);
	}
	inspections.Insert(pos,aux);
}

void mxRealTimeInspectionEditor::OnToggleButton (wxCommandEvent & evt) {
	for(int i=0;i<inspections.GetSize();i++) { 
		if (evt.GetEventObject()==inspections[i].ctrl) { 
			Break(i);
			// no llamar directamente a resize porque si se llega a aca sin que el usuario
			// pause la ejecucion, ya estamos en un yield, tonces el layout y cia no actualizan
			// nada (al menos en linux con wx 2.8)
			m_resize_timer.Start(20,true); 
			return;
		}
	}
}

void mxRealTimeInspectionEditor::OnApplyChange (wxCommandEvent & evt) {
	if (mask_events) return;
	if (!debug->IsDebugging()) return;
	for(int i=0;i<inspections.GetSize();i++) {
		if (evt.GetEventObject()==inspections[i].ctrl) {
			_DEBUG_LAMBDA_2(mlbModifyAndUpdateParents,mxRealTimeInspectionEditor,rte,int,i,{
				SingleList<AuxRTIE> &inspections = rte->inspections;
				// cargar el nuevo valor
				wxString new_value = inspections[i].GetValue();
				if (rte->inspections[i].di->IsCString()) new_value = mxUT::EscapeString(new_value,true);
				bool ok = rte->inspections[i].di->ModifyValue(new_value);
				rte->inspections[i].SetState(ok?AuxRTIE::st_ok:AuxRTIE::st_err);
				// forzar la actualizacion de los padres
				while (true) {
					int p = i-1;
					while(p>=0 && inspections[p].level==inspections[i].level) --p;
					if (p<0) break; 
					i=p; inspections[i].di->UpdateValue(true);
				}
			});
			debug->PauseFor(new mlbModifyAndUpdateParents(this,i));
			return;
		}
	}
}

void mxRealTimeInspectionEditor::Resize(bool only_grow_h) {
	wxSize old_size = WindowToClientSize(GetSize());
	sizer->Layout(); /*wxYield();*/
	sizer->RecalcSizes();
	wxSize fit_size = sizer->ComputeFittingClientSize(this);
	int h = fit_size.GetHeight() + 10;
	if  (only_grow_h) {
		if (h>old_size.GetHeight()) {
			SetSize(ClientToWindowSize(wxSize(old_size.GetWidth(),h)));
		}
	} else {
		int w=0;
		for(int i=0;i<inspections.GetSize();i++) { 
			wxSize sz = inspections[i].label->GetSize();
			if (sz.GetWidth()>w) w=sz.GetWidth();
		}
		SetSize(ClientToWindowSize(wxSize(w+100,h)));
	}
#ifdef __WIN32__
	Refresh();
#endif
 }

void mxRealTimeInspectionEditor::OnDIError (DebuggerInspection * di) {
	for(int i=0;i<inspections.GetSize();i++) { 
		if (inspections[i].di==di) {
			inspections[i].ctrl->Enable(false);
			return;
		}
	}
}

void mxRealTimeInspectionEditor::OnDIValueChanged (DebuggerInspection * di) {
	for(int i=0;i<inspections.GetSize();i++) {
		if (inspections[i].di==di) { inspections[i].SetValueFromDI(); break; }
	}
}

void mxRealTimeInspectionEditor::OnDIOutOfScope (DebuggerInspection * di) {
	OnDIError(di);
}


// none of this seems to work if gdb is stopped without a breakpoint (so it stops 
// but do not now where, evaluations gives old value or nothing)
void mxRealTimeInspectionEditor::OnUpdateValues (wxCommandEvent & evt) {
	if (!debug->IsPaused()) {
		_DEBUG_LAMBDA_1( lmbUpdateRTIEditor, mxRealTimeInspectionEditor,win, 
			             { wxCommandEvent evt; win->OnUpdateValues(evt); } );
		debug->PauseFor(new lmbUpdateRTIEditor(this));
	} else {
		inspections[0].di->ForceVOUpdate();
		for(int i=0;i<inspections.GetSize();i++) {
			if (!inspections[i].IsOk()) inspections[i].SetValueFromDI();
			if (inspections[i].di->RequiresManualUpdate())
				inspections[i].di->UpdateValue(true);
		}
	}
}

void mxRealTimeInspectionEditor::OnResizeTimer (wxTimerEvent & evt) {
	Resize(true);
}

void mxRealTimeInspectionEditor::OnContinueOrRefresh (wxCommandEvent & evt) {
	if (debug->IsPaused()) debug->Continue();
	else if (debug->IsDebugging()) OnUpdateValues(evt);
}

void mxRealTimeInspectionEditor::OnText (wxCommandEvent & evt) {
	for(int i=0;i<inspections.GetSize();i++) {
		if (evt.GetEventObject()==inspections[i].ctrl) {
			inspections[i].SetState(AuxRTIE::st_mod); return;
		}
	}
}

void mxRealTimeInspectionEditor::AuxRTIE::SetValueFromDI() {
	switch(ctrl_type) {
		case AuxRTIE::ct_button:
			GetButton()->SetLabel(di->GetValue());
			break;
		case AuxRTIE::ct_text:
			{
				wxString value = di->GetValue(); if (di->IsCString()) value = mxUT::UnEscapeString(value,true);
				GetText()->SetValue(value);
			} break;
		case AuxRTIE::ct_check:
			GetCheck()->SetValue(di->GetValue()=="true");
			break;
		case AuxRTIE::ct_combo:
			GetCombo()->SetValue(StripScope(di->GetValue()));
			break;
		default:
			;
	}
	SetState(st_ok);
}

