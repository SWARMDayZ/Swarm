// Webhook callback handler
class SwarmObserverWebhookCallback extends RestCallback
{
	string m_PlayerName;
	
	void SwarmObserverWebhookCallback(string playerName)
	{
		m_PlayerName = playerName;
	}
	
	override void OnSuccess(string data, int dataSize)
	{
		Print("[SwarmObserver] Webhook sent successfully for player: " + m_PlayerName);
	}
	
	override void OnError(int errorCode)
	{
		Print("[SwarmObserver] Webhook failed with error code: " + errorCode + " for player: " + m_PlayerName);
	}
	
	override void OnTimeout()
	{
		Print("[SwarmObserver] Webhook timeout for player: " + m_PlayerName);
	}
}

// Webhook sender
class SwarmObserverWebhook
{
	// Send disconnect notification to Discord webhook
	static void SendDisconnectNotification(string steamID, string playerName, string areaName, int timestamp)
	{
		SwarmObserverSettings settings = SwarmObserverSettings.GetInstance();
		
		// Check if webhook URL is configured
		if (settings.WebhookURL == "" || settings.WebhookURL.Contains("YOUR_WEBHOOK"))
		{
			Print("[SwarmObserver] Webhook URL not configured, skipping notification");
			return;
		}
		
		// Get player record for violations
		SwarmObserverPlayerRecord record = SwarmObserverHistory.GetPlayerRecord(steamID);
		if (!record)
		{
			Print("[SwarmObserver] No record found for player, skipping webhook");
			return;
		}
		
		int violationCount = record.GetCount();
		
		// Get current time
		string timeStr = GetCurrentTimeFormatted();
		
		// Build message content
		string message = "Player **" + playerName + "** has disconnected in **" + areaName + "** at **" + timeStr + "**";
		
		// Build embed fields for each violation
		string fieldsJson = "";
		for (int i = 0; i < record.Violations.Count(); i++)
		{
			SwarmObserverViolation violation = record.Violations.Get(i);
			string violationTime = violation.Timestamp;
			
			if (i > 0)
				fieldsJson += ",\n";
			
			fieldsJson += "        {\n";
			fieldsJson += "          \"name\": \"Violation at " + violationTime + "\",\n";
			fieldsJson += "          \"value\": \"" + violation.AreaName + "\"\n";
			fieldsJson += "        }";
		}
		
		// Build Steam profile URL
		string steamProfileURL = "https://steamcommunity.com/profiles/" + steamID;
		
		// Build Discord webhook JSON payload with embeds
		// TODO This could potentially contains json injection if playerName or areaName contains special characters
		// it should NOT be possible since players are not allowed to use special characters in their names
		// if it breaks the history, comes from here and we should implement a proper way to escape special characters in the future
		string jsonPayload = "{\n";
		jsonPayload += "  \"username\": \"Swarm Observer\",\n";
		jsonPayload += "  \"content\": \"" + message + "\",\n";
		jsonPayload += "  \"embeds\": [\n";
		jsonPayload += "    {\n";
		jsonPayload += "      \"title\": \"" + playerName + "\",\n";
		jsonPayload += "      \"url\": \"" + steamProfileURL + "\",\n";
		jsonPayload += "      \"description\": \"History\",\n";
		jsonPayload += "      \"color\": 15258703,\n";
		jsonPayload += "      \"fields\": [\n";
		jsonPayload += fieldsJson;
		jsonPayload += "\n      ],\n";
		jsonPayload += "      \"footer\": {\n";
		jsonPayload += "        \"text\": \"This user has " + violationCount + " violation";
		if (violationCount != 1)
			jsonPayload += "s";
		jsonPayload += " in the last " + settings.HistoryDays + " days\"\n";
		jsonPayload += "      }\n";
		jsonPayload += "    }\n";
		jsonPayload += "  ]\n";
		jsonPayload += "}";
		
		Print("[SwarmObserver] Sending webhook for " + playerName + " (Count: " + violationCount + ")");
		
		// Parse webhook URL to extract host and path
		string webhookURL = settings.WebhookURL;
		string host = "";
		string path = "";
		
		// Remove https:// or http://
		if (webhookURL.IndexOf("https://") == 0)
		{
			webhookURL = webhookURL.Substring(8, webhookURL.Length() - 8);
		}
		else if (webhookURL.IndexOf("http://") == 0)
		{
			webhookURL = webhookURL.Substring(7, webhookURL.Length() - 7);
		}
		
		// Split host and path
		int slashPos = webhookURL.IndexOf("/");
		if (slashPos > 0)
		{
			host = webhookURL.Substring(0, slashPos);
			path = webhookURL.Substring(slashPos, webhookURL.Length() - slashPos);
		}
		else
		{
			host = webhookURL;
			path = "/";
		}
		
		// Send webhook via RestApi
		RestContext ctx = GetRestApi().GetRestContext("https://" + host);
		if (ctx)
		{
			ctx.SetHeader("application/json");
			ctx.POST(new SwarmObserverWebhookCallback(playerName), path, jsonPayload);
		}
		else
		{
			Print("[SwarmObserver] Failed to create REST context for webhook");
		}
	}
	
	// Get current system time formatted as DD/MM/YYYY HH:MM:SS
	static string GetCurrentTimeFormatted()
	{
		int year, month, day, hour, minute, second;
		GetYearMonthDay(year, month, day);
		GetHourMinuteSecond(hour, minute, second);
		
		// Format with zero padding
		string dayStr = day.ToString();
		if (day < 10) dayStr = "0" + dayStr;
		
		string monthStr = month.ToString();
		if (month < 10) monthStr = "0" + monthStr;
		
		string hourStr = hour.ToString();
		if (hour < 10) hourStr = "0" + hourStr;
		
		string minStr = minute.ToString();
		if (minute < 10) minStr = "0" + minStr;
		
		string secStr = second.ToString();
		if (second < 10) secStr = "0" + secStr;
		
		return dayStr + "/" + monthStr + "/" + year.ToString() + " " + hourStr + ":" + minStr + ":" + secStr;
	}
}
