#ifndef SCHEDULITE_TIME_HPP
#define SCHEDULITE_TIME_HPP

#include <cinttypes>
#include <string>
#include <string_view>

namespace backend {

// Basic types
using TimeInt = uint32_t;
struct TimeInfo {
	int year;
	unsigned month, day, hour, minute;
};

// Conversions
TimeInfo ToTimeInfo(TimeInt time_int);
TimeInfo ToTimeInfo(const char *str);
inline TimeInfo ToTimeInfo(const std::string &str) { return ToTimeInfo(str.c_str()); }
std::string ToTimeStr(const TimeInfo &time_info);
inline std::string ToTimeStr(TimeInt time_int) { return ToTimeStr(ToTimeInfo(time_int)); }
TimeInt ToTimeInt(const TimeInfo &time_info);
inline TimeInt ToTimeInt(const char *str) { return ToTimeInt(ToTimeInfo(str)); }
inline TimeInt ToTimeInt(const std::string &str) { return ToTimeInt(str.c_str()); }

// Now
TimeInt GetTimeIntNow();
TimeInfo GetTimeInfoNow();
inline std::string GetTimeStrNow() { return ToTimeStr(GetTimeInfoNow()); }

} // namespace backend

#endif
