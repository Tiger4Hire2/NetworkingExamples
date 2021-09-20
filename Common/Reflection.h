#pragma once
#include <tuple>
#include <utility>
#include <string_view>
class ObjectStart{};
class ObjectEnd{};

template <class Cls, class T>
struct Member
{
	constexpr Member(std::string_view n, T Cls::* m)
		: name(n)
		, pointer(m)
	{}
	std::string_view	name;
	T Cls::* pointer;
};
template<class Class, class ... Members> struct is_same_class : public std::false_type {};
template<class Class, class ... Members>
struct is_same_class <std::tuple<Member<Class, Members>...>> : public std::true_type
{
};
template<class T> constexpr bool members_are_same_class()
{
	return is_same_class<decltype(T::get_members())>{}();
}

template<class T>
constexpr bool members_are_ordered()
{
	const auto mbrs = T::get_members();
	return std::apply([](const auto...mbr)
		{
			constexpr auto nullobj = (T*)nullptr;
			std::ptrdiff_t mbr_list[] = { (std::ptrdiff_t)&(nullobj->*mbr.pointer) ... };
			for (size_t idx = 1; idx < std::size(mbr_list); ++idx)
				if (mbr_list[idx] <= mbr_list[idx - 1])
					return false;
			return true;
		}, mbrs);
}

template<typename T> concept ReflectionStruct =
requires(T t) { t.get_members(); } &&
	members_are_same_class<T>();// && 
	//members_are_ordered<T>();


template <typename T, typename Fn>
void visit_recursive(T& obj, Fn&& fn)
{
	fn(obj);
};

template <ReflectionStruct RS, typename Fn>
void visit_recursive(RS& obj, Fn&& fn)
{
	const auto mbrs = RS::get_members();

	const auto call_fn = [&](const auto&...mbr)
	{
		(visit_recursive(obj.*mbr.pointer, std::forward<Fn>(fn)), ...);
	};
	std::apply(call_fn, mbrs);
};


template <ReflectionStruct RS, typename Fn>
void visit(RS& obj, Fn&& fn)
{
	const auto mbrs = RS::get_members();

	const auto call_fn = [&](const auto&...mbr)
	{
		(fn(obj.*mbr.pointer), ...);
	};
	std::apply(call_fn, mbrs);
};

template <typename T, typename Fn>
void visit_recursive(T& obj, std::string_view name, Fn&& fn)
{
	fn(name, obj);
};

template <ReflectionStruct RS, typename Fn>
void enumerate_recursive(RS& obj, Fn&& fn)
{
	const auto mbrs = RS::get_members();

	const auto call_fn = [&](const auto&...mbr)
	{
		(enumerate_recursive(obj.*mbr.pointer, mbr.name, std::forward<Fn>(fn)), ...);
	};
	std::apply(call_fn, mbrs);
};

template <ReflectionStruct RS, typename Fn>
void enumerate_recursive(RS& obj, std::string_view name, Fn&& fn)
{
	const auto mbrs = RS::get_members();
	fn(ObjectStart{}, name);
	const auto call_fn = [&](const auto&...mbr)
	{
		(enumerate_recursive(obj.*mbr.pointer, mbr.name, std::forward<Fn>(fn)), ...);
	};
	std::apply(call_fn, mbrs);
	fn(ObjectEnd{}, name);
};

template <typename T, typename Fn>
void enumerate_recursive(T& ptr, std::string_view name, Fn&& fn)
{
	fn(ptr, name);
};


template <ReflectionStruct RS, typename Fn>
void enumerate(RS& obj, Fn&& fn)
{
	const auto mbrs = RS::get_members();

	const auto call_fn = [&](const auto&...mbr)
	{
		(fn(obj.*mbr.pointer, mbr.name), ...);
	};
	std::apply(call_fn, mbrs);
};


#define DECL(TYPE, MBR) Member(#MBR, &TYPE::MBR),
