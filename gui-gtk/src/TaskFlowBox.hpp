#ifndef SCHEDULITE_GTK_TASKLISTBOX_HPP
#define SCHEDULITE_GTK_TASKLISTBOX_HPP

#include <gtkmm.h>

#include "TaskFlowBoxChild.hpp"
#include <backend/Schedule.hpp>
#include <unordered_map>

namespace gui {

class TaskFlowBox : public Gtk::FlowBox {
public:
	TaskFlowBox();
	~TaskFlowBox() override = default;

	void refilter();

	void set_tasks(const std::vector<backend::Task> &tasks);
	sigc::signal<void(const backend::Task &)> signal_task_selected() { return m_signal_task_selected; }

	void set_status_filter(backend::TaskStatus status, bool activate);
	void set_type_filter(backend::TaskType type, bool activate);
	void set_priority_filter(backend::TaskPriority priority, bool activate);

protected:
	sigc::signal<void(const backend::Task &)> m_signal_task_selected;

private:
	std::unordered_map<uint32_t, TaskFlowBoxChild *> m_children;
	void init_widget();

	uint32_t m_status_filter = -1, m_type_filter = -1, m_priority_filter = -1;
};

} // namespace gui

#endif
