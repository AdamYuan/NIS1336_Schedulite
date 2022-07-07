#ifndef SCHEDULITE_GTK_WINDOW_HPP
#define SCHEDULITE_GTK_WINDOW_HPP

#include "EnumFilterBox.hpp"
#include "TaskDetailBox.hpp"
#include "TaskFlowBox.hpp"
#include "TaskInsertBox.hpp"
#include <backend/Schedule.hpp>
#include <gtkmm.h>

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
	void initialize_user_panel();
	void initialize_body();
	void login_button_click();
	void register_button_click();

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
	void goto_insert_page();
	void goto_detail_page();

	struct {
		Gtk::Popover *p_popover{};
		Gtk::Box *p_current_user_box{};
		Gtk::ComboBoxText *p_login_username_combo{};
		Gtk::Label *p_username_label{};
		Gtk::Entry *p_login_username{}, *p_login_password{}, *p_register_username{}, *p_register_password1{},
		    *p_register_password2{};
		Gtk::Button *p_login_button{}, *p_register_button{};
	} m_user;

	struct {
		Gtk::MenuButton user_button, status_filter_button, priority_filter_button, type_filter_button, more_button;
		Gtk::Button insert_button, back_button;
		Gtk::HeaderBar bar;
		Gtk::ButtonBox filter_button_box;
		Gtk::Popover status_filter_popover, priority_filter_popover, type_filter_popover;
		EnumFilterBox status_filter_box{backend::GetTaskStatusStrings()},
		    priority_filter_box{backend::GetTaskPriorityStrings()}, type_filter_box{backend::GetTaskTypeStrings()};
	} m_header;

	struct {
		Gtk::Box box;
		Gtk::Stack stack;

		Gtk::ScrolledWindow scrolled_window;
		TaskFlowBox task_flow_box;
		TaskInsertBox task_insert_box;
		TaskDetailBox task_detail_box;
	} m_body;
};

} // namespace gui

#endif
