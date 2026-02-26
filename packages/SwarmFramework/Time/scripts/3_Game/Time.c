/**
 * Time
 *
 * Static utility class for date and time formatting.
 * Centralizes timestamp logic previously duplicated across SwarmObserver
 * (GetCurrentTimeFormatted, GetCurrentTimestampISO, FormatTwoDigits).
 *
 * Usage:
 *   string iso       = TimeUtil.ISO();             // "2026-02-27T13:05:00Z"
 *   string formatted = TimeUtil.Formatted();        // "27/02/2026 13:05:00"
 *   string padded    = Time.PadTwo(7);          // "07"
 */
class TimeUtil
{
	// ---------------------------------------------------------------
	// Padding helper
	// ---------------------------------------------------------------

	/**
	 * Returns the integer as a zero-padded two-digit string.
	 * e.g. 7 → "07", 12 → "12"
	 */
	static string PadTwo(int value)
	{
		if (value < 10)
			return "0" + value.ToString();

		return value.ToString();
	}

	// ---------------------------------------------------------------
	// Current time snapshots
	// ---------------------------------------------------------------

	/**
	 * Returns the current server time as an ISO 8601 string.
	 * Format: YYYY-MM-DDTHH:MM:SSZ
	 * e.g. "2026-02-27T13:05:00Z"
	 */
	static string ISO()
	{
		int year, month, day, hour, minute, second;
		GetYearMonthDay(year, month, day);
		GetHourMinuteSecond(hour, minute, second);

		return year.ToString() + "-" + PadTwo(month) + "-" + PadTwo(day) + "T" + PadTwo(hour) + ":" + PadTwo(minute) + ":" + PadTwo(second) + "Z";
	}

	/**
	 * Returns the current server time as a human-readable string.
	 * Format: DD/MM/YYYY HH:MM:SS
	 * e.g. "27/02/2026 13:05:00"
	 */
	static string Formatted()
	{
		int year, month, day, hour, minute, second;
		GetYearMonthDay(year, month, day);
		GetHourMinuteSecond(hour, minute, second);

		return PadTwo(day) + "/" + PadTwo(month) + "/" + year.ToString() + " " + PadTwo(hour) + ":" + PadTwo(minute) + ":" + PadTwo(second);
	}

	/**
	 * Returns only the current date as a string.
	 * Format: YYYY-MM-DD
	 * e.g. "2026-02-27"
	 */
	static string Date()
	{
		int year, month, day;
		GetYearMonthDay(year, month, day);

		return year.ToString() + "-" + PadTwo(month) + "-" + PadTwo(day);
	}

	/**
	 * Returns only the current time as a string.
	 * Format: HH:MM:SS
	 * e.g. "13:05:00"
	 */
	static string Clock()
	{
		int hour, minute, second;
		GetHourMinuteSecond(hour, minute, second);

		return PadTwo(hour) + ":" + PadTwo(minute) + ":" + PadTwo(second);
	}
}
