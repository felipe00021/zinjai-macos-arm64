#include <algorithm>
#include "mxSourceComments.h"
#include "mxSource.h"

static bool isWhite(mxSource *src, int p) { char c = src->GetCharAt(p); return c==' '||c=='\t'; }

int mxSourceComments::CommentWord (mxSource * src, int pfrom, int pto) {
	src->InsertText(std::max(pfrom,pto),"*/");
	src->InsertText(std::min(pfrom,pto),"/*");
	return 4;
}

int mxSourceComments::CommentLines (mxSource * src, int lfrom, int lto) {
	return (src->IsCppOrJustC()?CommentLinesCpp:CommentLinesC)(src,lfrom,lto);
}

int mxSourceComments::CommentLinesCpp (mxSource * src, int lfrom, int lto) {
	for (int i=lfrom;i<=lto;i++) 
		src->InsertText(src->PositionFromLine(i),"//");
	return 2*(lto-lfrom+1);
}

int mxSourceComments::CommentLinesC (mxSource *src, int lfrom, int lto) {
	int retval = 0;
	for (int i=lfrom;i<=lto;i++) {
		int lp=src->PositionFromLine(i), ll=src->GetLineEndPosition(i); 
		while (true) {
			int l0=lp;
			while (lp<ll && isWhite(src,lp)) lp++;
			if (lp==ll) break;
			if (src->IsComment(lp)) {
				while (lp<ll && src->IsComment(lp)) lp++;
				if (lp==ll) break;
			} else {
				int le=lp; 
				while (le<ll && !src->IsComment(lp)) le++;
				src->InsertText(le,"*/");
				src->InsertText(l0,"/*");
				lp=le+4; ll+=4; retval+=4;
			}
		}
	}
	return retval;
}

int mxSourceComments::UnCommentLines (mxSource *src, int lfrom, int lto) {
	int retval = 0;
	for (int i=lfrom;i<=lto;i++) {
		int p=src->GetLineIndentPosition(i); char c=src->GetCharAt(p+1); // s y c se reutilizan para II_*
		if (src->IsComment(p)) {
			bool remove_asterisco_barra = false, add_barra_asterisco = false;
			if (src->GetCharAt(p)=='/' && (c=='/' || c=='*')) {
				src->DeleteText(p,2); retval-=2;
				remove_asterisco_barra = c=='*';
			} else if (i>0) {
				p = src->GetLineEndPosition(i-1);
				src->InsertText(p,"*/"); retval+=2; p+=2;
				remove_asterisco_barra = add_barra_asterisco = true;
			}
			if (remove_asterisco_barra||add_barra_asterisco) {
				int p2=p,pl=src->GetLineEndPosition(i);
				while (p2<pl && !(src->GetCharAt(p2)=='*' && src->GetCharAt(p2+1)=='/') ) p2++;
				if (p2!=pl) {
					src->DeleteText(p2,2); retval-=2;
//					Colourise(p,p2); necesario???
				} else if (add_barra_asterisco && i+1<src->GetLineCount()) {
					p = src->PositionFromLine(i+1);
					src->InsertText(p,"/*"); retval += 2;
				}
			} 
		}
	}
	return retval;
}

int mxSourceComments::UnCommentWord (mxSource *src, int p, bool and_select) {
	int p2=p, l=src->GetLength(); if (!p2) p2++;
	while (src->GetCharAt(p)!='/' || src->GetCharAt(p+1)!='*') if  (--p<0) return 0;
	while (src->GetCharAt(p2-1)!='*' || src->GetCharAt(p2)!='/') if(++p2==l) return 0;
	src->DeleteText(p2-1,2); src->DeleteText(p,2);
	if (and_select) src->SetSelection(p,p2-3);
	return 4;
}

void mxSourceComments::AlignComments (mxSource *src, int lfrom, int lto, int col) {
	for(int i=lfrom;i<=lto;i++) {
		
		// la linea comienza en p0 y va hasta pe
		int p0 = src->PositionFromLine(i), pe = src->GetLineEndPosition(i);
		
		// si el comentario comienza en la col 0 con //, no mover, probablemente sea código comentado
		if (pe>=p0+2 && src->GetCharAt(p0)=='/' && src->GetCharAt(p0+1)=='/') continue;
		
		// el comentario comienza en p2 (>=p1, puede o no haber código antes)
		int p2 = pe; while (p2>p0 && src->IsComment(p2-1)) --p2;
		if (p2==pe) continue; // si no había comentarios en esta linea
		// si volvimos para atras por espacios, "trimear" la pos a la izquierda
		while (p2<pe && isWhite(src,p2)) ++p2;
		
		int cp = src->GetColumn(p2); // columna donde comienza el comentario (no es lo mismo que cant de chars, por los tabs)
		
		if (cp<col) { // si va más a la derecha, agregar espacios
			src->InsertText(p2,wxString(wxChar(' '),col-cp));
		} else if (col<cp) { // si va más a la izquierda quitar espacios entre el código y el comentario
			// buscar cuantos espacios hay entre que termina el código (p1) y empiezan los comentarios (p2)
			int count = 0, to_remove = col-cp, p1 = p2; 
			while (p1>p0 && isWhite(src,p1-1) && count<to_remove) { 
				if (src->GetCharAt(--p1)==' ') --count;
				else count-=src->config_source.tabWidth;
			}
			src->DeleteText(p1,p2-p1);
		}
	}
	
}

void mxSourceComments::RemoveComments (mxSource *src, int lfrom, int lto) {
	for(int i=lfrom;i<=lto;) {
		int p0 = src->PositionFromLine(i), pe = src->GetLineEndPosition(i);
		// buscar donde empieza el comentario (p0)
		while (p0<pe && !src->IsComment(p0)) p0++;
		if (p0==pe) { i++; continue; }
		// buscar donde termina el comentario (p1)
		int p1=p0;
		while (p1<pe && src->IsComment(p1)) p1++;
		// si es toda la linea de comentario, borrar entera
		if (p0<=src->GetLineIndentPosition(i) && p1==pe)  { 
			p0 = src->PositionFromLine(i); p1++;
			--lto;
		}
		// borrar comentario
		src->DeleteText(p0,p1-p0);
	}
}

wxString wrap_line(const wxString &str, int max_line_len) {
	wxString res;
	int cur_line_start=0, last_space=-1, last_dash=-1, p=0, l=str.Len();
	do {
		char c = str[p];
		if (c==' '||c=='\t') last_space = p;
		else if (!(c>='a'&&c<='z')&&!(c>='A'&&c>='Z')&&!(c>='0'&&c<='9')&&c!='.') last_dash = p;
		++p;
		if (p==l || p-cur_line_start>max_line_len) {
			int break_point = p--;
			if (last_space!=-1) break_point = last_space;
			res << str.Mid(cur_line_start,break_point-cur_line_start) << "\n";
			cur_line_start = break_point+1; last_space = last_dash = -1;
		}
	} while(p<l);
	return res;
}

void mxSourceComments::WrapComment(mxSource *src, int lfrom, int lto, int width) {
	
}

