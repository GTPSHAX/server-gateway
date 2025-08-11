#include "ConsoleInterface.h"

#include "FileSystem2.h"

// Static member initialization
bool ConsoleInterface::m_initialized = false;
bool ConsoleInterface::m_ansi_supported = false;

#ifdef _WIN32
HANDLE ConsoleInterface::m_console_handle = nullptr;
DWORD ConsoleInterface::m_original_out_mode = 0;
#else
struct termios ConsoleInterface::m_original_termios;
#endif

// Helper function to convert custom Color enum to fmt::color
fmt::color ConsoleInterface::to_fmt_color(Color c) {
    // This mapping ensures that our enum values directly translate to fmt::color enum values
    // for common colors. For bright colors, we map to fmt's specific bright colors or close approximations.
    switch (c) {
    case Color::BLACK: return fmt::color::black;
    case Color::RED: return fmt::color::red;
    case Color::GREEN: return fmt::color::green;
    case Color::YELLOW: return fmt::color::yellow;
    case Color::BLUE: return fmt::color::blue;
    case Color::MAGENTA: return fmt::color::magenta;
    case Color::CYAN: return fmt::color::cyan;
    case Color::WHITE: return fmt::color::white;
    case Color::BRIGHT_BLACK: return fmt::color::gray; // fmt::color::gray is a good approximation for bright black
    case Color::BRIGHT_RED: return fmt::color::light_pink; // Corrected to use light_red
    case Color::BRIGHT_GREEN: return fmt::color::light_green;
    case Color::BRIGHT_YELLOW: return fmt::color::light_golden_rod_yellow;
    case Color::BRIGHT_BLUE: return fmt::color::light_blue;
    case Color::BRIGHT_MAGENTA: return fmt::color::magenta; // fmt doesn't have bright_magenta directly, use base
    case Color::BRIGHT_CYAN: return fmt::color::light_cyan; // Corrected to use light_cyan
    case Color::BRIGHT_WHITE: return fmt::color::white; // fmt doesn't have bright_white directly, use base
    default: return fmt::color::white; // Default to white
    }
}

void ConsoleInterface::setup_ansi_for_windows() {
#ifdef _WIN32
    m_console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (m_console_handle == INVALID_HANDLE_VALUE) {
        // Handle error, maybe log it
        return;
    }
    // Get the current output mode
    if (!GetConsoleMode(m_console_handle, &m_original_out_mode)) {
        // Handle error
        return;
    }
    // Set the new output mode with ENABLE_VIRTUAL_TERMINAL_PROCESSING
    DWORD dwMode = m_original_out_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(m_console_handle, dwMode)) {
        // Handle error
        // Fallback or disable ANSI support if setting mode fails
        m_ansi_supported = false;
    }
    else {
        m_ansi_supported = true; // Successfully enabled ANSI
    }
#endif
}

void ConsoleInterface::initialize() {
    if (m_initialized) return;

#ifdef _WIN32
    // --- START OF ADDITION ---
    // Set console code page to UTF-8 for Windows
    // Redirect output to nul to prevent "Active code page: 65001" message
    if (system("chcp 65001 > nul") != 0) {
        // Optionally log an error if setting code page fails
        // print_warning(CONSOLE_LOG_ARGS, "Failed to set console code page to UTF-8.");
    }
    // --- END OF ADDITION ---

    setup_ansi_for_windows();
#else
    // Save original terminal settings for non-Windows
    if (tcgetattr(STDIN_FILENO, &m_original_termios) != 0) {
        // Handle error, unable to get terminal attributes
        m_ansi_supported = false; // Assume ANSI not supported if can't get termios
    }
    else {
        m_ansi_supported = is_terminal_capable();
    }
#endif

    m_initialized = true;

    // Welcome animation only if ANSI is supported and it's the first time initializing
    if (m_ansi_supported) {
        /*clear_screen();
        print_header("Console Interface Initialized", Color::BRIGHT_GREEN);
        println();*/
    }
}

