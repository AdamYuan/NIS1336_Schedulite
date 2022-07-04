#include "TaskFlowBox.hpp"

namespace gui {

TaskFlowBox::TaskFlowBox() { initialize(); }
TaskFlowBox::TaskFlowBox(const std::shared_ptr<backend::Schedule> &schedule_ptr) {
	initialize();
	UpdateSchedule(schedule_ptr);
}

void TaskFlowBox::initialize() {
	set_selection_mode(Gtk::SELECTION_NONE);
	set_column_spacing(16);
	set_row_spacing(16);
	signal_child_activated().connect([this](Gtk::FlowBoxChild *child) {
		auto task_child = (TaskFlowBoxChild *)child;
		if (task_child) {
			printf("select: %s\n", task_child->m_task.property.name.c_str());
		}
	});
}

void TaskFlowBox::UpdateSchedule(const std::shared_ptr<backend::Schedule> &schedule_ptr) {
	m_schedule_ptr = schedule_ptr;
	for (auto *widget : get_children())
		remove(*widget);
	for (const auto &task : m_schedule_ptr->GetTasks()) {
		auto row = Gtk::make_managed<TaskFlowBoxChild>(task);
		m_rows[task.id] = row;
		insert(*row, -1);
		row->show();
	}
}

} // namespace gui