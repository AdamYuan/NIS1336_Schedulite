#ifndef SCHEDULITE_GTK_TASKINSERTBOX_HPP
#define SCHEDULITE_GTK_TASKINSERTBOX_HPP

#include <backend/Task.hpp>
#include <gtkmm.h>

#include "EnumSelectPopover.hpp"
#include "TimePopover.hpp"

namespace gui {
class TaskInsertBox : public Gtk::Box {
public:
	inline TaskInsertBox() { init_widget(); }
	~TaskInsertBox() override = default;

	void restore();
	backend::TaskProperty get_task_property();

private:
	Gtk::Grid *m_p_grid{};
	Gtk::Entry *m_p_task_name_entry{};
	Gtk::MenuButton *m_p_begin_time_button{}, *m_p_remind_time_button{}, *m_p_priority_button{}, *m_p_type_button{};
	Gtk::Button *m_p_ok_button{};
	Gtk::Label *m_p_priority_label{}, *m_p_type_label{};
	Gtk::Image *m_p_priority_icon{}, *m_p_type_icon{};
	TimePopover m_begin_time_popover{false}, m_remind_time_popover{false};
	EnumSelectPopover m_priority_popover{backend::GetTaskPriorityStrings()},
	    m_type_popover{backend::GetTaskTypeStrings()};
	void init_widget();
	void set_priority(backend::TaskPriority priority);
	void set_type(backend::TaskType type);

protected:
	sigc::signal<void(const backend::TaskProperty &)> m_signal_task_inserted;

public:
	decltype(m_signal_task_inserted) signal_task_inserted() { return m_signal_task_inserted; }
};
} // namespace gui

#endif
