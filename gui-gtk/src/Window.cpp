#include "Window.hpp"

namespace gui {

Window::Window() { initialize(); }

void Window::initialize() {
	set_title("Basic application");
	set_default_size(400, 400);

	m_header_bar.set_show_close_button(TRUE);
	m_header_bar.set_title("Schedulite");
	set_titlebar(m_header_bar);

	add(m_button);

	show_all();
}

} // namespace gui
