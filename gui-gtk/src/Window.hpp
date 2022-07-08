#ifndef SCHEDULITE_GTK_WINDOW_HPP
#define SCHEDULITE_GTK_WINDOW_HPP

#include "EnumFilterBox.hpp"
#include "TaskDetailBox.hpp"
#include "TaskFlowBox.hpp"
#include "TaskInsertBox.hpp"
#include "UserBox.hpp"
#include <backend/Schedule.hpp>
#include <gtkmm.h>
#include <handy.h>

#include <atomic>
#include <condition_variable>
#include <thread>

namespace gui {

class Window : public Gtk::Window {
public:
	Window();
	~Window() override;

protected:
	std::shared_ptr<backend::Instance> m_instance_ptr;
	std::shared_ptr<backend::Schedule> m_schedule_ptr;

	void message(Gtk::MessageType type, const char *str);
	void message_error(backend::Error error);
	void message_error(const char *str);

	void initialize();
	void initialize_header_bar();
	void initialize_body();
	void user_login(const char *username, const char *password);
	void user_register(const char *username, const char *password1, const char *password2);

	void set_schedule(const std::shared_ptr<backend::Schedule> &schedule_ptr);

	struct {
		Glib::Dispatcher sync_dispatcher;
		std::atomic_bool run;
		std::condition_variable cv;
		std::thread thread;
	} m_sync_thread;
	void sync_thread_init();
	void sync_thread_func();
	void sync_thread_join();
	void sync_thread_launch();

	void goto_list_page();
	void goto_user_page();
	void goto_insert_page();
	void goto_detail_page();

	struct {
		Gtk::MenuButton status_filter_button, priority_filter_button, type_filter_button, more_button;
		Gtk::ToggleButton user_button, insert_button;
		Gtk::HeaderBar bar;
		Gtk::ButtonBox filter_button_box;
		Gtk::Popover status_filter_popover, priority_filter_popover, type_filter_popover;
		EnumFilterBox status_filter_box{backend::GetTaskStatusStrings()},
		    priority_filter_box{backend::GetTaskPriorityStrings()}, type_filter_box{backend::GetTaskTypeStrings()};
	} m_header;

	struct {
		Gtk::Box box;
		Gtk::Stack stack;

		GtkWidget *flap;

		Gtk::ScrolledWindow scrolled_window;
		TaskFlowBox task_flow_box;
		TaskInsertBox task_insert_box;
		TaskDetailBox task_detail_box;
		UserBox user_box;
	} m_body;

	static void flap_switched(GtkWidget *flap, guint index, gint64 duration, Window *window);
};

} // namespace gui

#endif