void ConsoleInterface::cleanup() {
    if (!m_initialized) return;

#ifdef _WIN32
    if (m_console_handle) {
        SetConsoleMode(m_console_handle, m_original_out_mode); // Restore original mode
    }
#else
    tcsetattr(STDIN_FILENO, TCSANOW, &m_original_termios); // Restore original termios settings
#endif

    show_cursor();
    m_initialized = false;
}

bool ConsoleInterface::is_initialized() {
    return m_initialized;
}

void ConsoleInterface::print(const std::string& message, Color color) {
    if (!m_initialized) initialize();

    if (m_ansi_supported) {
        fmt::print(fmt::fg(to_fmt_color(color)), "{}", message);
    }
    else {
        std::cout << message;
    }
    std::cout.flush(); // Always flush for immediate output
}

void ConsoleInterface::println(const std::string& message, Color color) {
    print(message + "\n", color);
}

void ConsoleInterface::print_colored(const std::string& message, Color fg, Color bg) {
    if (!m_initialized) initialize();

    std::string safe_message = is_valid_utf8(message) ? message : sanitize_utf8(message);

    if (m_ansi_supported) {
        fmt::print(fmt::fg(to_fmt_color(fg)) | fmt::bg(to_fmt_color(bg)), "{}", safe_message);
    }
    else {
        std::cout << message;
    }
    std::cout.flush();
}

std::string ConsoleInterface::get_filename(const std::string& filepath) {
    size_t pos = filepath.find_last_of("/\\");
    return (pos == std::string::npos) ? filepath : filepath.substr(pos + 1);
}

std::string ConsoleInterface::format_file_info(const char* file, int line, const char* function) {
    return fmt::format("{}:{}:{}", get_filename(file), line, function);
}

// Unified logging function template as a private static member function
template<fmt::color TextColor, fmt::color BoxColor, fmt::color TagColor, fmt::color FileInfoColor>
void ConsoleInterface::log_message_template(const char* file, int line, const char* function, const std::string& message, const std::string& tag, const std::string& icon) {
    ConsoleInterface::initialize(); // Ensure initialized

    std::string file_info = ConsoleInterface::format_file_info(file, line, function);
    auto safe_message = is_valid_utf8(message) ? message : sanitize_utf8(message);

    if (ConsoleInterface::m_ansi_supported) {
        // Corrected fmt::print arguments for clarity and to match the definition
        fmt::print(stderr, "{}{}{}{}{}{}{}{}{}\n",
            fmt::styled(icon, fmt::fg(BoxColor)),
            fmt::styled("[", fmt::fg(TagColor)),
            fmt::styled(tag, fmt::fg(TagColor) | fmt::emphasis::bold),
            fmt::styled("]", fmt::fg(TagColor)),
            fmt::styled("[", fmt::fg(FileInfoColor)),
            fmt::styled(file_info, fmt::fg(FileInfoColor)),
            fmt::styled("]", fmt::fg(FileInfoColor)),
            fmt::styled(" ", fmt::fg(TextColor)), // Space before message
            fmt::styled(safe_message, fmt::fg(TextColor))
        );
    }
    else {
        fmt::print(stderr, "[{}] {}: {}\n", tag, file_info, safe_message);
    }
    // TODO: Add file logging here. You will need a static std::ofstream and std::mutex.
    // For now, I'll just keep the console output.
    // ConsoleInterface::log_to_file(fmt::format("[{}] {}: {}", tag, file_info, message));
}

void ConsoleInterface::p_info(const char* file, int line, const char* function, const std::string& message) {
    log_message_template<fmt::color::white, fmt::color::cyan, fmt::color::light_blue, fmt::color::dark_gray>(
        file, line, function, message, "INFO", "[ℹ️]"
    );
}

