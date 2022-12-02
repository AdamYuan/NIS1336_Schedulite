#include <gtkmm.h>

#include "Window.hpp"

int main(int argc, char *argv[]) {
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.adamyuan.schedulite");

	gui::Window window;
	return app->run(window);
}