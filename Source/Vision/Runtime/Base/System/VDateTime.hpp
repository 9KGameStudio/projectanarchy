/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VDateTime.hpp

#ifndef VBASE_VDATETIME_HPP
#define VBASE_VDATETIME_HPP

#define VDATETIME_FORMAT_ISO8601_DATE_AND_TIME  "%Y-%m-%dT%H:%M:%S"
#define VDATETIME_FORMAT_ISO8601_TIME           "%H:%M:%S"
#define VDATETIME_FORMAT_ISO8601_DATE           "%Y-%m-%d"

#define VDATETIME_FORMAT_HUMAN_DATE_AND_TIME    "%c"
#define VDATETIME_FORMAT_HUMAN_TIME             "%X"
#define VDATETIME_FORMAT_HUMAN_DATE             "%x"

/// \brief A class representing a time span.
class VBASE_IMPEXP_CLASS VTimeSpan
{
  friend class VDateTime;

public:
  /// \brief Creates a time span from a given number of days.
  static VTimeSpan FromDays(__int64 iDays)
  {
    VTimeSpan span;
    span.m_iTotalNanoSeconds = iDays * 24 * 60 * 60 * 1000 * 1000 * 1000;
    return span;
  }

  /// \brief Creates a time span from a given number of days.
  static VTimeSpan FromDays(double fDays)
  {
    VTimeSpan span;
    span.m_iTotalNanoSeconds = static_cast<__int64>(fDays * 24.0 * 60.0 * 60.0 * 1000.0 * 1000.0 * 1000.0);
    return span;
  }

  /// \brief Creates a time span from a given number of hours.
  static VTimeSpan FromHours(__int64 iHours)
  {
    VTimeSpan span;
    span.m_iTotalNanoSeconds = iHours * 60 * 60 * 1000 * 1000 * 1000;
    return span;
  }

  /// \brief Creates a time span from a given number of hours.
  static VTimeSpan FromHours(double fHours)
  {
    VTimeSpan span;
    span.m_iTotalNanoSeconds = static_cast<__int64>(fHours * 60.0 * 60.0 * 1000.0 * 1000.0 * 1000.0);
    return span;
  }

  /// \brief Creates a time span from a given number of minutes.
  static VTimeSpan FromMinutes(__int64 iMinutes)
  {
    VTimeSpan span;
    span.m_iTotalNanoSeconds = iMinutes * 60 * 1000 * 1000 * 1000;
    return span;
  }

  /// \brief Creates a time span from a given number of minutes.
  static VTimeSpan FromMinutes(double fMinutes)
  {
    VTimeSpan span;
    span.m_iTotalNanoSeconds = static_cast<__int64>(fMinutes * 60.0 * 1000.0 * 1000.0 * 1000.0);
    return span;
  }

  /// \brief Creates a time span from a given number of seconds.
  static VTimeSpan FromSeconds(__int64 iSeconds)
  {
    VTimeSpan span;
    span.m_iTotalNanoSeconds = iSeconds * 1000 * 1000 * 1000;
    return span;
  }

  /// \brief Creates a time span from a given number of seconds.
  static VTimeSpan FromSeconds(double fSeconds)
  {
    VTimeSpan span;
    span.m_iTotalNanoSeconds = static_cast<__int64>(fSeconds * 1000.0 * 1000.0 * 1000.0);
    return span;
  }

  /// \brief Creates a time span from a given number of milliseconds.
  static VTimeSpan FromMilliSeconds(__int64 iMilliSeconds)
  {
    VTimeSpan span;
    span.m_iTotalNanoSeconds = iMilliSeconds * 1000 * 1000;
    return span;
  }

  /// \brief Creates a time span from a given number of milliseconds.
  static VTimeSpan FromMilliSeconds(double fMilliSeconds)
  {
    VTimeSpan span;
    span.m_iTotalNanoSeconds = static_cast<__int64>(fMilliSeconds * 1000.0 * 1000.0);
    return span;
  }

  /// \brief Creates a time span from a given number of microseconds.
  static VTimeSpan FromMicroSeconds(__int64 iMicroSeconds)
  {
    VTimeSpan span;
    span.m_iTotalNanoSeconds = iMicroSeconds * 1000;
    return span;
  }

