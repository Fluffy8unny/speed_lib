#pragma once

#include <algorithm>
#include <charconv>
#include <cstring>
#include <format>
#include <optional>
#include <string>
#include <string_view>

#include "common.hpp"

namespace speed_lib
{
    template <typename DimensionTag>
    struct ParsedView
    {
        const char *unit = nullptr;
        std::optional<UnitForTag_t<DimensionTag>> target_unit = std::nullopt;
        std::optional<unsigned> width = std::nullopt;
        std::optional<unsigned> precision = std::nullopt;
    };

    /**
     * @brief Parses an unsigned integer from a string view starting at a given cursor position.
     * If parsing is successful, the cursor is advanced past the parsed number.
     *
     * @param[in] s The string view to parse from.
     * @param[in,out] cursor The current position in the string view. This will be updated to point behind the number if parsing is successful.
     * @return An optional containing the parsed unsigned integer if parsing was successful, or std::nullopt
     */
    constexpr std::optional<unsigned> parse_unsigned_at(std::string_view s, std::size_t &cursor)
    {
        if (cursor >= s.size())
            return std::nullopt;

        const auto tail = s.substr(cursor);

        unsigned v{};
        const auto [ptr, ec] = std::from_chars(tail.data(), tail.data() + tail.size(), v, 10);
        if (ptr == tail.data() or ec != std::errc{})
            return std::nullopt;

        cursor += static_cast<std::size_t>(ptr - tail.data());
        return v;
    }

    /**
     * @brief Tries to parse a specific unit suffix at the current cursor position.
     * @tparam DimensionTag Dimension category (e.g. SpeedTag).
     * @tparam ParsedUnit Candidate unit to parse (e.g. SPEED_UNIT::KMH).
     * @param[in] s Full format specifier input.
     * @param[in,out] cursor Current parse position, advanced on success.
     * @param[out] out Parsed view updated with unit metadata on success.
     * @return true when the candidate suffix is found and consumed; otherwise false.
     */
    template <typename DimensionTag, UnitForTag_t<DimensionTag> ParsedUnit>
    constexpr bool try_parse_unit(std::string_view s, std::size_t &cursor, ParsedView<DimensionTag> &out)
    {
        const auto suffix = UnitTraits<DimensionTag, ParsedUnit>::suffix;
        if (!s.substr(cursor).starts_with(suffix))
            return false;

        out.unit = UnitTraits<DimensionTag, ParsedUnit>::format_specifier;
        out.target_unit = ParsedUnit;
        cursor += std::strlen(suffix);
        return true;
    }

    /**
     * @brief Tries all provided units and parses the first matching unit suffix.
     * @tparam DimensionTag Dimension category (e.g. SpeedTag).
     * @tparam Units Candidate units that are valid for this formatter.
     * @param[in] s Full format specifier input.
     * @param[in,out] cursor Current parse position, advanced when a unit matches.
     * @param[out] out Parsed view updated with the matched unit metadata.
     * @return true if any unit suffix matched; otherwise false.
     */
    template <typename DimensionTag, UnitForTag_t<DimensionTag>... Units>
    constexpr bool try_parse_any_unit(std::string_view s, std::size_t &cursor, ParsedView<DimensionTag> &out)
    {
        return (try_parse_unit<DimensionTag, Units>(s, cursor, out) or ...);
    }

    /**
     * @brief Tries to parse a formatter specifier into unit, width, and precision components.
     * @tparam DimensionTag Dimension category (e.g. SpeedTag).
     * @tparam Units Valid unit suffixes for this dimension (e.g. SPEED_UNIT::MS, SPEED_UNIT::KMH).
     * @param[in] s Format specifier content without braces (e.g. "kmh8.2f").
     * @return ParsedView when the specifier is valid; otherwise std::nullopt.
     * @note The unit is mandatory. Width is optional. Precision is optional and uses ".<n>f" syntax.
     */
    template <typename DimensionTag, UnitForTag_t<DimensionTag>... Units>
    constexpr std::optional<ParsedView<DimensionTag>> parse_quantity(std::string_view s)
    {
        // We're parsing in 3 steps, that represent the 3 main components of a format specifier: the unit, the width, and the precision
        // The unit is mandatory, while the width and precision are optional
        std::size_t cursor = 0;
        auto consume_character = [&](char c) constexpr -> bool
        {
            return cursor < s.size() and s[cursor] == c ? (++cursor, true) : false;
        };
        // parse the unit
        ParsedView<DimensionTag> result{};
        if (!try_parse_any_unit<DimensionTag, Units...>(s, cursor, result))
            return std::nullopt; // no unit means it's not a valid format specifier

        // try to parse the width, which is an optional unsigned integer that comes after the unit
        if (auto w = parse_unsigned_at(s, cursor))
            result.width = *w;
        // try to parse the precision, which is an optional unsigned integer that comes after a '.' character, and is followed by an 'f'
        // this is meant to mirror the syntax of the precision specifier for floating point types in the standard library
        if (consume_character('.'))
        {
            if (auto p = parse_unsigned_at(s, cursor); p)
                result.precision = *p;
            else
                return std::nullopt;
            // can't forget the f
            if (!consume_character('f'))
                return std::nullopt;
        }
        // there is junk at the end, this is invalid
        if (cursor != s.size())
            return std::nullopt;

        return result;
    }

