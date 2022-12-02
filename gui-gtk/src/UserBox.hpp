#ifndef SCHEDULITE_GTK_USERBOX_HPP
#define SCHEDULITE_GTK_USERBOX_HPP

#include <gtkmm.h>

namespace gui {

class UserBox : public Gtk::Box {
public:
	inline UserBox() { init_widget(); }
	~UserBox() override = default;

	void clear_login_data();
	void clear_register_data();
	void set_current_user(bool have_current_user, const char *name = nullptr);
	void goto_current_user_page();
	void set_username_options(const std::vector<std::string> &usernames);

private:
	Gtk::Grid *m_p_grid{};
	Gtk::Box *m_p_current_user_box{};
	Gtk::ComboBoxText *m_p_login_username_combo{};
	Gtk::Label *m_p_username_label{};
	Gtk::Entry *m_p_login_username{}, *m_p_login_password{}, *m_p_register_username{}, *m_p_register_password1{},
	    *m_p_register_password2{};
	Gtk::Button *m_p_login_button{}, *m_p_register_button{};

	void init_widget();

protected:
	sigc::signal<void(const char *, const char *)> m_signal_login;
	sigc::signal<void(const char *, const char *, const char *)> m_signal_register;

public:
	decltype(m_signal_login) signal_login() { return m_signal_login; }
	decltype(m_signal_register) signal_register() { return m_signal_register; }
};

} // namespace gui

#endif
