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

Window::~Window() {
	gtk_widget_destroy(m_body.flap);
	sync_thread_join();
}

void Window::initialize() {
	set_title(backend::kAppName);
	set_default_size(640, 480);

	initialize_header_bar();
	initialize_body();
	set_schedule(nullptr);
	m_header.user_button.set_active(true);

	signal_size_allocate().connect(
	    [this](const Gtk::Allocation &size) { m_body.stack.set_size_request(size.get_width() * 3 / 10, -1); });
}

void Window::initialize_body() {
	m_body.scrolled_window.add(m_body.task_flow_box);
	m_body.task_flow_box.show();

	m_body.flap = hdy_flap_new();

	m_body.stack.add(m_body.user_box);
	m_body.stack.add(m_body.task_insert_box);
	m_body.stack.add(m_body.task_detail_box);

	gtk_box_pack_end(GTK_BOX(m_body.box.gobj()), m_body.flap, false, true, 0);
	hdy_flap_set_content(HDY_FLAP(m_body.flap), GTK_WIDGET(m_body.scrolled_window.gobj()));
	hdy_flap_set_separator(HDY_FLAP(m_body.flap), GTK_WIDGET(m_body.separator.gobj()));
	m_body.separator.show();
	hdy_flap_set_flap(HDY_FLAP(m_body.flap), GTK_WIDGET(m_body.stack.gobj()));
	hdy_flap_set_fold_policy(HDY_FLAP(m_body.flap), HDY_FLAP_FOLD_POLICY_ALWAYS);
	hdy_flap_set_transition_type(HDY_FLAP(m_body.flap), HDY_FLAP_TRANSITION_TYPE_UNDER);
	gtk_widget_show(GTK_WIDGET(m_body.flap));

	g_signal_connect(G_OBJECT(m_body.flap), "child-switched", G_CALLBACK(Window::flap_switched), this);

	m_body.stack.show();

	m_body.scrolled_window.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
	m_body.scrolled_window.show();

	add(m_body.box);
	m_body.box.set_orientation(Gtk::ORIENTATION_VERTICAL);
	m_body.box.show();

	m_body.task_flow_box.signal_task_selected().connect([this](const backend::Task &task) {
		m_body.task_detail_box.set_task(task);
		goto_detail_page();
	});

	m_body.task_insert_box.signal_task_inserted().connect([this](const backend::TaskProperty &property) {
		if (!m_schedule_ptr)
			return;
		auto error = m_schedule_ptr->TaskInsert(property);
		if (error == backend::Error::kSuccess) {
			goto_list_page();
			m_body.task_insert_box.restore();
		} else
			message_error(error);
	});

	m_body.task_detail_box.signal_task_edited().connect(
	    [this](uint32_t id, const backend::TaskProperty &property, backend::TaskPropertyMask mask) {
		    if (!m_schedule_ptr)
			    return;
		    auto error = m_schedule_ptr->TaskEdit(id, property, mask);
		    if (error != backend::Error::kSuccess) {
			    message_error(error);
		    }
	    });

	m_body.task_detail_box.signal_task_erased().connect([this](uint32_t id) {
		if (!m_schedule_ptr)
			return;
		auto error = m_schedule_ptr->TaskErase(id);
		if (error != backend::Error::kSuccess) {
			message_error(error);
		}
	});

	m_body.user_box.signal_login().connect(sigc::mem_fun(*this, &Window::user_login));
	m_body.user_box.signal_register().connect(sigc::mem_fun(*this, &Window::user_register));
}

