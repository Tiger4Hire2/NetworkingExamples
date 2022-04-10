#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <span>
#include "Common/Reflection.h"

class NonVarIntTag{};
class FixedTag{};
class SignedTag{};

template<class T, class Tag = NonVarIntTag>
class BasicWrapper {
    T value;
public:
    typedef T value_type;
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


template <class T> using FixedInt = BasicWrapper<T,FixedTag>;
template <class T> using SignedInt = BasicWrapper<T,SignedTag>;

template<typename T> concept EnumType = std::is_enum_v<T>;
enum class WireType { VARINT, FIXED32=5, FIXED64=1, DELIMITED=2};
template<class T> constexpr WireType OnWireType(); // deliberatly not defined
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
std::byte EncodeField(FieldID id)
{
    return std::byte{static_cast<std::byte>(id<<3) | static_cast<std::byte>(OnWireType<T>())};
}


template<class T>
void WriteAsVarint(DataBlock& tgt, T&& obj)
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
void WriteAsSignedVarint32(DataBlock& tgt, T&& obj)
{
    const std::int32_t val = obj;
    const std::uint32_t sval = (val>=0)? (val<<1) : (abs(val+1)<<1)|1;
    WriteAsVarint(tgt, sval);
}

template<class T>
void WriteAsSignedVarint64(DataBlock& tgt, T&& obj)
{
    std::uint64_t val = obj;
    std::uint64_t sval = (val<<1);
    if (val&0x80000000)
        sval |= 1;
    WriteAsVarint(tgt, sval);
}


template<class T>
inline DataBlock& operator<<(DataBlock& tgt, const T& obj)
{
    switch(OnWireType<T>())
    {
    case WireType::VARINT:
        WriteAsVarint(tgt, obj);
        break;
    }
    return tgt;
}



template<ProtoStruct PS>
inline DataBlock& operator<<(DataBlock& tgt, const PS& obj)
{
    proto_visit(obj, [&tgt](const auto& mbr, const auto& id)
		{
            tgt << EncodeField<decltype(mbr)>(id);
//			tgt << mbr;
		});
    return tgt;
}