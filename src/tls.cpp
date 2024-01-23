#include <oxen/quic/crypto.hpp>
#include <oxen/quic/gnutls_crypto.hpp>
#include <stdexcept>

#include "common.hpp"
#include "utils.hpp"

namespace oxen::quic
{

    void pybind_tls(py::module_& m)
    {

        py::class_<TLSCreds, std::shared_ptr<TLSCreds>>(
                m, "TLSCreds", "Base class; typically used via the GNUTLSCreds subclass");
        py::class_<GNUTLSCreds, TLSCreds, std::shared_ptr<GNUTLSCreds>>(m, "GNUTLSCreds")
                .def(py::init([](py::bytes b_seed, py::bytes b_pubkey) {
                         std::string seed{b_seed}, pubkey{b_pubkey};
                         if (seed.size() == 64)
                         {
                             if (pubkey.size() == 32)
                             {
                                 if (pubkey != std::string_view{seed}.substr(32))
                                     throw std::invalid_argument{
                                             "Secret/public key mismatch: given pubkey does not "
                                             "match the secret key value"};
                             }
                             else if (!pubkey.empty())
                             {
                                 throw std::invalid_argument{
                                         "GNUTLSCreds requires an empty or 32-byte pubkey when "
                                         "providing a 64-byte secret key"};
                             }
                             return GNUTLSCreds::make_from_ed_seckey(std::move(seed));
                         }
                         if (seed.size() == 32 && pubkey.size() == 32)
                             return GNUTLSCreds::make_from_ed_keys(
                                     std::move(seed), std::move(pubkey));
                         throw std::invalid_argument{
                                 "GNUTLSCreds requires a 64-byte secret key, or 32-byte seed & "
                                 "pubkey"};
                     }),
                     "secret"_a,
                     "pubkey"_a = py::none(),
                     "Constructs TLS credentials from a Ed25519 seed/secret key/pubkey.\n"
                     "`seed` can be a 64-byte libsodium secret key value, or a 32-byte seed "
                     "value.\n"
                     "`pubkey` can be omitted when using a 64-byte secret key, otherwise must be "
                     "the 32-byte pubkey.");
    }

}  // namespace oxen::quic
