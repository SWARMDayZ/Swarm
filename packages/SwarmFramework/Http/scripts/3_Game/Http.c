/**
 * Http
 *
 * Static utility class for HTTP operations.
 * Centralizes URL parsing and JSON POST logic previously duplicated across
 * SwarmObserverWebhook and CombatWebhook.
 *
 * Usage:
 *   // Fire-and-forget JSON POST with logging callback
 *   Http.PostJson(webhookURL, jsonPayload, new HttpCallback("[MyMod]", playerName));
 *
 *   // URL parsing only
 *   string host, path;
 *   Http.ParseUrl(webhookURL, host, path);
 */
class Http
{
	// ---------------------------------------------------------------
	// URL parsing
	// ---------------------------------------------------------------

	/**
	 * Splits a URL into its host and path components.
	 * Strips the http:// or https:// scheme prefix.
	 *
	 * e.g. "https://discord.com/api/webhooks/123/abc"
	 *   → host = "discord.com", path = "/api/webhooks/123/abc"
	 *
	 * @param url   The full URL
	 * @param host  Output: hostname (without scheme or trailing slash)
	 * @param path  Output: path starting with "/" (defaults to "/" if none)
	 */
	static void ParseUrl(string url, out string host, out string path)
	{
		// Strip scheme
		if (url.IndexOf("https://") == 0)
			url = url.Substring(8, url.Length() - 8);
		else if (url.IndexOf("http://") == 0)
			url = url.Substring(7, url.Length() - 7);

		// Split at first slash
		int slashPos = url.IndexOf("/");
		if (slashPos > 0)
		{
			host = url.Substring(0, slashPos);
			path = url.Substring(slashPos, url.Length() - slashPos);
		}
		else
		{
			host = url;
			path = "/";
		}
	}

	// ---------------------------------------------------------------
	// Requests
	// ---------------------------------------------------------------

	/**
	 * Sends a JSON payload via HTTP POST to the given URL.
	 * Sets the Content-Type header to "application/json" automatically.
	 *
	 * @param url      Full URL (http:// or https://)
	 * @param payload  JSON string body
	 * @param callback RestCallback to receive the response (can be null to ignore)
	 * @return true if the REST context was created and the request was sent
	 */
	static bool PostJson(string url, string payload, RestCallback callback = null)
	{
		string host, path;
		ParseUrl(url, host, path);

		RestContext ctx = GetRestApi().GetRestContext("https://" + host);
		if (!ctx)
		{
			Print("[Http] Failed to create REST context for: " + host);
			return false;
		}

		ctx.SetHeader("application/json");
		ctx.POST(callback, path, payload);
		return true;
	}

	/**
	 * Sends an HTTP GET request to the given URL.
	 *
	 * @param url      Full URL (http:// or https://)
	 * @param callback RestCallback to receive the response (can be null to ignore)
	 * @return true if the REST context was created and the request was sent
	 */
	static bool Get(string url, RestCallback callback = null)
	{
		string host, path;
		ParseUrl(url, host, path);

		RestContext ctx = GetRestApi().GetRestContext("https://" + host);
		if (!ctx)
		{
			Print("[Http] Failed to create REST context for: " + host);
			return false;
		}

		ctx.GET(callback, path);
		return true;
	}
}
