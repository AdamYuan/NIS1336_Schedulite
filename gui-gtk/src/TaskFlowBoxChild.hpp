#ifndef SCHEDULITE_GTK_TASKLISTBOXROW_HPP
#define SCHEDULITE_GTK_TASKLISTBOXROW_HPP

#include <gtkmm.h>

#include <backend/Task.hpp>

namespace gui {
class TaskFlowBoxChild : public Gtk::FlowBoxChild {
public:
	explicit TaskFlowBoxChild(backend::Task task);
	inline const backend::Task &GetTask() const { return m_task; }
	~TaskFlowBoxChild() override = default;

private:
	backend::Task m_task;

	Gtk::Label *m_p_name_label{nullptr}, *m_p_begin_time_label{nullptr}, *m_p_remind_time_label{nullptr},
	    *m_p_type_label{nullptr};
	Gtk::Image *m_p_priority_icon{nullptr}, *m_p_type_icon{nullptr};

	void initialize();
	void update();

	friend class TaskFlowBox;
};
} // namespace gui

#endif
