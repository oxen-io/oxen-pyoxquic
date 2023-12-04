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

    template <typename T>
    constexpr bool need_pointer_hack =
            std::is_same_v<T, Stream*> || std::is_same_v<T, dgram_interface*> ||
            std::is_same_v<T, Connection*> || std::is_same_v<T, connection_interface*> ||
            std::is_same_v<T, Endpoint*>;

    template <typename T>
    using pointer_hack_t = std::conditional_t<need_pointer_hack<T>, std::remove_pointer_t<T>&, T>;

    template <typename T, std::enable_if_t<need_pointer_hack<T>, int> = 0>
    T pointer_hack(std::remove_pointer_t<T>& ref)
    {
        static_assert(std::is_pointer_v<T>);
        static_assert(std::is_lvalue_reference_v<decltype(ref)>);
        return &ref;
    }
    template <typename T, std::enable_if_t<!need_pointer_hack<T>, int> = 0>
    T pointer_hack(T v)
    {
        return std::forward<T>(v);
    }

    // If we directly bind stream_open_callback and so on, which are std::function's taking a
    // reference such as `connection_interface&` argument, pybind will attempt to copy the object
    // (which fails, since most of these are non-copyable) before calling into the Python function,
    // but with a pointer Pybind doesn't do that, so work around it with this wrapper: we wrap
    // versions that take raw pointers with pybind, then use this function to convert those into the
    // reference-taking functions that libquic wants, where the body does the conversion to pointer
    // when invoked.
    //
    // For instance, call this with a `std::function<void(Stream*, int)>` and get back a
    // `std::function<void(Stream&, int)>` that calls the former when invoked.
    //
    // Also this function does a std::move on the given function for you, for free!  Hurray!
    template <typename R, typename... T>
    std::function<R(pointer_hack_t<T>...)> move_hack_function_wrapper(
            std::function<R(T... args)>& py_func)
    {
        static_assert(
                (need_pointer_hack<T> || ...),
                "make_function_wrapper requires a std::function with at least one "
                "Stream/connection_interface/Endpoint reference argument");

        if (py_func)
            return [f = std::move(py_func)](pointer_hack_t<T>... args) {
                return f(pointer_hack<T>(args)...);
            };
        return nullptr;
    }

    // Various typedefs for functions designed to be used with the above:
    using pystream_data = std::function<void(Stream*, bstring_view)>;
    using pystream_close = std::function<void(Stream*, uint64_t error_code)>;
    using pystream_constructor =
            std::function<std::shared_ptr<Stream>(Connection*, Endpoint*, std::optional<int64_t>)>;
    using pystream_open = std::function<uint64_t(Stream*)>;

    using pydgram_data = std::function<void(dgram_interface*, bstring)>;

    using pyconnection_established = std::function<void(connection_interface* conn)>;
    using pyconnection_closed = std::function<void(connection_interface* conn, uint64_t ec)>;

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
