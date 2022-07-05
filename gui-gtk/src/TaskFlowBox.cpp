#include "TaskFlowBox.hpp"

namespace gui {

TaskFlowBox::TaskFlowBox() { initialize(); }
TaskFlowBox::TaskFlowBox(const std::shared_ptr<backend::Schedule> &schedule_ptr) {
	initialize();
	set_schedule(schedule_ptr);
}

void TaskFlowBox::initialize() {
	set_selection_mode(Gtk::SELECTION_NONE);
	set_column_spacing(16);
	set_row_spacing(16);
	set_border_width(16);
	set_homogeneous(true);
	set_max_children_per_line(3);
}

void TaskFlowBox::set_schedule(const std::shared_ptr<backend::Schedule> &schedule_ptr) {
	m_schedule_ptr = schedule_ptr;
	for (auto *widget : get_children())
		remove(*widget);
	m_rows.clear();
	for (const auto &task : m_schedule_ptr->GetTasks()) {
		auto row = Gtk::make_managed<TaskFlowBoxChild>(task, m_signal_task_selected);
		m_rows[task.id] = row;
		insert(*row, -1);
		row->show();
	}
}

} // namespace gui