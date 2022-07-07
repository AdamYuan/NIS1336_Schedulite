#include "TaskDetailBox.hpp"

#include "Icon.hpp"

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

	builder->get_widget("name_entry", m_p_name_entry);
	builder->get_widget("name_apply_button", m_p_name_apply_button);

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

	m_p_edit_begin_time_button->set_popover(m_begin_time_popover);
	m_p_edit_remind_time_button->set_popover(m_remind_time_popover);
	m_p_edit_priority_button->set_popover(m_priority_popover);
	m_p_edit_type_button->set_popover(m_type_popover);

	m_p_name_popover->signal_show().connect([this]() { m_p_name_entry->set_text(m_task.property.name); });
	m_begin_time_popover.signal_show().connect(
	    [this]() { m_begin_time_popover.set_time(backend::ToTimeInfo(m_task.property.begin_time)); });
	m_remind_time_popover.signal_show().connect(
	    [this]() { m_remind_time_popover.set_time(backend::ToTimeInfo(m_task.property.remind_time)); });

	// edit signals
	m_p_name_apply_button->signal_clicked().connect([this]() {
		backend::TaskProperty p;
		p.name = m_p_name_entry->get_text();
		m_signal_task_edited.emit(m_task.id, p, backend::TaskPropertyMask::kName);
		m_p_name_popover->hide();
	});
	m_begin_time_popover.signal_time_selected().connect([this](const backend::TimeInfo &info) {
		backend::TaskProperty p;
		p.begin_time = backend::ToTimeInt(info);
		m_signal_task_edited.emit(m_task.id, p, backend::TaskPropertyMask::kBeginTime);
	});
	m_remind_time_popover.signal_time_selected().connect([this](const backend::TimeInfo &info) {
		backend::TaskProperty p;
		p.remind_time = backend::ToTimeInt(info);
		m_signal_task_edited.emit(m_task.id, p, backend::TaskPropertyMask::kRemindTime);
	});
	m_priority_popover.signal_selected().connect([this](const char *str) {
		backend::TaskProperty p;
		p.priority = backend::TaskPriorityFromStr(str);
		m_signal_task_edited.emit(m_task.id, p, backend::TaskPropertyMask::kPriority);
	});
	m_type_popover.signal_selected().connect([this](const char *str) {
		backend::TaskProperty p;
		p.type = backend::TaskTypeFromStr(str);
		m_signal_task_edited.emit(m_task.id, p, backend::TaskPropertyMask::kType);
	});
	m_p_undone_button->signal_clicked().connect([this]() {
		backend::TaskProperty p;
		p.done = false;
		m_signal_task_edited.emit(m_task.id, p, backend::TaskPropertyMask::kDone);
	});
	m_p_undone_button->signal_clicked().connect([this]() {
		backend::TaskProperty p;
		p.done = false;
		m_signal_task_edited.emit(m_task.id, p, backend::TaskPropertyMask::kDone);
	});
	m_p_done_button->signal_clicked().connect([this]() {
		backend::TaskProperty p;
		p.done = true;
		m_signal_task_edited.emit(m_task.id, p, backend::TaskPropertyMask::kDone);
	});
}

void TaskDetailBox::set_task(const backend::Task &task) {
	m_task = task;
	m_p_name_label->set_text(m_task.property.name);
	m_p_begin_time_label->set_text(backend::ToTimeStr(m_task.property.begin_time));
	m_p_remind_time_label->set_text(backend::ToTimeStr(m_task.property.remind_time));
	m_p_priority_label->set_text(backend::StrFromTaskPriority(m_task.property.priority));
	m_p_priority_icon->set_from_icon_name(GetTaskPriorityIconName(m_task.property.priority), Gtk::ICON_SIZE_DND);
	m_p_type_label->set_text(backend::StrFromTaskType(m_task.property.type));
	m_p_type_icon->set_from_icon_name(GetTaskTypeIconName(m_task.property.type), Gtk::ICON_SIZE_DND);
	if (m_task.property.done) {
		m_p_done_button->hide();
		m_p_undone_button->show();
	} else {
		m_p_done_button->show();
		m_p_undone_button->hide();
	}
	update_status();
}
void TaskDetailBox::update_status() {
	backend::TimeInt now_int = backend::GetTimeIntNow();
	auto status = backend::TaskStatusFromTask(m_task, now_int);
	m_p_status_label->set_text(backend::StrFromTaskStatus(status));
	m_p_status_icon->set_from_icon_name(GetTaskStatusIconName(status), Gtk::ICON_SIZE_DND);
}

bool TaskDetailBox::update_from_tasks(const std::vector<backend::Task> &tasks) {
	auto it = std::find_if(tasks.begin(), tasks.end(), [this](const backend::Task &t) { return m_task.id == t.id; });
	if (it == tasks.end())
		return false;
	set_task(*it);
	return true;
}

} // namespace gui
