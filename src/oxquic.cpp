#include "common.hpp"

PYBIND11_MODULE(oxquic, m) {
    using namespace oxen::quic;

    pybind_address(m);
    pybind_connection(m);
    pybind_crypto(m);
    pybind_endpoint(m);
    pybind_network(m);
    pybind_stream(m);
}
