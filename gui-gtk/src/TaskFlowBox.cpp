#include "TaskFlowBox.hpp"

namespace gui {

TaskFlowBox::TaskFlowBox() { initialize(); }
TaskFlowBox::TaskFlowBox(const std::shared_ptr<backend::Schedule> &schedule_ptr) {
	initialize();
	UpdateSchedule(schedule_ptr);
}

void TaskFlowBox::initialize() {
	/* signal_row_selected().connect([this](Gtk::ListBoxRow *row) {
	    auto task_row = (TaskFlowBoxChild *)row;
	    if (task_row) {
	        printf("select: %s\n", task_row->m_task.property.name.c_str());
	    }
	}); */
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