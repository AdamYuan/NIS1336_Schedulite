#include <cli/Format.hpp>

#include <iostream>
#include <tabulate/table.hpp>

namespace cli {
void PrintTasks(const std::vector<backend::Task> &tasks) {
	tabulate::Table table;
	table.add_row({"ID", "Name", "Begin time", "Remind time", "Priority", "Type", "Status"});
	uint32_t row = 1;
	backend::TimeInt time_int_now = backend::GetTimeIntNow();
	for (const auto &task : tasks) {
		auto status = backend::TaskStatusFromTask(task, time_int_now);
		table.add_row({std::to_string(task.id), task.name, backend::ToTimeStr(task.begin_time),
		               backend::ToTimeStr(task.remind_time), backend::StrFromTaskPriority(task.priority),
		               backend::StrFromTaskType(task.type), backend::StrFromTaskStatus(status)});
		if (status == backend::TaskStatus::kBegun)
			table.row(row).format().font_style({tabulate::FontStyle::bold});
		else if (status == backend::TaskStatus::kDone)
			table.row(row).format().font_style({tabulate::FontStyle::dark});
		++row;
	}
	std::cout << table << std::endl;
}
} // namespace cli
