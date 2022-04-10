#include "Common/Reflection.h"
#include "Common/Json.h"
#include "protobuf.h"
#include <utility>
#include <span>
#include <iostream>
#include <sstream>
#include <cinttypes>

struct Person {
  enum class PhoneType {
    MOBILE = 0,
    HOME = 1,
    WORK = 2
  };

  std::string name;
  FixedInt<std::int32_t> id;
  std::string email;


  struct PhoneNumber {
    std::string number;
    PhoneType type;
	static constexpr auto get_members()
	{
		return std::make_tuple(
			ProtoMember("number", 1, &PhoneNumber::number),
			ProtoMember("type", 2, &PhoneNumber::type)
		);
	}
  };

  std::vector<PhoneNumber> phones;

//  google.protobuf.Timestamp last_updated = 5;

	static constexpr auto get_members()
	{
		return std::make_tuple(
			ProtoMember("name", 1, &Person::name),
			ProtoMember("id", 2, &Person::id),
			ProtoMember("email", 3, &Person::email),
			ProtoMember("phones", 4, &Person::phones)
		);
	}
};
std::string to_string(Person::PhoneType pt)
{
	switch(pt)
	{
		case Person::PhoneType::MOBILE: return "Mobile"; break;
		case Person::PhoneType::HOME: return "Home"; break;
		case Person::PhoneType::WORK: return "Work"; break;
	}
	return "illegal value";
}


struct AddressBook
{
    std::vector<Person> person;

	static constexpr auto get_members()
	{
		return std::make_tuple(
			ProtoMember("person", 1, &AddressBook::person)
		);
	}
};

template<class T>
void ReverseEndian(T& v)
{
	const auto as_bytes = std::as_writable_bytes(std::span(&v, 1));
	std::reverse(as_bytes.begin(), as_bytes.end());
}


int main()
{
	AddressBook book{ {Person{.name="Frank", .id=0, .email="frank@mailinator.com", .phones={{"999", Person::PhoneType::WORK}}},
					   Person{.name="Jane", .id=1, .email="jane@mailinator.com", .phones={{"012345678", Person::PhoneType::HOME}}}} };

	DataBlock data;
//	data << book;
	std::cout << std::endl;
};