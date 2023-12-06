#include "callbacks.hpp"
#include "utils.hpp"

namespace oxen::quic
{
    void pybind_stream(py::module_& m)
    {
        py::class_<Stream, std::shared_ptr<Stream>>(m, "Stream")
                .def(py::init([](connection_interface& conni,
                                 Endpoint& endpoint,
                                 std::optional<py::function> data_cb,
                                 std::optional<py::function> close_cb) {
                         auto* conn = dynamic_cast<Connection*>(&conni);
                         if (!conn)
                             throw std::runtime_error{
                                     "Internal error: can't downcast to Connection!"};

                         return std::make_shared<Stream>(
                                 *conn,
                                 endpoint,
                                 wrap_stream_data_cb(data_cb),
                                 wrap_stream_close_cb(close_cb));
                     }),
                     "conn"_a,
                     "endpoint"_a,
                     "data_cb"_a = nullptr,
                     "close_cb"_a = nullptr)
                .def_property_readonly("available", &Stream::available)
                .def_property_readonly("id", &Stream::stream_id)
                .def_property_readonly("has_unsent", &Stream::has_unsent)
                .def(
                        "close",
                        [](Stream& self, uint64_t error_code) { self.close(io_error{error_code}); },
                        "error_code"_a)
                .def(
                        "send",
                        [](Stream& self, bstring data) { self.send(std::move(data)); },
                        "data"_a)
                .def(
                        "send",
                        [](Stream& self, std::string data) { self.send(std::move(data)); },
                        "data"_a)
                .def_property_readonly("conn_id", &Stream::conn_id)
                .def_property_readonly(
                        "conn",
                        [](Stream& self) -> connection_interface* { return &self.conn; },
                        py::return_value_policy::reference);

        py::class_<message>(m, "Message")
                .def(py::init<BTRequestStream&, std::string, bool>(),
                     "bstream"_a,
                     "request"_a,
                     "is_error"_a = false)
                .def("respond", &message::respond, "body"_a, "is_error"_a = false)
                .def("view", &message::view)
                .def("type", &message::type)
                .def("endpoint", &message::endpoint_str)
                .def("body", &message::body_str)
                .def("scid", &message::scid)
                .def("rid", &message::rid)
                .def("stream", &message::stream)
                .def("__bool__", &message::operator bool);

        py::class_<BTRequestStream, Stream, std::shared_ptr<BTRequestStream>>(m, "BTStream")
                .def(py::init([](connection_interface& conni,
                                 Endpoint& endpoint,
                                 std::optional<py::function> close_cb) {
                         auto* conn = dynamic_cast<Connection*>(&conni);
                         if (!conn)
                             throw std::runtime_error{
                                     "Internal error: can't downcast to Connection!"};
                         return std::make_shared<BTRequestStream>(
                                 *conn, endpoint, wrap_stream_close_cb(close_cb));
                     }),
                     "conn"_a,
                     "endpoint"_a,
                     "on_close"_a = nullptr)
                .def(
                        "request",
                        [](BTRequestStream& bstream,
                           std::string endpoint,
                           std::string body,
                           std::function<void(message)> hook) {
                            bstream.command(std::move(endpoint), std::move(body), std::move(hook));
                        },
                        "request"_a,
                        "body"_a,
                        "hook"_a)
                .def(
                        "command",
                        [](BTRequestStream& bstream, std::string endpoint, std::string body) {
                            bstream.command(std::move(endpoint), std::move(body));
                        },
                        "command"_a,
                        "body"_a)
                .def("register_command",
                     &BTRequestStream::register_command,
                     "endpoint"_a,
                     "hook"_a);
    }

}  // namespace oxen::quic
