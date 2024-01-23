#include <oxen/quic/connection.hpp>
#include <oxen/quic/endpoint.hpp>
#include <oxen/quic/stream.hpp>

#include "callbacks.hpp"
#include "utils.hpp"

namespace oxen::quic
{
    void pybind_stream(py::module_& m)
    {
        py::class_<Stream, std::shared_ptr<Stream>>(m, "Stream")
                .def_property_readonly("available", &Stream::available)
                .def_property_readonly("id", &Stream::stream_id)
                .def_readonly("conn_id", &Stream::reference_id)
                .def_property_readonly("has_unsent", &Stream::has_unsent)
                .def_property_readonly("is_ready", &Stream::is_ready)
                .def_property_readonly("is_empty", &Stream::is_empty)
                .def_property_readonly("unsent", &Stream::unsent)
                .def_property_readonly("has_unsent", &Stream::has_unsent)
                .def_property_readonly("is_closing", &Stream::is_closing)
                .def_property_readonly("path", &Stream::path)
                .def(
                        "close",
                        [](Stream& self, uint64_t error_code) { self.close(error_code); },
                        "error_code"_a)
                .def(
                        "send",
                        [](Stream& self, bstring data) { self.send(std::move(data)); },
                        "data"_a)
                .def(
                        "send",
                        [](Stream& self, std::string data) { self.send(std::move(data)); },
                        "data"_a)
                .def_property_readonly(
                        "conn",
                        [](Stream& self) { return self.endpoint.get_conn(self.reference_id); })
                /**/;
    }

}  // namespace oxen::quic
