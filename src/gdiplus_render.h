
#pragma once
#include <render.h>
#include <Windows.h>
// #include <Gdiplus.h>

class GdiplusRender:public Render {
	GdiplusRender();
	~GdiplusRender();
	int DrawImage();
};
