#include "TaskListBoxRow.hpp"

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

TaskListBoxRow::TaskListBoxRow(backend::Task task) : m_task{std::move(task)} {
	initialize();
	update();
}

void TaskListBoxRow::initialize() {
	add(m_main_box);

	m_priority_box.pack_start(m_priority_icon, Gtk::PACK_EXPAND_WIDGET);
	// m_priority_box.set_size_request(64, -1);
	m_priority_box.set_tooltip_text("Task Priority");

	m_type_box.set_orientation(Gtk::ORIENTATION_VERTICAL);
	m_type_box.pack_start(m_type_icon);
	m_type_box.pack_start(m_type_label);
	m_type_box.set_size_request(64, -1);
	m_type_box.set_tooltip_text("Task Type");

	//m_begin_time_box.pack_end(m_begin_time_icon, Gtk::PACK_SHRINK);
	m_begin_time_box.pack_end(m_begin_time_label, Gtk::PACK_SHRINK, 4);
	m_begin_time_box.set_tooltip_text("Begin Time");

	m_remind_time_box.pack_end(m_remind_time_icon, Gtk::PACK_SHRINK);
	m_remind_time_box.pack_end(m_remind_time_label, Gtk::PACK_SHRINK, 4);
	m_remind_time_box.set_tooltip_text("Remind Time");
	m_remind_time_label.set_state(Gtk::STATE_INSENSITIVE);
	m_remind_time_icon.set_state(Gtk::STATE_INSENSITIVE);

	m_main_box.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
	m_main_box.pack_start(m_priority_box, Gtk::PACK_SHRINK, 16);
	m_main_box.pack_start(m_type_box, Gtk::PACK_SHRINK);
	m_main_box.pack_start(m_box_0, Gtk::PACK_EXPAND_PADDING);
	m_main_box.pack_end(m_box_1, Gtk::PACK_SHRINK, 16);

	m_box_0.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
	m_box_0.pack_start(m_name_label, Gtk::PACK_SHRINK);

	m_box_1.set_orientation(Gtk::ORIENTATION_VERTICAL);
	m_box_1.pack_start(m_begin_time_box, Gtk::PACK_EXPAND_PADDING);

	m_box_1.pack_start(m_remind_time_box, Gtk::PACK_EXPAND_PADDING);

	// TODO: Done and Edit button
	m_hbox_2.pack_end(m_done_button, Gtk::PACK_SHRINK);
	m_hbox_2.pack_end(m_edit_button, Gtk::PACK_SHRINK);

	m_edit_button.set_label("Edit");
	m_done_button.set_label("Done");

	m_begin_time_icon.set_from_icon_name("emblem-urgent", Gtk::ICON_SIZE_BUTTON);
	m_remind_time_icon.set_from_icon_name("dialog-information", Gtk::ICON_SIZE_BUTTON);

	show_all();
}
void TaskListBoxRow::update() {
	m_priority_icon.set_from_icon_name(get_priority_icon_name(m_task.property.priority), Gtk::ICON_SIZE_DND);
	m_type_icon.set_from_icon_name(get_type_icon_name(m_task.property.type), Gtk::ICON_SIZE_DND);
	if (m_task.property.type != backend::TaskType::kNone) {
		m_type_label.set_text(backend::StrFromTaskType(m_task.property.type));
		m_type_label.show();
	} else {
		m_type_label.hide();
	}

	m_begin_time_label.set_markup("<big>" + backend::ToTimeStr(m_task.property.begin_time) + "</big>");

	m_remind_time_label.set_label(backend::ToTimeStr(m_task.property.remind_time));

	m_name_label.set_markup("<big>" + m_task.property.name + "</big>");
}

} // namespace gui
