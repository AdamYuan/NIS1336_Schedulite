#include "TaskFlowBoxChild.hpp"

#include <utility>

namespace gui {

static constexpr const char *get_priority_icon_name(backend::TaskPriority priority) {
	switch (priority) {
	case backend::TaskPriority::kHigh:
		return "starred";
	case backend::TaskPriority::kMedium:
		return "semi-starred";
	case backend::TaskPriority::kLow:
		return "non-starred";
	}
	return get_priority_icon_name(backend::kDefaultTaskPriority);
}

static constexpr const char *get_type_icon_name(backend::TaskType type) {
	switch (type) {
	case backend::TaskType::kNone:
		return "task-due";
	case backend::TaskType::kWork:
		return "applications-office";
	case backend::TaskType::kStudy:
		return "accessories-dictionary";
	case backend::TaskType::kLife:
		return "preferences-desktop-accessibility";
	case backend::TaskType::kPlay:
		return "applications-games";
	}
	return get_priority_icon_name(backend::kDefaultTaskPriority);
}

TaskFlowBoxChild::TaskFlowBoxChild(backend::Task task) : m_task{std::move(task)} {
	initialize();
	update();
}

void TaskFlowBoxChild::initialize() {
#include <ui/Task.hpp>
	auto builder = Gtk::Builder::create_from_string(kTaskUIString);
	builder->get_widget("priority_icon", m_p_priority_icon);
	builder->get_widget("type_icon", m_p_type_icon);
	builder->get_widget("type_label", m_p_type_label);
	builder->get_widget("name_label", m_p_name_label);
	builder->get_widget("begin_time_label", m_p_begin_time_label);
	builder->get_widget("remind_time_label", m_p_remind_time_label);

	Gtk::Box *p_task_box;
	builder->get_widget("task_box", p_task_box);
	add(*p_task_box);
	show_all();
}
void TaskFlowBoxChild::update() {
	m_p_priority_icon->set_from_icon_name(get_priority_icon_name(m_task.property.priority), Gtk::ICON_SIZE_DND);
	m_p_type_icon->set_from_icon_name(get_type_icon_name(m_task.property.type), Gtk::ICON_SIZE_DND);
	if (m_task.property.type != backend::TaskType::kNone) {
		m_p_type_label->set_text(backend::StrFromTaskType(m_task.property.type));
		m_p_type_label->show();
	} else {
		m_p_type_label->hide();
	}

	m_p_begin_time_label->set_markup("<big>" + backend::ToTimeStr(m_task.property.begin_time) + "</big>");

	m_p_remind_time_label->set_label(backend::ToTimeStr(m_task.property.remind_time));

	m_p_name_label->set_markup("<big>" + m_task.property.name + "</big>");
}

} // namespace gui
