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
	explicit TaskFlowBox(const std::shared_ptr<backend::Schedule> &schedule_ptr);
	~TaskFlowBox() override = default;

	void set_schedule(const std::shared_ptr<backend::Schedule> &schedule_ptr);
	sigc::signal<void(const backend::Task &)> signal_task_selected() { return m_signal_task_selected; }

protected:
	sigc::signal<void(const backend::Task &)> m_signal_task_selected;

private:
	std::shared_ptr<backend::Schedule> m_schedule_ptr;
	std::unordered_map<uint32_t, TaskFlowBoxChild *> m_rows;

	void initialize();
};

} // namespace gui

#endif
