#pragma once
#include <string>
#include <controls.h>

typedef void* GS_Pen;
typedef void* GS_Brush;

class Render {
public:
	using string = std::string;

	void* _window_id;
	int _x, _y;

	Render();
	virtual ~Render();
	virtual void MoveTo(int x, int y);
	virtual void LineTo(int x, int y);
	virtual int DrawString(const GS_Rect& rect,const char* str,size_t length, GS_Font* font, AnchorPoints align= TOPLEFT);
	virtual int DrawLine(GS_Pen pen,const GS_Pos& start, const GS_Pos& end);
	virtual int DrawRectangle(GS_Brush brush, const GS_Rect& rect);
	virtual int FillRectangle(GS_Brush brush, const GS_Rect& rect);
	virtual int FillEllipse(GS_Brush brush, const GS_Rect& rect);
};
