/**
 * Json
 * 
 * Static utility class providing JSON operations for Swarm mods.
 * Wraps DayZ's built-in JsonSerializer for typed objects and provides
 * a full dynamic JSON parser, stringifier, and string escape helpers.
 * 
 * === Quick Reference ===
 * 
 * Serialize a typed object to JSON string:
 *   string json;
 *   Json.Serialize(myObject, json);
 * 
 * Deserialize a JSON string into a typed object:
 *   string error;
 *   Json.Deserialize(jsonString, myObject, error);
 * 
 * Parse a JSON string into a dynamic node tree:
 *   JsonNode root = Json.Parse("{\"name\": \"test\", \"value\": 42}");
 *   string name = root.Get("name").GetString();   // "test"
 *   int value   = root.Get("value").GetInt();      // 42
 * 
 * Convert a node tree back to a JSON string:
 *   string json = Json.Stringify(root);           // compact
 *   string pretty = Json.Stringify(root, true);   // pretty-printed
 * 
 * Escape a string for safe embedding in JSON:
 *   string safe = Json.Escape(playerName);
 *   string payload = "{\"name\": \"" + safe + "\"}";
 * 
 * Build JSON with the fluent builder:
 *   string json = new JsonBuilder()
 *       .BeginObject()
 *           .AddString("name", playerName)
 *           .AddInt("health", 100)
 *       .EndObject()
 *       .Build();
 */
class Json
{
	// ---------------------------------------------------------------
	// Serialize / Deserialize (typed objects via JsonSerializer)
	// ---------------------------------------------------------------

	/**
	 * Serialize a typed script object to a JSON string.
	 * Uses DayZ's built-in JsonSerializer which auto-maps class members.
	 * 
	 * @param obj     The object to serialize
	 * @param result  Output JSON string
	 * @param pretty  If true, output is indented (default: true)
	 * @return true on success
	 */
	static bool Serialize(Class obj, out string result, bool pretty = true)
	{
		if (!obj)
		{
			result = "null";
			return true;
		}

		JsonSerializer serializer = new JsonSerializer();
		bool success = serializer.WriteToString(obj, pretty, result);

		if (!success)
			Print("[Json] Serialize failed for " + obj.ClassName());

		return success;
	}

	/**
	 * Deserialize a JSON string into a typed script object.
	 * Uses DayZ's built-in JsonSerializer which auto-maps JSON keys to class members.
	 * 
	 * @param json   The JSON string to parse
	 * @param obj    The target object to populate
	 * @param error  Output error message if parsing fails
	 * @return true on success
	 */
	static bool Deserialize(string json, Class obj, out string error)
	{
		if (!obj)
		{
			error = "Target object is null";
			return false;
		}

		JsonSerializer serializer = new JsonSerializer();
		bool success = serializer.ReadFromString(obj, json, error);

		if (!success)
			Print("[Json] Deserialize failed: " + error);

		return success;
	}

	// ---------------------------------------------------------------
	// Parse (string → JsonNode tree)
	// ---------------------------------------------------------------

	/**
	 * Parse a JSON string into a dynamic JsonNode tree.
	 * Returns null if the JSON is invalid (error is printed to log).
	 * 
	 * @param json  The JSON string to parse
	 * @return Root node of the parsed tree, or null on error
	 */
	static JsonNode Parse(string json)
	{
		if (json == "")
			return null;

		JsonParser parser = new JsonParser();
		return parser.Parse(json);
	}

	// ---------------------------------------------------------------
	// Stringify (JsonNode tree → string)
	// ---------------------------------------------------------------

	/**
	 * Convert a JsonNode tree to a JSON string.
	 * 
	 * @param node    The root node to stringify
	 * @param pretty  If true, output is indented with 2 spaces (default: false)
	 * @return The JSON string
	 */
	static string Stringify(JsonNode node, bool pretty = false)
	{
		if (!node)
			return "null";

		return StringifyNode(node, pretty, 0);
	}

	protected static string StringifyNode(JsonNode node, bool pretty, int depth)
	{
		switch (node.GetType())
		{
			case JsonType.NULL_VALUE:
				return "null";

			case JsonType.BOOL:
				if (node.GetBool())
					return "true";
				return "false";

			case JsonType.INT:
				return node.GetInt().ToString();

			case JsonType.FLOAT:
				return node.GetFloat().ToString();

			case JsonType.STRING:
				return "\"" + Escape(node.GetString()) + "\"";

			case JsonType.ARRAY:
				return StringifyArray(node, pretty, depth);

			case JsonType.OBJECT:
				return StringifyObject(node, pretty, depth);
		}

		return "null";
	}

