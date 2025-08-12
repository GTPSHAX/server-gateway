#pragma once

#include <BaseApp.h>

enum class PlayerRole {
	NONE = 0,
	MERCHANT = 1 << 0,
	ADMIN = 1 << 10
};

struct RolePrefix {
	std::string chat = "`0",
		name = "`0";
};

enum class PlayerAttribute {
	NONE = 0
};