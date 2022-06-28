#include <backend/Time.hpp>

#include <chrono>
#include <cstdio>
#include <date/date.h>
#include <date/tz.h>

namespace backend {

using Clock = std::chrono::system_clock;
using TimeDuration = std::chrono::minutes;
using TimePoint = std::chrono::time_point<Clock, TimeDuration>;

inline TimeInt ToTimeInt(const TimePoint &time_point) { return (time_point - TimePoint{}).count(); }
inline TimeInfo ToTimeInfo(const TimePoint &time_point) {
	auto zoned_time_point = date::zoned_time{date::current_zone(), time_point}.get_local_time();
	auto date_point = date::floor<date::days>(zoned_time_point);
	date::year_month_day ymd{date_point};
	date::hh_mm_ss hms{zoned_time_point - date_point};
	return {(int)ymd.year(), (unsigned)ymd.month(), (unsigned)ymd.day(), (unsigned)hms.hours().count(),
	        (unsigned)hms.minutes().count()};
}
inline TimePoint ToTimePoint(TimeInt time_int) { return TimePoint{} + TimeDuration(time_int); }
inline TimePoint ToTimePoint(const TimeInfo &time_info) {
	auto zoned_time_point =
	    date::local_days{date::day(time_info.day) / date::month(time_info.month) / date::year(time_info.year)} +
	    std::chrono::hours(time_info.hour) + std::chrono::minutes(time_info.minute);
	return std::chrono::time_point_cast<TimeDuration>(
	    date::zoned_time{date::current_zone(), zoned_time_point}.get_sys_time());
}

inline TimePoint GetTimePointNow() { return std::chrono::time_point_cast<TimeDuration>(Clock::now()); }

// Definitions
TimeInfo ToTimeInfo(uint32_t time_int) { return ToTimeInfo(ToTimePoint(time_int)); }
uint32_t ToTimeInt(const TimeInfo &time_info) { return ToTimeInt(ToTimePoint(time_info)); }

std::string ToTimeStr(const TimeInfo &time_info) {
	thread_local char buf[32];
	sprintf(buf, "%04d/%02u/%02u %02u:%02u", time_info.year, time_info.month, time_info.day, time_info.hour,
	        time_info.minute);
	return {buf};
}
TimeInfo ToTimeInfo(const char *str) {
	TimeInfo time_info{};
	sscanf(str, "%04d/%02u/%02u%02u:%02u", &time_info.year, &time_info.month, &time_info.day, &time_info.hour,
	       &time_info.minute);
	return time_info;
}

uint32_t GetTimeIntNow() { return ToTimeInt(GetTimePointNow()); }
TimeInfo GetTimeInfoNow() { return ToTimeInfo(GetTimePointNow()); }

} // namespace backend