void ConsoleInterface::p_warning(const char* file, int line, const char* function, const std::string& message) {
    log_message_template<fmt::color::yellow, fmt::color::yellow, fmt::color::orange, fmt::color::dark_gray>(
        file, line, function, message, "WARN", "[⚠️]"
    );
}
void ConsoleInterface::p_success(const char* file, int line, const char* function, const std::string& message) {
    log_message_template<fmt::color::light_green, fmt::color::green, fmt::color::lime_green, fmt::color::dark_gray>(
        file, line, function, message, "SUCCESS", "[✔️]" 
    );
}
void ConsoleInterface::p_debug(const char* file, int line, const char* function, const std::string& message) {
    log_message_template<fmt::color::light_golden_rod_yellow, fmt::color::magenta, fmt::color::purple, fmt::color::dark_gray>(
        file, line, function, message, "DEBUG", "[⚙️]"
    );
    write_log("logs/debug.log", "{}:{}:{} | {}", get_filename(file), line, function, message);
}
void ConsoleInterface::p_error(const char* file, int line, const char* function, const std::string& message) {
    log_message_template<fmt::color::red, fmt::color::red, fmt::color::crimson, fmt::color::dark_gray>(
        file, line, function, message, "ERROR", "[❌]"
    );
    write_log("logs/error.log", "{}:{}:{} | {}", get_filename(file), line, function, message);
}


std::string ConsoleInterface::input(const std::string& prompt, Color prompt_color) {
    if (!m_initialized) initialize();

    if (!prompt.empty()) {
        if (m_ansi_supported) {
            // Corrected fmt::print to provide arguments for the format string
            fmt::print("{} {} {} ",
                fmt::styled("▶", fmt::fg(fmt::color::cyan)), // Icon
                fmt::styled(prompt, fmt::fg(to_fmt_color(prompt_color))),
                fmt::styled("", fmt::fg(fmt::color::white)) // Reset color for input
            );
        }
        else {
            std::cout << prompt << " ";
        }
    }

    std::string input_text;
    std::getline(std::cin, input_text);
    return input_text;
}

std::string ConsoleInterface::input_password(const std::string& prompt, char mask) {
    if (!m_initialized) initialize();

    if (m_ansi_supported) {
        // Corrected fmt::print to provide arguments for the format string
        fmt::print("{} {}{} ",
            fmt::styled("🔒", fmt::fg(fmt::color::yellow)), // Icon
            fmt::styled(prompt, fmt::fg(fmt::color::yellow)),
            fmt::styled("", fmt::fg(fmt::color::white)) // Reset color
        );
    }
    else {
        std::cout << prompt << " ";
    }

    std::string password;
    char ch;

    hide_cursor();
    while (true) {
        ch = getch_cross_platform();
        if (ch == '\n' || ch == '\r') {
            break;
        }
        else if (ch == '\b' || ch == 127) { // Backspace or Delete
            if (!password.empty()) {
                password.pop_back();
                // Erase character from console
                std::cout << "\b \b" << std::flush;
            }
        }
        else if (ch >= 32 && ch <= 126) { // Printable ASCII characters
            password += ch;
            std::cout << mask << std::flush;
        }
    }
    show_cursor();
    std::cout << std::endl; // Newline after input

    return password;
}

int ConsoleInterface::input_int(const std::string& prompt, int min_val, int max_val) {
    int value;
    std::string input_str;

    while (true) {
        input_str = input(prompt, Color::CYAN);

        // Check for empty input if that's an error condition
        if (input_str.empty()) {
            p_error(CONSOLE_LOG_ARGS, "Input cannot be empty. Please enter a number.");
            continue;
        }

        try {
            size_t pos;
            value = std::stoi(input_str, &pos);
            // Check if entire string was converted
            if (pos != input_str.length()) {
                p_error(CONSOLE_LOG_ARGS, "Invalid characters found after number. Please enter a valid integer.");
                continue;
            }

            if (value >= min_val && value <= max_val) {
                break;
            }
            else {
                p_error(CONSOLE_LOG_ARGS, fmt::format("Value must be between {} and {}", min_val, max_val));
            }
        }
        catch (const std::out_of_range& e) {
            p_error(CONSOLE_LOG_ARGS, fmt::format("Number out of integer range. ({})", e.what()));
        }
        catch (const std::invalid_argument& e) {
            p_error(CONSOLE_LOG_ARGS, fmt::format("Invalid number format. ({})", e.what()));
        }
    }

    return value;
}