  /// \brief Creates a time span from a given number of microseconds.
  static VTimeSpan FromMicroSeconds(double fMicroSeconds)
  {
    VTimeSpan span;
    span.m_iTotalNanoSeconds = static_cast<__int64>(fMicroSeconds * 1000.0);
    return span;
  }

  /// \brief Creates a time span from a given number of nanoseconds.
  static VTimeSpan FromNanoSeconds(__int64 iNanoSeconds)
  {
    VTimeSpan span;
    span.m_iTotalNanoSeconds = iNanoSeconds;
    return span;
  }

  /// \brief Creates a time span from a given number of nanoseconds.
  static VTimeSpan FromNanoSeconds(double fNanoSeconds)
  {
    VTimeSpan span;
    span.m_iTotalNanoSeconds = static_cast<__int64>(fNanoSeconds);
    return span;
  }

  /// \brief Returns the total number of days of the time span.
  double TotalDays() const
  {
    // Prevent optimizer from turning division into multiplication
    static volatile double divisor = 24.0 * 60.0 * 60.0 * 1000.0 * 1000.0 * 1000.0;
    return m_iTotalNanoSeconds / divisor;
  }

  /// \brief Returns the total number of hours of the time span.
  double TotalHours() const
  {
    // Prevent optimizer from turning division into multiplication
    static volatile double divisor = 60.0 * 60.0 * 1000.0 * 1000.0 * 1000.0;
    return m_iTotalNanoSeconds / divisor;
  }

  /// \brief Returns the total number of minutes of the time span.
  double TotalMinutes() const
  {
    // Prevent optimizer from turning division into multiplication
    static volatile double divisor = 60.0 * 1000.0 * 1000.0 * 1000.0;
    return m_iTotalNanoSeconds / divisor;
  }

  /// \brief Returns the total number of seconds of the time span.
  double TotalSeconds() const
  {
    // Prevent optimizer from turning division into multiplication
    static volatile double divisor = 1000.0 * 1000.0 * 1000.0;
    return m_iTotalNanoSeconds / divisor;
  }

  /// \brief Returns the total number of milliseconds of the time span.
  double TotalMilliSeconds() const
  {
    // Prevent optimizer from turning division into multiplication
    static volatile double divisor = 1000.0 * 1000.0;
    return m_iTotalNanoSeconds / divisor;
  }

  /// \brief Returns the total number of microseconds of the time span.
  double TotalMicroSeconds() const
  {
    // Prevent optimizer from turning division into multiplication
    static volatile double divisor = 1000.0;
    return m_iTotalNanoSeconds / divisor;
  }

  /// \brief Returns the total number of nanoseconds of the time span.
  double TotalNanoSeconds() const
  {
    return static_cast<double>(m_iTotalNanoSeconds);
  }

  /// \brief Adds two time spans.
  VTimeSpan operator+(const VTimeSpan& other) const
  {
    return VTimeSpan::FromNanoSeconds(m_iTotalNanoSeconds + other.m_iTotalNanoSeconds);
  }

  /// \brief Increases the time span by another time span.
  VTimeSpan& operator+=(const VTimeSpan& other)
  {
    m_iTotalNanoSeconds += other.m_iTotalNanoSeconds;
    return *this;
  }

  /// \brief Subtracts two time spans.
  VTimeSpan operator-(const VTimeSpan& other) const
  {
    return VTimeSpan::FromNanoSeconds(m_iTotalNanoSeconds - other.m_iTotalNanoSeconds);
  }

  /// \brief Decreases the time span by another time span.
  VTimeSpan& operator-=(const VTimeSpan& other)
  {
    m_iTotalNanoSeconds -= other.m_iTotalNanoSeconds;
    return *this;
  }
  
  /// \brief Compares two given date times.
  bool operator==(const VTimeSpan& other) const
  {
    return m_iTotalNanoSeconds == other.m_iTotalNanoSeconds;
  }

  /// \brief Compares two given time spans.
  bool operator!=(const VTimeSpan& other) const
  {
    return m_iTotalNanoSeconds != other.m_iTotalNanoSeconds;
  }

  /// \brief Compares two given time spans.
  bool operator<=(const VTimeSpan& other) const
  {
    return m_iTotalNanoSeconds <= other.m_iTotalNanoSeconds;
  }

