#include <pybind11/cast.h>

#include <oxen/quic/btstream.hpp>

#include "callbacks.hpp"
#include "common.hpp"
#include "utils.hpp"

namespace oxen::quic
{
    void pybind_btreqstream(py::module_& m)
    {
        py::class_<message>(m, "Message")
                .def("respond",
                     py::overload_cast<std::string_view, bool>(&message::respond, py::const_),
                     "body"_a,
                     "is_error"_a = false)
                .def("respond",
                     py::overload_cast<bstring_view, bool>(&message::respond, py::const_),
                     "body"_a,
                     "is_error"_a = false)
                .def_property_readonly("type", &message::type)
                .def_property_readonly("endpoint", &message::endpoint)
                .def_property_readonly("body", &message::body<std::byte>)
                .def_property_readonly("conn_rid", &message::conn_rid)
                .def_property_readonly("request_id", &message::rid)
                .def_property_readonly("stream", &message::stream)
                .def("__bool__", &message::operator bool);

        py::class_<BTRequestStream, Stream, std::shared_ptr<BTRequestStream>>(m, "BTRequestStream")
                .def(
                        "request",
                        [](BTRequestStream& bstream,
                           std::string endpoint,
                           std::string body,
                           std::function<void(message)> hook,
                           std::optional<double> timeout) {
                            bstream.command(
                                    std::move(endpoint),
                                    std::move(body),
                                    std::move(hook),
                                    duration_option<
                                            std::chrono::milliseconds,
                                            std::chrono::milliseconds>(timeout));
                        },
                        "request"_a,
                        "body"_a,
                        "hook"_a,
                        py::kw_only{},
                        "timeout"_a = nullptr)
                .def(
                        "command",
                        [](BTRequestStream& bstream, std::string endpoint, std::string body) {
                            bstream.command(std::move(endpoint), std::move(body));
                        },
                        "command"_a,
                        "body"_a)

                .def("register_handler", &BTRequestStream::register_handler, "endpoint"_a, "hook"_a)
                .def("register_generic_handler",
                     &BTRequestStream::register_generic_handler,
                     "hook"_a);
        /**/;
    }

}  // namespace oxen::quic
