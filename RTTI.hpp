#pragma once

#include <optional>
#include <iosfwd>

namespace RTTI {

    class IntegerType;
    class FloatType;
    class PointerType;

#define BIT(n) (1 << (n))

    enum class TypeFlags {
        None      = 0,
        Const     = BIT(0),
        Volatile  = BIT(1),
        Reference = BIT(2),
    };

    inline TypeFlags operator&(const TypeFlags& a, const TypeFlags& b) {
        using T = std::underlying_type_t<TypeFlags>;
        return static_cast<TypeFlags>(static_cast<T>(a) & static_cast<T>(b));
    }

    inline TypeFlags& operator&=(TypeFlags& a, const TypeFlags& b) {
        a = a & b;
        return a;
    }

    inline TypeFlags operator|(const TypeFlags& a, const TypeFlags& b) {
        using T = std::underlying_type_t<TypeFlags>;
        return static_cast<TypeFlags>(static_cast<T>(a) | static_cast<T>(b));
    }

    inline TypeFlags& operator|=(TypeFlags& a, const TypeFlags& b) {
        a = a | b;
        return a;
    }

#undef BIT

    template<typename T>
    inline TypeFlags GetTypeFlags() {
        TypeFlags flags = TypeFlags::None;
        if constexpr (!std::is_same_v<std::remove_reference_t<T>, std::remove_const_t<std::remove_reference_t<T>>>) {
            flags |= TypeFlags::Const;
        }
        if constexpr (!std::is_same_v<std::remove_reference_t<T>, std::remove_volatile_t<std::remove_reference_t<T>>>) {
            flags |= TypeFlags::Volatile;
        }
        if constexpr (!std::is_same_v<T, std::remove_reference_t<T>>) {
            flags |= TypeFlags::Reference;
        }
        return flags;
    }

    enum class TypeKind {
        Void,
        Char,
        Integer,
        Float,
        Pointer,
    };

    class Type {
    public:
        virtual ~Type() = default;
        [[nodiscard]] virtual std::size_t GetSize();
        [[nodiscard]] TypeKind GetKind() const;
        [[nodiscard]] TypeFlags GetFlags() const;
        [[nodiscard]] const IntegerType* AsIntegerType() const;
        [[nodiscard]] const FloatType* AsFloatType() const;
        [[nodiscard]] const PointerType* AsPointerType() const;

        friend std::ostream& operator<<(std::ostream& os, const Type& type);

        template<typename T>
        static const Type& Get() {
            return Get<std::remove_cvref_t<T>>(GetTypeFlags<T>());
        }
    private:
        template<typename T>
        static std::enable_if_t<!std::is_pointer_v<T>, const Type&> Get(TypeFlags flags);
        template<typename T>
        static std::enable_if_t<std::is_pointer_v<T>, const Type&> Get(TypeFlags flags);
    protected:
        Type(TypeKind kind, TypeFlags flags);
    private:
        TypeKind m_Kind;
        TypeFlags m_Flags;
    public:
        Type(Type const&) = delete;
        Type(Type&&)      = delete;
    };

    enum class IntegerKind {
        Char,
        Short,
        Int,
        Long,
        LongLong,
    };

    class IntegerType final: public Type {
        friend class Type;
    public:
        [[nodiscard]] std::size_t GetSize() override;
        [[nodiscard]] IntegerKind GetIntegerKind() const;
        [[nodiscard]] bool IsSigned() const;

        friend std::ostream& operator<<(std::ostream& os, const IntegerType& type);
    private:
        IntegerType(TypeFlags flags, IntegerKind kind, bool signed_);
        IntegerKind m_Kind;
        bool m_Signed;
    public:
        IntegerType(IntegerType const&) = delete;
        IntegerType(IntegerType&&)      = delete;
    };

    enum class FloatKind {
        Float,
        Double,
    };

    class FloatType final: public Type {
        friend class Type;
    public:
        [[nodiscard]] std::size_t GetSize() override;
        [[nodiscard]] FloatKind GetFloatKind() const;

        friend std::ostream& operator<<(std::ostream& os, const FloatType& type);
    private:
        FloatType(TypeFlags flags, FloatKind kind);
        FloatKind m_Kind;
    public:
        FloatType(FloatType const&) = delete;
        FloatType(FloatType&&)      = delete;
    };

    class PointerType final: public Type {
        friend class Type;
    public:
        [[nodiscard]] std::size_t GetSize() override;
        [[nodiscard]] const Type& GetPointedTo() const;

        friend std::ostream& operator<<(std::ostream& os, const PointerType& type);
    protected:
        PointerType(TypeFlags flags, const Type* pointedTo);
        const Type* m_PointedTo;
    public:
        PointerType(PointerType const&) = delete;
        PointerType(PointerType&&)      = delete;
    };

    template<typename T>
    inline std::enable_if_t<!std::is_pointer_v<T>, const Type&> Type::Get(TypeFlags flags) {
        static_assert(false, "unknown type");
    }

    template<typename T>
    inline std::enable_if_t<std::is_pointer_v<T>, const Type&> Type::Get(TypeFlags flags) {
        static const PointerType pointerType(flags, &Get<std::remove_pointer_t<T>>());
        return pointerType;
    }

    template<>
    inline const Type& Type::Get<void>(TypeFlags flags) {
        static const Type voidType(TypeKind::Void, flags);
        return voidType;
    }

    template<>
    inline const Type& Type::Get<char>(TypeFlags flags) {
        static const Type charType(TypeKind::Char, flags);
        return charType;
    }

    template<>
    inline const Type& Type::Get<signed char>(TypeFlags flags) {
        static const IntegerType integerType(flags, IntegerKind::Char, true);
        return integerType;
    }

    template<>
    inline const Type& Type::Get<unsigned char>(TypeFlags flags) {
        static const IntegerType integerType(flags, IntegerKind::Char, false);
        return integerType;
    }

    template<>
    inline const Type& Type::Get<signed short>(TypeFlags flags) {
        static const IntegerType integerType(flags, IntegerKind::Short, true);
        return integerType;
    }

    template<>
    inline const Type& Type::Get<unsigned short>(TypeFlags flags) {
        static const IntegerType integerType(flags, IntegerKind::Short, false);
        return integerType;
    }

    template<>
    inline const Type& Type::Get<signed int>(TypeFlags flags) {
        static const IntegerType integerType(flags, IntegerKind::Int, true);
        return integerType;
    }

    template<>
    inline const Type& Type::Get<unsigned int>(TypeFlags flags) {
        static const IntegerType integerType(flags, IntegerKind::Int, false);
        return integerType;
    }

    template<>
    inline const Type& Type::Get<signed long>(TypeFlags flags) {
        static const IntegerType integerType(flags, IntegerKind::Long, true);
        return integerType;
    }

    template<>
    inline const Type& Type::Get<unsigned long>(TypeFlags flags) {
        static const IntegerType integerType(flags, IntegerKind::Long, false);
        return integerType;
    }

    template<>
    inline const Type& Type::Get<signed long long>(TypeFlags flags) {
        static const IntegerType integerType(flags, IntegerKind::LongLong, true);
        return integerType;
    }

    template<>
    inline const Type& Type::Get<unsigned long long>(TypeFlags flags) {
        static const IntegerType integerType(flags, IntegerKind::LongLong, false);
        return integerType;
    }

}
