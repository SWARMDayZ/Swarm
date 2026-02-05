// Individual violation record
class SwarmObserverViolation
{
	string AreaName;
	string Timestamp;
	
	void SwarmObserverViolation(string areaName = "", string timestamp = "")
	{
		AreaName = areaName;
		Timestamp = timestamp;
	}
}

// Player disconnect record
class SwarmObserverPlayerRecord
{
	string SteamID;
	string PlayerName;
	autoptr array<ref SwarmObserverViolation> Violations;
	string LastAcknowledgedTimestamp; // When player last acknowledged the notice
	
	void SwarmObserverPlayerRecord(string steamID = "", string playerName = "")
	{
		SteamID = steamID;
		PlayerName = playerName;
		Violations = new array<ref SwarmObserverViolation>;
		LastAcknowledgedTimestamp = "";
	}
	
	// Check if record has expired (older than HistoryDays)
	bool IsExpired()
	{
		// For now, we keep all violations and let them expire based on date comparison
		// This would require date parsing which is complex, so we'll keep violations indefinitely
		// until manual cleanup or we implement proper date comparison
		return Violations.Count() == 0;
	}
	
	// Remove expired violations (placeholder for future implementation)
	void CleanExpiredViolations()
	{
		// TODO: Implement date-based expiration if needed
		// For now, violations are kept indefinitely
	}
	
	// Add violation
	void AddViolation(string areaName, string timestamp)
	{
		SwarmObserverViolation violation = new SwarmObserverViolation(areaName, timestamp);
		Violations.Insert(violation);
	}
	
	// Get violation count
	int GetCount()
	{
		CleanExpiredViolations();
		return Violations.Count();
	}
	
	// Get count of violations that occurred after last acknowledgement
	int GetUnacknowledgedCount()
	{
		CleanExpiredViolations();
		
		// If never acknowledged, return all violations
		if (LastAcknowledgedTimestamp == "")
			return Violations.Count();
		
		// Count violations after acknowledgement
		int count = 0;
		foreach (SwarmObserverViolation violation : Violations)
		{
			if (violation.Timestamp > LastAcknowledgedTimestamp)
				count++;
		}
		return count;
	}
	
	// Mark violations as acknowledged
	void AcknowledgeViolations()
	{
		int year, month, day, hour, minute, second;
		GetYearMonthDay(year, month, day);
		GetHourMinuteSecond(hour, minute, second);
		LastAcknowledgedTimestamp = day.ToString() + "/" + month.ToString() + "/" + year.ToString() + " " + hour.ToString() + ":" + minute.ToString() + ":" + second.ToString();
	}
}

// History data container for JSON serialization
class SwarmObserverHistoryData
{
	autoptr array<ref SwarmObserverPlayerRecord> Players;
	
	void SwarmObserverHistoryData()
	{
		Players = new array<ref SwarmObserverPlayerRecord>;
	}
}

// History management singleton
class SwarmObserverHistory
{
	private static ref SwarmObserverHistory s_Instance;
	private ref map<string, ref SwarmObserverPlayerRecord> m_Records;
	
	void SwarmObserverHistory()
	{
		m_Records = new map<string, ref SwarmObserverPlayerRecord>;
	}
	
	// Get singleton instance
	static SwarmObserverHistory GetInstance()
	{
		if (!s_Instance)
		{
			s_Instance = new SwarmObserverHistory();
		}
		return s_Instance;
	}
	
	// Load history from file
	static bool LoadHistory()
	{
		SwarmObserverHistory history = GetInstance();
		history.m_Records.Clear();
		
		if (FileExist(SwarmObserverConstants.FILE_HISTORY))
		{
			SwarmObserverHistoryData data = new SwarmObserverHistoryData();
			JsonFileLoader<SwarmObserverHistoryData>.JsonLoadFile(SwarmObserverConstants.FILE_HISTORY, data);
			
			// Convert array to map for faster lookups
			foreach (SwarmObserverPlayerRecord record : data.Players)
			{
				history.m_Records.Set(record.SteamID, record);
			}
			
			// Clean expired records
			history.CleanExpiredRecords();
			
			Print("[SwarmObserver] History loaded: " + history.m_Records.Count() + " active records");
		}
		else
		{
			Print("[SwarmObserver] History file not found, starting fresh");
		}
		
		return true;
	}
	
