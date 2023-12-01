#!/usr/bin/python3

import oxquic
from oxquic import Network, GNUTLSCreds, Address, RemoteAddress
from nacl.signing import SigningKey
import time

oxquic.enable_logging("stderr", "trace")


k1 = SigningKey.generate()
t1 = GNUTLSCreds(k1.encode(), k1.verify_key.encode())

n1 = Network()
e1 = n1.endpoint(Address("127.0.0.5", 1234))


def on_connect(c):
    return
    print(
        f"{'Incom' if c.inbound else 'Outgo'}ing connection established with {c.remote}."
    )


def on_close(c, code):
    print(
        f"{'Incom' if c.inbound else 'Outgo'}ing connection with {c.remote} has closed (status {code})."
    )


def stream_opened(s):
    c = s.conn
    print(
        f"{'Incoming' if c.inbound else 'Outgoing'} stream {s} established with {c.remote}."
    )


def stream_closed(s, code):
    c = s.conn
    print(
        f"{'Incoming' if c.inbound else 'Outgoing'} stream {s} with {c.remote} has closed (status {code})."
    )


def stream_data(s, data):
    c = s.conn
    # If it's printable ASCII then we can just print it directly, otherwise we'll let python format
    # the bytes
    if all(0x20 <= x <= 0x7E):
        data = data.decode()

    print(f"{c.remote.host}:{c.remote.port}[s.id]» {data}")


c1 = e1.listen(
    t1,
    on_connection=on_connect,
    on_closed=on_close,
    on_stream_open=stream_opened,
    on_stream_close=stream_closed,
    on_stream_data=stream_data,
)

k2 = SigningKey.generate()
t2 = GNUTLSCreds(k2.encode(), k2.verify_key.encode())

#n2 = Network()
e2 = n1.endpoint(Address())
c2 = e2.connect(
    RemoteAddress(k1.verify_key.encode(), "127.0.0.5", 1234),
    t2,
    on_connection=on_connect,
    on_closed=on_close,
    on_stream_open=stream_opened,
    on_stream_close=stream_closed,
    on_stream_data=stream_data,
)

time.sleep(100)
