#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <span>
#include <assert.h>
#include <numeric>
#include "Common/Reflection.h"
#include "Common/traits.h"

class NonVarIntTag{};
class FixedTag{};
class SignedTag{};

template<class T, class Tag = NonVarIntTag>
class BasicWrapper {
    T value;
public:
    using value_type = T;
    using tag_type = Tag;
    BasicWrapper() :value() {}
    BasicWrapper(T v) :value(v) {}
    operator T() const {return value;}
    //modifiers
    BasicWrapper& operator=(T v) {value=v; return *this;}
    BasicWrapper& operator+=(T v) {value+=v; return *this;}
    BasicWrapper& operator-=(T v) {value-=v; return *this;}
    BasicWrapper& operator*=(T v) {value*=value; return *this;}
    BasicWrapper& operator/=(T v) {value/=value; return *this;}
    BasicWrapper& operator%=(T v) {value%=value; return *this;}
    BasicWrapper& operator++() {++value; return *this;}
    BasicWrapper& operator--() {--value; return *this;}
    BasicWrapper operator++(int) {return BasicWrapper(value++);}
    BasicWrapper operator--(int) {return BasicWrapper(value--);}
    BasicWrapper& operator&=(T v) {value&=v; return *this;}
    BasicWrapper& operator|=(T v) {value|=v; return *this;}
    BasicWrapper& operator^=(T v) {value^=v; return *this;}
    BasicWrapper& operator<<=(T v) {value<<=v; return *this;}
    BasicWrapper& operator>>=(T v) {value>>=v; return *this;}
    // cast
    operator T() { return value; } 
    //accessors
    BasicWrapper operator+() const {return BasicWrapper(+value);}
    BasicWrapper operator-() const {return BasicWrapper(-value);}
    BasicWrapper operator!() const {return BasicWrapper(!value);}
    BasicWrapper operator~() const {return BasicWrapper(~value);}
    //friends
    friend BasicWrapper operator+(BasicWrapper iw, BasicWrapper v) {return iw+=v;}
    friend BasicWrapper operator+(BasicWrapper iw, T v) {return iw+=v;}
    friend BasicWrapper operator+(T v, BasicWrapper iw) {return BasicWrapper(v)+=iw;}
    friend BasicWrapper operator-(BasicWrapper iw, BasicWrapper v) {return iw-=v;}
    friend BasicWrapper operator-(BasicWrapper iw, T v) {return iw-=v;}
    friend BasicWrapper operator-(T v, BasicWrapper iw) {return BasicWrapper(v)-=iw;}
    friend BasicWrapper operator*(BasicWrapper iw, BasicWrapper v) {return iw*=v;}
    friend BasicWrapper operator*(BasicWrapper iw, T v) {return iw*=v;}
    friend BasicWrapper operator*(T v, BasicWrapper iw) {return BasicWrapper(v)*=iw;}
    friend BasicWrapper operator/(BasicWrapper iw, BasicWrapper v) {return iw/=v;}
    friend BasicWrapper operator/(BasicWrapper iw, T v) {return iw/=v;}
    friend BasicWrapper operator/(T v, BasicWrapper iw) {return BasicWrapper(v)/=iw;}
    friend BasicWrapper operator%(BasicWrapper iw, BasicWrapper v) {return iw%=v;}
    friend BasicWrapper operator%(BasicWrapper iw, T v) {return iw%=v;}
    friend BasicWrapper operator%(T v, BasicWrapper iw) {return BasicWrapper(v)%=iw;}
    friend BasicWrapper operator&(BasicWrapper iw, BasicWrapper v) {return iw&=v;}
    friend BasicWrapper operator&(BasicWrapper iw, T v) {return iw&=v;}
    friend BasicWrapper operator&(T v, BasicWrapper iw) {return BasicWrapper(v)&=iw;}
    friend BasicWrapper operator|(BasicWrapper iw, BasicWrapper v) {return iw|=v;}
    friend BasicWrapper operator|(BasicWrapper iw, T v) {return iw|=v;}
    friend BasicWrapper operator|(T v, BasicWrapper iw) {return BasicWrapper(v)|=iw;}
    friend BasicWrapper operator^(BasicWrapper iw, BasicWrapper v) {return iw^=v;}
    friend BasicWrapper operator^(BasicWrapper iw, T v) {return iw^=v;}
    friend BasicWrapper operator^(T v, BasicWrapper iw) {return BasicWrapper(v)^=iw;}
    friend BasicWrapper operator<<(BasicWrapper iw, BasicWrapper v) {return iw<<=v;}
    friend BasicWrapper operator<<(BasicWrapper iw, T v) {return iw<<=v;}
    friend BasicWrapper operator<<(T v, BasicWrapper iw) {return BasicWrapper(v)<<=iw;}
    friend BasicWrapper operator>>(BasicWrapper iw, BasicWrapper v) {return iw>>=v;}
    friend BasicWrapper operator>>(BasicWrapper iw, T v) {return iw>>=v;}
    friend BasicWrapper operator>>(T v, BasicWrapper iw) {return BasicWrapper(v)>>=iw;}
};


