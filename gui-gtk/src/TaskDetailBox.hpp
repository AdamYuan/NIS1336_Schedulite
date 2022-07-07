#ifndef SCHEDULITE_TASKDETAILBOX_HPP
#define SCHEDULITE_TASKDETAILBOX_HPP

#include <backend/Task.hpp>
#include <gtkmm.h>

#include "EnumSelectPopover.hpp"
#include "TimePopover.hpp"

namespace gui {
class TaskDetailBox : public Gtk::Box {
public:
	inline TaskDetailBox() { init_widget(); }
	~TaskDetailBox() override = default;

	void set_task(const backend::Task &task);
	const backend::Task &get_task() const { return m_task; }
	bool update_from_tasks(const std::vector<backend::Task> &tasks);
	void update_status();

private:
	backend::Task m_task;

	Gtk::Box *m_p_box{};
	Gtk::Label *m_p_name_label{}, *m_p_begin_time_label{}, *m_p_remind_time_label{}, *m_p_priority_label{},
	    *m_p_type_label{}, *m_p_status_label{};
	Gtk::Entry *m_p_name_entry{};
	Gtk::Image *m_p_priority_icon{}, *m_p_type_icon{}, *m_p_status_icon{};
	Gtk::Popover *m_p_name_popover{};
	Gtk::Button *m_p_done_button{}, *m_p_undone_button{}, *m_p_name_apply_button{};
	Gtk::MenuButton *m_p_erase_button{}, *m_p_edit_begin_time_button{}, *m_p_edit_remind_time_button{},
	    *m_p_edit_priority_button{}, *m_p_edit_type_button{};
	TimePopover m_begin_time_popover{true}, m_remind_time_popover{true};
	EnumSelectPopover m_priority_popover{backend::GetTaskPriorityStrings()},
	    m_type_popover{backend::GetTaskTypeStrings()};
	void init_widget();

protected:
	sigc::signal<void(uint32_t, const backend::TaskProperty &, backend::TaskPropertyMask)> m_signal_task_edited;

public:
	inline decltype(m_signal_task_edited) signal_task_edited() { return m_signal_task_edited; }
};
} // namespace gui

#endif
