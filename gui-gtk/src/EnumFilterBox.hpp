#ifndef SCHEDULITE_GTK_ENUMFILTERBOX_HPP
#define SCHEDULITE_GTK_ENUMFILTERBOX_HPP

#include <gtkmm.h>
#include <vector>

namespace gui {
class EnumFilterBox : public Gtk::Box {
public:
	template <class CONTAINER> explicit EnumFilterBox(const CONTAINER &data) {
		set_border_width(16);
		set_orientation(Gtk::ORIENTATION_VERTICAL);
		set_spacing(8);
		for (const char *str : data) {
			auto checkbutton = Gtk::make_managed<Gtk::CheckButton>();
			checkbutton->set_active();
			checkbutton->set_halign(Gtk::ALIGN_FILL);
			checkbutton->set_valign(Gtk::ALIGN_FILL);
			checkbutton->set_label(str);
			checkbutton->signal_toggled().connect([this, checkbutton] {
				m_signal_modified.emit(checkbutton->get_label().c_str(), checkbutton->get_active());
			});
			pack_start(*checkbutton);
		}
		show_all();
	}
	~EnumFilterBox() override = default;

protected:
	sigc::signal<void(const char *, bool)> m_signal_modified;

public:
	decltype(m_signal_modified) signal_modified() { return m_signal_modified; }
};
} // namespace gui

#endif
