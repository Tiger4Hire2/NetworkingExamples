#pragma once
#include "Reflection.h"
#include <iostream>
#include <iomanip>
#include <string_view>
#include <span>
#include <functional>
#include <unordered_map>
#include <algorithm>

struct UnexpectedCharacter: public std::exception
{

};

struct UnknownMemberName : public std::exception
{

};

inline void Check(std::istream& is, std::string_view expected)
{
	std::string next;
	if (!(is >> next) || std::string_view{ next }.substr(0, expected.size()) != expected)
		throw UnexpectedCharacter{};
	auto excess = std::string{ next.substr(expected.size()) };
	std::reverse(excess.begin(), excess.end());
	for (char c : excess)
		is.putback(c);
}

bool Test(std::istream& is, std::string_view expected)
{
	std::string next;
	if (!(is >> next) || std::string_view{ next }.substr(0, expected.size()) != expected)
	{
		std::reverse(next.begin(), next.end());
		for (char c : next)
			is.putback(c);
		return false;
	}
	auto excess = std::string{ next.substr(expected.size()) };
	std::reverse(excess.begin(), excess.end());
	for (char c : excess)
		is.putback(c);
	return true;
}

template<typename T> concept ArrayType = std::is_array_v<T> && !std::is_same_v<T, char>;
template<ArrayType AT> std::ostream& JSON(std::ostream& os, const AT& obj);
template<typename T> std::ostream& JSON(std::ostream& os, const T& obj);
std::ostream& JSON(std::ostream& os, std::string_view);
template<std::size_t N> std::ostream& JSON(std::ostream& os, const char (&)[N]);
std::ostream& JSON(std::ostream& os, const std::string& val);
std::ostream& JSON(std::ostream& os, bool);

template<ReflectionStruct RS>
std::ostream& JSON(std::ostream& os, const RS& obj)
{
	bool first{ true };
	enumerate(obj, [&](const auto& mbr, const auto& name)
		{
			os << (first ? "{\n" : ",\n");
			first = false;
			os << std::quoted(name) << " = ";
			JSON(os, mbr);
		});
	os << "}\n";
	return os;
}

template<std::size_t N>
std::ostream& JSON(std::ostream& os, const char(& c_str)[N])
{
	return JSON(os, std::string_view{ c_str, N });
}

template<ArrayType AT>
std::ostream& JSON(std::ostream& os, const AT& obj)
{
	os << "{";
	const auto as_span = std::span(obj);
	static_assert(!std::is_same_v<decltype(as_span[0]), char>);
	for (int idx = 0; idx<std::size(obj); ++idx)
			os << (idx ? ",\n" : "\n") << JSON(os, as_span[idx]) << "\n";
	os << "}\n";
	return os;
}

std::ostream& JSON(std::ostream& os, std::string_view val)
{
	os << std::quoted(val);
	return os;
}

std::ostream& JSON(std::ostream& os, const std::string& val)
{
	os << std::quoted(val);
	return os;
}

std::ostream& JSON(std::ostream& os, bool val)
{
	os << std::boolalpha << val;
	return os;
}

template<typename T>
std::ostream& JSON(std::ostream& os, const T& obj)
{
	os << obj;
	return os;
}

template<typename T> std::istream& JSON(std::istream& is, T& obj);


std::istream& JSON(std::istream& is, std::string& val)
{
	is >> std::quoted(val);
	return is;
}

template<std::size_t N>
std::istream& JSON(std::istream& is, char(&val)[N])
{
	std::string temp;
	is >> std::quoted(temp);
	const size_t copy_sz = std::min(temp.size(), N);
	std::copy_n(temp.begin(), copy_sz, val);
	return is;
}

std::istream& JSON(std::istream& is, bool val)
{
	is >> std::boolalpha >> val;
	return is;
}

template<ReflectionStruct RS>
std::istream& JSON(std::istream& is, RS& obj)
{
	using ReadFn = std::function<void(std::istream&)>;
	using member_map = std::unordered_map< std::string, ReadFn>;
	member_map members;
	enumerate(obj, [&](auto& mbr, const auto& name)
		{
			members.insert(std::make_pair(name, [&](std::istream& is)
				{
					JSON(is, mbr);
				}));
		});
	bool first{ true };
	while (!Test(is, "}"))
	{
		if (first)
			Check(is, "{");
		else
			Check(is, ",");
		first = false;
		std::string name;
		if (is >> std::quoted(name))
		{
			Check(is, "=");
			const auto found = members.find(name);
			if (found == members.end())
				throw UnknownMemberName{};
			const ReadFn& fn{ found->second };
			fn(is);
		}
	}
	return is;
}

template<ArrayType AT>
std::istream& JSON(std::istream& is, AT& obj)
{
	Check(is, "{");
	bool first{ true };
	while (Test(is, "}"))
	{
		using Element = decltype(obj[0]);
		if (first)
			Check(is, "{");
		else
			Check(is, ",");
		first = false;
		Element e;
		JSON(is, e);
		obj.emplace_back(e);
	}
	return is;
}
template<typename T>
std::istream& JSON(std::istream& is, T& obj)
{
	static_assert(std::is_fundamental_v<T>);
	static_assert(!std::is_const_v<T>);
	is >> obj;
	return is;
}

template<ReflectionStruct RS>
struct AsJson
{
	RS& m_obj;
	AsJson(RS& o) :m_obj(o) {}
};

template<ReflectionStruct RS>
std::istream& operator>>(std::istream& is, const AsJson<RS>& wrapper)
{
	JSON(is, wrapper.m_obj);
	return is;
}

template<ReflectionStruct RS>
std::ostream& operator<<(std::ostream& os, const AsJson<RS>& wrapper)
{
	JSON(os, wrapper.m_obj);
	return os;
}