#include <oxen/log.hpp>
#include <quic/utils.hpp>

#include "common.hpp"
#include "utils.hpp"

PYBIND11_MODULE(oxquic, m)
{
    using namespace oxen::quic;

    pybind_address(m);
    pybind_connection(m);
    pybind_crypto(m);
    pybind_endpoint(m);
    pybind_network(m);
    pybind_stream(m);
    pybind_tls(m);

    m.def(
            "enable_logging",
            [](std::string out, std::string level) {
                auto lvl = log::level_from_string(level);

                constexpr std::array print_vals = {
                        "stdout", "-", "", "stderr", "nocolor", "stdout-nocolor", "stderr-nocolor"};
                log::Type type;
                if (std::count(print_vals.begin(), print_vals.end(), out))
                    type = log::Type::Print;
                else if (out == "syslog")
                    type = log::Type::System;
                else
                    type = log::Type::File;

                oxen::quic::logger_config(out, type, lvl);
            },

            "out"_a = "stderr",
            "level"_a = "critical",
            "Call to enable quic internals logging output.\n\n"
            "- out is the output destination:\n"
            "    stdout/-/stdout-nocolor/stderr/stderr-nocolor to print\n"
            "    FILENAME to write to a file\n"
            "- level is the log level (critical, warning, info, debug, trace)\n");
}
