#pragma once
#include <string>
#include <vector>
#include <surface.h>

enum AnchorPoints:uint8_t {
	TOPLEFT,TOP,TOPRIGHT,LEFT,CENTER,RIGHT,BOTTOMLEFT,BOTTOM,BOTTOMRIGHT
};
struct Pos {
	int x;
	int y;
};
struct Size {
	int width;
	int height;
};
struct Rect:Pos, Size {
};
struct Color {
	union Format{
		struct RGBA {
			uint8_t r,g,b,a;
		};
		struct ARGB {
			uint8_t a,r, g, b;
		};
		RGBA rgba;
		RGBA argb;
		uint32_t value;
	};
	Format u;
};

struct Background {
	enum Formats{
		Format_NORMAL,
		Format_BLP,
		Format_PNG,
		Format_JPG,
		Format_BMP,
	};
	void* src;
	Size size;
	std::string file;
};

struct Font {
	char name[32];
	uint16_t size;
	void* src;
};

union ControlShortcutKey {
	enum ExtKeys {
		ExtKey_NULL = 0,
		ExtKey_CTRL=1,
		ExtKey_ALT = 2,
		ExtKey_SHIFT = 4,
		ExtKey_CAPSLOCK = 8,
		ExtKey_TAB = 16,
	};
	struct Format{
		uint8_t key;
		ExtKeys extkey;
	};
	Format u;
	uint16_t full;
};
// 约等于128byte,1000个125KB。
class Control{
public:
	struct Insets {
		int left, top, right, bottom;
	};
	enum {
		DefaultHeight = 200,
		DefaultWidth  = 200
	};
	std::string _type;
	std::string _name;
	void* _vm_handle;
	float _x, _y,_w,_h;
	float _relative_x, _relative_y;
	float _border;
	int _flag;
	int _level;
	Control* _parent;
	AnchorPoints _point;
	AnchorPoints relative_frame;
	AnchorPoints relative_point;
	std::vector<Control*> _controls;
	ControlShortcutKey _shortcut_key;
	bool _decorate_filenames;
	bool _set_allpoints;
	bool _use_active_context;
	bool _show, _enable;

	Control();
	virtual ~Control();
	virtual bool draw(Surface& surface);
	void notify_parent(uint32_t msg,uintptr_t param);
};

class Layer{
	std::string _mode;
	std::vector<Control*> _controls;
};

class Backdrop:public Control {
	enum CornerFlags {
		CornerFlag_NULL=0,
		CornerFlag_UL=1,
		CornerFlag_UR=2,
		CornerFlag_BL=4,
		CornerFlag_BR=8,
		CornerFlag_T=16,
		CornerFlag_L=32,
		CornerFlag_B=64,
		CornerFlag_R=128
	};
	
	Background _background;
	Background _edgefile;
	Insets _insets;
	int _corner_flags;
	bool _tile,_mirrored,_blend;

	Backdrop();
	virtual ~Backdrop();
	virtual bool draw(Surface& surface);
};
class Text :public Control {
	enum Flags {
		TextFlag_NORMAL,
		TextFlag_PASSWORD,
	};
	enum JustificationH {
		JUSTIFYLEFT,
		JUSTIFYCENTER,
		JUSTIFYRIGHT,
	};
	enum JustificationV {
		JUSTIFYTOP,
		JUSTIFYMIDDLE,
		JUSTIFYBOTTOM,
	};
	Font _font;
	JustificationH _justification_h;
	JustificationV _justification_v;
	Color _highlight_color;
	Color _disable_color;
	Color _shadow_color;
	Pos  _shadow_offset;
	Pos _justification_offset;
	std::string _text;
	virtual ~Text();
};
class TextArea :public Text {
	float _line_height;
	float _line_distance;
	int _max_lines;
};
class Highlight :public Control {
	Background _file;
	std::string _type;
	std::string _mode;
	std::string _layer_style;
	Color _color;
};

class Button :public Control {
	Text* _text;
	Pos  _down_text_offset;
	Highlight* _focus_highlight;
	Highlight* _mouse_over_highlight;
	Background _backdrop;
	Background _disable_backdrop;
	Background _down_backdrop;
	Background _down_disable_backdrop;
};
class CheckBox :public Control {
	Text* _text;
	Highlight* _highlight;
	Highlight* _disable_highlight;
	Background _backdrop;
	Background _disable_backdrop;
	Background _checked_backdrop;
	Background _disable_checked_backdrop;
};
class Slider :public Control {
	enum Layouts {
		Layout_Horizontal,
		Layout_Vertical
	};
	Layouts _layout;
	uint32_t _min_value，_max_value,_init_value,_step_size;
	Button* _thumb;
	Background _backdrop;
	Background _disable_backdrop;
	virtual ~Slider();
};
class ScrollBar :public Slider {
	Button* _dec;
	Button* _inc;
};

class ListBox :public Control {
	ScrollBar* _scrollbar;
	Background _backdrop;
};
class Model :public Control {
	std::string _mode;
	std::string _flle;
};
class Sprite :public Model {
	std::string _layer_style;
};
class Menu :public Control {
	Background _backdrop;
	Background _disable_backdrop;
	Font* _font;
	Highlight* _highlight;
	float _item_height;
	int _inset;
	std::vector<Button*> _items;
};
class PopupMenu :public Menu {
	
};
class Dialog :public Control {
	enum {
		DefaultLevel=32767,
	};
	Background _backdrop;
};
class Texture :public Control {
	Background _file;
	std::string _alpha_mode;
};
class SimpleFrame :public Control {

};

