#undef SPEED_LIB_DEFINE_TAG_WITH_UNIT
#undef SPEED_LIB_DEFINE_UNIT_TRAITS
#undef SPEED_LIB_DEFINE_LITERAL_OPERATOR

#define SPEED_LIB_DEFINE_TAG_WITH_UNIT(_tag, _unit) \
    struct _tag                                     \
    {                                               \
    };                                              \
    template <>                                     \
    struct UnitForTag<_tag>                         \
    {                                               \
        using type = _unit;                         \
    };

#define SPEED_LIB_DEFINE_UNIT_TRAITS(_tag, _unit, scale, offset, _suffix, _format_specifier, _dimension_name) \
    template <>                                                                                               \
    struct UnitTraits<_tag, _unit>                                                                            \
    {                                                                                                         \
        static constexpr long double scale_to_base{scale};                 /**< Multiplicative factor to convert this unit value to the base unit. */ \
        static constexpr long double offset_to_base{offset};               /**< Additive offset applied after scaling when converting to the base unit. */ \
        static constexpr const char *suffix = _suffix;                     /**< Human-readable short suffix for this unit (for parsing/specification). */ \
        static constexpr const char *format_specifier = _format_specifier; /**< Display suffix used when formatting output text. */ \
        static constexpr const char *dimension_name = _dimension_name;     /**< Human-readable name of the physical dimension. */ \
    };

#define SPEED_LIB_DEFINE_LITERAL_OPERATOR(DimensionTag, UnitType, unit, suffix)                                    \
    constexpr Quantity<DimensionTag, UnitType::unit, LiteralBase> operator""_##suffix(LiteralBase value)           \
    {                                                                                                              \
        return Quantity<DimensionTag, UnitType::unit, LiteralBase>{value};                                         \
    }                                                                                                              \
    constexpr Quantity<DimensionTag, UnitType::unit, LiteralBaseInt> operator""_##suffix(unsigned long long value) \
    {                                                                                                              \
        return Quantity<DimensionTag, UnitType::unit, LiteralBaseInt>{static_cast<LiteralBaseInt>(value)};         \
    }
