#include "Window.hpp"

#include "Icon.hpp"
#include <backend/Environment.hpp>

namespace gui {

Window::Window() {
	m_instance_ptr = backend::Instance::Create();
	sync_thread_init();
	sync_thread_launch();
	initialize();
}

Window::~Window() { sync_thread_join(); }

void Window::initialize() {
	set_title(backend::kAppName);
	set_default_size(640, 480);

	initialize_header_bar();
	initialize_user_panel();
	initialize_body();
	set_schedule(nullptr);
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

	m_user.p_login_username->set_max_length(backend::User::kMaxUsernameLength);
	m_user.p_register_username->set_max_length(backend::User::kMaxUsernameLength);

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

		m_header.bar.pack_end(m_header.more_button);

		m_header.right_button_box.set_layout(Gtk::BUTTONBOX_EXPAND);

		m_header.user_button.set_always_show_image(true);
		m_header.user_button.set_image_from_icon_name("user-info", Gtk::ICON_SIZE_DND);
		m_header.user_button.set_tooltip_text("User");

		m_header.insert_button.set_always_show_image(true);
		m_header.insert_button.set_image_from_icon_name("document-new-symbolic", Gtk::ICON_SIZE_DND);
		m_header.insert_button.set_tooltip_text("Insert Task");

		m_header.more_button.set_image_from_icon_name("view-more", Gtk::ICON_SIZE_DND);
		m_header.more_button.set_tooltip_text("More");

		// Filters
		m_header.status_filter_popover.add(m_header.status_filter_box);
		m_header.status_filter_box.show();
		m_header.status_filter_box.signal_modified().connect([this](const char *str, bool activate) {
			m_body.task_list_box.set_status_filter(backend::TaskStatusFromStr(str), activate);
			m_body.task_list_box.refilter();
		});
		m_header.type_filter_popover.add(m_header.type_filter_box);
		m_header.type_filter_box.show();
		m_header.type_filter_box.signal_modified().connect([this](const char *str, bool activate) {
			m_body.task_list_box.set_type_filter(backend::TaskTypeFromStr(str), activate);
			m_body.task_list_box.refilter();
		});
		m_header.priority_filter_popover.add(m_header.priority_filter_box);
		m_header.priority_filter_box.show();
		m_header.priority_filter_box.signal_modified().connect([this](const char *str, bool activate) {
			m_body.task_list_box.set_priority_filter(backend::TaskPriorityFromStr(str), activate);
			m_body.task_list_box.refilter();
		});

		// Filter buttons
		m_header.status_filter_button.set_always_show_image(true);
		m_header.status_filter_button.set_label("Status");
		m_header.status_filter_button.set_popover(m_header.status_filter_popover);
		m_header.status_filter_button.set_image_from_icon_name(GetTaskStatusIconName(backend::TaskStatus::kOngoing),
		                                                       Gtk::ICON_SIZE_LARGE_TOOLBAR);
		m_header.priority_filter_button.set_always_show_image(true);
		m_header.priority_filter_button.set_label("Priority");
		m_header.priority_filter_button.set_popover(m_header.priority_filter_popover);
		m_header.priority_filter_button.set_image_from_icon_name(GetTaskPriorityIconName(backend::kDefaultTaskPriority),
		                                                         Gtk::ICON_SIZE_LARGE_TOOLBAR);
		m_header.type_filter_button.set_always_show_image(true);
		m_header.type_filter_button.set_label("Type");
		m_header.type_filter_button.set_popover(m_header.type_filter_popover);
		m_header.type_filter_button.set_image_from_icon_name(GetTaskTypeIconName(backend::kDefaultTaskType),
		                                                     Gtk::ICON_SIZE_LARGE_TOOLBAR);
		m_header.filter_button_box.set_homogeneous(true);
		m_header.filter_button_box.set_layout(Gtk::BUTTONBOX_EXPAND);
		m_header.filter_button_box.add(m_header.priority_filter_button);
		m_header.filter_button_box.add(m_header.type_filter_button);
		m_header.filter_button_box.add(m_header.status_filter_button);

		m_header.bar.set_custom_title(m_header.filter_button_box);

		m_header.bar.show_all();
		m_header.bar.set_size_request(-1, 64);
	}
}

void Window::login_button_click() {
	std::string username = (std::string)m_user.p_login_username->get_text();
	if (m_schedule_ptr && username == m_schedule_ptr->GetUserPtr()->GetName()) {
		message(Gtk::MESSAGE_WARNING, "User already logged in");
		return;
	}
	std::shared_ptr<backend::User> user;
	std::shared_ptr<backend::Schedule> schedule;
	backend::Error error;

	std::tie(user, error) =
	    backend::User::Login(m_instance_ptr, username, (std::string)m_user.p_login_password->get_text());
	if (!user) {
		message_error(error);
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
	std::atomic_store(&m_schedule_ptr, schedule_ptr);
	if (!schedule_ptr) {
		m_user.p_username_label->set_label("");
		// m_header.user_button.set_label("");
		m_header.insert_button.set_sensitive(false);
		m_header.filter_button_box.set_sensitive(false);
	} else {
		m_user.p_username_label->set_label(m_schedule_ptr->GetUserPtr()->GetName());
		// m_header.user_button.set_label(m_schedule_ptr->GetUserPtr()->GetName());
		m_header.insert_button.set_sensitive(true);
		m_header.filter_button_box.set_sensitive(true);
	}
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

void Window::sync_thread_func() {
	std::mutex cv_mutex;
	std::unique_lock cv_lock{cv_mutex};
	while (m_sync_thread.run.load(std::memory_order_acquire)) {
		m_sync_thread.sync_dispatcher();
		m_sync_thread.cv.wait_for(cv_lock, std::chrono::milliseconds(100),
		                          [this]() { return !m_sync_thread.run.load(std::memory_order_acquire); });
	}
}
void Window::sync_thread_join() {
	if (m_sync_thread.thread.joinable()) {
		m_sync_thread.run.store(false, std::memory_order_release);
		m_sync_thread.cv.notify_all();
		m_sync_thread.thread.join();
	}
}
void Window::sync_thread_launch() {
	m_sync_thread.run.store(true, std::memory_order_release);
	m_sync_thread.thread = std::thread(&Window::sync_thread_func, this);
}

void Window::sync_thread_init() {
	m_sync_thread.sync_dispatcher.connect([this]() {
		auto schedule = std::atomic_load(&m_schedule_ptr);
		if (!schedule)
			return;
		bool updated;
		std::vector<backend::Task> tasks;
		tasks = schedule->GetTasks(&updated);
		if (updated) {
			m_body.task_list_box.set_tasks(tasks);
		}
	});
}

} // namespace gui
