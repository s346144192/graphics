#include <window_win.h>


HINSTANCE window_win::_hinstance=NULL;
ATOM window_win::_class_atom=0;
const char* window_win::_class_name=nullptr;
std::unordered_map<HWND, window_win*> window_win::window_map;
window_win::window_win() {

}
window_win::~window_win() {

}


bool window_win::InitClass(HINSTANCE hinstance, LPCSTR class_name) {
	_hinstance = hinstance;
	_class_name = class_name;
	WNDCLASSEX  winclass;
	winclass.cbSize = sizeof(WNDCLASSEX);
	winclass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC | CS_DBLCLKS;
	winclass.lpfnWndProc = MessageProc;
	winclass.cbClsExtra = 16;
	winclass.cbWndExtra = 16;
	winclass.hInstance = (HINSTANCE)hinstance;
	winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor = LoadCursor(NULL, IDC_ARROW);;
	winclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName = NULL;
	winclass.lpszClassName = class_name;
	winclass.hIconSm = nullptr;
	_class_atom = RegisterClassEx(&winclass);
	return (_class_atom != 0);
}
bool window_win::ReleaseClass() {
	if (_class_atom!=0) {
		_class_atom = 0;
		return UnregisterClass(_class_name, _hinstance);
	}
	return false;
}
int window_win::Create(const char* title, int x, int y, int w, int h, int flag, int extflag) {
	if (_class_atom == 0) {
		return -1;
	}
	Destroy();
	_window = CreateWindowEx(extflag, _class_name, title, flag, x, y, w, h, NULL, NULL, _hinstance, 0);
	if (_window == 0) {
		return -2;
	}
	return 0;
}
int window_win::Destroy() {
	BOOL result = FALSE;
	if (_window != 0) {
		result = DestroyWindow(_window);
		_window = nullptr;
	}
	return result;
}
int window_win::GetX() {
	if (!_window) {
		return 0;
	}
	RECT rect = { 0,0,0,0 };
	GetWindowRect(_window,&rect);
	return rect.left;
}
int window_win::GetY() {
	if (!_window) {
		return 0;
	}
	RECT rect = { 0,0,0,0 };
	GetWindowRect(_window, &rect);
	return rect.top;
}
int window_win::GetWidth() {
	if (!_window) {
		return 0;
	}
	RECT rect = { 0,0,0,0 };
	GetWindowRect(_window, &rect);
	return rect.right-rect.left;
}
int window_win::GetHeight() {
	if (!_window) {
		return 0;
	}
	RECT rect = { 0,0,0,0 };
	GetWindowRect(_window, &rect);
	return rect.bottom - rect.top;
}
void window_win::SetPos(int x, int y) {
	if (!_window) {
		return;
	}
	POINT point = { x,y };
	ScreenToClient(_window,&point);
	SetWindowPos(_window, HWND_TOP, point.x, point.y,NULL,NULL, SWP_NOSIZE);
}
void window_win::SetSize(int width, int height) {
	if (!_window) {
		return;
	}
	POINT point = { width,height };
	ScreenToClient(_window, &point);
	SetWindowPos(_window, HWND_TOP, NULL, NULL, point.x, point.y, SWP_NOMOVE);
}
void window_win::SetX(int x) {
	if (!_window) {
		return;
	}
	RECT rect = { 0,0,0,0 };
	GetClientRect(_window, &rect);
	POINT point = { x,rect.top };
	ScreenToClient(_window, &point);
	SetWindowPos(_window, HWND_TOP, point.x, point.y, NULL, NULL, SWP_NOSIZE);
}
void window_win::SetY(int y) {
	if (!_window) {
		return;
	}
	RECT rect = { 0,0,0,0 };
	GetClientRect(_window, &rect);
	POINT point = { rect.left,y };
	ScreenToClient(_window, &point);
	SetWindowPos(_window, HWND_TOP,  point.x, point.y, NULL, NULL, SWP_NOSIZE);
}
void window_win::SetWidth(int width) {
	if (!_window) {
		return;
	}
	RECT rect = { 0,0,0,0 };
	GetClientRect(_window, &rect);
	POINT point = { width,rect.bottom-rect.top };
	ScreenToClient(_window, &point);
	SetWindowPos(_window, HWND_TOP, NULL, NULL, point.x, point.y, SWP_NOMOVE);
}
void window_win::SetHeight(int height) {
	if (!_window) {
		return;
	}
	RECT rect = { 0,0,0,0 };
	GetClientRect(_window, &rect);
	POINT point = { rect.right-rect.left,height };
	ScreenToClient(_window, &point);
	SetWindowPos(_window, HWND_TOP, NULL, NULL, point.x, point.y, SWP_NOMOVE);
}
void window_win::Enable(bool enable) {
	
}
void window_win::Show(bool enable) {
	ShowWindow(_window, (enable)? SW_SHOW: SW_HIDE);
}
void window_win::ZoomMin() {
	ShowWindow(_window, SW_MINIMIZE);
}
bool window_win::IsMin() {
	return IsIconic(_window);
}
void window_win::ZoomMax() {
	ShowWindow(_window, SW_MAXIMIZE);
}
bool window_win::IsMax() {
	return IsZoomed(_window);
}
void window_win::ZoomNormal() {
	ShowWindow(_window, SW_NORMAL);
}
HWND window_win::Parent() {
	return GetParent(_window);
}
void window_win::Parent(HWND window) {
	SetParent(_window, window);
}
int window_win::SetIcon(HICON hicon) {
	return SendMessage(_window, WM_SETICON, ICON_BIG, (LPARAM)hicon) 
		| SendMessage(_window, WM_SETICON, ICON_SMALL, (LPARAM)hicon)
		| SendMessage(_window, WM_SETICON, ICON_SMALL2, (LPARAM)hicon);
}
int window_win::GetTitle(char* buffer, size_t count) {
	return GetWindowTextA(_window, buffer, count);
}
bool window_win::SetTitle(const char* str) {
	return SetWindowTextA(_window, str);
}
bool window_win::FilterMessage(HWND& hwnd, UINT& msg, WPARAM& param1, LPARAM& param2) {
	return false;
}
bool window_win::RelayMessage(UINT msg, WPARAM param1, LPARAM param2) {

	return false;
}
bool window_win::ProcessEvent(uint32_t eventid, uintptr_t param) {
	return false;
}

LRESULT CALLBACK window_win::MessageProc(HWND hwnd, UINT msg, WPARAM param1, LPARAM param2) {
	const auto& it = window_map.find(hwnd);
	if (it != window_map.end() && (*it).second!=nullptr) {
		window_win* win = (*it).second;
		win->RelayMessage(msg, param1, param2);
		if (win->FilterMessage(hwnd, msg, param1, param2)) {
			return (LRESULT)param2;
		}
	}
	return DefWindowProc(hwnd, msg, param1, param2);
}
int window_win::LoopMessage() {
	MSG msg = { 0 };
	// 主消息循环:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}