float ConsoleInterface::input_float(const std::string& prompt) {
    float value;
    std::string input_str;

    while (true) {
        input_str = input(prompt, Color::CYAN);

        if (input_str.empty()) {
            p_error(CONSOLE_LOG_ARGS, "Input cannot be empty. Please enter a number.");
            continue;
        }

        try {
            size_t pos;
            value = std::stof(input_str, &pos);
            if (pos != input_str.length()) {
                p_error(CONSOLE_LOG_ARGS, "Invalid characters found after number. Please enter a valid float.");
                continue;
            }
            break;
        }
        catch (const std::out_of_range& e) {
            p_error(CONSOLE_LOG_ARGS, fmt::format("Number out of float range. ({})", e.what()));
        }
        catch (const std::invalid_argument& e) {
            p_error(CONSOLE_LOG_ARGS, fmt::format("Invalid number format. ({})", e.what()));
        }
    }

    return value;
}

bool ConsoleInterface::input_bool(const std::string& prompt) {
    std::string input_str;

    while (true) {
        input_str = input(prompt, Color::CYAN);
        std::transform(input_str.begin(), input_str.end(), input_str.begin(), ::tolower);

        if (input_str == "y" || input_str == "yes" || input_str == "1" || input_str == "true") {
            return true;
        }
        else if (input_str == "n" || input_str == "no" || input_str == "0" || input_str == "false") {
            return false;
        }
        else {
            p_error(CONSOLE_LOG_ARGS, "Please enter y/n, yes/no, 1/0, or true/false");
        }
    }
}

int ConsoleInterface::show_menu(const std::string& title, const std::vector<std::string>& options, bool numbered) {
    if (!m_initialized) initialize();

    if (options.empty()) {
        p_warning(CONSOLE_LOG_ARGS, "Menu options are empty.");
        return -1; // Indicate no selection possible
    }

    print_header(title, Color::BRIGHT_CYAN);
    println();

    for (size_t i = 0; i < options.size(); ++i) {
        if (numbered) {
            if (m_ansi_supported) {
                // Corrected fmt::print to ensure proper arguments for styled calls
                fmt::print("{}{}{}{} {}\n",
                    fmt::styled("[", fmt::fg(fmt::color::cyan)),
                    fmt::styled(std::to_string(i + 1), fmt::fg(fmt::color::light_cyan) | fmt::emphasis::bold),
                    fmt::styled("]", fmt::fg(fmt::color::cyan)),
                    fmt::styled(" ", fmt::fg(fmt::color::white)), // Space after bracket
                    fmt::styled(options[i], fmt::fg(fmt::color::white))
                );
            }
            else {
                fmt::print("[{}] {}\n", i + 1, options[i]);
            }
        }
        else {
            if (m_ansi_supported) {
                // Corrected fmt::print to ensure proper arguments for styled calls
                fmt::print("{} {}\n",
                    fmt::styled("▶", fmt::fg(fmt::color::cyan)), // Icon
                    fmt::styled(options[i], fmt::fg(fmt::color::white))
                );
            }
            else {
                fmt::print("* {}\n", options[i]);
            }
        }
    }

    println();
    if (numbered) {
        return input_int("Select option: ", 1, static_cast<int>(options.size())) - 1;
    }
    else {
        pause("Press Enter to continue..."); // Use pause for non-numbered menu
        return -1; // No selection made for non-numbered menu
    }
}

