#pragma once
#include <stdint.h>
class window_base {
public:
	window_base() = default;
	virtual ~window_base()=default;
	virtual int Create(const char* title, int x,int y,int w, int h, int flag, int extflag) = 0;
	virtual int Destroy()=0;
	virtual int GetX() = 0;
	virtual int GetY() = 0;
	virtual int GetWidth() = 0;
	virtual int GetHeight() = 0;
	virtual void SetPos(int x,int y) = 0;
	virtual void SetSize(int width, int height) = 0;
	virtual void SetX(int x) = 0;
	virtual void SetY(int y) = 0;
	virtual void SetWidth(int width) = 0;
	virtual void SetHeight(int height) = 0;
	virtual	void Enable(bool enable) = 0;
	virtual	void Show(bool enable) = 0;
	virtual	void ZoomMin() = 0;
	virtual	void ZoomMax() = 0;
	virtual	void ZoomNormal() = 0;
	//virtual	void* Parent() = 0;
	//virtual	void Parent(void* window) = 0;
	virtual	int GetTitle(char* buffer,size_t count) = 0;
	virtual	bool SetTitle(const char* str) = 0;
	//virtual bool FilterMessage(uint32_t msg,uintptr_t param1, uintptr_t param2) = 0;
	//virtual bool ReplyMessage(uint32_t msg, uintptr_t param1, uintptr_t param2) = 0;
	//virtual bool ProcessEvent(uint32_t eventid, uintptr_t param) = 0;

	//static int ProcessMessage(void* window, uint32_t msg, uintptr_t param1, uintptr_t param2) {};
	//static int LoopMessage() {};

};