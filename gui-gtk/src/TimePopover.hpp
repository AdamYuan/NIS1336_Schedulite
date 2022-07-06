#ifndef SCHEDULITE_GTK_TIMEPOPOVER_HPP
#define SCHEDULITE_GTK_TIMEPOPOVER_HPP

#include <backend/Time.hpp>
#include <gtkmm.h>

namespace gui {
class TimePopover : public Gtk::Popover {
public:
	inline TimePopover() { init_widget(); }
	void set_time(const backend::TimeInfo &time);

private:
	void init_widget();

	Gtk::Box *m_p_box{};
	Gtk::Calendar *m_p_date{};
	Gtk::SpinButton *m_p_hour{}, *m_p_minute{};
	Gtk::ModelButton *m_p_ok_button{};
};
} // namespace gui

#endif
