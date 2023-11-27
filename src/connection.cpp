#include "utils.hpp"

namespace oxen::quic
{
    void pybind_connection(py::module_& m) {
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
                .def("close_connection", &Connection::close_connection, "error_code"_a = 0)
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
                        "data"_a)
                .def("create_btreq_stream", [](Connection& self,
                            std::function<void(Stream&, uint64_t)> close_callback) {
                    return self.get_new_stream<BTRequestStream>(std::move(close_callback));
                },
                "on_close"_a = nullptr
                )
                .def("create_stream", [](Connection& self,
                            stream_data_callback on_data,
                            stream_close_callback on_close) {
                    return self.get_new_stream(std::move(on_data), std::move(on_close));
                },
                "on_data"_a = nullptr,
                "on_close"_a = nullptr)
                ;
    };

}  // namespace oxen::quic