  /// \brief Compares two given time spans.
  bool operator<(const VTimeSpan& other) const
  {
    return m_iTotalNanoSeconds < other.m_iTotalNanoSeconds;
  }

  /// \brief Compares two given time spans.
  bool operator>=(const VTimeSpan& other) const
  {
    return m_iTotalNanoSeconds >= other.m_iTotalNanoSeconds;
  }

  /// \brief Compares two given time spans.
  bool operator>(const VTimeSpan& other) const
  {
    return m_iTotalNanoSeconds > other.m_iTotalNanoSeconds;
  }


private:
  __int64 m_iTotalNanoSeconds;
};

/// \brief A class representing a date and time.
class VBASE_IMPEXP_CLASS VDateTime
{
public:
  /// \brief The time zone type.
  enum TimeZone
  {
    UTC,
    LOCAL
  };

  /// \brief Constructs an invalid date time.
  VDateTime()
  {
    m_iNanoSecondsSinceEpoch = s_invalidTime;
  }

  /// \brief Returns the current time.
  static VDateTime Now()
  {
    __time64_t t;
    _time64(&t);
    return FromNative(t);
  }
  
  /// \brief Returns a date time object representing the date 1970-01-01.
  static VDateTime GetUnixEpoch()
  {
    VDateTime result;
    result.m_iNanoSecondsSinceEpoch = 0;
    return result;
  }

  /// \brief Retrieves the date of the date time.
  void GetDate(TimeZone timeZone, int& iYear, int& iMonth, int& iDay) const
  {
    __time64_t time;
    ToNative(time);
    const tm* t = (timeZone == UTC ? _gmtime64 : _localtime64)(&time);

    iYear = t->tm_year + 1900;
    iMonth = t->tm_mon + 1;
    iDay = t->tm_mday;
  }

  /// \brief Retrieves the time of the date time.
  void GetTime(TimeZone timeZone, int& iHour, int& iMinute, int& iSecond) const
  {
    __time64_t time;
    ToNative(time);
    const tm* t = (timeZone == UTC ? _gmtime64 : _localtime64)(&time);

    iHour = t->tm_hour;
    iMinute = t->tm_min;
    iSecond = t->tm_sec;
  }

  /// \brief Returns the year of the date time.
  int GetYear(TimeZone timeZone) const
  {
    __time64_t time;
    ToNative(time);
    return (timeZone == UTC ? _gmtime64 : _localtime64)(&time)->tm_year + 1900;
  }

  /// \brief Returns the 1-based month of the date time.
  int GetMonth(TimeZone timeZone) const
  {
    __time64_t time;
    ToNative(time);
    return (timeZone == UTC ? _gmtime64 : _localtime64)(&time)->tm_mon + 1;
  }

  /// \brief Returns the 1-based day of month of the date time.
  int GetDayOfMonth(TimeZone timeZone) const
  {
    __time64_t time;
    ToNative(time);
    return (timeZone == UTC ? _gmtime64 : _localtime64)(&time)->tm_mday;
  }

  /// \brief Returns the hour of the date time.
  int GetHour(TimeZone timeZone) const
  {
    __time64_t time;
    ToNative(time);
    return (timeZone == UTC ? _gmtime64 : _localtime64)(&time)->tm_hour;
  }

  /// \brief Returns the minute of the date time.
  int GetMinute(TimeZone timeZone) const
  {
    __time64_t time;
    ToNative(time);
    return (timeZone == UTC ? _gmtime64 : _localtime64)(&time)->tm_min;
  }

  /// \brief Returns the second of the date time.
  int GetSecond(TimeZone timeZone) const
  {
    __time64_t time;
    ToNative(time);
    return (timeZone == UTC ? _gmtime64 : _localtime64)(&time)->tm_sec;
  }

  /// \brief Returns the millisecond of the date time.
  int GetMilliSecond() const
  {
    return (m_iNanoSecondsSinceEpoch % (1000 * 1000 * 1000)) / (1000 * 1000);
  }

  /// \brief Returns the microsecond of the date time.
  int GetMicroSecond() const
  {
    return (m_iNanoSecondsSinceEpoch % (1000 * 1000)) / 1000;
  }

  /// \brief Returns the nanosecond of the date time.
  int GetNanoSecond() const
  {
    return m_iNanoSecondsSinceEpoch % 1000;
  }

