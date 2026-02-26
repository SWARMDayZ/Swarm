/**
 * SwarmJsonBuilder
 * 
 * Fluent API for building JSON strings with proper nesting, escaping, and
 * optional pretty-printing. Supports nested objects and arrays.
 * 
 * Usage:
 *   string json = new SwarmJsonBuilder()
 *       .BeginObject()
 *           .AddString("username", "Swarm Observer")
 *           .AddInt("color", 15158332)
 *           .AddArray("embeds")
 *               .ItemObject()
 *                   .AddString("title", "Alert")
 *                   .AddString("description", "Player disconnected")
 *               .EndObject()
 *           .EndArray()
 *       .EndObject()
 *       .Build();
 *
 * Pretty-printing:
 *   string json = new SwarmJsonBuilder(true)
 *       .BeginObject()
 *           .AddString("name", "test")
 *       .EndObject()
 *       .Build();
 *   // Output:
 *   // {
 *   //   "name": "test"
 *   // }
 */
class SwarmJsonBuilder
{
	// Internal context types
	static const int CTX_OBJECT = 0;
	static const int CTX_ARRAY = 1;

	protected string m_Buffer;
	protected ref array<int> m_ContextStack;   // stack of CTX_OBJECT / CTX_ARRAY
	protected ref array<bool> m_NeedsComma;    // whether next item needs a leading comma
	protected bool m_Pretty;
	protected int m_Indent;

	// ---------------------------------------------------------------
	// Constructor
	// ---------------------------------------------------------------

	//! Create a new JSON builder. Set pretty = true for indented output.
	void SwarmJsonBuilder(bool pretty = false)
	{
		m_Buffer = "";
		m_ContextStack = new array<int>();
		m_NeedsComma = new array<bool>();
		m_Pretty = pretty;
		m_Indent = 0;
	}

	// ---------------------------------------------------------------
	// Internal helpers
	// ---------------------------------------------------------------

	protected void AppendComma()
	{
		if (m_NeedsComma.Count() > 0 && m_NeedsComma.Get(m_NeedsComma.Count() - 1))
		{
			m_Buffer += ",";
			if (m_Pretty)
				m_Buffer += "\n";
		}
	}

	protected void AppendIndent()
	{
		if (!m_Pretty)
			return;

		for (int i = 0; i < m_Indent; i++)
			m_Buffer += "  ";
	}

	protected void MarkNeedsComma()
	{
		if (m_NeedsComma.Count() > 0)
			m_NeedsComma.Set(m_NeedsComma.Count() - 1, true);
	}

	protected void PushContext(int ctx)
	{
		m_ContextStack.Insert(ctx);
		m_NeedsComma.Insert(false);
	}

	protected void PopContext()
	{
		if (m_ContextStack.Count() > 0)
			m_ContextStack.RemoveOrdered(m_ContextStack.Count() - 1);

		if (m_NeedsComma.Count() > 0)
			m_NeedsComma.RemoveOrdered(m_NeedsComma.Count() - 1);
	}

	protected void AppendKey(string key)
	{
		AppendComma();
		AppendIndent();
		m_Buffer += "\"" + SwarmJSON.Escape(key) + "\":";
		if (m_Pretty)
			m_Buffer += " ";
	}

	protected void AppendRawValue(string raw)
	{
		m_Buffer += raw;
		MarkNeedsComma();
	}

	protected void AppendItemPrefix()
	{
		AppendComma();
		AppendIndent();
	}

	// ---------------------------------------------------------------
	// Structure
	// ---------------------------------------------------------------

	//! Begin a root-level or nested JSON object
	SwarmJsonBuilder BeginObject()
	{
		if (m_ContextStack.Count() > 0)
			AppendItemPrefix();

		m_Buffer += "{";
		if (m_Pretty)
			m_Buffer += "\n";

		m_Indent++;
		PushContext(CTX_OBJECT);
		return this;
	}

	//! End the current JSON object
	SwarmJsonBuilder EndObject()
	{
		m_Indent--;
		if (m_Pretty && m_NeedsComma.Count() > 0 && m_NeedsComma.Get(m_NeedsComma.Count() - 1))
			m_Buffer += "\n";

		AppendIndent();
		m_Buffer += "}";
		PopContext();
		MarkNeedsComma();
		return this;
	}

	//! Begin a root-level or nested JSON array
	SwarmJsonBuilder BeginArray()
	{
		if (m_ContextStack.Count() > 0)
			AppendItemPrefix();

		m_Buffer += "[";
		if (m_Pretty)
			m_Buffer += "\n";

		m_Indent++;
		PushContext(CTX_ARRAY);
		return this;
	}

