#include <pybind11/cast.h>
#include <pybind11/gil.h>

#include <quic/opt.hpp>

#include "callbacks.hpp"
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
                           std::optional<py::function> f_st_open,
                           std::optional<py::function> f_st_data,
                           std::optional<py::function> f_st_close,
                           std::optional<py::function> f_st_ctor,
                           std::optional<py::function> f_conn_est,
                           std::optional<py::function> f_conn_closed) {
                            auto st_open = wrap_stream_open_cb(f_st_open);
                            auto st_data = wrap_stream_data_cb(f_st_data);
                            auto st_close = wrap_stream_close_cb(f_st_close);
                            auto st_ctor = wrap_stream_close_cb(f_st_ctor);
                            auto conn_est = wrap_conn_established_cb(f_conn_est);
                            auto conn_closed = wrap_conn_closed_cb(f_conn_closed);
                            py::gil_scoped_release bye_gil{};
                            self.listen(
                                    std::move(tls),
                                    max_streams ? std::make_optional<opt::max_streams>(*max_streams)
                                                : std::nullopt,
                                    std::move(st_open),
                                    std::move(st_data),
                                    std::move(st_close),
                                    std::move(st_ctor),
                                    std::move(conn_est),
                                    std::move(conn_closed));
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
                           std::optional<py::function> f_st_open,
                           std::optional<py::function> f_st_data,
                           std::optional<py::function> f_st_close,
                           std::optional<py::function> f_st_ctor,
                           std::optional<py::function> f_conn_est,
                           std::optional<py::function> f_conn_closed) {
                            auto st_open = wrap_stream_open_cb(f_st_open);
                            auto st_data = wrap_stream_data_cb(f_st_data);
                            auto st_close = wrap_stream_close_cb(f_st_close);
                            auto st_ctor = wrap_stream_ctor_cb(f_st_ctor);
                            auto conn_est = wrap_conn_established_cb(f_conn_est);
                            auto conn_closed = wrap_conn_closed_cb(f_conn_closed);
                            py::gil_scoped_release bye_gil{};
                            return self.connect(
                                    std::move(remote),
                                    std::move(tls),
                                    max_streams ? std::make_optional<opt::max_streams>(*max_streams)
                                                : std::nullopt,
                                    std::move(st_open),
                                    std::move(st_data),
                                    std::move(st_close),
                                    std::move(st_ctor),
                                    std::move(conn_est),
                                    std::move(conn_closed));
                        },
                        py::keep_alive<0, 1>(),
                        "remote"_a,
                        "tls_creds"_a,
                        py::kw_only(),
                        "max_streams"_a = nullptr,
                        "on_stream_open"_a = nullptr,
                        "on_stream_data"_a = nullptr,
                        "on_stream_close"_a = nullptr,
                        "on_stream_construct"_a = nullptr,
                        "on_connection"_a = nullptr,
                        "on_closed"_a = nullptr);
    }
}  // namespace oxen::quic