  /// \brief Stores the date time in a platform-specific native time representation.
  void ToNative(__time64_t& time) const;

  /// \brief Returns a date time object constructed from a platform-specific native representation.
  static VDateTime FromNative(__time64_t time);

  /// \brief Returns a value that can be used for lossless storage of the date time object in a serialized form.
  ///
  /// This value has no meaning and can not be used for any purpose other than deserialization.
  /// Do not use it for comparison or any computation.
  __int64 ToSerializable() const;

  /// \brief Returns a date time object constructed from a previously serialized date time value.
  static VDateTime FromSerializable(__int64 value);

  /// \brief Adds a time span to the date time.
  VDateTime operator+(const VTimeSpan& span) const
  {
    VDateTime result;
    result.m_iNanoSecondsSinceEpoch = m_iNanoSecondsSinceEpoch + span.m_iTotalNanoSeconds;
    return result;
  }

  /// \brief Advances the date time by the given time span.
  VDateTime& operator+=(const VTimeSpan& span)
  {
    m_iNanoSecondsSinceEpoch += span.m_iTotalNanoSeconds;
    return *this;
  }

  /// \brief Subtracts a time span from the date time.
  VDateTime operator-(const VTimeSpan& span) const
  {
    VDateTime result;
    result.m_iNanoSecondsSinceEpoch = m_iNanoSecondsSinceEpoch - span.m_iTotalNanoSeconds;
    return result;
  }

  /// \brief Advances the date time backwards by the given time span.
  VDateTime& operator-=(const VTimeSpan& span)
  {
    m_iNanoSecondsSinceEpoch -= span.m_iTotalNanoSeconds;
    return *this;
  }

  /// \brief Returns the time span between two given date times.
  VTimeSpan operator-(const VDateTime& other) const
  {
    return VTimeSpan::FromNanoSeconds(m_iNanoSecondsSinceEpoch - other.m_iNanoSecondsSinceEpoch);
  }

  /// \brief Compares two given date times.
  bool operator==(const VDateTime& other) const
  {
    return m_iNanoSecondsSinceEpoch == other.m_iNanoSecondsSinceEpoch;
  }

  /// \brief Compares two given date times.
  bool operator!=(const VDateTime& other) const
  {
    return m_iNanoSecondsSinceEpoch != other.m_iNanoSecondsSinceEpoch;
  }

  /// \brief Compares two given date times.
  bool operator<=(const VDateTime& other) const
  {
    return m_iNanoSecondsSinceEpoch <= other.m_iNanoSecondsSinceEpoch;
  }

  /// \brief Compares two given date times.
  bool operator<(const VDateTime& other) const
  {
    return m_iNanoSecondsSinceEpoch < other.m_iNanoSecondsSinceEpoch;
  }

  /// \brief Compares two given date times.
  bool operator>=(const VDateTime& other) const
  {
    return m_iNanoSecondsSinceEpoch >= other.m_iNanoSecondsSinceEpoch;
  }

  /// \brief Compares two given date times.
  bool operator>(const VDateTime& other) const
  {
    return m_iNanoSecondsSinceEpoch > other.m_iNanoSecondsSinceEpoch;
  }

  /// \brief Returns true if the object represents a valid date time.
  bool IsValid() const
  {
    return m_iNanoSecondsSinceEpoch != s_invalidTime;
  }

  /// \brief Formats the date time to a string representation.
  ///
  /// \param buffer
  ///   A buffer to hold the result.
  ///
  /// \param szFormat
  ///   A format specifier as defined by the C library function strftime.
  ///
  /// \param timeZone
  ///   Time zone type.
  ///
  /// \returns HKV_FAILURE if the passed buffer is too small.
  template<int N>
  hkvResult Format(char (&buffer)[N], const char* szFormat, TimeZone timeZone)
  {
    __time64_t time;
    ToNative(time);
    tm* t = (timeZone == UTC ? _gmtime64 : _localtime64)(&time);

    return strftime(buffer, N, szFormat, t) != 0 ? HKV_SUCCESS : HKV_FAILURE;
  }

private:
  __int64 m_iNanoSecondsSinceEpoch;

  static const __int64 s_invalidTime = 0x8000000000000000LL;
};

#endif

/*
 * Havok SDK - Base file, BUILD(#20131218)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available from salesteam@havok.com.
 * 
 */
