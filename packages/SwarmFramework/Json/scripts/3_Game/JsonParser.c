/**
 * JsonParser
 * 
 * Internal recursive-descent JSON parser. Converts a JSON string into a
 * JsonNode tree. Not intended for direct use - call Json.Parse() instead.
 * 
 * Supports the full JSON spec: objects, arrays, strings (with escape sequences),
 * numbers (int and float), booleans, and null.
 */
class JsonParser
{
	protected string m_Json;
	protected int m_Pos;
	protected int m_Len;
	protected string m_Error;

	// ---------------------------------------------------------------
	// Public entry point
	// ---------------------------------------------------------------

	//! Parse a JSON string into a node tree. Returns null on error.
	JsonNode Parse(string json)
	{
		m_Json = json;
		m_Pos = 0;
		m_Len = json.Length();
		m_Error = "";

		JsonNode result = ParseValue();

		if (m_Error != "")
		{
			Print("[Json] Parse error: " + m_Error);
			return null;
		}

		return result;
	}

	//! Returns the last parse error message, or empty string if no error.
	string GetError()
	{
		return m_Error;
	}

	// ---------------------------------------------------------------
	// Character helpers
	// ---------------------------------------------------------------

	protected bool IsWhitespace(string ch)
	{
		return ch == " " || ch == "\t" || ch == "\n" || ch == "\r";
	}

	protected bool IsDigit(string ch)
	{
		if (ch == "0" || ch == "1" || ch == "2" || ch == "3" || ch == "4")
			return true;
		if (ch == "5" || ch == "6" || ch == "7" || ch == "8" || ch == "9")
			return true;
		return false;
	}

	protected string Peek()
	{
		if (m_Pos >= m_Len)
			return "";

		return m_Json[m_Pos];
	}

	protected string Advance()
	{
		if (m_Pos >= m_Len)
			return "";

		string ch = m_Json[m_Pos];
		m_Pos++;
		return ch;
	}

	protected void SkipWhitespace()
	{
		while (m_Pos < m_Len && IsWhitespace(m_Json[m_Pos]))
			m_Pos++;
	}

	protected void SetError(string msg)
	{
		if (m_Error == "")
			m_Error = msg + " at position " + m_Pos.ToString();
	}

	// ---------------------------------------------------------------
	// Recursive descent parser
	// ---------------------------------------------------------------

	protected JsonNode ParseValue()
	{
		SkipWhitespace();

		if (m_Pos >= m_Len)
		{
			SetError("Unexpected end of input");
			return null;
		}

		string ch = Peek();

		if (ch == "\"")
			return ParseString();

		if (ch == "{")
			return ParseObject();

		if (ch == "[")
			return ParseArray();

		if (ch == "t" || ch == "f")
			return ParseBool();

		if (ch == "n")
			return ParseNull();

		if (ch == "-" || IsDigit(ch))
			return ParseNumber();

		SetError("Unexpected character '" + ch + "'");
		return null;
	}

	// ---------------------------------------------------------------
	// String parsing
	// ---------------------------------------------------------------

	protected JsonNode ParseString()
	{
		string value = ParseStringRaw();
		if (m_Error != "")
			return null;

		return JsonNode.NewString(value);
	}

	//! Parse a quoted string and return its unescaped contents
	protected string ParseStringRaw()
	{
		// Skip opening quote
		if (Advance() != "\"")
		{
			SetError("Expected '\"'");
			return "";
		}

		string result = "";

		while (m_Pos < m_Len)
		{
			string ch = Advance();

			if (ch == "\"")
				return result;

			if (ch == "\\")
			{
				if (m_Pos >= m_Len)
				{
					SetError("Unexpected end of string escape");
					return result;
				}

				string escaped = Advance();

				if (escaped == "\"")
					result += "\"";
				else if (escaped == "\\")
					result += "\\";
				else if (escaped == "/")
					result += "/";
				else if (escaped == "n")
					result += "\n";
				else if (escaped == "r")
					result += "\r";
				else if (escaped == "t")
					result += "\t";
				else if (escaped == "b")
					result += ""; // backspace - skip
				else if (escaped == "f")
					result += ""; // form feed - skip
				else if (escaped == "u")
				{
					// Unicode escape \uXXXX - read 4 hex digits
					if (m_Pos + 4 <= m_Len)
					{
						string hex = m_Json.Substring(m_Pos, 4);
						m_Pos += 4;
						// Convert hex to character if possible
						int codePoint = hex.HexToInt();
						if (codePoint > 0 && codePoint < 128)
						{
							// ASCII range - convert directly
							string asciiStr = codePoint.AsciiToString();
							result += asciiStr;
						}
						else
						{
							// Non-ASCII unicode - keep as escape sequence
							result += "\\u" + hex;
						}
					}
					else
					{
						SetError("Incomplete unicode escape");
						return result;
					}
				}
				else
				{
					// Unknown escape - keep as-is
					result += escaped;
				}
			}
			else
			{
				result += ch;
			}
		}

		SetError("Unterminated string");
		return result;
	}

