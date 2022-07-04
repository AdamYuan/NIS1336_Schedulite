#ifndef SCHEDULITE_GTK_TASKLISTBOX_HPP
#define SCHEDULITE_GTK_TASKLISTBOX_HPP

#include <gtkmm.h>

#include <backend/Schedule.hpp>

#include "TaskListBoxRow.hpp"

namespace gui {

class TaskListBox : public Gtk::ListBox {
public:
	TaskListBox();
	explicit TaskListBox(const std::shared_ptr<backend::Schedule> &schedule_ptr);
	~TaskListBox() override = default;

	void UpdateSchedule(const std::shared_ptr<backend::Schedule> &schedule_ptr);

	const std::shared_ptr<backend::Schedule> &GetSchedulePtr() const { return m_schedule_ptr; }

private:
	std::shared_ptr<backend::Schedule> m_schedule_ptr;
	std::vector<TaskListBoxRow> m_rows;

	void initialize();
};

} // namespace gui

#endif