	// Save history to file
	static bool SaveHistory()
	{
		SwarmObserverHistory history = GetInstance();
		
		// Convert map to array for serialization
		SwarmObserverHistoryData data = new SwarmObserverHistoryData();
		foreach (string steamID, SwarmObserverPlayerRecord record : history.m_Records)
		{
			data.Players.Insert(record);
		}
		
		JsonFileLoader<SwarmObserverHistoryData>.JsonSaveFile(SwarmObserverConstants.FILE_HISTORY, data);
		Print("[SwarmObserver] History saved: " + history.m_Records.Count() + " records");
		return true;
	}
	
	// Get violation count for a player
	static int GetViolationCount(string steamID)
	{
		SwarmObserverHistory history = GetInstance();
		
		if (history.m_Records.Contains(steamID))
		{
			SwarmObserverPlayerRecord record = history.m_Records.Get(steamID);
			
			// Check if expired
			if (record.IsExpired())
			{
				history.m_Records.Remove(steamID);
				return 0;
			}
			
			return record.GetCount();
		}
		
		return 0;
	}
	
	// Get player record
	static SwarmObserverPlayerRecord GetPlayerRecord(string steamID)
	{
		SwarmObserverHistory history = GetInstance();
		
		if (history.m_Records.Contains(steamID))
		{
			SwarmObserverPlayerRecord record = history.m_Records.Get(steamID);
			
			// Clean expired violations
			if (!record.IsExpired())
			{
				return record;
			}
			else
			{
				history.m_Records.Remove(steamID);
			}
		}
		
		return null;
	}
	
	// Increment violation for a player
	static void IncrementViolation(string steamID, string playerName, string areaName)
	{
		SwarmObserverHistory history = GetInstance();
		
		SwarmObserverPlayerRecord record;
		
		if (history.m_Records.Contains(steamID))
		{
			record = history.m_Records.Get(steamID);
			
			// Clean expired violations
			record.CleanExpiredViolations();
		}
		else
		{
			record = new SwarmObserverPlayerRecord(steamID, playerName);
			history.m_Records.Set(steamID, record);
		}
		
		// Get current timestamp
		int year, month, day, hour, minute, second;
		GetYearMonthDay(year, month, day);
		GetHourMinuteSecond(hour, minute, second);
		string timestamp = day.ToString() + "/" + month.ToString() + "/" + year.ToString() + " " + hour.ToString() + ":" + minute.ToString() + ":" + second.ToString();
		
		record.AddViolation(areaName, timestamp);
		
		Print("[SwarmObserver] Violation recorded for " + playerName + " (" + steamID + "): " + record.GetCount() + " total");
		
		// Save after each increment
		SaveHistory();
	}
	
	// Remove expired records
	void CleanExpiredRecords()
	{
		array<string> toRemove = new array<string>;
		
		foreach (string steamID, SwarmObserverPlayerRecord record : m_Records)
		{
			if (record.IsExpired())
			{
				toRemove.Insert(steamID);
			}
		}
		
		foreach (string expiredSteamID : toRemove)
		{
			m_Records.Remove(expiredSteamID);
		}
		
		if (toRemove.Count() > 0)
		{
			Print("[SwarmObserver] Cleaned " + toRemove.Count() + " expired records");
		}
	}
	
	// Mark player's violations as acknowledged
	static void AcknowledgeViolations(string steamID)
	{
		SwarmObserverHistory history = GetInstance();
		
		if (history.m_Records.Contains(steamID))
		{
			SwarmObserverPlayerRecord record = history.m_Records.Get(steamID);
			record.AcknowledgeViolations();
			
			Print("[SwarmObserver] Violations acknowledged for " + record.PlayerName + " (" + steamID + ")");
			
			// Save after acknowledgement
			SaveHistory();
		}
	}
}
