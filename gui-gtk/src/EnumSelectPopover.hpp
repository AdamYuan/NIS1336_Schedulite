#ifndef SCHEDULITE_ENUMSELECTPOPOVER_HPP
#define SCHEDULITE_ENUMSELECTPOPOVER_HPP

#include <gtkmm.h>

namespace gui {
class EnumSelectPopover : public Gtk::Popover {
public:
	template <class CONTAINER> explicit EnumSelectPopover(const CONTAINER &data) { init_widget(data); }
	~EnumSelectPopover() override = default;

private:
	sigc::signal<void(const char *)> m_signal_selected;

	template <class CONTAINER> void init_widget(const CONTAINER &data) {
		set_border_width(16);
		auto box = Gtk::make_managed<Gtk::Box>();
		box->set_orientation(Gtk::ORIENTATION_VERTICAL);
		box->set_spacing(8);
		for (const char *str : data) {
			auto button = Gtk::make_managed<Gtk::ModelButton>();
			button->set_halign(Gtk::ALIGN_FILL);
			button->set_valign(Gtk::ALIGN_FILL);
			button->set_label(str);
			button->signal_clicked().connect([this, button] { m_signal_selected.emit(button->get_label().c_str()); });
			box->pack_start(*button);
		}
		add(*box);
		box->show_all();
	}

public:
	decltype(m_signal_selected) signal_selected() { return m_signal_selected; }
};
} // namespace gui

#endif
