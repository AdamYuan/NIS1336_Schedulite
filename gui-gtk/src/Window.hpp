#ifndef SCHEDULITE_GTK_WINDOW_HPP
#define SCHEDULITE_GTK_WINDOW_HPP

#include <gtkmm.h>

#include "TaskFlowBox.hpp"

#include <backend/Schedule.hpp>

namespace gui {

class Window : public Gtk::Window {
public:
	Window();
	~Window() override = default;

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
		Glib::Thread *p_thread;
		Glib::Dispatcher dispatcher;
	} m_sync_thread;

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
		Gtk::MenuButton user_button, status_filter_button, priority_filter_button, type_filter_button;
		Gtk::Button insert_button;
		Gtk::HeaderBar bar;
		Gtk::ButtonBox button_box;
	} m_header;

	struct {
		Gtk::Box box;
		Gtk::ScrolledWindow scrolled_window;
		TaskFlowBox task_list_box;
	} m_body;
};

} // namespace gui

#endif