	// ---------------------------------------------------------------
	// Number parsing
	// ---------------------------------------------------------------

	protected JsonNode ParseNumber()
	{
		int startPos = m_Pos;
		bool isFloat = false;

		// Optional leading minus
		if (Peek() == "-")
			m_Pos++;

		// Integer part
		if (!IsDigit(Peek()))
		{
			SetError("Expected digit");
			return null;
		}

		while (m_Pos < m_Len && IsDigit(Peek()))
			m_Pos++;

		// Fractional part
		if (m_Pos < m_Len && Peek() == ".")
		{
			isFloat = true;
			m_Pos++; // skip '.'

			if (!IsDigit(Peek()))
			{
				SetError("Expected digit after decimal point");
				return null;
			}

			while (m_Pos < m_Len && IsDigit(Peek()))
				m_Pos++;
		}

		// Exponent part
		if (m_Pos < m_Len)
		{
			string expCh = Peek();
			if (expCh == "e" || expCh == "E")
			{
				isFloat = true;
				m_Pos++; // skip 'e'/'E'

				// Optional sign
				string signCh = Peek();
				if (signCh == "+" || signCh == "-")
					m_Pos++;

				if (!IsDigit(Peek()))
				{
					SetError("Expected digit in exponent");
					return null;
				}

				while (m_Pos < m_Len && IsDigit(Peek()))
					m_Pos++;
			}
		}

		string numStr = m_Json.Substring(startPos, m_Pos - startPos);

		if (isFloat)
			return JsonNode.NewFloat(numStr.ToFloat());
		else
			return JsonNode.NewInt(numStr.ToInt());
	}

	// ---------------------------------------------------------------
	// Boolean parsing
	// ---------------------------------------------------------------

	protected JsonNode ParseBool()
	{
		if (m_Pos + 4 <= m_Len && m_Json.Substring(m_Pos, 4) == "true")
		{
			m_Pos += 4;
			return JsonNode.NewBool(true);
		}

		if (m_Pos + 5 <= m_Len && m_Json.Substring(m_Pos, 5) == "false")
		{
			m_Pos += 5;
			return JsonNode.NewBool(false);
		}

		SetError("Expected 'true' or 'false'");
		return null;
	}

	// ---------------------------------------------------------------
	// Null parsing
	// ---------------------------------------------------------------

	protected JsonNode ParseNull()
	{
		if (m_Pos + 4 <= m_Len && m_Json.Substring(m_Pos, 4) == "null")
		{
			m_Pos += 4;
			return JsonNode.NewNull();
		}

		SetError("Expected 'null'");
		return null;
	}

	// ---------------------------------------------------------------
	// Object parsing
	// ---------------------------------------------------------------

	protected JsonNode ParseObject()
	{
		// Skip opening brace
		m_Pos++;

		JsonNode obj = JsonNode.NewObject();

		SkipWhitespace();

		// Empty object
		if (Peek() == "}")
		{
			m_Pos++;
			return obj;
		}

		while (m_Pos < m_Len)
		{
			SkipWhitespace();

			// Parse key
			if (Peek() != "\"")
			{
				SetError("Expected string key in object");
				return null;
			}

			string key = ParseStringRaw();
			if (m_Error != "")
				return null;

			// Expect colon
			SkipWhitespace();
			if (Advance() != ":")
			{
				SetError("Expected ':' after object key");
				return null;
			}

			// Parse value
			SkipWhitespace();
			JsonNode value = ParseValue();
			if (m_Error != "")
				return null;

			obj.Set(key, value);

			// Check for comma or closing brace
			SkipWhitespace();
			string next = Peek();

			if (next == "}")
			{
				m_Pos++;
				return obj;
			}

			if (next == ",")
			{
				m_Pos++;
				continue;
			}

			SetError("Expected ',' or '}' in object");
			return null;
		}

		SetError("Unterminated object");
		return null;
	}

	// ---------------------------------------------------------------
	// Array parsing
	// ---------------------------------------------------------------

	protected JsonNode ParseArray()
	{
		// Skip opening bracket
		m_Pos++;

		JsonNode arr = JsonNode.NewArray();

		SkipWhitespace();

		// Empty array
		if (Peek() == "]")
		{
			m_Pos++;
			return arr;
		}

		while (m_Pos < m_Len)
		{
			SkipWhitespace();

			// Parse value
			JsonNode value = ParseValue();
			if (m_Error != "")
				return null;

			arr.Add(value);

			// Check for comma or closing bracket
			SkipWhitespace();
			string next = Peek();

			if (next == "]")
			{
				m_Pos++;
				return arr;
			}

			if (next == ",")
			{
				m_Pos++;
				continue;
			}

			SetError("Expected ',' or ']' in array");
			return null;
		}

		SetError("Unterminated array");
		return null;
	}
}
