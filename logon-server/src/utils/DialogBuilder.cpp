#include "DialogBuilder.h"

DialogBuilder::DialogBuilder(std::string default_color) {
	if (default_color.empty()) return;
	ctx += "default_color|" + default_color;
}
std::string DialogBuilder::get_size(const eContentSize& size) {
	switch (size) {
	case SIZE_BIG: return "big";
	case SIZE_SMALL: return "small";
	default: return "UNKNOWN";
	}
}
std::string DialogBuilder::get_direction(const eContentDirection& direction) {
	switch (direction) {
	case DIRECTION_LEFT: return "left";
	case DIRECTION_CENTER: return "center";
	default: return "UNKNOWN";
	}
}
std::string DialogBuilder::get_spacer(const eSpacer& spacer) {
	switch (spacer) {
	case SPACER_SMALL: return "small";
	case SPACER_BIG: return "big";
	default: return "UNKNOWN";
	}
}

DialogBuilder* DialogBuilder::add_label(const std::string& message, eContentSize size, eContentDirection direction) {
	const auto& size_ = get_size(size), direction_ = get_direction(direction);
	ctx += fmt::format("\nadd_label|{}|{}|{}|", size_, message, direction_);
	return this;
}
DialogBuilder* DialogBuilder::add_label_with_icon_button(const std::string& button_name, const uint32_t& item_id, const std::string& message, eContentSize size, eContentDirection direction) {
	const auto& size_ = get_size(size), direction_ = get_direction(direction);
	ctx += fmt::format("\nadd_label_with_icon_button|{}|{}|{}|{}|{}|", size_, message, direction_, item_id, button_name);
	return this;
}
DialogBuilder* DialogBuilder::add_quick_exit() {
	ctx += "\nadd_quick_exit|";
	return this;
}
DialogBuilder* DialogBuilder::end_dialog(const std::string& dialog_name, const std::string button_accept, const std::string button_cancel) {
	ctx += "\nend_dialog|" + dialog_name + "|" + button_cancel + "|" + button_accept + "|";
	return this;
}
DialogBuilder* DialogBuilder::add_button(const std::string& button_name, const std::string& message) {
	ctx += "\nadd_button|" + button_name + "|" + message + "|noflags|0|0|";
	return this;
}
DialogBuilder* DialogBuilder::add_checkbox(const std::string& button_name, const std::string& message, const bool is_clicked) {
	ctx += "\nadd_checkbox|" + button_name + "|" + message + "|" + (is_clicked ? "1" : "0") + "|";
	return this;
}
DialogBuilder* DialogBuilder::add_smalltext(const std::string& message, eContentDirection direction) {
	ctx += "\nadd_smalltext|" + message + "|" + get_direction(direction) + "|";
	return this;
}
DialogBuilder* DialogBuilder::add_textbox(const std::string& message, eContentDirection direction) {
	ctx += "\nadd_textbox|" + message + "|" + get_direction(direction) + "|";
	return this;
}
DialogBuilder* DialogBuilder::add_spacer(const eSpacer& spacer) {
	ctx += "\nadd_spacer|" + get_spacer(spacer) + "|";
	return this;
}
DialogBuilder* DialogBuilder::set_border_color(const std::string& color) {
	ctx += "\nset_border_color|" + color + "|";
	return this;
}
DialogBuilder* DialogBuilder::set_bg_color(const std::string& color) {
	ctx += "\nset_bg_color|" + color + "|";
	return this;
}
DialogBuilder* DialogBuilder::add_label_with_icon(const uint32_t& item_id, const std::string& message, eContentSize size, eContentDirection direction) {
	const auto& size_ = get_size(size), direction_ = get_direction(direction);
	ctx += fmt::format("\nadd_label_with_icon|{}|{}|{}|{}|", size_, message, direction_, item_id);
	return this;
}