#ifndef SCHEDULITE_GTK_TASKLISTBOXROW_HPP
#define SCHEDULITE_GTK_TASKLISTBOXROW_HPP

#include <gtkmm.h>

#include <backend/Task.hpp>

namespace gui {
class TaskFlowBoxChild : public Gtk::FlowBoxChild {
public:
	explicit TaskFlowBoxChild(backend::Task task,
	                          sigc::signal<void(const backend::Task &)> &parent_signal_task_selected);
	~TaskFlowBoxChild() override = default;

	inline const backend::Task &get_task() const { return m_task; }
	inline void set_task(const backend::Task &task) {
		m_task = task;
		update();
	}
	void on_grab_focus() override;

private:
	backend::Task m_task;
	sigc::signal<void(const backend::Task &)> &m_parent_signal_task_selected;

	Gtk::ToggleButton m_button;
	Gtk::Box m_content_box, m_time_box, m_remind_box;
	Gtk::Label m_name_label, m_begin_time_label, m_remind_time_label;
	Gtk::Image m_priority_icon, m_type_icon, m_status_icon, m_remind_icon;

	/* Gtk::Button *m_p_task_button{nullptr};
	Gtk::Box *m_p_task_content_box{};
	Gtk::Label *m_p_name_label{nullptr}, *m_p_begin_time_label{nullptr}, *m_p_remind_time_label{nullptr},
	    *m_p_type_label{nullptr};
	Gtk::Image *m_p_priority_icon{nullptr}, *m_p_type_icon{nullptr}, *m_p_status_icon{nullptr}; */

	void initialize();
	void update();

	friend class TaskFlowBox;
};
} // namespace gui

#endif
