#ifndef SCHEDULITE_GTK_WINDOW_HPP
#define SCHEDULITE_GTK_WINDOW_HPP

#include <gtkmm.h>

namespace gui {

class Window : public Gtk::Window {
public:
	Window();
	~Window() override = default;

protected:
	void initialize();

	Gtk::Box m_layout_box;
	Gtk::HeaderBar m_header_bar;
	Gtk::ListBox m_tasks_list_box;
	Gtk::Button m_button;
};

} // namespace gui

#endif
