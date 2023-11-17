#include "utils.hpp"

namespace oxen::quic
{
    PYBIND11_MODULE(connection, m)
    {
        py::class_<std::shared_ptr<void>>(m, "shared_ptr_void").def(py::init());

        py::class_<ConnectionID>(m, "ConnectionID")
                .def(py::init<>())
                .def(py::init<const ConnectionID&>())
                .def(py::init<const uint8_t*, size_t>())
                .def_static("random", &ConnectionID::random)
                .def("to_string", &ConnectionID::to_string)
                .def(py::self == py::self)
                .def(py::self != py::self)
                .def(py::hash(py::self))
                .def("__str__",
                     [](const ConnectionID& cid) {
                         return "<ConnectionID: " + cid.to_string() + ">";
                     })
                .def("__repr__", [](const ConnectionID& cid) {
                    return "<ConnectionID: " + cid.to_string() + ">";
                });

        // Must be held using a shared_ptr
        py::class_<Connection, std::shared_ptr<Connection>>(m, "Connection")
                .def(py::init(&Connection::make_conn))
                .def_static("make_conn", &Connection::make_conn)
                .def("close_connection", &Connection::close_connection, py::arg("error_code") = 0)
                .def("scid", &Connection::scid)
                .def("local", &Connection::local)
                .def("remote", &Connection::remote)
                .def("is_validated", &Connection::is_validated)
                .def("selected_alpn", &Connection::selected_alpn)
                .def("get_max_streams", &Connection::get_max_streams)
                .def("get_streams_available", &Connection::get_streams_available)
                .def("get_max_datagram_size", &Connection::get_max_datagram_size)
                .def("datagrams_enabled", &Connection::datagrams_enabled)
                .def("packet_splitting_enabled", &Connection::packet_splitting_enabled)
                .def(
                        "send_datagram",
                        [](Connection& self, bstring data) { self.send_datagram(std::move(data)); },
                        py::arg("data"))
                .def("get_new_stream_impl", &Connection::get_new_stream_impl)
                .def("get_new_stream", &connection_interface::get_new_stream<BTRequestStream>)
                .def("get_new_stream", &connection_interface::get_new_stream<Stream>);
    };

}  // namespace oxen::quic