	protected static string StringifyArray(JsonNode node, bool pretty, int depth)
	{
		int count = node.Count();

		if (count == 0)
			return "[]";

		string result = "[";
		string newline = "";
		string indent = "";
		string itemIndent = "";

		if (pretty)
		{
			newline = "\n";
			indent = MakeIndent(depth);
			itemIndent = MakeIndent(depth + 1);
		}

		for (int i = 0; i < count; i++)
		{
			if (i > 0)
				result += ",";

			result += newline + itemIndent;
			result += StringifyNode(node.GetAt(i), pretty, depth + 1);
		}

		result += newline + indent + "]";
		return result;
	}

	protected static string StringifyObject(JsonNode node, bool pretty, int depth)
	{
		array<string> keys = node.GetKeys();

		if (!keys || keys.Count() == 0)
			return "{}";

		string result = "{";
		string newline = "";
		string indent = "";
		string itemIndent = "";
		string colonSep = ":";

		if (pretty)
		{
			newline = "\n";
			indent = MakeIndent(depth);
			itemIndent = MakeIndent(depth + 1);
			colonSep = ": ";
		}

		for (int i = 0; i < keys.Count(); i++)
		{
			if (i > 0)
				result += ",";

			string key = keys.Get(i);
			JsonNode value = node.Get(key);

			result += newline + itemIndent;
			result += "\"" + Escape(key) + "\"" + colonSep;
			result += StringifyNode(value, pretty, depth + 1);
		}

		result += newline + indent + "}";
		return result;
	}

	protected static string MakeIndent(int depth)
	{
		string indent = "";
		for (int i = 0; i < depth; i++)
			indent += "  ";
		return indent;
	}

	// ---------------------------------------------------------------
	// Escape / Unescape
	// ---------------------------------------------------------------

	/**
	 * Properly escape a string for safe use inside a JSON string value.
	 * Handles: backslash, double quotes, newlines, carriage returns, tabs.
	 * 
	 * @param value  The raw string value
	 * @return The escaped string (without surrounding quotes)
	 */
	static string Escape(string value)
	{
		if (value == "")
			return "";

		// Build escape characters dynamically to avoid multi-escape string literals
		// which the EnforceScript parser cannot handle
		int bsCode = 92;
		int dqCode = 34;
		string bs = bsCode.AsciiToString();   // backslash character
		string dq = dqCode.AsciiToString();   // double quote character

		string result = value;
		// Backslash must be escaped FIRST to avoid double-escaping
		result.Replace(bs, bs + bs);
		result.Replace(dq, bs + dq);
		result.Replace("\n", bs + "n");
		result.Replace("\r", bs + "r");
		result.Replace("\t", bs + "t");
		return result;
	}

	/**
	 * Unescape a JSON-escaped string, converting escape sequences back
	 * to their original characters.
	 * 
	 * @param value  The escaped string (without surrounding quotes)
	 * @return The unescaped string
	 */
	static string Unescape(string value)
	{
		if (value == "")
			return "";

		string result = "";
		int i = 0;
		int len = value.Length();

		while (i < len)
		{
			string ch = value[i];

			if (ch == "\\" && i + 1 < len)
			{
				string next = value[i + 1];

				if (next == "\"")
				{
					result += "\"";
					i += 2;
					continue;
				}
				if (next == "\\")
				{
					result += "\\";
					i += 2;
					continue;
				}
				if (next == "/")
				{
					result += "/";
					i += 2;
					continue;
				}
				if (next == "n")
				{
					result += "\n";
					i += 2;
					continue;
				}
				if (next == "r")
				{
					result += "\r";
					i += 2;
					continue;
				}
				if (next == "t")
				{
					result += "\t";
					i += 2;
					continue;
				}
				if (next == "b" || next == "f")
				{
					// Backspace and form feed - skip
					i += 2;
					continue;
				}
				if (next == "u" && i + 5 < len)
				{
					// Unicode escape \uXXXX - keep as-is
					result += value.Substring(i, 6);
					i += 6;
					continue;
				}
			}

			result += ch;
			i++;
		}

		return result;
	}
}
