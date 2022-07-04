#include "TaskListBox.hpp"

namespace gui {

TaskListBox::TaskListBox() { initialize(); }
TaskListBox::TaskListBox(const std::shared_ptr<backend::Schedule> &schedule_ptr) {
	initialize();
	UpdateSchedule(schedule_ptr);
}

void TaskListBox::initialize() {
	signal_row_selected().connect([this](Gtk::ListBoxRow *row) {
		auto task_row = (TaskListBoxRow *)row;
		if (task_row) {
			printf("select: %s\n", task_row->m_task.property.name.c_str());
		}
	});
}

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