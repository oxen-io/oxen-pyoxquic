#include <pybind11/cast.h>
#include <pybind11/functional.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>

#include <iostream>
#include <quic.hpp>

namespace py = pybind11;

PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>);

namespace PYBIND11_NAMESPACE
{
    namespace detail
    {
        template <>
        struct type_caster<oxen::quic::bstring>
        {
          public:
            PYBIND11_TYPE_CASTER(oxen::quic::bstring, const_name("bstring"));

            bool load(handle src, bool)
            {
                char* buf;
                ssize_t len;

                if (PyBytes_AsStringAndSize(src.ptr(), &buf, &len) != 0)
                    throw error_already_set{};

                value = oxen::quic::bstring{
                        reinterpret_cast<std::byte*>(buf), static_cast<size_t>(len)};
                return true;
            }

            static handle cast(
                    oxen::quic::bstring src, return_value_policy /* policy */, handle /* parent */)
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

        template <>
        struct type_caster<oxen::quic::bstring_view>
        {
          public:
            PYBIND11_TYPE_CASTER(oxen::quic::bstring_view, const_name("bstring_view"));

            bool load(handle src, bool)
            {
                char* buf;
                ssize_t len;

                if (PyBytes_AsStringAndSize(src.ptr(), &buf, &len) != 0)
                    throw error_already_set{};

                value = oxen::quic::bstring_view{
                        reinterpret_cast<std::byte*>(buf), static_cast<size_t>(len)};
                return true;
            }

            static handle cast(
                    oxen::quic::bstring_view src, return_value_policy /* policy */,
                    handle /* parent */)
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
    }  // namespace detail
}  // namespace PYBIND11_NAMESPACE

namespace oxen::quic
{}
