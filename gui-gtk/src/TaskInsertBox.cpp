#include "TaskInsertBox.hpp"

#include "Icon.hpp"
#include <backend/Time.hpp>

namespace gui {

void TaskInsertBox::init_widget() {
#include <ui/Insert.hpp>
	auto builder = Gtk::Builder::create_from_string(kInsertUIString);
	builder->get_widget("grid", m_p_grid);
	builder->get_widget("task_name_entry", m_p_task_name_entry);
	builder->get_widget("begin_time_button", m_p_begin_time_button);
	builder->get_widget("remind_time_button", m_p_remind_time_button);
	builder->get_widget("priority_button", m_p_priority_button);
	builder->get_widget("type_button", m_p_type_button);
	builder->get_widget("ok_button", m_p_ok_button);
	builder->get_widget("priority_label", m_p_priority_label);
	builder->get_widget("priority_icon", m_p_priority_icon);
	builder->get_widget("type_label", m_p_type_label);
	builder->get_widget("type_icon", m_p_type_icon);
	pack_start(*m_p_grid);
	show_all();
	m_p_begin_time_button->set_popover(m_begin_time_popover);
	m_p_remind_time_button->set_popover(m_remind_time_popover);
	m_p_priority_button->set_popover(m_priority_popover);
	m_p_type_button->set_popover(m_type_popover);
	restore();

	m_begin_time_popover.signal_time_selected().connect(
	    [this](const backend::TimeInfo &info) { m_p_begin_time_button->set_label(backend::ToTimeStr(info)); });

	m_remind_time_popover.signal_time_selected().connect(
	    [this](const backend::TimeInfo &info) { m_p_remind_time_button->set_label(backend::ToTimeStr(info)); });

	m_priority_popover.signal_selected().connect(
	    [this](const char *str) { set_priority(backend::TaskPriorityFromStr(str)); });
	m_type_popover.signal_selected().connect([this](const char *str) { set_type(backend::TaskTypeFromStr(str)); });

	m_p_ok_button->signal_clicked().connect([this]() { m_signal_task_inserted.emit(get_task_property()); });
}

void TaskInsertBox::restore() {
	m_p_task_name_entry->set_text("Task Name");
	auto time_info = backend::GetTimeInfoNow();
	auto time_str = backend::ToTimeStr(time_info);
	m_p_begin_time_button->set_label(time_str);
	m_p_remind_time_button->set_label(time_str);
	m_begin_time_popover.set_time(time_info);
	m_remind_time_popover.set_time(time_info);
	set_priority(backend::kDefaultTaskPriority);
	set_type(backend::kDefaultTaskType);
}

backend::TaskProperty TaskInsertBox::get_task_property() {
	backend::TaskProperty p{};
	p.name = m_p_task_name_entry->get_text();
	p.begin_time = backend::ToTimeInt(m_p_begin_time_button->get_label());
	p.remind_time = backend::ToTimeInt(m_p_remind_time_button->get_label());
	p.priority = backend::TaskPriorityFromStr(m_p_priority_label->get_text().c_str());
	p.type = backend::TaskTypeFromStr(m_p_type_label->get_text().c_str());
	p.done = false;
	return p;
}

void TaskInsertBox::set_priority(backend::TaskPriority priority) {
	m_p_priority_icon->set_from_icon_name(GetTaskPriorityIconName(priority), Gtk::ICON_SIZE_DND);
	m_p_priority_label->set_text(backend::StrFromTaskPriority(priority));
}
void TaskInsertBox::set_type(backend::TaskType type) {
	m_p_type_icon->set_from_icon_name(GetTaskTypeIconName(type), Gtk::ICON_SIZE_DND);
	m_p_type_label->set_text(backend::StrFromTaskType(type));
}

} // namespace gui