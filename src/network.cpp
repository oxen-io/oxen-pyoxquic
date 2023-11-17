#include "utils.hpp"

namespace oxen::quic
{
    PYBIND11_MODULE(network, m)
    {
        py::class_<Network>(m, "Network").def(py::init<>());
    };
}  // namespace oxen::quic
