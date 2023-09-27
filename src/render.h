#pragma once
#include <string>
#include <controls.h>

typedef void* sPen;
typedef void* sBrush;

class Render {
	using string = std::string;

	void* _window_id;
	int _x, _y;

	Render();
	~Render();
	void MoveTo(int x, int y);
	void LineTo(int x, int y);
	int DrawString(const Rect& rect,const char* str,size_t length,Font* font, AnchorPoints align= TOPLEFT);
	int DrawLine(sPen pen,const Pos& start, const Pos& end);
	int DrawRectangle(sBrush brush, const Rect& rect);
	int FillRectangle(sBrush brush, const Rect& rect);
	int FillEllipse(sBrush brush, const Rect& rect);
};
