#ifndef SCHEDULITE_TIME_HPP
#define SCHEDULITE_TIME_HPP

#include <chrono>
#include <cinttypes>
#include <string>
#include <string_view>

namespace backend {

/** @brief Default clock. */
using Clock = std::chrono::system_clock;
/** @brief Default time interval (1 minute). */
using TimeDuration = std::chrono::minutes;

/** @brief Time point. */
using TimePoint = std::chrono::time_point<Clock, TimeDuration>;
/** @brief Integer to store time. */
using TimeInt = uint32_t;
/** @brief Time and date information. */
struct TimeInfo {
	int year;
	unsigned month, day, hour, minute;
};

/**
 * @brief Convert TimeInt to TimeInfo.
 * @see ToTimeInfo(const char *str)
 * @see ToTimeInfo(const std::string &str)
 * @see ToTimeInfo(const TimePoint &time_point)
 */
TimeInfo ToTimeInfo(TimeInt time_int);
/**
 * @brief Convert "YYYY/MM/DD hh:mm" string to TimeInfo.
 * @see ToTimeInfo(const std::string &str)
 * @see ToTimeInfo(TimeInt time_int)
 * @see ToTimeInfo(const TimePoint &time_point)
 */
TimeInfo ToTimeInfo(const char *str);
/**
 * @brief Convert "YYYY/MM/DD hh:mm" string to TimeInfo.
 * @see ToTimeInfo(const char *str)
 * @see ToTimeInfo(TimeInt time_int)
 * @see ToTimeInfo(const TimePoint &time_point)
 */
inline TimeInfo ToTimeInfo(const std::string &str) { return ToTimeInfo(str.c_str()); }
/**
 * @brief Convert TimePoint to TimeInfo.
 * @see ToTimeInfo(const char *str)
 * @see ToTimeInfo(TimeInt time_int)
 * @see ToTimeInfo(const std::string &str)
 */
TimeInfo ToTimeInfo(const TimePoint &time_point);

/**
 * @brief Convert TimeInfo to "YYYY/MM/DD hh:mm" string.
 * @see ToTimeStr(TimeInt time_int)
 * @see ToTimeStr(const TimePoint &time_point)
 */
std::string ToTimeStr(const TimeInfo &time_info);
/**
 * @brief Convert TimeInt to "YYYY/MM/DD hh:mm" string.
 * @see ToTimeStr(const TimeInfo &time_info)
 * @see ToTimeStr(const TimePoint &time_point)
 */
inline std::string ToTimeStr(TimeInt time_int) { return ToTimeStr(ToTimeInfo(time_int)); }
/**
 * @brief Convert TimePoint to "YYYY/MM/DD hh:mm" string.
 * @see ToTimeStr(TimeInt time_int)
 * @see ToTimeStr(const TimeInfo &time_info)
 */
inline std::string ToTimeStr(const TimePoint &time_point) { return ToTimeStr(ToTimeInfo(time_point)); }

/**
 * @brief Convert TimeInfo to TimeInt.
 * @see ToTimeInt(const char *str)
 * @see ToTimeInt(const std::string &str)
 * @see ToTimeInt(const TimePoint &time_point)
 */
TimeInt ToTimeInt(const TimeInfo &time_info);
/**
 * @brief Convert "YYYY/MM/DD hh:mm" string to TimeInt.
 * @see ToTimeInt(const TimeInfo &time_info)
 * @see ToTimeInt(const std::string &str)
 * @see ToTimeInt(const TimePoint &time_point)
 */
inline TimeInt ToTimeInt(const char *str) { return ToTimeInt(ToTimeInfo(str)); }
/**
 * @brief Convert "YYYY/MM/DD hh:mm" string to TimeInt.
 * @see ToTimeInt(const TimeInfo &time_info)
 * @see ToTimeInt(const char *str)
 * @see ToTimeInt(const TimePoint &time_point)
 */
inline TimeInt ToTimeInt(const std::string &str) { return ToTimeInt(str.c_str()); }
/**
 * @brief Convert TimePoint to TimeInt.
 * @see ToTimeInt(const TimeInfo &time_info)
 * @see ToTimeInt(const char *str)
 * @see ToTimeInt(const std::string &str)
 */
inline TimeInt ToTimeInt(const TimePoint &time_point) { return (time_point - TimePoint{}).count(); }

/**
 * @brief Convert TimeInt to TimePoint.
 * @see ToTimePoint(const TimeInfo &time_info)
 * @see ToTimePoint(const char *str)
 * @see ToTimePoint(const std::string &str)
 */
inline TimePoint ToTimePoint(TimeInt time_int) { return TimePoint{} + TimeDuration(time_int); }
/**
 * @brief Convert TimeInfo to TimePoint.
 * @see ToTimePoint(TimeInt time_int)
 * @see ToTimePoint(const char *str)
 * @see ToTimePoint(const std::string &str)
 */
TimePoint ToTimePoint(const TimeInfo &time_info);
/**
 * @brief Convert "YYYY/MM/DD hh:mm" string to TimePoint.
 * @see ToTimePoint(TimeInt time_int)
 * @see ToTimePoint(const TimeInfo &time_info)
 * @see ToTimePoint(const std::string &str)
 */
inline TimePoint ToTimePoint(const char *str) { return ToTimePoint(ToTimeInfo(str)); }
/**
 * @brief Convert "YYYY/MM/DD hh:mm" string to TimePoint.
 * @see ToTimePoint(TimeInt time_int)
 * @see ToTimePoint(const TimeInfo &time_info)
 * @see ToTimePoint(const char *str)
 */
inline TimePoint ToTimePoint(const std::string &str) { return ToTimePoint(str.c_str()); }

/**
 * @brief Get TimePoint of now.
 */
inline TimePoint GetTimePointNow() { return std::chrono::time_point_cast<TimeDuration>(Clock::now()); }
/**
 * @brief Get TimeInt of now.
 */
inline TimeInt GetTimeIntNow() { return ToTimeInt(GetTimePointNow()); }
/**
 * @brief Get TimeInfo of now.
 */
inline TimeInfo GetTimeInfoNow() { return ToTimeInfo(GetTimePointNow()); }
/**
 * @brief Get "YYYY/MM/DD hh:mm" string of now.
 */
inline std::string GetTimeStrNow() { return ToTimeStr(GetTimeInfoNow()); }

} // namespace backend

#endif
