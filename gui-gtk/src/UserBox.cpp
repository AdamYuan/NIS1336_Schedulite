#include "UserBox.hpp"

#include <backend/User.hpp>

namespace gui {

void UserBox::init_widget() {
#include <ui/User.hpp>
	auto user_builder = Gtk::Builder::create_from_string(kUserUIString);
	user_builder->get_widget("grid", m_p_grid);
	user_builder->get_widget("current_user_box", m_p_current_user_box);
	user_builder->get_widget("username_label", m_p_username_label);
	user_builder->get_widget("login_button", m_p_login_button);
	user_builder->get_widget("login_username", m_p_login_username);
	user_builder->get_widget("login_username_combo", m_p_login_username_combo);
	user_builder->get_widget("login_password", m_p_login_password);
	user_builder->get_widget("register_button", m_p_register_button);
	user_builder->get_widget("register_username", m_p_register_username);
	user_builder->get_widget("register_password1", m_p_register_password1);
	user_builder->get_widget("register_password2", m_p_register_password2);

	pack_start(*m_p_grid, Gtk::PACK_SHRINK);
	set_hexpand(false);

	m_p_login_button->signal_clicked().connect([this]() {
		m_signal_login.emit(m_p_login_username->get_text().c_str(), m_p_login_password->get_text().c_str());
	});
	m_p_register_button->signal_clicked().connect([this]() {
		m_signal_register.emit(m_p_register_username->get_text().c_str(), m_p_register_password1->get_text().c_str(),
		                       m_p_register_password2->get_text().c_str());
	});

	m_p_login_username->set_max_length(backend::User::kMaxUsernameLength);
	m_p_register_username->set_max_length(backend::User::kMaxUsernameLength);

	{
		m_p_login_username->signal_changed().connect([this]() {
			m_p_login_button->set_sensitive(!m_p_login_username->get_text().empty() &&
			                                !m_p_login_password->get_text().empty());
		});
		m_p_login_password->signal_changed().connect([this]() {
			m_p_login_button->set_sensitive(!m_p_login_username->get_text().empty() &&
			                                !m_p_login_password->get_text().empty());
		});
	}

	{
		m_p_register_username->signal_changed().connect([this]() {
			m_p_register_button->set_sensitive(!m_p_register_username->get_text().empty() &&
			                                   !m_p_register_password1->get_text().empty() &&
			                                   !m_p_register_password2->get_text().empty());
		});
		m_p_register_password1->signal_changed().connect([this]() {
			m_p_register_button->set_sensitive(!m_p_register_username->get_text().empty() &&
			                                   !m_p_register_password1->get_text().empty() &&
			                                   !m_p_register_password2->get_text().empty());
		});
		m_p_register_password2->signal_changed().connect([this]() {
			m_p_register_button->set_sensitive(!m_p_register_username->get_text().empty() &&
			                                   !m_p_register_password1->get_text().empty() &&
			                                   !m_p_register_password2->get_text().empty());
		});
	}
}
void UserBox::set_current_user(bool have_current_user, const char *name) {
	if (have_current_user) {
		m_p_current_user_box->show();
		m_p_username_label->set_text(name);
	} else
		m_p_current_user_box->hide();
}
void UserBox::set_username_options(const std::vector<std::string> &usernames) {
	m_p_login_username_combo->remove_all();
	for (const std::string &name : usernames)
		m_p_login_username_combo->append(name);
}
void UserBox::goto_current_user_page() {
	((Gtk::Stack *)m_p_current_user_box->get_parent())->set_visible_child(*m_p_current_user_box);
}
void UserBox::clear_login_data() {
	m_p_login_username->set_text("");
	m_p_login_password->set_text("");
}
void UserBox::clear_register_data() {
	m_p_register_username->set_text("");
	m_p_register_password1->set_text("");
	m_p_register_password2->set_text("");
}

} // namespace gui
