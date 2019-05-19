#ifndef PBUS_STRING_H
#define PBUS_STRING_H

#include <string>
#include <utility>

namespace pbus
{
	class String
	{
		std::string		_str;
		std::size_t		_hash;

	public:
		template<typename ... Args>
		String(Args ... args) noexcept:
			_str(std::forward<Args>(args)...),
			_hash(std::hash<std::string>()(_str))
		{ }

		const std::string & Get() const
		{ return _str; }
		std::size_t GetHash() const
		{ return _hash; }

		bool operator == (const String &str) const
		{ return _str == str._str; }

		template<typename T>
		bool operator != (const String &str) const
		{ return !((*this) == str); }

		struct Hash
		{
			std::size_t operator() (const String & str) const
			{ return str.GetHash(); }
		};
	};
}

TOOLKIT_DECLARE_STD_HASH(pbus::String, pbus::String::Hash)

#endif

