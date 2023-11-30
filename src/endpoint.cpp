#include <pybind11/cast.h>
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
                           dgram_data_callback f_dgram,
                           stream_open_callback f_st_open,
                           stream_data_callback f_st_data,
                           stream_close_callback f_st_close,
                           stream_constructor_callback f_st_ctor,
                           connection_established_callback f_conn_est,
                           connection_closed_callback f_conn_closed) {
                            return self.listen(
                                    std::move(tls),
                                    max_streams ? std::make_optional<opt::max_streams>(*max_streams)
                                                : std::nullopt,
                                    std::move(f_dgram),
                                    std::move(f_st_open),
                                    std::move(f_st_data),
                                    std::move(f_st_close),
                                    std::move(f_st_ctor),
                                    std::move(f_conn_est),
                                    std::move(f_conn_closed));
                        },
                        "tls_creds"_a,
                        py::kw_only(),
                        "max_streams"_a = nullptr,
                        "on_dgram_data"_a = nullptr,
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
                           dgram_data_callback f_dgram,
                           stream_open_callback f_st_open,
                           stream_data_callback f_st_data,
                           stream_close_callback f_st_close,
                           stream_constructor_callback f_st_ctor,
                           connection_established_callback f_conn_est,
                           connection_closed_callback f_conn_closed) {
                            return self.connect(
                                    std::move(remote),
                                    std::move(tls),
                                    max_streams ? std::make_optional<opt::max_streams>(*max_streams)
                                                : std::nullopt,
                                    std::move(f_dgram),
                                    std::move(f_st_open),
                                    std::move(f_st_data),
                                    std::move(f_st_close),
                                    std::move(f_st_ctor),
                                    std::move(f_conn_est),
                                    std::move(f_conn_closed));
                        },
                        "remote"_a,
                        "tls_creds"_a,
                        py::kw_only(),
                        "max_streams"_a = nullptr,
                        "on_dgram_data"_a = nullptr,
                        "on_stream_open"_a = nullptr,
                        "on_stream_data"_a = nullptr,
                        "on_stream_close"_a = nullptr,
                        "initial_stream"_a = nullptr,
                        "on_connection"_a = nullptr,
                        "on_closed"_a = nullptr);
    }
}  // namespace oxen::quic
