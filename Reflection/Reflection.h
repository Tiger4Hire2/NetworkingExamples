#pragma once
#pragma once
#include <tuple>
#include <utility>
#include <string_view>

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
			void* mbr_list[] = { (void*)&(nullobj->*mbr.member) ... };
			for (size_t idx = 1; idx < std::size(mbr_list); ++idx)
				if (mbr_list[idx] <= mbr_list[idx - 1])
					return false;
			return true;
		}, mbrs);
}


template<typename T> concept ReflectionStruct =
requires(T t) { t.get_members(); }&&
members_are_same_class<T>();
// && members_are_ordered<T>();        Unfortunately the compiler does not like this
