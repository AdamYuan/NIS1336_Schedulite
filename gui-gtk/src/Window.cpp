#include "Window.hpp"

#include "Icon.hpp"
#include <backend/Environment.hpp>

namespace gui {

Window::Window() {
	m_instance_ptr = backend::Instance::Create();
	initialize();
}

void Window::initialize() {
	set_title(backend::kAppName);
	set_default_size(640, 480);

	initialize_header_bar();
	initialize_user_panel();
	initialize_body();
}

void Window::initialize_body() {
	m_body.scrolled_window.add(m_body.task_list_box);
	m_body.task_list_box.show();

	m_body.box.pack_end(m_body.scrolled_window, Gtk::PACK_EXPAND_WIDGET);
	m_body.scrolled_window.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
	m_body.scrolled_window.show();

	add(m_body.box);
	m_body.box.set_orientation(Gtk::ORIENTATION_VERTICAL);
	m_body.box.show();

	m_body.task_list_box.signal_task_selected().connect(
	    [](const backend::Task &task) { printf("%s\n", task.property.name.c_str()); });
}

void Window::initialize_user_panel() {
#include <ui/User.hpp>
	auto user_builder = Gtk::Builder::create_from_string(kUserUIString);
	user_builder->get_widget("user_popover", m_user.p_popover);
	user_builder->get_widget("current_user_box", m_user.p_current_user_box);
	user_builder->get_widget("username_label", m_user.p_username_label);
	user_builder->get_widget("login_button", m_user.p_login_button);
	user_builder->get_widget("login_username", m_user.p_login_username);
	user_builder->get_widget("login_username_combo", m_user.p_login_username_combo);
	user_builder->get_widget("login_password", m_user.p_login_password);
	user_builder->get_widget("register_button", m_user.p_register_button);
	user_builder->get_widget("register_username", m_user.p_register_username);
	user_builder->get_widget("register_password1", m_user.p_register_password1);
	user_builder->get_widget("register_password2", m_user.p_register_password2);
	m_user.p_login_button->signal_clicked().connect([this]() { login_button_click(); });
	m_user.p_register_button->signal_clicked().connect([this]() { register_button_click(); });

	{
		m_user.p_login_username->signal_changed().connect([this]() {
			m_user.p_login_button->set_sensitive(!m_user.p_login_username->get_text().empty() &&
			                                     !m_user.p_login_password->get_text().empty());
		});
		m_user.p_login_password->signal_changed().connect([this]() {
			m_user.p_login_button->set_sensitive(!m_user.p_login_username->get_text().empty() &&
			                                     !m_user.p_login_password->get_text().empty());
		});
	}

	{
		m_user.p_register_username->signal_changed().connect([this]() {
			m_user.p_register_button->set_sensitive(!m_user.p_register_username->get_text().empty() &&
			                                        !m_user.p_register_password1->get_text().empty() &&
			                                        !m_user.p_register_password2->get_text().empty());
		});
		m_user.p_register_password1->signal_changed().connect([this]() {
			m_user.p_register_button->set_sensitive(!m_user.p_register_username->get_text().empty() &&
			                                        !m_user.p_register_password1->get_text().empty() &&
			                                        !m_user.p_register_password2->get_text().empty());
		});
		m_user.p_register_password2->signal_changed().connect([this]() {
			m_user.p_register_button->set_sensitive(!m_user.p_register_username->get_text().empty() &&
			                                        !m_user.p_register_password1->get_text().empty() &&
			                                        !m_user.p_register_password2->get_text().empty());
		});
	}

	{
		m_user.p_popover->signal_show().connect([this]() {
			m_user.p_login_username_combo->remove_all();
			for (const std::string &name : m_instance_ptr->FetchUsernames())
				m_user.p_login_username_combo->append(name);

			if (!m_schedule_ptr)
				m_user.p_current_user_box->hide();
			else {
				m_user.p_current_user_box->show();
				((Gtk::Stack *)m_user.p_current_user_box->get_parent())->set_visible_child(*m_user.p_current_user_box);
			}
		});
	}
	m_header.user_button.set_popover(*m_user.p_popover);
}

void Window::initialize_header_bar() {
	set_titlebar(m_header.bar);
	{
		m_header.bar.set_show_close_button(TRUE);
		m_header.bar.set_title(backend::kAppName);

		m_header.bar.pack_start(m_header.user_button);
		m_header.bar.pack_start(m_header.insert_button);

		m_header.user_button.set_always_show_image(true);
		m_header.user_button.set_image_from_icon_name("user-info", Gtk::ICON_SIZE_DND);
		m_header.user_button.set_tooltip_text("User");

		m_header.insert_button.set_always_show_image(true);
		m_header.insert_button.set_image_from_icon_name("document-new-symbolic", Gtk::ICON_SIZE_DND);
		m_header.insert_button.set_tooltip_text("Insert Task");
		// m_header.insert_button.set_label("Insert");
		m_header.insert_button.set_sensitive(false);

		m_header.status_filter_button.set_always_show_image(true);
		m_header.status_filter_button.set_label("Status");
		m_header.status_filter_button.set_image_from_icon_name(GetTaskStatusIconName(backend::TaskStatus::kBegun),
		                                                       Gtk::ICON_SIZE_DND);
		m_header.priority_filter_button.set_always_show_image(true);
		m_header.priority_filter_button.set_label("Priority");
		m_header.priority_filter_button.set_image_from_icon_name(GetTaskPriorityIconName(backend::kDefaultTaskPriority),
		                                                         Gtk::ICON_SIZE_DND);
		m_header.type_filter_button.set_always_show_image(true);
		m_header.type_filter_button.set_label("Type");
		m_header.type_filter_button.set_image_from_icon_name(GetTaskTypeIconName(backend::kDefaultTaskType),
		                                                     Gtk::ICON_SIZE_DND);
		m_header.button_box.set_homogeneous(true);
		m_header.button_box.set_layout(Gtk::BUTTONBOX_EXPAND);
		m_header.button_box.add(m_header.priority_filter_button);
		m_header.button_box.add(m_header.type_filter_button);
		m_header.button_box.add(m_header.status_filter_button);

		m_header.bar.set_custom_title(m_header.button_box);

		m_header.bar.show_all();
		m_header.bar.set_size_request(-1, 64);
	}
}

void Window::login_button_click() {
	std::shared_ptr<backend::User> user;
	std::shared_ptr<backend::Schedule> schedule;
	backend::Error error;

	std::tie(user, error) = backend::User::Login(m_instance_ptr, (std::string)m_user.p_login_username->get_text(),
	                                             (std::string)m_user.p_login_password->get_text());
	if (!user) {
		message_error(error);
		return;
	}
	if (m_schedule_ptr && user->GetName() == m_schedule_ptr->GetUserPtr()->GetName()) {
		message(Gtk::MESSAGE_WARNING, "User already logged in");
		return;
	}
	std::tie(schedule, error) = backend::Schedule::Acquire(user);
	if (!schedule) {
		message_error(error);
		return;
	}
	m_user.p_login_username->set_text("");
	m_user.p_login_password->set_text("");
	m_user.p_login_button->set_sensitive(false);

	set_schedule(schedule);
	message_error(backend::Error::kSuccess);
}

void Window::register_button_click() {
	if (m_user.p_register_password1->get_text() != m_user.p_register_password2->get_text()) {
		message_error("Passwords not equal");
		return;
	}
	std::shared_ptr<backend::User> user;
	std::shared_ptr<backend::Schedule> schedule;
	backend::Error error;
	std::tie(user, error) = backend::User::Register(m_instance_ptr, (std::string)m_user.p_register_username->get_text(),
	                                                (std::string)m_user.p_register_password1->get_text());
	if (!user) {
		message_error(error);
		return;
	}
	std::tie(schedule, error) = backend::Schedule::Acquire(user);
	if (!schedule) {
		message_error(error);
		return;
	}
	m_user.p_register_username->set_text("");
	m_user.p_register_password1->set_text("");
	m_user.p_register_password2->set_text("");
	m_user.p_register_button->set_sensitive(false);

	set_schedule(schedule);
	message_error(backend::Error::kSuccess);
}

void Window::set_schedule(const std::shared_ptr<backend::Schedule> &schedule_ptr) {
	if (!schedule_ptr)
		return;
	m_schedule_ptr = schedule_ptr;

	m_header.user_button.set_label(m_schedule_ptr->GetUserPtr()->GetName());
	m_header.insert_button.set_sensitive(true);

	m_user.p_username_label->set_text(m_schedule_ptr->GetUserPtr()->GetName());
	m_body.task_list_box.set_tasks(m_schedule_ptr->GetTasks());
}

void Window::message(Gtk::MessageType type, const char *str) {
	auto info_bar = Gtk::make_managed<Gtk::InfoBar>();
	info_bar->set_message_type(type);
	info_bar->set_show_close_button(true);
	auto label = Gtk::make_managed<Gtk::Label>();
	label->set_text(str);
	label->set_line_wrap(true);
	((Gtk::Box *)info_bar->get_content_area())->pack_start(*label, Gtk::PACK_SHRINK);

	info_bar->show_all();
	m_body.box.pack_start(*info_bar, Gtk::PACK_SHRINK);
	info_bar->show();

	info_bar->signal_response().connect([info_bar, this](int id) {
		if (id == Gtk::RESPONSE_CLOSE) {
			info_bar->hide();
			m_body.box.remove(*info_bar);
		}
	});
}
void Window::message_error(const char *str) { message(Gtk::MESSAGE_ERROR, str); }
void Window::message_error(backend::Error error) {
	message(error == backend::Error::kSuccess ? Gtk::MESSAGE_INFO : Gtk::MESSAGE_ERROR,
	        backend::GetErrorMessage(error));
}

} // namespace gui
