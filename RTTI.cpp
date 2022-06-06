#include "RTTI.hpp"

#include <ostream>

namespace RTTI {

    Type::Type(TypeKind kind, TypeFlags flags) : m_Kind(kind), m_Flags(flags) {}

    std::size_t Type::GetSize() {
        switch (m_Kind) {
            case TypeKind::Void:
                return 0;
            case TypeKind::Char:
                return 1;
            case TypeKind::Integer:
            case TypeKind::Float:
            case TypeKind::Pointer:
                return 0;
        }
        return 0;
    }

    TypeKind Type::GetKind() const {
        return m_Kind;
    }

    TypeFlags Type::GetFlags() const {
        return m_Flags;
    }

    const IntegerType* Type::AsIntegerType() const {
        if (m_Kind == TypeKind::Integer)
            return dynamic_cast<const IntegerType*>(this);
        return nullptr;
    }

    const FloatType* Type::AsFloatType() const {
        if (m_Kind == TypeKind::Float)
            return dynamic_cast<const FloatType*>(this);
        return nullptr;
    }

    const PointerType* Type::AsPointerType() const {
        if (m_Kind == TypeKind::Pointer)
            return dynamic_cast<const PointerType*>(this);
        return nullptr;
    }

    std::ostream& operator<<(std::ostream& os, const Type& type) {
        switch (type.m_Kind) {
            case TypeKind::Void:
                os << "void";
                break;
            case TypeKind::Char:
                os << "char";
                break;
            case TypeKind::Integer:
                os << *type.AsIntegerType();
                break;
            case TypeKind::Float:
                os << *type.AsFloatType();
                break;
            case TypeKind::Pointer:
                os << *type.AsPointerType();
                break;
        }
        if ((type.m_Flags & TypeFlags::Const) != TypeFlags::None)
            os << " const";
        if ((type.m_Flags & TypeFlags::Volatile) != TypeFlags::None)
            os << " volatile";
        if ((type.m_Flags & TypeFlags::Reference) != TypeFlags::None)
            os << "&";
        return os;
    }

    IntegerType::IntegerType(TypeFlags flags, IntegerKind kind, bool signed_)
        : Type(TypeKind::Integer, flags), m_Kind(kind), m_Signed(signed_) {}

    [[nodiscard]] std::size_t IntegerType::GetSize() {
        switch (m_Kind) {
            case IntegerKind::Char:
                return sizeof(char);
            case IntegerKind::Short:
                return sizeof(short);
            case IntegerKind::Int:
                return sizeof(int);
            case IntegerKind::Long:
                return sizeof(long);
            case IntegerKind::LongLong:
                return sizeof(long long);
        }
        return 0;
    }

    IntegerKind IntegerType::GetIntegerKind() const {
        return m_Kind;
    }

    bool IntegerType::IsSigned() const {
        return m_Signed;
    }

    std::ostream& operator<<(std::ostream& os, const IntegerType& type) {
        if (type.m_Signed) {
            os << "signed ";
        } else {
            os << "unsigned ";
        }
        switch (type.m_Kind) {
            case IntegerKind::Char:
                os << "char";
                break;
            case IntegerKind::Short:
                os << "short";
                break;
            case IntegerKind::Int:
                os << "int";
                break;
            case IntegerKind::Long:
                os << "long";
                break;
            case IntegerKind::LongLong:
                os << "long long";
                break;
        }
        return os;
    }

    FloatType::FloatType(TypeFlags flags, FloatKind kind) : Type(TypeKind::Float, flags), m_Kind(kind) {}

    [[nodiscard]] std::size_t FloatType::GetSize() {
        switch (m_Kind) {
            case FloatKind::Float:
                return sizeof(float);
            case FloatKind::Double:
                return sizeof(double);
        }
        return 0;
    }

    [[nodiscard]] FloatKind FloatType::GetFloatKind() const {
        return m_Kind;
    }

    std::ostream& operator<<(std::ostream& os, const FloatType& type) {
        switch (type.m_Kind) {
            case FloatKind::Float:
                os << "float";
                break;
            case FloatKind::Double:
                os << "double";
                break;
        }
        return os;
    }

    PointerType::PointerType(TypeFlags flags, const Type* pointedTo) : Type(TypeKind::Pointer, flags), m_PointedTo(pointedTo) {}

    [[nodiscard]] std::size_t PointerType::GetSize() {
        return sizeof(void*);
    }

    const Type& PointerType::GetPointedTo() const {
        return *m_PointedTo;
    }

    std::ostream& operator<<(std::ostream& os, const PointerType& type) {
        return os << *type.m_PointedTo << "*";
    }

}
