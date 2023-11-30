#pragma once

#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace oxen::quic
{
    void pybind_address(py::module_& m);
    void pybind_connection(py::module_& m);
    void pybind_crypto(py::module_& m);
    void pybind_endpoint(py::module_& m);
    void pybind_network(py::module_& m);
    void pybind_stream(py::module_& m);
    void pybind_tls(py::module_& m);

}  // namespace oxen::quic
