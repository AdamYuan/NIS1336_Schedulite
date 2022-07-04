#include "Window.hpp"

#include <backend/Environment.hpp>

namespace gui {

Window::Window() { initialize(); }

void Window::initialize() {
#include <ui/User.hpp>
	auto builder = Gtk::Builder::create_from_string(kUserUIString);
	builder->get_widget("user_popover", m_p_user_popover);
	builder->get_widget("username_label", m_p_user_popover_label);
	m_p_user_popover->set_size_request(200, -1);

	{
		Gtk::Button *p_login_button{nullptr}, *p_register_button{nullptr};
		builder->get_widget("login_button", p_login_button);
		builder->get_widget("register_button", p_register_button);
		p_login_button->signal_clicked().connect([this]() {
			m_p_user_popover->hide();
			printf("Login\n");
		});
		p_register_button->signal_clicked().connect([this]() { printf("Register\n"); });
	}

	set_title(backend::kAppName);
	set_default_size(640, 480);

	set_titlebar(m_header.bar);
	{
		m_header.bar.set_show_close_button(TRUE);
		m_header.bar.set_title(backend::kAppName);
		m_header.bar.show();

		m_header.bar.pack_start(m_header.user_button);
		m_header.bar.pack_end(m_header.insert_button);

		m_header.user_button.set_image_from_icon_name("user-info", Gtk::ICON_SIZE_DND);
		m_header.user_button.set_popover(*m_p_user_popover);
		m_header.user_button.show();

		m_header.insert_button.set_image_from_icon_name("document-new-symbolic");
		m_header.insert_button.set_label("Insert");
		m_header.insert_button.show();

		m_header.pending_button.set_label("Pending");
		m_header.ongoing_button.set_label("Ongoing");
		m_header.done_button.set_label("Done");
	}

	m_scrolled_window.add(m_task_list_box);
	m_scrolled_window.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
	m_task_list_box.show();

	add(m_scrolled_window);
	m_scrolled_window.show();

	{
		auto instance = backend::Instance::Create();
		auto [user, error] = backend::User::Login(instance, "YYT", "1234");
		auto [schedule, error2] = backend::Schedule::Acquire(user);
		m_schedule_ptr = schedule;
	}
	m_header.user_button.set_label(m_schedule_ptr->GetUserPtr()->GetName());
	m_p_user_popover_label->set_markup("<big>" + m_schedule_ptr->GetUserPtr()->GetName() + "</big>");
	m_task_list_box.UpdateSchedule(m_schedule_ptr);
}
} // namespace gui
