#include "TaskFlowBoxChild.hpp"

#include "Icon.hpp"
#include <utility>

namespace gui {

TaskFlowBoxChild::TaskFlowBoxChild(backend::Task task,
                                   sigc::signal<void(const backend::Task &)> &parent_signal_task_selected)
    : m_task{std::move(task)}, m_parent_signal_task_selected{parent_signal_task_selected} {
	initialize();
	update();
}

void TaskFlowBoxChild::initialize() {
	set_valign(Gtk::ALIGN_CENTER);
	set_halign(Gtk::ALIGN_BASELINE);
#include <ui/Task.hpp>
	auto builder = Gtk::Builder::create_from_string(kTaskUIString);
	builder->get_widget("priority_icon", m_p_priority_icon);
	builder->get_widget("type_icon", m_p_type_icon);
	builder->get_widget("status_icon", m_p_status_icon);
	builder->get_widget("type_label", m_p_type_label);
	builder->get_widget("name_label", m_p_name_label);
	builder->get_widget("begin_time_label", m_p_begin_time_label);
	builder->get_widget("remind_time_label", m_p_remind_time_label);
	builder->get_widget("task_content", m_p_task_content_box);
	builder->get_widget("task", m_p_task_button);
	add(*m_p_task_button);
	show_all();

	m_p_task_button->signal_clicked().connect([this]() { m_parent_signal_task_selected.emit(m_task); });
}
void TaskFlowBoxChild::update() {
	m_p_priority_icon->set_from_icon_name(GetTaskPriorityIconName(m_task.property.priority), Gtk::ICON_SIZE_DND);
	m_p_type_icon->set_from_icon_name(GetTaskTypeIconName(m_task.property.type), Gtk::ICON_SIZE_DND);
	m_p_status_icon->set_from_icon_name(GetTaskStatusIconName(backend::TaskStatusFromTask(m_task)), Gtk::ICON_SIZE_DND);

	if (m_task.property.type != backend::TaskType::kNone) {
		m_p_type_label->set_text(backend::StrFromTaskType(m_task.property.type));
	} else {
		m_p_type_label->set_text("Task");
	}

	m_p_task_content_box->set_sensitive(!m_task.property.done);

	m_p_begin_time_label->set_text(backend::ToTimeStr(m_task.property.begin_time));
	m_p_remind_time_label->set_text(backend::ToTimeStr(m_task.property.remind_time));
	m_p_name_label->set_text(m_task.property.name);
}

void TaskFlowBoxChild::on_grab_focus() {
	Gtk::FlowBoxChild::on_grab_focus();
	m_p_task_button->grab_focus();
}

} // namespace gui
