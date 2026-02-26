/**
 * JsonNode
 * 
 * Represents a dynamic JSON value that can be null, bool, int, float, string,
 * array, or object. Provides type checking, value access, and mutation methods.
 * 
 * Use the static factory methods to create nodes:
 *   JsonNode node = JsonNode.NewString("hello");
 *   JsonNode obj  = JsonNode.NewObject();
 *   obj.Set("key", JsonNode.NewInt(42));
 * 
 * Use Get/GetAt to traverse parsed JSON trees:
 *   JsonNode root = Json.Parse(jsonString);
 *   string name = root.Get("name").GetString();
 *   int count   = root.Get("items").GetAt(0).GetInt();
 */
class JsonNode
{
	protected JsonType m_Type;
	protected bool m_BoolValue;
	protected int m_IntValue;
	protected float m_FloatValue;
	protected string m_StringValue;
	protected ref array<ref JsonNode> m_ArrayValue;
	protected ref map<string, ref JsonNode> m_ObjectValue;
	protected ref array<string> m_ObjectKeys; // preserves insertion order

	// ---------------------------------------------------------------
	// Constructors (private - use static factory methods)
	// ---------------------------------------------------------------

	void JsonNode()
	{
		m_Type = JsonType.NULL_VALUE;
	}

	// ---------------------------------------------------------------
	// Static factory methods
	// ---------------------------------------------------------------

	//! Create a null node
	static JsonNode NewNull()
	{
		JsonNode node = new JsonNode();
		node.m_Type = JsonType.NULL_VALUE;
		return node;
	}

	//! Create a boolean node
	static JsonNode NewBool(bool value)
	{
		JsonNode node = new JsonNode();
		node.m_Type = JsonType.BOOL;
		node.m_BoolValue = value;
		return node;
	}

	//! Create an integer node
	static JsonNode NewInt(int value)
	{
		JsonNode node = new JsonNode();
		node.m_Type = JsonType.INT;
		node.m_IntValue = value;
		return node;
	}

	//! Create a float node
	static JsonNode NewFloat(float value)
	{
		JsonNode node = new JsonNode();
		node.m_Type = JsonType.FLOAT;
		node.m_FloatValue = value;
		return node;
	}

	//! Create a string node
	static JsonNode NewString(string value)
	{
		JsonNode node = new JsonNode();
		node.m_Type = JsonType.STRING;
		node.m_StringValue = value;
		return node;
	}

	//! Create an empty array node
	static JsonNode NewArray()
	{
		JsonNode node = new JsonNode();
		node.m_Type = JsonType.ARRAY;
		node.m_ArrayValue = new array<ref JsonNode>();
		return node;
	}

	//! Create an empty object node
	static JsonNode NewObject()
	{
		JsonNode node = new JsonNode();
		node.m_Type = JsonType.OBJECT;
		node.m_ObjectValue = new map<string, ref JsonNode>();
		node.m_ObjectKeys = new array<string>();
		return node;
	}

	// ---------------------------------------------------------------
	// Type checking
	// ---------------------------------------------------------------

	//! Returns the JSON type of this node
	JsonType GetType()
	{
		return m_Type;
	}

	bool IsNull()
	{
		return m_Type == JsonType.NULL_VALUE;
	}

	bool IsBool()
	{
		return m_Type == JsonType.BOOL;
	}

	bool IsInt()
	{
		return m_Type == JsonType.INT;
	}

	bool IsFloat()
	{
		return m_Type == JsonType.FLOAT;
	}

	//! Returns true if the node is an int or float
	bool IsNumber()
	{
		return m_Type == JsonType.INT || m_Type == JsonType.FLOAT;
	}

	bool IsString()
	{
		return m_Type == JsonType.STRING;
	}

	bool IsArray()
	{
		return m_Type == JsonType.ARRAY;
	}

	bool IsObject()
	{
		return m_Type == JsonType.OBJECT;
	}

	// ---------------------------------------------------------------
	// Value getters
	// ---------------------------------------------------------------

	//! Get boolean value (returns false if not a bool node)
	bool GetBool()
	{
		return m_BoolValue;
	}

