#include "TaskDetailBox.hpp"

namespace gui {

void TaskDetailBox::init_widget() {
#include <ui/Detail.hpp>
	auto builder = Gtk::Builder::create_from_string(kDetailUIString);
	builder->get_widget("box", m_p_box);
	builder->get_widget("name_label", m_p_name_label);
	builder->get_widget("begin_time_label", m_p_begin_time_label);
	builder->get_widget("remind_time_label", m_p_remind_time_label);
	builder->get_widget("priority_label", m_p_priority_label);
	builder->get_widget("type_label", m_p_type_label);
	builder->get_widget("status_label", m_p_status_label);

	builder->get_widget("priority_icon", m_p_priority_icon);
	builder->get_widget("type_icon", m_p_type_icon);
	builder->get_widget("status_icon", m_p_status_icon);

	builder->get_widget("name_popover", m_p_name_popover);

	builder->get_widget("undone_button", m_p_undone_button);
	builder->get_widget("done_button", m_p_done_button);
	builder->get_widget("erase_button", m_p_erase_button);
	builder->get_widget("edit_begin_time_button", m_p_edit_begin_time_button);
	builder->get_widget("edit_remind_time_button", m_p_edit_remind_time_button);
	builder->get_widget("edit_priority_button", m_p_edit_priority_button);
	builder->get_widget("edit_type_button", m_p_edit_type_button);

	pack_start(*m_p_box);
}

} // namespace gui