void ConsoleInterface::print_box(const std::string& content, int width, Color border_color) {
    if (!m_initialized) initialize();

    auto terminal_size = get_terminal_size();
    if (width <= 0 || width > terminal_size.first) {
        width = min(80, terminal_size.first - 4); // Max 80, or terminal width - 4 for padding
    }
    width = max(10, width); // Minimum width to prevent odd behavior

    std::vector<std::string> lines;
    std::istringstream stream(content);
    std::string line;

    // Word wrapping logic
    while (std::getline(stream, line)) {
        if (line.empty()) {
            lines.push_back(""); // Preserve empty lines
            continue;
        }
        size_t current_pos = 0;
        while (current_pos < line.length()) {
            size_t wrap_width = width - 4; // Space for borders and padding
            if (line.length() - current_pos <= wrap_width) {
                lines.push_back(line.substr(current_pos));
                current_pos = line.length();
            }
            else {
                size_t break_pos = line.rfind(' ', current_pos + wrap_width);
                if (break_pos == std::string::npos || break_pos < current_pos) {
                    // No space found, force break
                    lines.push_back(line.substr(current_pos, wrap_width));
                    current_pos += wrap_width;
                }
                else {
                    lines.push_back(line.substr(current_pos, break_pos - current_pos));
                    current_pos = break_pos + 1; // Move past the space
                }
            }
        }
    }

    // Adjust width for border characters
    int inner_width = width - 2;

    if (m_ansi_supported) {
        fmt::color fmt_border_color = to_fmt_color(border_color);

        // Top border - Added ConsoleInterface::BoxChars:: scope
        fmt::print("{}{}{}{}\n",
            fmt::styled(ConsoleInterface::BoxChars::TOP_LEFT, fmt::fg(fmt_border_color)),
            fmt::styled(std::string(inner_width, *ConsoleInterface::BoxChars::HORIZONTAL), fmt::fg(fmt_border_color)),
            fmt::styled(ConsoleInterface::BoxChars::TOP_RIGHT, fmt::fg(fmt_border_color)),
            fmt::styled("", fmt::fg(fmt::color::white)) // Reset after border
        );

        // Content - Added ConsoleInterface::BoxChars:: scope
        for (const auto& content_line : lines) {
            fmt::print("{} {:<{}} {}\n",
                fmt::styled(ConsoleInterface::BoxChars::VERTICAL, fmt::fg(fmt_border_color)),
                content_line,
                width - 4, // Inner content width (total - borders - padding)
                fmt::styled(ConsoleInterface::BoxChars::VERTICAL, fmt::fg(fmt_border_color))
            );
        }

        // Bottom border - Added ConsoleInterface::BoxChars:: scope and consistent color
        fmt::print("{}{}{}{}\n",
            fmt::styled(ConsoleInterface::BoxChars::BOTTOM_LEFT, fmt::fg(fmt_border_color)),
            fmt::styled(std::string(inner_width, *ConsoleInterface::BoxChars::HORIZONTAL), fmt::fg(fmt_border_color)),
            fmt::styled(ConsoleInterface::BoxChars::BOTTOM_RIGHT, fmt::fg(fmt_border_color)),
            fmt::styled("", fmt::fg(fmt::color::white)) // Reset after border
        );
    }
    else {
        // Fallback ASCII box
        fmt::print("+{}+\n", std::string(inner_width, '-'));
        for (const auto& content_line : lines) {
            fmt::print("| {:<{}} |\n", content_line, width - 4);
        }
        fmt::print("+{}+\n", std::string(inner_width, '-'));
    }
}


void ConsoleInterface::print_separator(int width, Color color) {
    if (!m_initialized) initialize();

    if (width <= 0) width = get_terminal_size().first;
    width = max(5, width); // Minimum width

    if (m_ansi_supported) {
        // Added ConsoleInterface::BoxChars:: scope and dereference for single char
        fmt::print("{}\n",
            fmt::styled(std::string(width, *ConsoleInterface::BoxChars::HORIZONTAL), fmt::fg(to_fmt_color(color)))
        );
    }
    else {
        fmt::print("{}\n", std::string(width, '-'));
    }
}

