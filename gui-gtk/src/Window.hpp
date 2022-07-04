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

	Gtk::MenuButton m_user_button;
	Gtk::Button m_insert_button;

	Gtk::HeaderBar m_header_bar;
	Gtk::ScrolledWindow m_scrolled_window;
	TaskFlowBox m_task_list_box;
};

} // namespace gui

#endif
