#ifndef SCHEDULITE_GTK_TIMEPOPOVER_HPP
#define SCHEDULITE_GTK_TIMEPOPOVER_HPP

#include <backend/Time.hpp>
#include <gtkmm.h>

namespace gui {
class TimePopover : public Gtk::Popover {
public:
	inline TimePopover(bool use_ok_button) : m_use_ok_button{use_ok_button} {
		init_widget();
	}
	void set_time(const backend::TimeInfo &time);
	backend::TimeInfo get_time();

	sigc::signal<void(const backend::TimeInfo &)> signal_time_selected() { return m_signal_time_selected; };

protected:
	sigc::signal<void(const backend::TimeInfo &)> m_signal_time_selected;

private:
	void init_widget();

	bool m_use_ok_button;

	Gtk::Box *m_p_box{};
	Gtk::Calendar *m_p_date{};
	Gtk::SpinButton *m_p_hour{}, *m_p_minute{};
	Gtk::Button *m_p_ok_button{};
};
} // namespace gui

#endif