void Window::initialize_header_bar() {
	set_titlebar(m_header.bar);
	{
		m_header.bar.set_show_close_button(TRUE);
		m_header.bar.set_title(backend::kAppName);

		m_header.bar.pack_start(m_header.user_button);
		m_header.bar.pack_start(m_header.insert_button);
		m_header.bar.pack_end(m_header.more_button);

		m_header.user_button.set_always_show_image(true);
		m_header.user_button.set_image_from_icon_name("user-info", Gtk::ICON_SIZE_DND);
		m_header.user_button.set_tooltip_text("User");
		m_header.user_button.signal_clicked().connect([this]() {
			if (m_header.user_button.get_active()) {
				m_header.insert_button.set_active(false);
				goto_user_page();
			} else if (!m_header.insert_button.get_active())
				goto_list_page();
		});

		m_header.insert_button.set_always_show_image(true);
		m_header.insert_button.set_image_from_icon_name("document-new-symbolic", Gtk::ICON_SIZE_DND);
		m_header.insert_button.set_tooltip_text("Insert Task");
		m_header.insert_button.signal_clicked().connect([this]() {
			if (m_header.insert_button.get_active()) {
				m_header.user_button.set_active(false);
				goto_insert_page();
			} else if (!m_header.user_button.get_active())
				goto_list_page();
		});

		m_header.more_button.set_image_from_icon_name("view-more", Gtk::ICON_SIZE_DND);
		m_header.more_button.set_tooltip_text("More");

		// Filters
		m_header.status_filter_popover.add(m_header.status_filter_box);
		m_header.status_filter_box.show();
		m_header.status_filter_box.signal_modified().connect([this](const char *str, bool activate) {
			m_body.task_flow_box.set_status_filter(backend::TaskStatusFromStr(str), activate);
			m_body.task_flow_box.refilter();
		});
		m_header.type_filter_popover.add(m_header.type_filter_box);
		m_header.type_filter_box.show();
		m_header.type_filter_box.signal_modified().connect([this](const char *str, bool activate) {
			m_body.task_flow_box.set_type_filter(backend::TaskTypeFromStr(str), activate);
			m_body.task_flow_box.refilter();
		});
		m_header.priority_filter_popover.add(m_header.priority_filter_box);
		m_header.priority_filter_box.show();
		m_header.priority_filter_box.signal_modified().connect([this](const char *str, bool activate) {
			m_body.task_flow_box.set_priority_filter(backend::TaskPriorityFromStr(str), activate);
			m_body.task_flow_box.refilter();
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
	}
}

void Window::user_login(const char *username, const char *password) {
	if (m_schedule_ptr && username == m_schedule_ptr->GetUserPtr()->GetName()) {
		message(Gtk::MESSAGE_WARNING, "User already logged in");
		return;
	}
	std::shared_ptr<backend::User> user;
	std::shared_ptr<backend::Schedule> schedule;
	backend::Error error;

	std::tie(user, error) = backend::User::Login(m_instance_ptr, username, password);
	if (!user) {
		message_error(error);
		return;
	}
	std::tie(schedule, error) = backend::Schedule::Acquire(user);
	if (!schedule) {
		message_error(error);
		return;
	}

	set_schedule(schedule);
	m_body.user_box.clear_login_data();
	m_body.user_box.goto_current_user_page();

	goto_list_page();

	message_error(backend::Error::kSuccess);
}

void Window::user_register(const char *username, const char *password1, const char *password2) {
	if (std::string_view{password1} != password2) {
		message_error("Passwords not equal");
		return;
	}
	std::shared_ptr<backend::User> user;
	std::shared_ptr<backend::Schedule> schedule;
	backend::Error error;
	std::tie(user, error) = backend::User::Register(m_instance_ptr, username, password1);
	if (!user) {
		message_error(error);
		return;
	}
	std::tie(schedule, error) = backend::Schedule::Acquire(user);
	if (!schedule) {
		message_error(error);
		return;
	}

	set_schedule(schedule);
	m_body.user_box.clear_register_data();
	m_body.user_box.goto_current_user_page();

	goto_list_page();

	message_error(backend::Error::kSuccess);
}

void Window::set_schedule(const std::shared_ptr<backend::Schedule> &schedule_ptr) {
	std::atomic_store(&m_schedule_ptr, schedule_ptr);
	if (!schedule_ptr) {
		m_body.user_box.set_current_user(false);
		m_header.insert_button.set_sensitive(false);
		m_header.filter_button_box.set_sensitive(false);
	} else {
		m_body.user_box.set_current_user(true, m_schedule_ptr->GetUserPtr()->GetName().c_str());
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
			m_body.task_flow_box.set_tasks(tasks);
			if (m_body.task_detail_box.is_visible() && !m_body.task_detail_box.update_from_tasks(tasks)) {
				goto_list_page();
			}
		}
	});
}

void Window::goto_list_page() {
	m_body.user_box.hide();
	m_body.task_insert_box.hide();
	m_body.task_detail_box.hide();
	hdy_flap_set_reveal_flap(HDY_FLAP(m_body.flap), false);
}
void Window::goto_user_page() {
	if (m_schedule_ptr)
		m_body.user_box.goto_current_user_page();
	m_body.user_box.set_username_options(m_instance_ptr->FetchUsernames());

	m_body.user_box.show();
	m_body.task_insert_box.hide();
	m_body.task_detail_box.hide();
	m_body.stack.set_visible_child(m_body.user_box);

	hdy_flap_set_flap_position(HDY_FLAP(m_body.flap), GTK_PACK_START);
	hdy_flap_set_reveal_flap(HDY_FLAP(m_body.flap), true);
}
void Window::goto_insert_page() {
	m_body.user_box.hide();
	m_body.task_insert_box.show();
	m_body.task_detail_box.hide();
	m_body.stack.set_visible_child(m_body.task_insert_box);

	hdy_flap_set_flap_position(HDY_FLAP(m_body.flap), GTK_PACK_START);
	hdy_flap_set_reveal_flap(HDY_FLAP(m_body.flap), true);

	/*m_header.back_button.show();
	m_header.user_button.hide();
	m_header.insert_button.hide();
	m_header.filter_button_box.hide();
	m_header.bar.set_title("Insert Task"); */
}
void Window::goto_detail_page() {
	m_body.user_box.hide();
	m_body.task_insert_box.hide();
	m_body.task_detail_box.show();
	m_body.stack.set_visible_child(m_body.task_detail_box);

	hdy_flap_set_flap_position(HDY_FLAP(m_body.flap), GTK_PACK_END);
	hdy_flap_set_reveal_flap(HDY_FLAP(m_body.flap), true);

	/* m_header.back_button.show();
	m_header.user_button.hide();
	m_header.insert_button.hide();
	m_header.filter_button_box.hide();
	m_header.bar.set_title("Task Detail"); */
}

void Window::flap_switched(GtkWidget *flap, guint index, gint64 duration, Window *window) {
	if (index == 0) {
		if (window->m_header.user_button.get_active())
			window->m_header.user_button.set_active(false);
		if (window->m_header.insert_button.get_active())
			window->m_header.insert_button.set_active(false);
	}
}

} // namespace gui
