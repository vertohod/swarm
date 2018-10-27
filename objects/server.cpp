#include "server.h"
#include "defines_key_store.h"

namespace swarm
{

DEFINITION_FIELDS_FUNCTIONS_STORES(server)

DEFINITION_CONSTS(server, "server", BIT_NULL, 1)

DEFINITION_FIELD(server, OID, id);
DEFINITION_FIELD(server, int, socket_id);
DEFINITION_FIELD(server, OID, remainder);
DEFINITION_FIELD(server, time_tt, time);
DEFINITION_FIELD(server, bool, approved);
DEFINITION_FIELD(server, bool, main);
DEFINITION_FIELD(server, sptr_cstr, salt);

server::server() :
    FIELD(id)(0),
    FIELD(socket_id)(0),
    FIELD(remainder)(0),
    FIELD(time)(0),
    FIELD(approved)(false),
    FIELD(main)(false)
{}

// обязательные методы ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

DEFINITION_INIT_KEYS(server,
    DEFINITION_KEY_STORE(UNIQUE_MODEL, UNIQUE_UNSORTED_STORE, key_server_server)
    DEFINITION_KEY_STORE(MULTI_MODEL, MULTI_UNSORTED_STORE, key_server_socket)
    DEFINITION_KEY_STORE(MULTI_MODEL, MULTI_UNSORTED_STORE, key_server_remainder)
    DEFINITION_KEY_STORE(MULTI_MODEL, MULTI_UNSORTED_STORE, key_server_main)
)

DEFINITION_UNIQUE_KYES(server)
DEFINITION_SERIALIZATION(server)
DEFINITION_DESERIALIZATION_1(server)
DEFINITION_DESERIALIZATION_2(server)
DEFINITION_MAKE_COPY(server)
DEFINITION_SET_INDEX_FIELDS(server)

// ключи ++++++++++++++++++++++++++++++++++++++++++++++++++++++

DEFINITION_KEY(key_server_server, INIT(id), EQUAL(id), LESS(id))
DEFINITION_FIELD(key_server_server, OID, id)
size_t key_server_server::hash() const
{
	return std::hash<OID>()(FIELD(id));
}

DEFINITION_KEY(key_server_socket, INIT(socket_id), EQUAL(socket_id), LESS(socket_id))
DEFINITION_FIELD(key_server_socket, int, socket_id)
size_t key_server_socket::hash() const
{
	return std::hash<int>()(FIELD(socket_id));
}

DEFINITION_KEY(key_server_remainder, INIT(remainder), EQUAL(remainder), LESS(remainder))
DEFINITION_FIELD(key_server_remainder, OID, remainder)
size_t key_server_remainder::hash() const
{
	return std::hash<OID>()(FIELD(remainder));
}

DEFINITION_KEY(key_server_main, INIT(main), EQUAL(main), LESS(main))
DEFINITION_FIELD(key_server_main, bool, main)
size_t key_server_main::hash() const
{
	return std::hash<bool>()(FIELD(main));
}

} // end of namespace

