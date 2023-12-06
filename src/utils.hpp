#pragma once

#include <pybind11/cast.h>
#include <pybind11/functional.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <iostream>
#include <quic.hpp>

namespace py = pybind11;

using namespace py::literals;

// PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>);

namespace oxen::quic
{
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
            auto obj = reinterpret_steal<object>(PYBIND11_BYTES_FROM_STRING_AND_SIZE(
                    reinterpret_cast<const char*>(src.data()), src.size()));
            if (!obj)
            {
                pybind11_fail("Could not allocate bytes object!");
            }
            return obj;
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
