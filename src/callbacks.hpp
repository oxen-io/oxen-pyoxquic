#include "utils.hpp"

namespace oxen::quic
{
    // Wrapper functions that take a Python callable and construct a lambda of the right type (for
    // libquic) around it that invokes the callable.  This is similar to the built-in pybind
    // std::function casters, except that we ensure we never take ownership of the arguments (which
    // pybind's version does, and breaks things).
    stream_data_callback wrap_stream_data_cb(std::optional<py::function> f);
    stream_open_callback wrap_stream_open_cb(std::optional<py::function> f);
    stream_close_callback wrap_stream_close_cb(std::optional<py::function> f);
    stream_constructor_callback wrap_stream_ctor_cb(std::optional<py::function> f);
    connection_established_callback wrap_conn_established_cb(std::optional<py::function> f);
    connection_closed_callback wrap_conn_closed_cb(std::optional<py::function> f);
    dgram_data_callback wrap_dgram_data_cb(std::optional<py::function> f);

}  // namespace oxen::quic
