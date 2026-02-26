/**
 * HttpCallback
 *
 * Generic RestCallback for logging HTTP request results.
 * Replaces the per-feature callback classes (SwarmObserverWebhookCallback,
 * CombatWebhookCallback) which were all identical except for their log prefix.
 *
 * Usage:
 *   Http.PostJson(url, payload, new HttpCallback("[SwarmObserver]", playerName));
 */
class HttpCallback extends RestCallback
{
	protected string m_Tag;
	protected string m_Context;

	/**
	 * @param tag     Log prefix, e.g. "[SwarmObserver]"
	 * @param context Optional context string shown in log messages, e.g. player name
	 */
	void HttpCallback(string tag, string context = "")
	{
		m_Tag = tag;
		m_Context = context;
	}

	override void OnSuccess(string data, int dataSize)
	{
		if (m_Context != "")
			Print(m_Tag + " HTTP request succeeded (" + m_Context + ")");
		else
			Print(m_Tag + " HTTP request succeeded");
	}

	override void OnError(int errorCode)
	{
		if (m_Context != "")
			Print(m_Tag + " HTTP request failed with error " + errorCode.ToString() + " (" + m_Context + ")");
		else
			Print(m_Tag + " HTTP request failed with error " + errorCode.ToString());
	}

	override void OnTimeout()
	{
		if (m_Context != "")
			Print(m_Tag + " HTTP request timed out (" + m_Context + ")");
		else
			Print(m_Tag + " HTTP request timed out");
	}
}
