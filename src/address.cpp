#include <oxen/quic/address.hpp>

#include "utils.hpp"

namespace oxen::quic
{
    void pybind_address(py::module_& m)
    {
        py::class_<Address>(m, "Address")
                .def(py::init<const std::string&, uint16_t>(), "ip"_a = ""s, "port"_a = 0)
                .def_property_readonly("is_public", &Address::is_public)
                .def_property_readonly("is_public_ip", &Address::is_public_ip)
                .def_property_readonly("is_loopback", &Address::is_loopback)
                .def_property_readonly("is_ipv4", &Address::is_ipv4)
                .def_property_readonly("is_ipv6", &Address::is_ipv6)
                .def_property_readonly("port", &Address::port)
                .def_property_readonly("host", &Address::host)
                .def("__str__", &Address::to_string)

                /**/;

        py::class_<RemoteAddress, Address>(m, "RemoteAddress")
                .def(py::init([](py::bytes b_remote_pk, std::string ip, uint16_t port) {
                         std::string remote_pk{b_remote_pk};
                         if (remote_pk.size() != 32)
                             throw std::invalid_argument{
                                     "RemoteAddress requires a 32-byte remote public key"};
                         return RemoteAddress{remote_pk, ip, port};
                     }),
                     "remote_pubkey"_a,
                     "ip"_a,
                     "port"_a,
                     "Constructs an Address for establishing an outgoing connection to a remote "
                     "pubkey, ip, and port.")
                .def(
                        "__str__",
                        [](RemoteAddress& self) {
                            return "{}/{}"_format(
                                    self.to_string(), oxenc::to_hex(self.get_remote_key()));
                        },
                        "Formats the address as IP:PORT/REMOTEPUBKEY")

                /**/;
    }

}  // namespace oxen::quic
