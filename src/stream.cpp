#include "utils.hpp"

namespace oxen::quic
{
    void pybind_stream(py::module_& m)
    {
        //py::class_<std::shared_ptr<void>>(m, "shared_ptr_void").def(py::init());

        py::class_<message>(m, "Message")
                .def(py::init<BTRequestStream&, std::string, bool>(), py::arg("bstream"),
                     py::arg("request"), py::arg("is_error") = false)
                .def("respond", &message::respond, py::arg("body"), py::arg("is_error") = false)
                .def("view", &message::view)
                .def("type", &message::type)
                .def("endpoint", &message::endpoint_str)
                .def("body", &message::body_str)
                .def("scid", &message::scid)
                .def("rid", &message::rid)
                .def("stream", &message::stream)
                .def("__bool__", &message::operator bool);

        py::class_<Stream, std::shared_ptr<Stream>>(m, "Stream")
                .def(py::init([](Connection& conn, Endpoint& endpoint,
                                 stream_data_callback data_cb = nullptr,
                                 stream_close_callback close_cb = nullptr) {
                         return std::make_shared<Stream>(
                                 conn, endpoint, std::move(data_cb), std::move(close_cb));
                     }),
                     py::arg("conn"), py::arg("endpoint"), py::arg("data_cb") = nullptr,
                     py::arg("close_cb") = nullptr)
                .def("available", &Stream::available)
                .def("stream_id", &Stream::stream_id)
                .def("has_unsent", &Stream::has_unsent)
                .def(
                        "close",
                        [](Stream& self, uint64_t error_code) { self.close(io_error{error_code}); },
                        py::arg("error_code"))
                .def(
                        "send", [](Stream& self, bstring data) { self.send(data); },
                        py::arg("data"));

        py::class_<BTRequestStream, std::shared_ptr<BTRequestStream>>(m, "BTStream")
                .def(py::init([](Connection& conn, Endpoint& endpoint,
                                 std::function<void(Stream&, uint64_t)> close_cb = nullptr) {
                    return std::make_shared<BTRequestStream>(conn, endpoint, std::move(close_cb));
                }))
                .def(
                        "request",
                        [](BTRequestStream& bstream, std::string endpoint, std::string body,
                           std::function<void(message)> hook) {
                            bstream.command(std::move(endpoint), std::move(body), std::move(hook));
                        },
                        py::arg("request"), py::arg("body"), py::arg("hook"))
                .def(
                        "command",
                        [](BTRequestStream& bstream, std::string endpoint, std::string body) {
                            bstream.command(std::move(endpoint), std::move(body));
                        },
                        py::arg("command"), py::arg("body"))
                .def("register_command", &BTRequestStream::register_command, py::arg("endpoint"),
                     py::arg("hook"));
    }

}  // namespace oxen::quic
