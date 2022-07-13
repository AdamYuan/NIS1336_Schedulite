#include <cli/Format.hpp>

#include <iostream>
#include <nowide/convert.hpp>
#include <nowide/iostream.hpp>
#include <tabulate/table.hpp>

namespace cli {
void PrintTasks(const std::vector<backend::Task> &tasks) {
	tabulate::Table table;
	table.add_row({"ID", "Name", "Begin time", "Remind time", "Priority", "Type", "Status"});
	uint32_t row = 1;
	backend::TimeInt time_int_now = backend::GetTimeIntNow();
	for (const auto &task : tasks) {
		auto status = backend::TaskStatusFromTask(task, time_int_now);
		table.add_row({std::to_string(task.id), task.property.name, backend::ToTimeStr(task.property.begin_time),
		               backend::ToTimeStr(task.property.remind_time),
		               backend::StrFromTaskPriority(task.property.priority),
		               backend::StrFromTaskType(task.property.type), backend::StrFromTaskStatus(status)});
		if (status == backend::TaskStatus::kOngoing) {
			table.row(row).format().font_style({tabulate::FontStyle::bold});
		} else if (status == backend::TaskStatus::kDone)
			table.row(row).format().font_style({tabulate::FontStyle::dark});

		if (status != backend::TaskStatus::kDone) {
			table.row(row).format().color(task.property.priority == backend::TaskPriority::kHigh
			                                  ? tabulate::Color::red
			                                  : (task.property.priority == backend::TaskPriority::kMedium
			                                         ? tabulate::Color::yellow
			                                         : tabulate::Color::none));
		}

		++row;
	}
	table.row(0).format().border_top("-").border_bottom("-").border_left("").border_right("").corner("");
	if (row == 2)
		table.row(1).format().border_top("-").border_bottom("-").border_left("").border_right("").corner("");
	else if (row > 2) {
		table.row(1).format().border_top("-").border_bottom(" ").border_left("").border_right("").corner("");
		for (int i = 2; i < row - 1; ++i)
			table.row(i).format().border_top(" ").border_bottom(" ").border_left("").border_right("").corner("");
		table.row(row - 1).format().border_top(" ").border_bottom("-").border_left("").border_right("").corner("");
	}
	nowide::cout << table << std::endl;
}
void PrintError(backend::Error error) {
	if (error != backend::Error::kSuccess)
		printf("ERROR: %s\n", backend::GetErrorMessage(error));
	else
		printf("Success\n");
}
void PrintError(std::string_view error_str) { std::cout << "ERROR: " << error_str << std::endl; }

} // namespace cli