void ConsoleInterface::print_header(const std::string& title, Color color) {
    if (!m_initialized) initialize();

    int width = get_terminal_size().first;
    if (width < static_cast<int>(title.length()) + 6) { // Ensure enough space for title + padding + borders
        width = static_cast<int>(title.length()) + 6;
    }

    int padding = (width - static_cast<int>(title.length()) - 4) / 2; // -4 for " | " and " | "
    if (padding < 0) padding = 0; // Should not happen with min width check

    int remaining_padding = width - static_cast<int>(title.length()) - (padding * 2) - 4;
    if (remaining_padding < 0) remaining_padding = 0; // Adjust if odd width

    std::string left_padding_str(padding, ' ');
    std::string right_padding_str(padding + remaining_padding, ' ');

    if (m_ansi_supported) {
        fmt::color fmt_color = to_fmt_color(color);
        int inner_width = width - 2;

        // Added ConsoleInterface::BoxChars:: scope and dereference for single char
        fmt::print("{}{}{}{}\n",
            fmt::styled(ConsoleInterface::BoxChars::TOP_LEFT, fmt::fg(fmt_color)),
            fmt::styled(std::string(inner_width, *ConsoleInterface::BoxChars::HORIZONTAL), fmt::fg(fmt_color)),
            fmt::styled(ConsoleInterface::BoxChars::TOP_RIGHT, fmt::fg(fmt_color)),
            fmt::styled("", fmt::fg(fmt::color::white))
        );

        // Added ConsoleInterface::BoxChars:: scope
        fmt::print("{} {}{}{} {}\n",
            fmt::styled(ConsoleInterface::BoxChars::VERTICAL, fmt::fg(fmt_color)),
            left_padding_str,
            fmt::styled(title, fmt::fg(fmt_color) | fmt::emphasis::bold),
            right_padding_str,
            fmt::styled(ConsoleInterface::BoxChars::VERTICAL, fmt::fg(fmt_color))
        );

        // Added ConsoleInterface::BoxChars:: scope and dereference for single char, consistent color
        fmt::print("{}{}{}{}\n",
            fmt::styled(ConsoleInterface::BoxChars::BOTTOM_LEFT, fmt::fg(fmt_color)),
            fmt::styled(std::string(inner_width, *ConsoleInterface::BoxChars::HORIZONTAL), fmt::fg(fmt_color)),
            fmt::styled(ConsoleInterface::BoxChars::BOTTOM_RIGHT, fmt::fg(fmt_color)),
            fmt::styled("", fmt::fg(fmt::color::white)) // Reset after border
        );
    }
    else {
        fmt::print("+{}+\n", std::string(width - 2, '-'));
        fmt::print("| {}{}{} |\n", left_padding_str, title, right_padding_str);
        fmt::print("+{}+\n", std::string(width - 2, '-'));
    }
}

void ConsoleInterface::print_progress_bar(float progress, int width, Color filled_color, Color empty_color) {
    if (!m_initialized) initialize();

    progress = max(0.0f, min(1.0f, progress));
    int filled = static_cast<int>(progress * width);
    int empty = width - filled;

    if (m_ansi_supported) {
        fmt::print("[{}{}] {:.1f}%\r",
            fmt::styled(std::string(filled, '█'), fmt::fg(to_fmt_color(filled_color))),
            fmt::styled(std::string(empty, '░'), fmt::fg(to_fmt_color(empty_color))),
            progress * 100.0f
        );
    }
    else {
        fmt::print("[{}{}] {:.1f}%\r",
            std::string(filled, '='),
            std::string(empty, '-'),
            progress * 100.0f
        );
    }
    std::cout.flush();
}

