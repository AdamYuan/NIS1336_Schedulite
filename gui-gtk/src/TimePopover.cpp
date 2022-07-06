#include "TimePopover.hpp"

namespace gui {

void TimePopover::init_widget() {
#include <ui/Time.hpp>
	auto builder = Gtk::Builder::create_from_string(kTimeUIString);
	builder->get_widget("box", m_p_box);
	builder->get_widget("ok", m_p_ok_button);
	builder->get_widget("date", m_p_date);
	builder->get_widget("hour", m_p_hour);
	builder->get_widget("minute", m_p_minute);
	add(*m_p_box);

	m_p_ok_button->signal_clicked().connect([this]() { m_signal_time_selected.emit(get_time()); });
}

void TimePopover::set_time(const backend::TimeInfo &time) {
	m_p_date->select_month(time.month, time.year);
	m_p_date->select_day(time.day);
	m_p_hour->set_value(time.hour);
	m_p_minute->set_value(time.minute);
}

backend::TimeInfo TimePopover::get_time() {
	Glib::Date date;
	m_p_date->get_date(date);
	return {date.get_year(), date.get_month(), date.get_day(), (unsigned)m_p_hour->get_value_as_int(),
	        (unsigned)m_p_minute->get_value_as_int()};
}

} // namespace gui
