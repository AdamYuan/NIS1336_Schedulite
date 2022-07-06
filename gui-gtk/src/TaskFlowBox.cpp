#include "TaskFlowBox.hpp"

namespace gui {

TaskFlowBox::TaskFlowBox() { initialize(); }

void TaskFlowBox::initialize() {
	set_selection_mode(Gtk::SELECTION_NONE);
	set_column_spacing(16);
	set_row_spacing(16);
	set_border_width(16);
	set_homogeneous(true);
	set_max_children_per_line(3);
}

void TaskFlowBox::set_tasks(const std::vector<backend::Task> &tasks) {
	std::unordered_map<uint32_t, TaskFlowBoxChild *> update_set{}, erase_set = std::move(m_children);
	for (const auto &task : tasks) {
		auto it = erase_set.find(task.id);
		if (it != erase_set.end()) {
			// The task ID already exists, modify it
			update_set.insert(*it);
			if (it->second->get_task() != task)
				it->second->set_task(task);
			erase_set.erase(task.id);
		} else {
			// Not exist, insert it
			auto child = Gtk::make_managed<TaskFlowBoxChild>(task, m_signal_task_selected);
			update_set.insert({task.id, child});
			Gtk::FlowBox::insert(*child, -1);
			child->show();
		}
	}
	for (const auto &p : erase_set) {
		p.second->hide();
		Gtk::FlowBox::remove(*p.second);
	}
	m_children = std::move(update_set);
}

} // namespace gui