void ConsoleInterface::show_loading(const std::string& message, int duration_ms, Color color) {
    if (!m_initialized) initialize();

    std::vector<std::string> spinner = { "⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏" };
    int spinner_idx = 0;
    auto start_time = std::chrono::high_resolution_clock::now();
    long long elapsed_ms = 0;

    hide_cursor();
    while (elapsed_ms < duration_ms) {
        clear_line();
        if (m_ansi_supported) {
            // Corrected fmt::print for styled arguments
            fmt::print("{}{} {}{}\r",
                fmt::styled(spinner[spinner_idx], fmt::fg(to_fmt_color(color))),
                fmt::styled(" ", fmt::fg(to_fmt_color(color))), // Space
                fmt::styled(message, fmt::fg(to_fmt_color(color))),
                fmt::styled("...", fmt::fg(to_fmt_color(color)))
            );
        }
        else {
            fmt::print("Loading {} ...\r", spinner[spinner_idx % 4]); // Fallback spinner (only 4 chars if spinner is smaller)
        }
        std::cout.flush();

        spinner_idx = (spinner_idx + 1) % spinner.size();
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Update every 100ms
        elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - start_time
        ).count();
    }
    clear_line(); // Clear the loading message
    show_cursor();
}
void ConsoleInterface::show_loading(const std::string& message, std::function<void()> callback, Color color) {
    if (!m_initialized) initialize();

    std::vector<std::string> spinner = { "⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏" };
    int spinner_idx = 0;
    std::atomic<bool> done{ false };

    hide_cursor();

    // Jalankan callback di thread terpisah
    std::thread([&] {
        while (!done) {
            clear_line();
            if (m_ansi_supported) {
                fmt::print("{}{} {}{}\r",
                    fmt::styled(spinner[spinner_idx], fmt::fg(to_fmt_color(color))),
                    fmt::styled(" ", fmt::fg(to_fmt_color(color))),
                    fmt::styled(message, fmt::fg(to_fmt_color(color))),
                    fmt::styled("...", fmt::fg(to_fmt_color(color)))
                );
            }
            else {
                fmt::print("Loading {} ...\r", spinner[spinner_idx]);
            }

            spinner_idx = spinner_idx >= spinner.size() - 1 ? 0 : spinner_idx + 1;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
		}).detach();

    callback();
    done = true;
    clear_line();
    show_cursor();
}

void ConsoleInterface::typewriter_effect(const std::string& text, int delay_ms, Color color) {
    if (!m_initialized) initialize();
    hide_cursor();
    for (char c : text) {
        print(std::string(1, c), color);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    }
    println();
    show_cursor();
}

void ConsoleInterface::rainbow_text(const std::string& text) {
    if (!m_initialized) initialize();

    // Defined fmt::color values for a rainbow effect (more granular than our enum)
    std::vector<fmt::color> rainbow_colors = {
        fmt::color::red,
        fmt::color::orange,
        fmt::color::yellow,
        fmt::color::green,
        fmt::color::blue,
        fmt::color::indigo, // closest to fmt's options
        fmt::color::purple
    };

    if (m_ansi_supported) {
        for (size_t i = 0; i < text.length(); ++i) {
            fmt::print(fmt::fg(rainbow_colors[i % rainbow_colors.size()]), "{}", text[i]);
        }
        fmt::print("\n"); // Newline after the rainbow text
    }
    else {
        // Fallback to white if ANSI not supported
        println(text, Color::WHITE);
    }
}

void ConsoleInterface::clear_screen() {
    if (!m_initialized) initialize();
#ifdef _WIN32
    // Use ANSI escape codes on Windows if supported, otherwise fallback to system("cls")
    if (m_ansi_supported) {
        fmt::print("\033[2J\033[H");
    }
    else {
        system("cls");
    }
#else
    // Using ANSI escape code for cross-platform clear without `system()`
    // This is generally preferred for performance and security.
    fmt::print("\033[2J\033[H");
#endif
    std::cout.flush();
}

void ConsoleInterface::clear_line() {
    if (!m_initialized) initialize();
    // ANSI escape code to clear line from cursor to end and move cursor to start of line
    fmt::print("\r\033[K");
    std::cout.flush();
}

void ConsoleInterface::move_cursor(int x, int y) {
    if (!m_initialized) initialize();
    fmt::print("\033[{};{}H", y, x);
    std::cout.flush();
}

void ConsoleInterface::hide_cursor() {
    if (!m_initialized) initialize();
    fmt::print("\033[?25l");
    std::cout.flush();
}

void ConsoleInterface::show_cursor() {
    if (!m_initialized) initialize();
    fmt::print("\033[?25h");
    std::cout.flush();
}

std::pair<int, int> ConsoleInterface::get_terminal_size() {
    if (!m_initialized) initialize();
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        int columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        int rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        return { columns, rows };
    }
    return { 80, 25 }; // Default fallback
#else
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        return { w.ws_col, w.ws_row };
    }
    return { 80, 25 }; // Default fallback
#endif
}