template<typename T> concept NonStringContainer =
requires(T t) { t.begin(); } &&
requires(T t) { t.end(); } &&
!std::is_same_v<std::string, T>;
	//members_are_ordered<T>();

template <class T> using FixedInt = BasicWrapper<T,FixedTag>;
template <class T> using SignedInt = BasicWrapper<T,SignedTag>;

template<typename T> concept EnumType = std::is_enum_v<T>;
enum class WireType { VARINT, FIXED32=5, FIXED64=1, DELIMITED=2};
template<class T> constexpr WireType OnWireType();
template<ProtoStruct PS> constexpr WireType OnWireType() { return WireType::DELIMITED; }
template<NonStringContainer PS> constexpr WireType OnWireType() { return WireType::DELIMITED; }

template<EnumType T> constexpr WireType OnWireType() { return WireType::VARINT; }
template<> constexpr WireType OnWireType<bool>() { return WireType::VARINT; }
template<> constexpr WireType OnWireType<int8_t>() { return WireType::VARINT; }
template<> constexpr WireType OnWireType<int16_t>() { return WireType::VARINT; }
template<> constexpr WireType OnWireType<int32_t>() { return WireType::VARINT; }
template<> constexpr WireType OnWireType<int64_t>() { return WireType::VARINT; }
template<> constexpr WireType OnWireType<uint8_t>() { return WireType::VARINT; }
template<> constexpr WireType OnWireType<uint16_t>() { return WireType::VARINT; }
template<> constexpr WireType OnWireType<uint32_t>() { return WireType::VARINT; }
template<> constexpr WireType OnWireType<uint64_t>() { return WireType::VARINT; }
// Signed
template<> constexpr WireType OnWireType<SignedInt<bool>>() { return WireType::VARINT; }
template<> constexpr WireType OnWireType<SignedInt<uint8_t>>() { return WireType::VARINT; }
template<> constexpr WireType OnWireType<SignedInt<int8_t>>() { return WireType::VARINT; }
template<> constexpr WireType OnWireType<SignedInt<uint16_t>>() { return WireType::VARINT; }
template<> constexpr WireType OnWireType<SignedInt<int16_t>>() { return WireType::VARINT; }
template<> constexpr WireType OnWireType<SignedInt<uint32_t>>() { return WireType::VARINT; }
template<> constexpr WireType OnWireType<SignedInt<int32_t>>() { return WireType::VARINT; }
template<> constexpr WireType OnWireType<SignedInt<uint64_t>>() { return WireType::VARINT; }
template<> constexpr WireType OnWireType<SignedInt<int64_t>>() { return WireType::VARINT; }
// Fixed
template<> constexpr WireType OnWireType<FixedInt<bool>>() { return WireType::FIXED32; }
template<> constexpr WireType OnWireType<FixedInt<uint8_t>>() { return WireType::FIXED32; }
template<> constexpr WireType OnWireType<FixedInt<int8_t>>() { return WireType::FIXED32; }
template<> constexpr WireType OnWireType<FixedInt<uint16_t>>() { return WireType::FIXED32; }
template<> constexpr WireType OnWireType<FixedInt<int16_t>>() { return WireType::FIXED32; }
template<> constexpr WireType OnWireType<FixedInt<uint32_t>>() { return WireType::FIXED32; }
template<> constexpr WireType OnWireType<FixedInt<int32_t>>() { return WireType::FIXED32; }
template<> constexpr WireType OnWireType<FixedInt<uint64_t>>() { return WireType::FIXED64; }
template<> constexpr WireType OnWireType<FixedInt<int64_t>>() { return WireType::FIXED64; }
template<> constexpr WireType OnWireType<float>() { return WireType::FIXED32; }
template<> constexpr WireType OnWireType<double>() { return WireType::FIXED64; }
// BString
template<> constexpr WireType OnWireType<std::string>() { return WireType::DELIMITED; }
template<> constexpr WireType OnWireType<char[]>() { return WireType::DELIMITED; }

template<class T> class IsSigned: public std::false_type {};
template<class T> class IsSigned<SignedInt<T>>: public std::true_type {};




using DataBlock = std::vector<std::byte>;

using ConstDataBlock = std::span<const std::byte>;
inline ConstDataBlock as_const(const DataBlock& v) {return ConstDataBlock{v}; }
inline DataBlock& operator<<(DataBlock& tgt, std::byte v)
{
    tgt.push_back(v);
    return tgt;
}

inline ConstDataBlock operator>>(ConstDataBlock src, std::byte& v)
{
    v = src[0];
    return ConstDataBlock{src.subspan(1)};
}

template<class T>
inline std::byte EncodeField(FieldID id)
{
    return std::byte{static_cast<std::byte>(id<<3) | static_cast<std::byte>(OnWireType<T>())};
}


