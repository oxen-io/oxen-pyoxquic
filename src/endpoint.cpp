#include "utils.hpp"

namespace oxen::quic
{
    void pybind_endpoint(py::module_& m)
    {
        py::class_<Endpoint, std::shared_ptr<Endpoint>>(m, "Endpoint")
                .def_property_readonly("local_addr", &Endpoint::local)
                // TODO:
                //.def("connect",
                // .def("listen" ...)
                ;
    }
}  // namespace oxen::quic