void ConsoleInterface::pause(const std::string& message, Color color) {
    if (!m_initialized) initialize();
    if (m_ansi_supported) {
        // Corrected fmt::print for styled arguments
        fmt::print("{} {} {}\n",
            fmt::styled("⏸", fmt::fg(to_fmt_color(color))),
            fmt::styled(" ", fmt::fg(to_fmt_color(color))), // Space
            fmt::styled(message, fmt::fg(to_fmt_color(color)))
        );
    }
    else {
        std::cout << message << std::endl;
    }
    getch_cross_platform();
}

bool ConsoleInterface::is_terminal_capable() {
#ifdef _WIN32
    // On Windows, capability is determined by whether ENABLE_VIRTUAL_TERMINAL_PROCESSING is successfully enabled.
    // This is handled in setup_ansi_for_windows(). So this function simply returns m_ansi_supported.
    return m_ansi_supported; // This will be set by setup_ansi_for_windows
#else
    // For non-Windows, assume ANSI is supported if stdout is a TTY
    return isatty(STDOUT_FILENO);
#endif
}

char ConsoleInterface::getch_cross_platform() {
#ifdef _WIN32
    return _getch();
#else
    struct termios oldt, newt;
    char ch;
    // Save current terminal settings
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    // Disable canonical mode (line buffering) and echo
    newt.c_lflag &= ~(ICANON | ECHO);
    // Apply new settings immediately
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar(); // Read single character
    // Restore original terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
#endif
}

bool ConsoleInterface::is_valid_utf8(const std::string& str) {
    size_t i = 0;
    const size_t len = str.length();

    while (i < len) {
        unsigned char c = static_cast<unsigned char>(str[i]);
        if (c <= 0x7F) {
            i += 1;
        }
        else if ((c >> 5) == 0x6) {
            if (i + 1 >= len || (static_cast<unsigned char>(str[i + 1]) >> 6) != 0x2) return false;
            i += 2;
        }
        else if ((c >> 4) == 0xE) {
            if (i + 2 >= len ||
                (static_cast<unsigned char>(str[i + 1]) >> 6) != 0x2 ||
                (static_cast<unsigned char>(str[i + 2]) >> 6) != 0x2) return false;
            i += 3;
        }
        else if ((c >> 3) == 0x1E) {
            if (i + 3 >= len ||
                (static_cast<unsigned char>(str[i + 1]) >> 6) != 0x2 ||
                (static_cast<unsigned char>(str[i + 2]) >> 6) != 0x2 ||
                (static_cast<unsigned char>(str[i + 3]) >> 6) != 0x2) return false;
            i += 4;
        }
        else {
            return false;
        }
    }
    return true;
}

std::string ConsoleInterface::sanitize_utf8(const std::string& input) {
    std::string output;
    size_t i = 0;
    const size_t len = input.length();

    while (i < len) {
        unsigned char c = static_cast<unsigned char>(input[i]);
        if (c <= 0x7F) {
            output += c;
            ++i;
        }
        else if ((c >> 5) == 0x6 && i + 1 < len) {
            output += input.substr(i, 2);
            i += 2;
        }
        else if ((c >> 4) == 0xE && i + 2 < len) {
            output += input.substr(i, 3);
            i += 3;
        }
        else if ((c >> 3) == 0x1E && i + 3 < len) {
            output += input.substr(i, 4);
            i += 4;
        }
        else {
            // Skip invalid byte
            ++i;
        }
    }
    return output;
}
void ConsoleInterface::write_log_file(const std::string& filepath, const std::string& content) {
    try {
        // Buat string timestamp
        auto now = std::chrono::system_clock::now();
        auto now_time = std::chrono::system_clock::to_time_t(now);
        std::string timestamp = std::string(std::ctime(&now_time));
        timestamp.pop_back(); // Remove trailing newline

        std::string log_line = fmt::format("[{}] {}\n", timestamp, content);
        FileSystem2::writeFile(filepath, log_line, true);
    }
    catch (const std::exception& e) {
        // fallback ke stderr
        print_error("[LOGGING ERROR] {}\n", e.what());
    }
}
