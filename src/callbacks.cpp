#include "callbacks.hpp"

#include <pybind11/gil.h>

namespace oxen::quic
{

    void pyfunc_destructor(py::function* f)
    {
        py::gil_scoped_acquire gil;
        delete f;
    }

    std::shared_ptr<py::function> gil_deleted_pyfunc(py::function f)
    {
        return std::shared_ptr<py::function>(new py::function(std::move(f)), pyfunc_destructor);
    }

    // We capture the std::shared_ptr's in the lambda because the std::functions could get copied
    // without the gil being held, but that isn't allowed for a py::function, so we have to isolate
    // it via the shared_ptr.

    stream_data_callback wrap_stream_data_cb(std::optional<py::function> f)
    {
        if (!f || f->is_none())
            return nullptr;

        return [f = gil_deleted_pyfunc(std::move(*f))](Stream& s, bstring_view data) {
            py::gil_scoped_acquire gil;
            (*f)(s.shared_from_this(), data);
        };
    }

    stream_open_callback wrap_stream_open_cb(std::optional<py::function> f)
    {
        if (!f || f->is_none())
            return nullptr;

        return [f = gil_deleted_pyfunc(std::move(*f))](Stream& s) {
            py::gil_scoped_acquire gil;
            return py::cast<uint64_t>((*f)(s.shared_from_this()));
        };
    }

    stream_close_callback wrap_stream_close_cb(std::optional<py::function> f)
    {
        if (!f || f->is_none())
            return nullptr;
        return [f = gil_deleted_pyfunc(std::move(*f))](Stream& s, uint64_t errcode) {
            py::gil_scoped_acquire gil;
            (*f)(s.shared_from_this(), errcode);
        };
    }

    stream_constructor_callback wrap_stream_ctor_cb(std::optional<py::function> f)
    {
        if (!f || f->is_none())
            return nullptr;
        return [f = gil_deleted_pyfunc(std::move(*f))](
                       connection_interface& c, Endpoint& e, std::optional<int64_t> streamid) {
            py::gil_scoped_acquire gil;
            return py::cast<std::shared_ptr<Stream>>(
                    (*f)(c.shared_from_this(), e.shared_from_this(), streamid));
        };
    }

    connection_established_callback wrap_conn_established_cb(std::optional<py::function> f)
    {
        if (!f || f->is_none())
            return nullptr;
        return [f = gil_deleted_pyfunc(std::move(*f))](connection_interface& c) {
            py::gil_scoped_acquire gil;
            (*f)(c.shared_from_this());
        };
    }

    connection_closed_callback wrap_conn_closed_cb(std::optional<py::function> f)
    {
        if (!f || f->is_none())
            return nullptr;
        return [f = gil_deleted_pyfunc(std::move(*f))](connection_interface& c, uint64_t errcode) {
            py::gil_scoped_acquire gil;
            (*f)(c.shared_from_this(), errcode);
        };
    }

    dgram_data_callback wrap_dgram_data_cb(std::optional<py::function> f)
    {
        if (!f || f->is_none())
            return nullptr;
        return [f = gil_deleted_pyfunc(std::move(*f))](dgram_interface& d, bstring b) {
            py::gil_scoped_acquire gil;
            (*f)(d.shared_from_this(), std::move(b));
        };
    }

}  // namespace oxen::quic
