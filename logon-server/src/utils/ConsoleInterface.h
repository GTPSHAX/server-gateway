#pragma once

#include "BaseApp.h"

// Macro definitions for easy logging
// Using a helper macro for file, line, function info to avoid repetition
#define CONSOLE_LOG_ARGS __FILE__, __LINE__, __FUNCTION__

// Corrected macro definitions to correctly pass the formatted message to the p_info family functions
#define print_info(...) ConsoleInterface::p_info(CONSOLE_LOG_ARGS, fmt::format(__VA_ARGS__))
#define print_warning(...) ConsoleInterface::p_warning(CONSOLE_LOG_ARGS, fmt::format(__VA_ARGS__))
#define print_error(...) ConsoleInterface::p_error(CONSOLE_LOG_ARGS, fmt::format(__VA_ARGS__))
#define print_success(...) ConsoleInterface::p_success(CONSOLE_LOG_ARGS, fmt::format(__VA_ARGS__))
#define write_log(path, ...) ConsoleInterface::write_log_file(path, fmt::format(__VA_ARGS__))

#if defined(_DEBUG) || defined(DEBUG) || defined(_PRE_RELEASE) || defined(PRE_RELEASE)
#define print_debug(...) ConsoleInterface::p_debug(CONSOLE_LOG_ARGS, fmt::format(__VA_ARGS__))
#else
#define print_debug(...) ((void)0)
#endif

class ConsoleInterface {
public:
    // Color constants - Mapped to fmt::color for direct use
    // Using explicit underlying type (int) for safety and direct casting
    enum class Color : int {
        BLACK = 0,
        RED = 1,
        GREEN = 2,
        YELLOW = 3,
        BLUE = 4,
        MAGENTA = 5,
        CYAN = 6,
        WHITE = 7,
        BRIGHT_BLACK = 8, // fmt::color::gray
        BRIGHT_RED = 9,   // Mapped to light_red, which exists in fmt
        BRIGHT_GREEN = 10, // fmt::color::light_green
        BRIGHT_YELLOW = 11, // fmt::color::light_golden_rod_yellow
        BRIGHT_BLUE = 12,  // fmt::color::light_blue
        BRIGHT_MAGENTA = 13, // fmt::color::magenta (using base magenta as bright_magenta is not direct)
        BRIGHT_CYAN = 14,  // Mapped to light_cyan, which exists in fmt
        BRIGHT_WHITE = 15 // fmt::color::white (using base white as bright_white is not direct)
    };


    struct BoxChars {
        static constexpr const char* TOP_LEFT = "+";
        static constexpr const char* TOP_RIGHT = "+";
        static constexpr const char* BOTTOM_LEFT = "+";
        static constexpr const char* BOTTOM_RIGHT = "+";
        static constexpr const char* HORIZONTAL = "-";
        static constexpr const char* VERTICAL = "|";
        static constexpr const char* CROSS = "+";
        static constexpr const char* T_DOWN = "+";
        static constexpr const char* T_UP = "+";
        static constexpr const char* T_RIGHT = "+";
        static constexpr const char* T_LEFT = "+";
    };

    // Initialization and cleanup
    static void initialize();
    static void cleanup();
    static bool is_initialized(); // New: Check if initialized

    // Basic printing functions
    static void print(const std::string& message, Color color = Color::WHITE);
    static void println(const std::string& message = "", Color color = Color::WHITE);
    static void print_colored(const std::string& message, Color fg, Color bg = Color::BLACK);

    // Logging functions with file info
    static void p_info(const char* file, int line, const char* function, const std::string& message);
    static void p_warning(const char* file, int line, const char* function, const std::string& message);
    static void p_error(const char* file, int line, const char* function, const std::string& message);
    static void p_success(const char* file, int line, const char* function, const std::string& message);
    static void p_debug(const char* file, int line, const char* function, const std::string& message);

    // Input functions
    static std::string input(const std::string& prompt = "", Color prompt_color = Color::CYAN);
    static std::string input_password(const std::string& prompt = "Password: ", char mask = '*');
    // Corrected default arguments for min_val and max_val by calling min() and max()
    static int input_int(const std::string& prompt = "Enter number: ", int min_val = 0, int max_val = 100000);
    static float input_float(const std::string& prompt = "Enter number: ");
    static bool input_bool(const std::string& prompt = "Enter (y/n): ");

    // Menu and selection functions
    static int show_menu(const std::string& title, const std::vector<std::string>& options, bool numbered = true);

    // Box and border functions
    static void print_box(const std::string& content, int width = 0, Color border_color = Color::CYAN);
    static void print_separator(int width = 0, Color color = Color::CYAN);
    static void print_header(const std::string& title, Color color = Color::BRIGHT_CYAN);

    // Progress and loading
    static void print_progress_bar(float progress, int width = 50, Color filled_color = Color::GREEN, Color empty_color = Color::WHITE);
    static void show_loading(const std::string& message = "Loading", int duration_ms = 2000, Color color = Color::CYAN); // Added color
    static void show_loading(const std::string& message = "Loading", std::function<void()> callback = nullptr, Color color = Color::CYAN);

    // Utility functions
    static void clear_screen();
    static void clear_line();
    static void move_cursor(int x, int y);
    static void hide_cursor();
    static void show_cursor();
    static std::pair<int, int> get_terminal_size();
    static void pause(const std::string& message = "Press any key to continue...", Color color = Color::YELLOW); // Added color

    // Animation effects
    static void typewriter_effect(const std::string& text, int delay_ms = 50, Color color = Color::WHITE);
    static void rainbow_text(const std::string& text); // Re-implementing for efficiency

    static bool is_terminal_capable();
    static char getch_cross_platform();
    static bool is_valid_utf8(const std::string& str);
    static std::string sanitize_utf8(const std::string& input);
    static std::string color_to_ansi_fg(Color c);
    static std::string color_to_ansi_bg(Color c);

    static void write_log_file(const std::string& filepath, const std::string& content);

private:
    // Private constructor/destructor to prevent instantiation (pure static class)
    ConsoleInterface() = delete;
    ~ConsoleInterface() = delete;

    static std::string get_filename(const std::string& filepath);
    static std::string format_file_info(const char* file, int line, const char* function);
    static void setup_ansi_for_windows(); // Renamed and consolidated Windows ANSI setup
    static fmt::color to_fmt_color(Color c); // Helper to convert enum Color to fmt::color

    // Unified logging function template to reduce code duplication
    template<fmt::color TextColor, fmt::color BoxColor, fmt::color TagColor, fmt::color FileInfoColor>
    static void log_message_template(const char* file, int line, const char* function, const std::string& message, const std::string& tag, const std::string& icon);

    static bool m_initialized;
    static bool m_ansi_supported; // Flag to indicate if ANSI escape codes are supported

#ifdef _WIN32
    static HANDLE m_console_handle;
    static DWORD m_original_out_mode; // Original output mode for console
#else
    static struct termios m_original_termios; // Original terminal settings for non-Windows
#endif
};