    // I'm willing to admit that using a variadic template here to generate an error message was just done for style points and is not strictly necessary
    /**
     * @brief Builds an error message for invalid format specifiers, listing the valid units for the given dimension.
     * @tparam DimensionTag Dimension category (e.g. SpeedTag).
     * @tparam FirstUnit First valid unit (e.g. SPEED_UNIT::MS).
     * @tparam RemainingUnits Remaining valid units (e.g. SPEED_UNIT::KMH).
     * @example auto msg = speed_lib::build_formatter_error_message<speed_lib::SpeedTag, speed_lib::SPEED_UNIT::MS>();
     */
    template <typename DimensionTag, UnitForTag_t<DimensionTag> FirstUnit, UnitForTag_t<DimensionTag>... RemainingUnits>
    std::string build_formatter_error_message()
    {
        std::string units;
        auto append_quoted_suffix = [&](const char *suffix)
        {
            units += std::format("{}'{}'", units.empty() ? "" : ", ", suffix);
        };

        append_quoted_suffix(UnitTraits<DimensionTag, FirstUnit>::suffix);
        (append_quoted_suffix(UnitTraits<DimensionTag, RemainingUnits>::suffix), ...);

        return std::format(
            "invalid format specifier for {}. Use {}, optionally followed by a number with an 'f' suffix (e.g., '{}8.2f').",
            UnitTraits<DimensionTag, FirstUnit>::dimension_name,
            units,
            UnitTraits<DimensionTag, FirstUnit>::suffix);
    }

    /**
     * @brief Formatter for Quantities. Prints the value followed by the unit.
     * Also parses the format string (:UNITWIDTH.PRECISIONf e.g. km10.4f) to allow for optional unit conversion and formatting options.
     * @note For example, "{:kmh8.2f}" would print the Quantity in km/h with a width of 8 and a precision of 2.
     * @tparam DimensionTag Dimension category (e.g. SpeedTag).
     * @tparam Unit Source unit (e.g. SPEED_UNIT::MS).
     * @tparam ValueType Numeric value type (e.g. long double).
     * @tparam ValidUnits Allowed target units (e.g. SPEED_UNIT::KMH).
     * @example std::format("{:kmh8.2f}", 10_ms);
     */
    template <typename DimensionTag, UnitForTag_t<DimensionTag> Unit, NumericalType ValueType, UnitForTag_t<DimensionTag>... ValidUnits>
    struct QuantityFormatter : std::formatter<ValueType, char>
    {
        ParsedView<DimensionTag> parsed_format;

        /**
         * @brief Converts the Quantity to the target unit if the target unit matches the candidate unit e.g. m->km
         * @tparam CandidateUnit Current candidate unit (e.g. SPEED_UNIT::KMH).
         * @tparam RemainingUnits Remaining candidate units (e.g. SPEED_UNIT::MPH).
         * @param[in] target_unit Requested target unit.
         * @param[in] quantity Source Quantity.
         * @param[out] formatted_value Converted numeric value in the matched candidate unit.
         * @example // Used internally by QuantityFormatter during format conversion.
         */
        template <UnitForTag_t<DimensionTag> CandidateUnit, UnitForTag_t<DimensionTag>... RemainingUnits>
        static constexpr void convert_if_unit_matches(
            UnitForTag_t<DimensionTag> target_unit,
            const Quantity<DimensionTag, Unit, ValueType> &quantity,
            ValueType &formatted_value)
        {
            if (target_unit == CandidateUnit)
            {
                formatted_value = quantity.template convert<CandidateUnit>().value;
                return;
            }

            if constexpr (sizeof...(RemainingUnits) > 0)
                convert_if_unit_matches<RemainingUnits...>(target_unit, quantity, formatted_value);
        }

        /**
         * @brief Parses the format specifier for the Quantity.
         * @param[in,out] ctx Format parse context consumed by the formatter.
         */
        constexpr auto parse(std::format_parse_context &ctx)
        {
            auto it = ctx.begin();
            auto end = ctx.end();

            if (it == end or *it == '}')
            {
                parsed_format = ParsedView<DimensionTag>{};
                return it;
            }

            auto start = it;
            it = std::find(it, end, '}');

            std::string_view spec{&*start, static_cast<size_t>(it - start)};
            if (auto parsed = parse_quantity<DimensionTag, ValidUnits...>(spec); parsed.has_value())
                parsed_format = *parsed;
            else
                throw std::format_error(build_formatter_error_message<DimensionTag, ValidUnits...>());

            return it;
        }

        /**
         * @brief Formats the Quantity according to the parsed format specifier.
         * @tparam FormatContext Formatting context type (e.g. std::format_context).
         * @param[in] quantity Quantity to format.
         * @param[in,out] ctx Output format context receiving formatted text.
         * @example std::format("{}", 10_ms);
         */
        template <typename FormatContext>
        auto format(const Quantity<DimensionTag, Unit, ValueType> &quantity, FormatContext &ctx) const
        {
            const auto unit = parsed_format.unit != nullptr
                                  ? parsed_format.unit
                                  : UnitTraits<DimensionTag, Unit>::format_specifier;

            ValueType formatted_value = quantity.value;
            if (parsed_format.target_unit.has_value())
            {
                const auto target_unit = *parsed_format.target_unit;
                convert_if_unit_matches<ValidUnits...>(target_unit, quantity, formatted_value);
            }

            if (parsed_format.width and parsed_format.precision)
            {
                std::format_to(ctx.out(), "{:>{}.{}f}", static_cast<long double>(formatted_value), *parsed_format.width, *parsed_format.precision);
            }
            else if (parsed_format.width)
            {
                std::format_to(ctx.out(), "{:>{}}", formatted_value, *parsed_format.width);
            }
            else if (parsed_format.precision)
            {
                std::format_to(ctx.out(), "{:.{}f}", static_cast<long double>(formatted_value), *parsed_format.precision);
            }
            else
            {
                std::format_to(ctx.out(), "{}", formatted_value);
            }

            return std::format_to(ctx.out(), " {}", unit);
        }
    };
}
