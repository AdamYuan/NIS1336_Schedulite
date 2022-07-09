#include "TaskFlowBoxChild.hpp"

#include "Icon.hpp"
#include "TaskFlowBox.hpp"
#include <utility>

namespace gui {

TaskFlowBoxChild::TaskFlowBoxChild(backend::Task task) : m_task{std::move(task)} {
	initialize();
	update();
}

void TaskFlowBoxChild::initialize() {
	set_valign(Gtk::ALIGN_CENTER);
	set_halign(Gtk::ALIGN_BASELINE);
	set_hexpand(true);
	set_vexpand(true);

	m_button.add(m_content_box);
	m_button.set_halign(Gtk::ALIGN_FILL);
	m_button.set_valign(Gtk::ALIGN_FILL);

	m_content_box.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
	m_content_box.set_border_width(16);

	m_content_box.pack_start(m_priority_icon, Gtk::PACK_SHRINK);
	m_content_box.pack_start(m_type_icon, Gtk::PACK_SHRINK);
	m_content_box.pack_start(m_name_label, Gtk::PACK_EXPAND_PADDING);
	m_content_box.pack_end(m_status_icon, Gtk::PACK_SHRINK);
	m_content_box.pack_end(m_time_box, Gtk::PACK_SHRINK);

	m_priority_icon.set_pixel_size(48);
	m_priority_icon.set_size_request(48, -1);
	m_priority_icon.set_margin_end(16);

	m_type_icon.set_pixel_size(48);
	m_type_icon.set_size_request(48, -1);

	m_status_icon.set_pixel_size(48);
	m_status_icon.set_size_request(48, -1);
	m_status_icon.set_margin_start(16);

	m_name_label.set_margin_start(16);
	m_name_label.set_margin_end(16);
	{
		Pango::AttrList attr_list;
		auto scale = Pango::Attribute::create_attr_scale(1.5);
		attr_list.change(scale);
		m_name_label.set_attributes(attr_list);
	}
	m_name_label.set_ellipsize(Pango::ELLIPSIZE_END);

	m_time_box.set_orientation(Gtk::ORIENTATION_VERTICAL);
	m_time_box.pack_start(m_begin_time_label);
	m_time_box.pack_start(m_remind_box);

	{
		Pango::AttrList attr_list;
		auto scale = Pango::Attribute::create_attr_scale(1.3);
		attr_list.change(scale);
		m_begin_time_label.set_attributes(attr_list);
	}
	m_begin_time_label.set_ellipsize(Pango::ELLIPSIZE_END);
	m_begin_time_label.set_tooltip_text("Begin Time");

	m_remind_box.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
	m_remind_box.set_spacing(8);
	m_remind_box.pack_end(m_remind_icon, Gtk::PACK_SHRINK);
	m_remind_box.pack_end(m_remind_time_label, Gtk::PACK_SHRINK);
	m_remind_box.set_tooltip_text("Remind Time");
	m_remind_box.set_sensitive(false);

	m_remind_time_label.set_ellipsize(Pango::ELLIPSIZE_END);
	m_remind_icon.set_from_icon_name("dialog-information", Gtk::ICON_SIZE_BUTTON);

	add(m_button);

	show_all();

	m_button.signal_clicked().connect([this]() {
		auto parent = (TaskFlowBox *)get_parent();
		if (m_button.get_active()) {
			if (parent->m_active_child && parent->m_active_child != this)
				parent->m_active_child->set_active(false);
			parent->m_active_child = this;
			parent->m_signal_task_selected.emit(m_task);
		} else if (parent->m_active_child == this) {
			parent->m_active_child = nullptr;
			parent->m_signal_deactivate.emit();
		}
	});
}
void TaskFlowBoxChild::update() {
	auto status = backend::TaskStatusFromTask(m_task);
	m_priority_icon.set_from_icon_name(GetTaskPriorityIconName(m_task.property.priority), Gtk::ICON_SIZE_DND);
	m_priority_icon.set_tooltip_text((std::string)backend::StrFromTaskPriority(m_task.property.priority) + " Priority");

	m_type_icon.set_from_icon_name(GetTaskTypeIconName(m_task.property.type), Gtk::ICON_SIZE_DND);
	m_type_icon.set_tooltip_text((std::string)backend::StrFromTaskType(m_task.property.type));

	m_status_icon.set_from_icon_name(GetTaskStatusIconName(status), Gtk::ICON_SIZE_DND);
	m_status_icon.set_tooltip_text((std::string)backend::StrFromTaskStatus(status));

	/* if (m_task.property.type != backend::TaskType::kNone) {
	    m_p_type_label->set_text(backend::StrFromTaskType(m_task.property.type));
	} else {
	    m_p_type_label->set_text("Task");
	} */
	m_content_box.set_sensitive(!m_task.property.done);

	m_begin_time_label.set_text(backend::ToTimeStr(m_task.property.begin_time));
	m_remind_time_label.set_text(backend::ToTimeStr(m_task.property.remind_time));
	m_name_label.set_text(m_task.property.name);
}

void TaskFlowBoxChild::on_grab_focus() {
	Gtk::FlowBoxChild::on_grab_focus();
	m_button.grab_focus();
}

} // namespace gui
