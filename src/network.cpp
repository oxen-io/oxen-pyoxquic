#include <oxen/quic/endpoint.hpp>
#include <oxen/quic/network.hpp>
#include <oxen/quic/opt.hpp>

#include "callbacks.hpp"
#include "utils.hpp"

namespace oxen::quic
{
    void pybind_network(py::module_& m)
    {
        py::class_<Network, std::shared_ptr<Network>>(m, "Network")
                .def(py::init<>())
                .def(
                        "endpoint",
                        [](Network& self,
                           const Address& local_addr,
                           std::optional<std::vector<std::string_view>> outbound_alpns,
                           std::optional<std::vector<std::string_view>> inbound_alpns,
                           std::optional<double> handshake_timeout,
                           bool datagrams,
                           std::optional<py::function> dgram_cb,
                           std::optional<py::function> established_cb,
                           std::optional<py::function> closed_cb) {
                            return self.endpoint(
                                    local_addr,
                                    flag_option<opt::enable_datagrams>(datagrams),
                                    duration_option<opt::handshake_timeout>(handshake_timeout),
                                    wrap_dgram_data_cb(dgram_cb),
                                    ustrings_option<opt::outbound_alpns>(outbound_alpns),
                                    ustrings_option<opt::inbound_alpns>(inbound_alpns),
                                    wrap_conn_established_cb(established_cb),
                                    wrap_conn_closed_cb(closed_cb));
                        },
                        // Network can't get destroyed (via Python GC) before endpoint:
                        py::keep_alive<0, 1>(),
                        "local_addr"_a,
                        py::kw_only{},
                        "outbound_alpns"_a = nullptr,
                        "inbound_alpns"_a = nullptr,
                        "handshake_timeout"_a = nullptr,
                        "enable_datagrams"_a = false,
                        "on_datagram"_a = nullptr,
                        "on_established"_a = nullptr,
                        "on_closed"_a = nullptr)
                /**/;
    }

}  // namespace oxen::quic
