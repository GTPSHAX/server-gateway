#pragma once

#include "BaseApp.h"

enum eContentSize {
	SIZE_SMALL,
	SIZE_BIG
};

enum eContentDirection {
	DIRECTION_LEFT,
	DIRECTION_CENTER
};

enum eSpacer {
	SPACER_SMALL,
	SPACER_BIG
};

class DialogBuilder {
private:
	std::string ctx;
	size_t width = 0;

	std::string get_size(const eContentSize&);
	std::string get_direction(const eContentDirection&);
	std::string get_spacer(const eSpacer&);

public:
	// Set default color
	DialogBuilder(std::string default_color = "`o");
	~DialogBuilder() = default;

	const std::string get_raw() { 
		std::string ctx_ = ctx + "\nadd_textbox|" + std::string(width, ' ') + "|left|";
		return ctx_;
	}
	DialogBuilder* add_raw(const std::string& raw) {
		ctx += raw;
		return this;
	}

	// Di hpku max 146 bg
	DialogBuilder* set_width(const size_t& width) {
		this->width = width;
		return this;
	}

	DialogBuilder* add_quick_exit();
	DialogBuilder* end_dialog(const std::string& dialog_name, const std::string button_accept = "", const std::string button_cancel = "Nevermind.");
	DialogBuilder* add_button(const std::string& button_name, const std::string& message);
	DialogBuilder* add_checkbox(const std::string& button_name, const std::string& message, const bool is_clicked = false);
	DialogBuilder* add_textbox(const std::string&, eContentDirection direction = DIRECTION_LEFT);
	DialogBuilder* add_smalltext(const std::string&, eContentDirection direction = DIRECTION_LEFT);
	DialogBuilder* add_spacer(const eSpacer&);
	DialogBuilder* set_bg_color(const std::string&);
	DialogBuilder* set_border_color(const std::string&);
	DialogBuilder* add_label(const std::string&, eContentSize size = SIZE_BIG, eContentDirection direction = DIRECTION_LEFT);
	DialogBuilder* add_label_with_icon(const uint32_t&, const std::string&, eContentSize size = SIZE_BIG, eContentDirection direction = DIRECTION_LEFT);
	DialogBuilder* add_label_with_icon_button(const std::string& button_name, const uint32_t&, const std::string&, eContentSize size = SIZE_BIG, eContentDirection direction = DIRECTION_LEFT);
};