	//! End the current JSON array
	SwarmJsonBuilder EndArray()
	{
		m_Indent--;
		if (m_Pretty && m_NeedsComma.Count() > 0 && m_NeedsComma.Get(m_NeedsComma.Count() - 1))
			m_Buffer += "\n";

		AppendIndent();
		m_Buffer += "]";
		PopContext();
		MarkNeedsComma();
		return this;
	}

	// ---------------------------------------------------------------
	// Object key-value pairs (use inside BeginObject/EndObject)
	// ---------------------------------------------------------------

	//! Add a string value with key
	SwarmJsonBuilder AddString(string key, string value)
	{
		AppendKey(key);
		AppendRawValue("\"" + SwarmJSON.Escape(value) + "\"");
		return this;
	}

	//! Add an integer value with key
	SwarmJsonBuilder AddInt(string key, int value)
	{
		AppendKey(key);
		AppendRawValue(value.ToString());
		return this;
	}

	//! Add a float value with key
	SwarmJsonBuilder AddFloat(string key, float value)
	{
		AppendKey(key);
		AppendRawValue(value.ToString());
		return this;
	}

	//! Add a boolean value with key
	SwarmJsonBuilder AddBool(string key, bool value)
	{
		AppendKey(key);
		if (value)
			AppendRawValue("true");
		else
			AppendRawValue("false");
		return this;
	}

	//! Add a null value with key
	SwarmJsonBuilder AddNull(string key)
	{
		AppendKey(key);
		AppendRawValue("null");
		return this;
	}

	//! Start a nested object with key. Must call EndObject() to close it.
	SwarmJsonBuilder AddObject(string key)
	{
		AppendKey(key);
		m_Buffer += "{";
		if (m_Pretty)
			m_Buffer += "\n";

		m_Indent++;
		PushContext(CTX_OBJECT);
		return this;
	}

	//! Start a nested array with key. Must call EndArray() to close it.
	SwarmJsonBuilder AddArray(string key)
	{
		AppendKey(key);
		m_Buffer += "[";
		if (m_Pretty)
			m_Buffer += "\n";

		m_Indent++;
		PushContext(CTX_ARRAY);
		return this;
	}

	//! Add a pre-built raw JSON string with key (no escaping applied)
	SwarmJsonBuilder AddRaw(string key, string rawJson)
	{
		AppendKey(key);
		AppendRawValue(rawJson);
		return this;
	}

	// ---------------------------------------------------------------
	// Array item methods (use inside BeginArray/EndArray)
	// ---------------------------------------------------------------

	//! Add a string item to the current array
	SwarmJsonBuilder ItemString(string value)
	{
		AppendItemPrefix();
		AppendRawValue("\"" + SwarmJSON.Escape(value) + "\"");
		return this;
	}

	//! Add an integer item to the current array
	SwarmJsonBuilder ItemInt(int value)
	{
		AppendItemPrefix();
		AppendRawValue(value.ToString());
		return this;
	}

	//! Add a float item to the current array
	SwarmJsonBuilder ItemFloat(float value)
	{
		AppendItemPrefix();
		AppendRawValue(value.ToString());
		return this;
	}

	//! Add a boolean item to the current array
	SwarmJsonBuilder ItemBool(bool value)
	{
		AppendItemPrefix();
		if (value)
			AppendRawValue("true");
		else
			AppendRawValue("false");
		return this;
	}

	//! Add a null item to the current array
	SwarmJsonBuilder ItemNull()
	{
		AppendItemPrefix();
		AppendRawValue("null");
		return this;
	}

	//! Start a nested object item in the current array. Must call EndObject() to close it.
	SwarmJsonBuilder ItemObject()
	{
		AppendItemPrefix();
		m_Buffer += "{";
		if (m_Pretty)
			m_Buffer += "\n";

		m_Indent++;
		PushContext(CTX_OBJECT);
		return this;
	}

	//! Start a nested array item in the current array. Must call EndArray() to close it.
	SwarmJsonBuilder ItemArray()
	{
		AppendItemPrefix();
		m_Buffer += "[";
		if (m_Pretty)
			m_Buffer += "\n";

		m_Indent++;
		PushContext(CTX_ARRAY);
		return this;
	}

	//! Add a pre-built raw JSON string as an array item (no escaping applied)
	SwarmJsonBuilder ItemRaw(string rawJson)
	{
		AppendItemPrefix();
		AppendRawValue(rawJson);
		return this;
	}

	// ---------------------------------------------------------------
	// Build
	// ---------------------------------------------------------------

	//! Returns the final JSON string. Auto-closes any unclosed objects/arrays.
	string Build()
	{
		// Auto-close any unclosed contexts
		while (m_ContextStack.Count() > 0)
		{
			int ctx = m_ContextStack.Get(m_ContextStack.Count() - 1);
			if (ctx == CTX_OBJECT)
				EndObject();
			else
				EndArray();
		}

		return m_Buffer;
	}
}
