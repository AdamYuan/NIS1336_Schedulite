#ifndef SCHEDULITE_TASKDETAILBOX_HPP
#define SCHEDULITE_TASKDETAILBOX_HPP

#include <gtkmm.h>

namespace gui {
class TaskDetailBox : public Gtk::Box {
public:
	inline TaskDetailBox() { init_widget(); }
	~TaskDetailBox() override = default;

private:
	Gtk::Box *m_p_box{};
	Gtk::Label *m_p_name_label{}, *m_p_begin_time_label{}, *m_p_remind_time_label{}, *m_p_priority_label{},
	    *m_p_type_label{}, *m_p_status_label{};
	Gtk::Image *m_p_priority_icon{}, *m_p_type_icon{}, *m_p_status_icon{};
	Gtk::Popover *m_p_name_popover{};
	Gtk::Button *m_p_undone_button{};
	Gtk::MenuButton *m_p_done_button{}, *m_p_erase_button{}, *m_p_edit_begin_time_button{},
	    *m_p_edit_remind_time_button{}, *m_p_edit_priority_button{}, *m_p_edit_type_button{};
	void init_widget();
};
} // namespace gui

#endif
