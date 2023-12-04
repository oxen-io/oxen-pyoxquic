#include "quic/opt.hpp"
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
                           std::optional<std::vector<std::string>> outbound_alpns,
                           std::optional<std::vector<std::string>> inbound_alpns,
                           std::optional<double> handshake_timeout,
                           bool datagrams,
                           pydgram_data dgram_cb,
                           pyconnection_established established_cb,
                           pyconnection_closed closed_cb) {
                            return self.endpoint(
                                    local_addr,
                                    datagrams ? std::make_optional<opt::enable_datagrams>()
                                              : std::nullopt,
                                    handshake_timeout
                                            ? std::make_optional<opt::handshake_timeout>(
                                                      std::chrono::nanoseconds{static_cast<int64_t>(
                                                              1e9 * *handshake_timeout)})
                                            : std::nullopt,
                                    move_hack_function_wrapper(dgram_cb),
                                    outbound_alpns ? std::make_optional<opt::outbound_alpns>(
                                                             std::move(*outbound_alpns))
                                                   : std::nullopt,
                                    inbound_alpns ? std::make_optional<opt::inbound_alpns>(
                                                            std::move(*inbound_alpns))
                                                  : std::nullopt,
                                    move_hack_function_wrapper(established_cb),
                                    move_hack_function_wrapper(closed_cb));
                        },
                        // Network can't get destroyed (via Python GC) before endpoint:
                        py::keep_alive<0, 1>(),
                        "local_addr"_a,
                        py::kw_only(),  //
                        "outbound_alpns"_a = nullptr,
                        "inbound_alpns"_a = nullptr,
                        "handshake_timeout"_a = nullptr,
                        "enable_datagrams"_a = false,
                        "on_datagram"_a = nullptr,
                        "on_established"_a = nullptr,
                        "on_closed"_a = nullptr);
    }

}  // namespace oxen::quic
