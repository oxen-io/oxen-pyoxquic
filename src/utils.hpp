#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <iostream>
#include <oxen/quic/utils.hpp>

namespace py = pybind11;

using namespace py::literals;

namespace oxen::quic
{
    // Helpers for constructing generic optional opt::whatever values from Python optional input
    // values.

    template <typename Opt, typename T>
    static std::optional<Opt> value_option(const std::optional<T>& val)
    {
        return val ? std::make_optional<Opt>(*val) : std::nullopt;
    }

    template <typename Opt>
    static std::optional<Opt> flag_option(bool enabled)
    {
        return enabled ? std::make_optional<Opt>() : std::nullopt;
    }

    template <typename Opt, typename TimeUnit = std::chrono::nanoseconds>
    static std::optional<Opt> duration_option(const std::optional<double>& seconds)
    {
        return seconds ? std::make_optional<Opt>(std::chrono::duration_cast<TimeUnit>(
                                 std::chrono::duration<double>(*seconds)))
                       : std::nullopt;
    }

    template <typename Opt>
    static std::optional<Opt> ustrings_option(
            const std::optional<std::vector<std::string_view>>& vals)
    {
        std::optional<Opt> result;
        if (vals)
        {
            std::vector<ustring> u_vals;
            u_vals.reserve(vals->size());
            for (const auto& v : *vals)
                u_vals.emplace_back(reinterpret_cast<const unsigned char*>(v.data()), v.size());
            result.emplace(std::move(u_vals));
        }
        return result;
    }

    using namespace pybind11;
    using namespace pybind11::detail;

    template <typename StringT>
    struct pybytes_caster
    {
        using Char = typename StringT::value_type;
        static_assert(
                sizeof(Char) == 1 && !std::is_same_v<Char, char>,
                "pybytes_caster requires a 1-byte, non-char type");

      public:
        PYBIND11_TYPE_CASTER(StringT, const_name("bytes"));

        bool load(handle src, bool)
        {
            char* buf;
            ssize_t len;

            if (!PyBytes_Check(src.ptr()))
                return false;

            if (PyBytes_AsStringAndSize(src.ptr(), &buf, &len) != 0)
                throw error_already_set{};

            value = StringT{
                    reinterpret_cast<typename StringT::value_type*>(buf), static_cast<size_t>(len)};
            return true;
        }

        static handle cast(StringT src, return_value_policy /* policy */, handle /* parent */)
        {
            return py::bytes{reinterpret_cast<const char*>(src.data()), src.size()}.release();
        }
    };

}  // namespace oxen::quic

namespace pybind11::detail
{
    template <>
    struct type_caster<oxen::quic::bstring> : oxen::quic::pybytes_caster<oxen::quic::bstring>
    {};
    template <>
    struct type_caster<oxen::quic::bstring_view>
            : oxen::quic::pybytes_caster<oxen::quic::bstring_view>
    {};
    template <>
    struct type_caster<oxen::quic::ustring> : oxen::quic::pybytes_caster<oxen::quic::ustring>
    {};
    template <>
    struct type_caster<oxen::quic::ustring_view>
            : oxen::quic::pybytes_caster<oxen::quic::ustring_view>
    {};
}  // namespace pybind11::detail
