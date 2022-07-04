#ifndef SCHEDULITE_GTK_TASKLISTBOXROW_HPP
#define SCHEDULITE_GTK_TASKLISTBOXROW_HPP

#include <gtkmm.h>

#include <backend/Task.hpp>

namespace gui {
class TaskListBoxRow : public Gtk::ListBoxRow {
public:
	explicit TaskListBoxRow(backend::Task task);
	inline const backend::Task &GetTask() const { return m_task; }
	~TaskListBoxRow() override = default;

private:
	backend::Task m_task;
	Gtk::Label m_name_label, m_begin_time_label, m_remind_time_label, m_priority_label, m_type_label;
	Gtk::Image m_begin_time_icon, m_remind_time_icon, m_priority_icon, m_type_icon;
	Gtk::ToggleButton m_edit_button;
	Gtk::ToolButton m_done_button;
	Gtk::Box m_main_box, m_box_0, m_box_1, m_hbox_2, m_priority_box, m_type_box, m_begin_time_box, m_remind_time_box;

	void initialize();
	void update();

	friend class TaskListBox;
};
} // namespace gui

#endif
