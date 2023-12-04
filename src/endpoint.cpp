#include <pybind11/cast.h>
#include <pybind11/gil.h>

#include <quic/opt.hpp>

#include "utils.hpp"

namespace oxen::quic
{
    void pybind_endpoint(py::module_& m)
    {
        py::class_<Endpoint, std::shared_ptr<Endpoint>>(m, "Endpoint")
                .def_property_readonly("local_addr", &Endpoint::local)
                .def(
                        "listen",
                        [](Endpoint& self,
                           std::shared_ptr<TLSCreds> tls,
                           std::optional<int> max_streams,
                           pystream_open f_st_open,
                           pystream_data f_st_data,
                           pystream_close f_st_close,
                           pystream_constructor f_st_ctor,
                           pyconnection_established f_conn_est,
                           pyconnection_closed f_conn_closed) {
                            py::gil_scoped_release bye_gil{};
                            self.listen(
                                    std::move(tls),
                                    max_streams ? std::make_optional<opt::max_streams>(*max_streams)
                                                : std::nullopt,
                                    move_hack_function_wrapper(f_st_open),
                                    move_hack_function_wrapper(f_st_data),
                                    move_hack_function_wrapper(f_st_close),
                                    move_hack_function_wrapper(f_st_ctor),
                                    move_hack_function_wrapper(f_conn_est),
                                    move_hack_function_wrapper(f_conn_closed));
                        },
                        py::keep_alive<0, 1>(),
                        "tls_creds"_a,
                        py::kw_only(),
                        "max_streams"_a = nullptr,
                        "on_stream_open"_a = nullptr,
                        "on_stream_data"_a = nullptr,
                        "on_stream_close"_a = nullptr,
                        "initial_stream"_a = nullptr,
                        "on_connection"_a = nullptr,
                        "on_closed"_a = nullptr)

                .def(
                        "connect",
                        [](Endpoint& self,
                           RemoteAddress remote,
                           std::shared_ptr<TLSCreds> tls,
                           std::optional<int> max_streams,
                           pystream_open f_st_open,
                           pystream_data f_st_data,
                           pystream_close f_st_close,
                           pystream_constructor f_st_ctor,
                           pyconnection_established f_conn_est,
                           pyconnection_closed f_conn_closed) {
                            py::gil_scoped_release bye_gil{};
                            return self.connect(
                                    std::move(remote),
                                    std::move(tls),
                                    max_streams ? std::make_optional<opt::max_streams>(*max_streams)
                                                : std::nullopt,
                                    move_hack_function_wrapper(f_st_open),
                                    move_hack_function_wrapper(f_st_data),
                                    move_hack_function_wrapper(f_st_close),
                                    move_hack_function_wrapper(f_st_ctor),
                                    move_hack_function_wrapper(f_conn_est),
                                    move_hack_function_wrapper(f_conn_closed));
                        },
                        py::keep_alive<0, 1>(),
                        "remote"_a,
                        "tls_creds"_a,
                        py::kw_only(),
                        "max_streams"_a = nullptr,
                        "on_stream_open"_a = nullptr,
                        "on_stream_data"_a = nullptr,
                        "on_stream_close"_a = nullptr,
                        "initial_stream"_a = nullptr,
                        "on_connection"_a = nullptr,
                        "on_closed"_a = nullptr);
    }
}  // namespace oxen::quic
