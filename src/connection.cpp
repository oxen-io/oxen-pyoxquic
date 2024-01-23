#include <pybind11/gil.h>
#include <pybind11/operators.h>

#include <oxen/quic/btstream.hpp>
#include <oxen/quic/connection.hpp>

#include "callbacks.hpp"
#include "utils.hpp"

namespace oxen::quic
{
    void pybind_connection(py::module_& m)
    {
        py::class_<ConnectionID>(m, "ConnectionID")
                .def(py::self == py::self)
                .def(py::self != py::self)
                .def(py::self < py::self)
                .def(py::hash(py::self))
                .def_readwrite("id", &ConnectionID::id)
                .def("__str__",
                     [](const ConnectionID& cid) {
                         return "<ConnectionID: " + cid.to_string() + ">";
                     })
                .def("__repr__",
                     [](const ConnectionID& cid) {
                         return "<ConnectionID: " + cid.to_string() + ">";
                     })
                /**/;

        py::class_<connection_interface, std::shared_ptr<connection_interface>>(m, "Connection")
                .def("close", &connection_interface::close_connection, "error_code"_a = 0)
                .def_property_readonly("scid", &connection_interface::reference_id)
                .def_property_readonly("local", &connection_interface::local)
                .def_property_readonly("remote", &connection_interface::remote)
                .def_property_readonly("inbound", &connection_interface::is_inbound)
                .def_property_readonly("outbound", &connection_interface::is_outbound)
                .def_property_readonly("is_validated", &connection_interface::is_validated)
                .def_property_readonly("selected_alpn", &connection_interface::selected_alpn)
                .def_property_readonly("max_streams", &connection_interface::get_max_streams)
                .def_property_readonly(
                        "streams_available", &connection_interface::get_streams_available)
                .def_property_readonly(
                        "max_datagram_size", &connection_interface::get_max_datagram_size)
                .def_property_readonly(
                        "datagrams_enabled", &connection_interface::datagrams_enabled)
                .def_property_readonly(
                        "packet_splitting_enabled", &connection_interface::packet_splitting_enabled)
                .def(
                        "send_datagram",
                        [](connection_interface& self, bstring data) {
                            self.send_datagram(std::move(data));
                        },
                        "data"_a)
                .def(
                        "open_stream",
                        [](connection_interface& self,
                           std::optional<py::function> on_data,
                           std::optional<py::function> on_close) {
                            return self.open_stream<Stream>(
                                    wrap_stream_data_cb(on_data), wrap_stream_close_cb(on_close));
                        },
                        "on_data"_a = nullptr,
                        "on_close"_a = nullptr)
                .def(
                        "queue_incoming_stream",
                        [](connection_interface& self,
                           std::optional<py::function> on_data,
                           std::optional<py::function> on_close) {
                            return self.queue_incoming_stream<Stream>(
                                    wrap_stream_data_cb(on_data), wrap_stream_close_cb(on_close));
                        },
                        "on_data"_a = nullptr,
                        "on_close"_a = nullptr)
                .def(
                        "open_btreq_stream",
                        [](connection_interface& self, std::optional<py::function> on_close) {
                            return self.open_stream<BTRequestStream>(
                                    wrap_stream_close_cb(on_close));
                        },
                        "on_close"_a = nullptr)
                .def(
                        "queue_incoming_btreq_stream",
                        [](connection_interface& self, std::optional<py::function> on_close) {
                            return self.queue_incoming_stream<BTRequestStream>(
                                    wrap_stream_close_cb(on_close));
                        },
                        "on_close"_a = nullptr)
                /**/;
    };

}  // namespace oxen::quic