	//! Get integer value (returns 0 if not an int node)
	int GetInt()
	{
		if (m_Type == JsonType.FLOAT)
			return (int)m_FloatValue;

		return m_IntValue;
	}

	//! Get float value (returns 0.0 if not a float node)
	float GetFloat()
	{
		if (m_Type == JsonType.INT)
			return (float)m_IntValue;

		return m_FloatValue;
	}

	//! Get string value (returns empty string if not a string node)
	string GetString()
	{
		return m_StringValue;
	}

	// ---------------------------------------------------------------
	// Object access
	// ---------------------------------------------------------------

	//! Get a child node by key (object mode). Returns null if key not found.
	JsonNode Get(string key)
	{
		if (m_Type != JsonType.OBJECT || !m_ObjectValue)
			return null;

		ref JsonNode value;
		if (m_ObjectValue.Find(key, value))
			return value;

		return null;
	}

	//! Check if the object contains a key
	bool Has(string key)
	{
		if (m_Type != JsonType.OBJECT || !m_ObjectValue)
			return false;

		return m_ObjectValue.Contains(key);
	}

	//! Get all keys of the object in insertion order
	array<string> GetKeys()
	{
		if (m_Type != JsonType.OBJECT || !m_ObjectKeys)
			return new array<string>();

		return m_ObjectKeys;
	}

	//! Set a key-value pair on the object. If the key exists, it is replaced.
	void Set(string key, JsonNode value)
	{
		if (m_Type != JsonType.OBJECT)
			return;

		if (!m_ObjectValue)
		{
			m_ObjectValue = new map<string, ref JsonNode>();
			m_ObjectKeys = new array<string>();
		}

		if (!m_ObjectValue.Contains(key))
			m_ObjectKeys.Insert(key);

		m_ObjectValue.Set(key, value);
	}

	//! Remove a key from the object
	void Remove(string key)
	{
		if (m_Type != JsonType.OBJECT || !m_ObjectValue)
			return;

		if (m_ObjectValue.Contains(key))
		{
			m_ObjectValue.Remove(key);
			int idx = m_ObjectKeys.Find(key);
			if (idx >= 0)
				m_ObjectKeys.RemoveOrdered(idx);
		}
	}

	// ---------------------------------------------------------------
	// Array access
	// ---------------------------------------------------------------

	//! Get a child node by index (array mode). Returns null if out of bounds.
	JsonNode GetAt(int index)
	{
		if (m_Type != JsonType.ARRAY || !m_ArrayValue)
			return null;

		if (index < 0 || index >= m_ArrayValue.Count())
			return null;

		return m_ArrayValue.Get(index);
	}

	//! Get the number of elements (array) or keys (object)
	int Count()
	{
		if (m_Type == JsonType.ARRAY && m_ArrayValue)
			return m_ArrayValue.Count();

		if (m_Type == JsonType.OBJECT && m_ObjectValue)
			return m_ObjectValue.Count();

		return 0;
	}

	//! Append a node to the array
	void Add(JsonNode value)
	{
		if (m_Type != JsonType.ARRAY)
			return;

		if (!m_ArrayValue)
			m_ArrayValue = new array<ref JsonNode>();

		m_ArrayValue.Insert(value);
	}

	//! Remove an element from the array by index
	void RemoveAt(int index)
	{
		if (m_Type != JsonType.ARRAY || !m_ArrayValue)
			return;

		if (index >= 0 && index < m_ArrayValue.Count())
			m_ArrayValue.RemoveOrdered(index);
	}

	// ---------------------------------------------------------------
	// Convenience
	// ---------------------------------------------------------------

	//! Returns a human-readable string representation of the value
	string AsString()
	{
		switch (m_Type)
		{
			case JsonType.NULL_VALUE:
				return "null";
			case JsonType.BOOL:
				if (m_BoolValue)
					return "true";
				return "false";
			case JsonType.INT:
				return m_IntValue.ToString();
			case JsonType.FLOAT:
				return m_FloatValue.ToString();
			case JsonType.STRING:
				return m_StringValue;
			case JsonType.ARRAY:
				return "[Array:" + Count().ToString() + "]";
			case JsonType.OBJECT:
				return "{Object:" + Count().ToString() + "}";
		}

		return "undefined";
	}
}
