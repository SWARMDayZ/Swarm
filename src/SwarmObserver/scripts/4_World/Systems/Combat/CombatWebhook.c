// Combat logout webhook sender
class CombatWebhook
{
	// Send combat logout notification to Discord webhook
	static void SendCombatLogoutNotification(PlayerBase player, CombatState state)
	{
		if (!player || !state || !GetGame().IsServer())
			return;
		
		SwarmObserverSettings settings = SwarmObserverSettings.GetInstance();
		
		// Check if webhook URL is configured
		string webhookURL = settings.CombatWebhookURL;
		if (webhookURL == "")
		{
			webhookURL = settings.WebhookURL;
		}
		
		if (webhookURL == "" || webhookURL.Contains("YOUR_WEBHOOK"))
		{
			Print("[SwarmObserver] Combat logout webhook URL not configured, skipping notification");
			return;
		}
		
		if (!player.GetIdentity())
		{
			Print("[SwarmObserver] Player identity not available for combat logout notification");
			return;
		}
		
		string playerName = player.GetIdentity().GetName();
		string steamID = player.GetIdentity().GetId();
		vector pos = state.m_LastPosition;
		string timestamp = GetCurrentTimestampISO();
		int remainingTime = state.GetRemainingTime(GetGame().GetTime(), settings.CombatDurationSeconds);
		
		// Build list of involved players
		string involvedPlayers = "";
		for (int i = 0; i < state.m_InvolvedPlayers.Count(); i++)
		{
			involvedPlayers += state.m_InvolvedPlayers[i];
			if (i < state.m_InvolvedPlayers.Count() - 1)
				involvedPlayers += ", ";
		}
		if (involvedPlayers == "")
			involvedPlayers = "None";
		
		// Build action summary (escaped for JSON)
		string actionSummary = state.GetActionSummary();
		actionSummary = EscapeJSON(actionSummary);
		
		// Build Steam profile URL
		string steamProfileURL = "https://steamcommunity.com/profiles/" + steamID;
		
		// Build Discord webhook JSON payload with embeds
		string jsonPayload = "{\n";
		jsonPayload += "  \"username\": \"Swarm Observer - Combat Logout\",\n";
		jsonPayload += "  \"embeds\": [\n";
		jsonPayload += "    {\n";
		jsonPayload += "      \"title\": \"⚠️ Combat Logout Detected\",\n";
		jsonPayload += "      \"url\": \"" + steamProfileURL + "\",\n";
		jsonPayload += "      \"color\": 15158332,\n";
		jsonPayload += "      \"fields\": [\n";
		jsonPayload += "        {\"name\": \"Player\", \"value\": \"" + EscapeJSON(playerName) + "\", \"inline\": true},\n";
		jsonPayload += "        {\"name\": \"Steam ID\", \"value\": \"" + steamID + "\", \"inline\": true},\n";
		jsonPayload += "        {\"name\": \"Timestamp\", \"value\": \"" + timestamp + "\", \"inline\": false},\n";
		jsonPayload += "        {\"name\": \"Position\", \"value\": \"[" + pos[0].ToString() + ", " + pos[1].ToString() + ", " + pos[2].ToString() + "]\", \"inline\": false},\n";
		jsonPayload += "        {\"name\": \"Health\", \"value\": \"" + (state.m_LastHealthLevel * 100).ToString() + "%\", \"inline\": true},\n";
		jsonPayload += "        {\"name\": \"Time Remaining\", \"value\": \"" + remainingTime.ToString() + "s\", \"inline\": true},\n";
		jsonPayload += "        {\"name\": \"Last Damage Type\", \"value\": \"" + EscapeJSON(state.m_LastDamageType) + "\", \"inline\": false},\n";
		jsonPayload += "        {\"name\": \"Involved Players\", \"value\": \"" + EscapeJSON(involvedPlayers) + "\", \"inline\": false},\n";
		jsonPayload += "        {\"name\": \"Recent Actions\", \"value\": \"" + actionSummary + "\", \"inline\": false}\n";
		jsonPayload += "      ],\n";
		jsonPayload += "      \"timestamp\": \"" + timestamp + "\",\n";
		jsonPayload += "      \"footer\": {\n";
		jsonPayload += "        \"text\": \"Combat logout detected\"\n";
		jsonPayload += "      }\n";
		jsonPayload += "    }\n";
		jsonPayload += "  ]\n";
		jsonPayload += "}";
		
		Print("[SwarmObserver] Sending combat logout webhook for " + playerName);
		
		// Parse webhook URL to extract host and path
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
			ctx.POST(new CombatWebhookCallback(playerName), path, jsonPayload);
		}
		else
		{
			Print("[SwarmObserver] Failed to create REST context for combat logout webhook");
		}
	}
	
	// Get current timestamp in ISO 8601 format
	static string GetCurrentTimestampISO()
	{
		int year, month, day, hour, minute, second;
		GetYearMonthDay(year, month, day);
		GetHourMinuteSecond(hour, minute, second);
		
		string monthStr = month.ToString();
		if (month < 10) monthStr = "0" + monthStr;
		
		string dayStr = day.ToString();
		if (day < 10) dayStr = "0" + dayStr;
		
		string hourStr = hour.ToString();
		if (hour < 10) hourStr = "0" + hourStr;
		
		string minStr = minute.ToString();
		if (minute < 10) minStr = "0" + minStr;
		
		string secStr = second.ToString();
		if (second < 10) secStr = "0" + secStr;
		
		return year.ToString() + "-" + monthStr + "-" + dayStr + "T" + hourStr + ":" + minStr + ":" + secStr + "Z";
	}
	
	// Escape special characters for JSON
	static string EscapeJSON(string str)
	{
		if (str == "")
			return "";
		
		// Replace double quotes with single quotes (safer than escaping)
		str.Replace("\"", "'");
		
		// Remove newlines and tabs (replace with space)
		str.Replace("\n", " ");
		str.Replace("\r", " ");
		str.Replace("\t", " ");
		
		return str;
	}
}
