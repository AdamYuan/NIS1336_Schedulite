#include "TaskFlowBox.hpp"

namespace gui {

TaskFlowBox::TaskFlowBox() { init_widget(); }

void TaskFlowBox::init_widget() {
	set_selection_mode(Gtk::SELECTION_NONE);
	set_column_spacing(16);
	set_row_spacing(16);
	set_border_width(16);
	set_homogeneous(true);
	set_max_children_per_line(3);
}

void TaskFlowBox::refilter() {
	for (Gtk::Widget *child : get_children()) {
		const backend::TaskProperty &p = ((TaskFlowBoxChild *)child)->get_task().property;
		bool show = bool(m_priority_filter & (1 << (uint32_t)p.priority)) &&
		            bool(m_type_filter & (1 << (uint32_t)p.type)) &&
		            bool(m_status_filter & (1 << (uint32_t)backend::TaskStatusFromTask(p)));
		if (show)
			child->show();
		else
			child->hide();
	}
}

void TaskFlowBox::set_tasks(const std::vector<backend::Task> &tasks) {
	std::unordered_map<uint32_t, TaskFlowBoxChild *> update_set{}, erase_set = std::move(m_children);
	int pos = 0;
	for (const auto &task : tasks) {
		auto it = erase_set.find(task.id);
		if (it != erase_set.end()) {
			// The task ID already exists, modify it
			update_set.insert(*it);
			auto child = it->second;
			bool key_changed = !backend::TaskKeyEqual(child->get_task(), task);
			if (child->get_task() != task)
				child->set_task(task);
			if (key_changed) {
				Gtk::FlowBox::remove(*child);
				Gtk::FlowBox::insert(*child, pos);
			}
			erase_set.erase(task.id);
		} else {
			// Not exist, insert it
			auto child = Gtk::make_managed<TaskFlowBoxChild>(task);
			update_set.insert({task.id, child});
			Gtk::FlowBox::insert(*child, pos);
			child->show();
		}
		++pos;
	}
	for (const auto &p : erase_set) {
		auto child = p.second;
		child->hide();
		if (child == m_active_child) {
			m_active_child = nullptr;
		}
		Gtk::FlowBox::remove(*child);
	}
	m_children = std::move(update_set);
}

void TaskFlowBox::set_status_filter(backend::TaskStatus status, bool activate) {
	uint32_t d = (uint32_t)status, mask = ~(1u << d);
	m_status_filter = (m_status_filter & mask) | ((uint32_t)activate << d);
}
void TaskFlowBox::set_type_filter(backend::TaskType type, bool activate) {
	uint32_t d = (uint32_t)type, mask = ~(1u << d);
	m_type_filter = (m_type_filter & mask) | ((uint32_t)activate << d);
}
void TaskFlowBox::set_priority_filter(backend::TaskPriority priority, bool activate) {
	uint32_t d = (uint32_t)priority, mask = ~(1u << d);
	m_priority_filter = (m_priority_filter & mask) | ((uint32_t)activate << d);
}

} // namespace gui