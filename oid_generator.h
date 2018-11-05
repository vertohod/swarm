#ifndef OID_GENERATOR_H
#define OID_GENERATOR_H

namespace swarm
{

class oid_generator
{
private:
    OID m_server_id;
    OID m_last_oid;

public:
    oid_generator(OID server_id);
    OID oid(bool increment = false, size_t bits = 52, size_t sn_bits = 12);

private:
    size_t get_mask(size_t bits, size_t sn_bits);
    OID get_server_id(size_t bits, size_t sn_bits);
};

} // end of namespace

#endif
