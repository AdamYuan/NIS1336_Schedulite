#ifndef SCHEDULITE_GTK_WINDOW_HPP
#define SCHEDULITE_GTK_WINDOW_HPP

#include <gtkmm.h>

#include "TaskFlowBox.hpp"

#include <backend/Schedule.hpp>

namespace gui {

class Window : public Gtk::Window {
public:
	Window();
	~Window() override = default;

protected:
	std::shared_ptr<backend::Schedule> m_schedule_ptr;

	void initialize();

	Gtk::Popover *m_p_user_popover{nullptr};
	Gtk::Label *m_p_user_popover_label{nullptr};

	struct {
		Gtk::MenuButton user_button;
		Gtk::Button insert_button;
		Gtk::HeaderBar bar;
		Gtk::ButtonBox button_box;
		Gtk::ToggleButton pending_button, ongoing_button, done_button;
	} m_header;

	Gtk::ScrolledWindow m_scrolled_window;
	TaskFlowBox m_task_list_box;
};

} // namespace gui

#endif
