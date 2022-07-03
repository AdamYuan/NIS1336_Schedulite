#include "Window.hpp"

#include <backend/Environment.hpp>

namespace gui {

Window::Window() { initialize(); }

void Window::initialize() {
	set_title(backend::kAppName);
	set_default_size(640, 480);

	set_titlebar(m_header_bar);
	{
		m_header_bar.set_show_close_button(TRUE);
		m_header_bar.set_title(backend::kAppName);
		m_header_bar.show();

		m_header_bar.pack_start(m_user_button);
		m_header_bar.pack_end(m_insert_button);

		m_user_button.set_image_from_icon_name("user-info", Gtk::ICON_SIZE_DND);
		m_user_button.set_popover(m_user_popover);
		m_user_button.show();

		m_insert_button.set_image_from_icon_name("document-new-symbolic");
		m_insert_button.set_label("Insert");
		m_insert_button.show();
	}

	m_scrolled_window.add(m_task_list_box);
	m_task_list_box.show();

	add(m_scrolled_window);
	m_scrolled_window.show();

	{
		auto instance = backend::Instance::Create();
		auto [user, error] = backend::User::Login(instance, "YYT", "1234");
		auto [schedule, error2] = backend::Schedule::Acquire(user);
		m_schedule_ptr = schedule;
	}
	m_user_button.set_label(m_schedule_ptr->GetUserPtr()->GetName());
	m_task_list_box.UpdateSchedule(m_schedule_ptr);
}

} // namespace gui
