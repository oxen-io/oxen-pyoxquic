#!/usr/bin/python3

import oxquic
from oxquic import Network, GNUTLSCreds, Address, RemoteAddress
from nacl.signing import SigningKey
import time

#oxquic.enable_logging("stderr", "trace")

server_key = SigningKey.generate()
server_credentials = GNUTLSCreds(server_key.encode(), server_key.verify_key.encode())

network = Network()
server = network.endpoint(Address("127.0.0.5", 1234))


def on_connect(c):
    return
    print(
        f"{'Incom' if c.inbound else 'Outgo'}ing connection established with {c.remote}."
    )


def on_close(c, code):
    print(
        f"{'Incom' if c.inbound else 'Outgo'}ing connection with {c.remote} has closed (status {code})."
    )


def stream_constructor(c, e, id):
    if (id == 0):
        return c.create_btreq_stream(c, e)
    return c.create_stream(c, e)


def stream_opened(s):
    c = s.conn
    print(
        f"{'Incoming' if c.inbound else 'Outgoing'} stream {s} established with {c.remote}."
    )
    return 0


def stream_closed(s, code):
    c = s.conn
    print(
        f"{'Incoming' if c.inbound else 'Outgoing'} stream {s} with {c.remote} has closed (status {code})."
    )


def stream_data(s, data):
    c = s.conn
    print(f"{c.remote.host}:{c.remote.port}[{s.id}]» {data}")


server.listen(
    server_credentials,
    on_connection=on_connect,
    on_closed=on_close,
    on_stream_open=stream_opened,
    on_stream_close=stream_closed,
    on_stream_construct=stream_constructor,
    on_stream_data=stream_data,
)


client_key = SigningKey.generate()
client_credentials = GNUTLSCreds(client_key.encode(), client_key.verify_key.encode())


client = network.endpoint(Address())
client_connection = client.connect(
    RemoteAddress(server_key.verify_key.encode(), "127.0.0.5", 1234),
    client_credentials,
    on_connection=on_connect,
    on_closed=on_close,
    on_stream_open=stream_opened,
    on_stream_close=stream_closed,
    on_stream_construct=stream_constructor,
    on_stream_data=stream_data,
)


s = client_connection.create_stream()
s.send(b'Hello world!')


while True:
    time.sleep(0.25)
    line = input('> ').encode()
    s.send(line)
