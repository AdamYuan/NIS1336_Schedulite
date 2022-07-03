#include "TaskListBox.hpp"

namespace gui {

TaskListBox::TaskListBox(const std::shared_ptr<backend::Schedule> &schedule_ptr) { UpdateSchedule(schedule_ptr); }
void TaskListBox::UpdateSchedule(const std::shared_ptr<backend::Schedule> &schedule_ptr) {
	m_schedule_ptr = schedule_ptr;
	for (auto *widget : get_children())
		remove(*widget);
	for (const auto &task : m_schedule_ptr->GetTasks()) {
		auto row = Gtk::make_managed<TaskListBoxRow>(task);
		append(*row);
		row->show();
	}
}

} // namespace gui