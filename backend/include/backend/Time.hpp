#ifndef SCHEDULITE_TIME_HPP
#define SCHEDULITE_TIME_HPP

#include <cinttypes>
#include <string>
#include <string_view>

namespace backend {

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
 */
TimeInfo ToTimeInfo(TimeInt time_int);
/**
 * @brief Convert "YYYY/MM/DD hh:mm" string to TimeInfo.
 * @see ToTimeInfo(const std::string &str)
 * @see ToTimeInfo(TimeInt time_int)
 */
TimeInfo ToTimeInfo(const char *str);
/**
 * @brief Convert "YYYY/MM/DD hh:mm" string to TimeInfo.
 * @see ToTimeInfo(const char *str)
 * @see ToTimeInfo(TimeInt time_int)
 */
inline TimeInfo ToTimeInfo(const std::string &str) { return ToTimeInfo(str.c_str()); }

/**
 * @brief Convert TimeInfo to "YYYY/MM/DD hh:mm" string.
 * @see ToTimeStr(TimeInt time_int)
 */
std::string ToTimeStr(const TimeInfo &time_info);
/**
 * @brief Convert TimeInt to "YYYY/MM/DD hh:mm" string.
 * @see ToTimeStr(const TimeInfo &time_info)
 */
inline std::string ToTimeStr(TimeInt time_int) { return ToTimeStr(ToTimeInfo(time_int)); }

/**
 * @brief Convert TimeInfo to TimeInt.
 * @see ToTimeInt(const char *str)
 * @see ToTimeInt(const std::string &str)
 */
TimeInt ToTimeInt(const TimeInfo &time_info);
/**
 * @brief Convert "YYYY/MM/DD hh:mm" string to TimeInt.
 * @see ToTimeInt(const TimeInfo &time_info)
 * @see ToTimeInt(const std::string &str)
 */
inline TimeInt ToTimeInt(const char *str) { return ToTimeInt(ToTimeInfo(str)); }
/**
 * @brief Convert "YYYY/MM/DD hh:mm" string to TimeInt.
 * @see ToTimeInt(const TimeInfo &time_info)
 * @see ToTimeInt(const char *str)
 */
inline TimeInt ToTimeInt(const std::string &str) { return ToTimeInt(str.c_str()); }

/**
 * @brief Get TimeInt of now.
 */
TimeInt GetTimeIntNow();
/**
 * @brief Get TimeInfo of now.
 */
TimeInfo GetTimeInfoNow();
/**
 * @brief Get "YYYY/MM/DD hh:mm" string of now.
 */
inline std::string GetTimeStrNow() { return ToTimeStr(GetTimeInfoNow()); }

} // namespace backend

#endif
