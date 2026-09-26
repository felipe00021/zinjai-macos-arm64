#ifndef MXSOURCECOMMENTS_H
#define MXSOURCECOMMENTS_H

class mxSource;

class mxSourceComments {
public:

	// comentar/descomentar... retornan la cantidad de caracteres agragados (puede ser <0 para descomentar)
	static int CommentWord(mxSource *src, int pfrom, int pto);
	static int CommentLines(mxSource *src, int lfrom, int lto);
	static int CommentLinesCpp(mxSource *src, int lfrom, int lto);
	static int CommentLinesC(mxSource *src, int lfrom, int lto);
	static int UnCommentLines(mxSource *src, int lfrom, int lto);
	static int UnCommentWord(mxSource *src, int p, bool and_select=false);
	
	static void AlignComments(mxSource *src, int lfrom, int lto, int col);
	static void RemoveComments(mxSource *src, int lfrom, int lto);
	static void WrapComment(mxSource *src, int lfrom, int lto, int width);
};

#endif

