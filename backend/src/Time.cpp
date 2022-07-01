#include <backend/Time.hpp>

#include <chrono>
#include <cstdio>
#include <ctime>

namespace backend {

using Clock = std::chrono::system_clock;
using TimeDuration = std::chrono::minutes;
using TimePoint = std::chrono::time_point<Clock, TimeDuration>;

inline static TimeInt ToTimeInt(const TimePoint &time_point) { return (time_point - TimePoint{}).count(); }
inline static TimeInfo ToTimeInfo(const TimePoint &time_point) {
	std::time_t t = Clock::to_time_t(time_point);
	std::tm tm = *std::localtime(&t);
	return {tm.tm_year + 1900, (unsigned)tm.tm_mon + 1, (unsigned)tm.tm_mday, (unsigned)tm.tm_hour,
	        (unsigned)tm.tm_min};
}
inline static TimePoint ToTimePoint(TimeInt time_int) { return TimePoint{} + TimeDuration(time_int); }
inline static TimePoint ToTimePoint(const TimeInfo &time_info) {
	std::tm tm{};
	tm.tm_year = time_info.year - 1900;
	tm.tm_mon = (int)time_info.month - 1;
	tm.tm_mday = (int)time_info.day;
	tm.tm_hour = (int)time_info.hour;
	tm.tm_min = (int)time_info.minute;
	tm.tm_isdst = 0;
	return std::chrono::time_point_cast<TimeDuration>(Clock::from_time_t(std::mktime(&tm)));
}

inline static TimePoint GetTimePointNow() { return std::chrono::time_point_cast<TimeDuration>(Clock::now()); }

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
	sscanf(str, "%d/%u/%u%u:%u", &time_info.year, &time_info.month, &time_info.day, &time_info.hour, &time_info.minute);
	return time_info;
}

uint32_t GetTimeIntNow() { return ToTimeInt(GetTimePointNow()); }
TimeInfo GetTimeInfoNow() { return ToTimeInfo(GetTimePointNow()); }

} // namespace backend
