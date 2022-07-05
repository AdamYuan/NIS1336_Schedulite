#ifndef SCHEDULITE_GTK_TASKLISTBOXROW_HPP
#define SCHEDULITE_GTK_TASKLISTBOXROW_HPP

#include <gtkmm.h>

#include <backend/Task.hpp>

namespace gui {
class TaskFlowBoxChild : public Gtk::FlowBoxChild {
public:
	explicit TaskFlowBoxChild(backend::Task task,
	                          sigc::signal<void(const backend::Task &)> &parent_signal_task_selected);
	inline const backend::Task &GetTask() const { return m_task; }
	~TaskFlowBoxChild() override = default;

	void on_grab_focus() override;

private:
	backend::Task m_task;
	sigc::signal<void(const backend::Task &)> &m_parent_signal_task_selected;

	Gtk::Button *m_p_task_button{nullptr};
	Gtk::Box *m_p_task_content_box{};
	Gtk::Label *m_p_name_label{nullptr}, *m_p_begin_time_label{nullptr}, *m_p_remind_time_label{nullptr},
	    *m_p_type_label{nullptr};
	Gtk::Image *m_p_priority_icon{nullptr}, *m_p_type_icon{nullptr}, *m_p_status_icon{nullptr};

	void initialize();
	void update();

	friend class TaskFlowBox;
};
} // namespace gui

#endif
