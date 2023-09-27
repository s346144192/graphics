#pragma once
#include <window_base.h>
#include <unordered_map>
#include <windows.h>

class window_win:public window_base {
public:
	HWND _window ;
	static HINSTANCE _hinstance;
	static ATOM _class_atom;
	static const char* _class_name;
	static std::unordered_map<HWND, window_win*> window_map;

	window_win();
	~window_win();
	static bool InitClass(HINSTANCE hinstance, LPCSTR class_name);
	static bool ReleaseClass();
	int Create(const char* title, int x, int y, int w, int h, int flag, int extflag) override;
	int Destroy() override;
	int GetX() override;
	int GetY() override;
	int GetWidth() override;
	int GetHeight() override;
	void SetPos(int x, int y) override;
	void SetSize(int width, int height) override;
	void SetX(int x) override;
	void SetY(int y) override;
	void SetWidth(int width) override;
	void SetHeight(int height) override;
	void Enable(bool enable) override;
	void Show(bool enable) override;
	void ZoomMin() override;
	bool IsMin();
	void ZoomMax() override;
	bool IsMax();
	void ZoomNormal() override;
	HWND Parent();
	void Parent(HWND window);
	int SetIcon(HICON hicon);
	int GetTitle(char* buffer, size_t count) override;
	bool SetTitle(const char* str) override;
	bool FilterMessage(HWND& hwnd, UINT& msg, WPARAM& param1, LPARAM& param2);
	bool RelayMessage(UINT msg, WPARAM param1, LPARAM param2);
	bool ProcessEvent(uint32_t eventid, uintptr_t param);

	static LRESULT CALLBACK MessageProc(HWND hwnd, UINT msg, WPARAM param1, LPARAM param2);
	static int LoopMessage();
};