template<class T>
inline void WriteAsVarint(DataBlock& tgt, T&& obj)
{
    std::uint64_t val = obj;
    do
    {
        std::byte next = static_cast<std::byte>(val)&std::byte{0x7F};
        val >>= 7;
        if (val)
            next |= std::byte{0x80};
        tgt << next;
    } while (val);
}

template<class T>
inline void WriteAsSignedVarint32(DataBlock& tgt, T&& obj)
{
    const std::int32_t val = static_cast<std::int32_t>(obj);
    const std::uint32_t sval = (val>=0) ? (val<<1) : (uint32_t)(std::abs(val+1)<<1)|1;
    WriteAsVarint(tgt, sval);
}

template<class T>
inline void WriteAsSignedVarint64(DataBlock& tgt, T&& obj)
{
    std::int64_t val = static_cast<std::int64_t>(obj);
    const std::uint64_t sval = (obj>=0) ? (val<<1) : ((uint64_t)((std::abs(val+1))<<1)|1);
    WriteAsVarint(tgt, sval);
}


template<class T>
inline void WriteAsFixed32(DataBlock& tgt, T&& obj)
{
    std::uint32_t val = static_cast<std::int32_t>(obj);
    const auto bytes = std::as_bytes(std::span{&val, 1});
    for(const auto& b: bytes)
        tgt << b;
}

template<class T>
inline void WriteAsFixed64(DataBlock& tgt, T&& obj)
{
    std::uint64_t val = static_cast<std::int32_t>(obj);
    const auto bytes = std::as_bytes(std::span{&val, 1});
    for(const auto& b: bytes)
        tgt << b;
}

template<class T>
inline void WriteDelimitedBytes(DataBlock& tgt, T&& obj)
{
    static_assert(sizeof(obj[0])==1);
    const auto as_span = std::span{obj.data(), obj.size()};
    const auto as_byte_span = std::as_bytes(as_span);
    WriteAsVarint(tgt, (int)as_byte_span.size());
    for (const std::byte& e: as_byte_span)
        tgt << e;
}

template<class T> class is_string: public std::false_type{};
template<class CHAR, class ALLOC> class is_string<std::basic_string<CHAR, ALLOC>>: public std::true_type{};

template<class T> struct is_non_string_conainer;
template<NonStringContainer T> struct is_non_string_conainer<T> : public std::true_type {};
template<class T> struct is_non_string_conainer : public std::false_type {};
template<class T> constexpr bool is_non_string_container_v = is_non_string_conainer<T>::value;

template<class T>
inline DataBlock& operator<<(DataBlock& tgt, const T& obj)
{
    using this_type = std::remove_const_t<std::remove_reference_t<T>>;
    constexpr auto type = OnWireType<this_type>();
    if constexpr (type==WireType::VARINT)
    {
        if constexpr (!IsSigned<this_type>())
            WriteAsVarint(tgt, obj);
        else if constexpr (IsSigned<this_type>::value && sizeof(typename this_type::value_type)<=4)
            WriteAsSignedVarint32(tgt, obj);
        else
            WriteAsSignedVarint64(tgt, obj);
    }
    if constexpr (type==WireType::FIXED32)
        WriteAsFixed32(tgt, obj);
    if constexpr (type==WireType::FIXED64)
        WriteAsFixed64(tgt, obj);
    if constexpr (type==WireType::DELIMITED)
    {
        if constexpr (std::is_same_v<this_type, std::string>)
            WriteDelimitedBytes(tgt, obj);
        else
            tgt << obj;
    }
    return tgt;
}

template<ProtoStruct PS>
inline DataBlock& operator<<(DataBlock& tgt, const PS& obj)
{
    proto_visit(obj, [&tgt](const auto& mbr, auto id)
		{
            using this_type = std::remove_const_t<std::remove_reference_t<decltype(mbr)>>;
            const auto tag = EncodeField<this_type>(id);
            if constexpr (is_non_string_container_v<this_type>)
            {
                for (const auto& elem:mbr)
                {
                    using elem_type = std::remove_const_t<std::remove_reference_t<decltype(elem)>>;
                    tgt << tag;
                    if constexpr (!is_proto_struct_v<elem_type>)
                        tgt << elem;
                    else
                    {
                        // message
                        DataBlock tmp;
                        tmp << elem;
                        WriteAsVarint(tgt, (int)std::size(tmp));
                        tgt.insert(tgt.end(), tmp.begin(), tmp.end());
                    }
                }
            }
            else if constexpr (!is_proto_struct_v<this_type>)
            {
                tgt << tag;
                tgt << mbr;
            }
            else
            {
                // message
                DataBlock tmp;
                tmp << mbr;
                tgt << tag;
                WriteAsVarint(tgt, (int)std::size(tmp));
                tgt.insert(tgt.end(), tmp.begin(), tmp.end());
            }
		});
    return tgt;
}