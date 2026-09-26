#include <wx/textctrl.h>
#include <wx/choicdlg.h>
#include <wx/checklst.h>
#include "mxMultipleChoiceEditor.h"
#include "mxUtils.h"
#include "mxSizers.h"


BEGIN_EVENT_TABLE(mxMultipleChoiceEditor,wxDialog)
	EVT_BUTTON(wxID_OK,mxMultipleChoiceEditor::OnOkButton)
	EVT_BUTTON(wxID_CANCEL,mxMultipleChoiceEditor::OnCancelButton)
//	EVT_BUTTON(mxID_HELP_BUTTON,mxMultipleChoiceEditor::OnHelpButton)
	EVT_TEXT(wxID_ANY,mxMultipleChoiceEditor::OnFilter)
END_EVENT_TABLE()
	
/**
* @brief Construye y muestra una ventana para seleccionar multiples opciones de una lista de posibles
*
* Recibe una lista de posibles opciones y un cuadro de texto, arma una ventana 
* con una lista de checks para las opciones, marca las que estaban presentes
* en el cuadro de texto, y si el usuario presiona aceptar actualiza el contenido
* del cuadro de texto. La ventana se ejecuta directamente en este constructor.
*
* @param parent         ventana padre, para hacerlo modal
* @param title          titulo de la ventana 
* @param message        texto para mostrar sobre la lista de checks
* @param text           control de texto de donde tomar las opciones seleccionadas y donde guardar el resultado
* @param options_array  lista de opciones posibles
* @param comma_splits   indica si la coma separa opciones en el cuadro de texto, para pasar a mxUtils::Split
**/
mxMultipleChoiceEditor::mxMultipleChoiceEditor(wxWindow *parent, wxString title, wxString message, wxTextCtrl *text, 
											   const wxArrayString &options_array, bool comma_splits) 
	: mxDialog(parent,title), m_filter_text(nullptr), m_dst_text(text)
{
	
	for(size_t i=0;i<options_array.size();i++)
		m_data[options_array[i]] = false;
	
	wxArrayString splitted_array;
	mxUT::Split(text->GetValue(),splitted_array,comma_splits,true);
	for (unsigned int i=0;i<splitted_array.GetCount();i++)
		m_data[splitted_array[i]] = true; // ademas de seleccionar, agrega las que no estaban

	CreateSizer sizer(this);
	
	if (!message.IsEmpty())
		sizer.BeginLabel( message ).EndLabel();
	
	m_check_list = new wxCheckListBox(this,wxID_ANY);
	m_check_list->SetMinSize(wxSize(300,300));
	sizer.Add(m_check_list,sizers->BA5_Exp1);
	Set2List();
	
	if (m_data.size()>15)
		sizer.BeginText( LANG(GENERAL_FILTER,"Filtrar") ).Short().EndText(m_filter_text);
	
	sizer.BeginBottom()/*.Help()*/.Ok().Cancel().EndBottom(this);
	sizer.SetAndFit();
	
	ShowModal();
}

void mxMultipleChoiceEditor::OnOkButton(wxCommandEvent &evt) {
	List2Set();
	wxString res;
	for(map_t::iterator it=m_data.begin();it!=m_data.end();++it)
		if (it->second) res << mxUT::Quotize(it->first) << " "; 
	if (!res.IsEmpty()) res = res.Mid(0,res.Len()-1);
	m_dst_text->SetValue(res);
	EndModal(1);
}

void mxMultipleChoiceEditor::OnCancelButton(wxCommandEvent &evt) {
	EndModal(0);
}

void mxMultipleChoiceEditor::OnFilter (wxCommandEvent & evt) {
	List2Set();
	Set2List();
}

void mxMultipleChoiceEditor::List2Set ( ) {
	for(size_t i=0;i<m_check_list->GetCount();i++)
		m_data[m_check_list->GetString(i)] = m_check_list->IsChecked(i);
}

void mxMultipleChoiceEditor::Set2List ( ) {
	m_check_list->Clear();
	wxString s; if (m_filter_text) s = m_filter_text->GetValue().Upper();
	for(map_t::iterator it=m_data.begin();it!=m_data.end();++it) {
		if (!s.IsEmpty() && !it->first.Upper().Contains(s)) continue;
		int id = m_check_list->Append(it->first);
		m_check_list->Check(id,it->second);
	}
}

