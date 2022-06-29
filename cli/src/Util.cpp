#include <cli/Util.hpp>

// From https://stackoverflow.com/questions/1413445/reading-a-password-from-stdcin
#ifdef WIN32
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif
static void SetStdinEcho(bool enable = true) {
#ifdef WIN32
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode;
	GetConsoleMode(hStdin, &mode);
	if (!enable)
		mode &= ~ENABLE_ECHO_INPUT;
	else
		mode |= ENABLE_ECHO_INPUT;
	SetConsoleMode(hStdin, mode);
#else
	termios tty{};
	tcgetattr(STDIN_FILENO, &tty);
	if (!enable)
		tty.c_lflag &= ~ECHO;
	else
		tty.c_lflag |= ECHO;
	(void)tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif
}

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>
#elif defined(__linux__)
#include <sys/ioctl.h>
#endif // Windows/Linux

static uint32_t GetTerminalWidth() {
#if defined(_WIN32)
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	return (uint32_t)(csbi.srWindow.Right - csbi.srWindow.Left + 1);
#elif defined(__linux__)
	winsize w{};
	ioctl(fileno(stdout), TIOCGWINSZ, &w);
	return (uint32_t)(w.ws_col);
#endif // Windows/Linux
}

namespace cli {

std::string Input(const char *prompt, bool echo) {
	printf("%s: ", prompt);
	std::string ret;
	if (!echo)
		SetStdinEcho(false);
	std::getline(std::cin, ret);
	if (!echo) {
		SetStdinEcho(true);
		putchar('\n');
	}
	return ret;
}

uint32_t GetTerminalWidth() { return ::GetTerminalWidth(); }

} // namespace cli
