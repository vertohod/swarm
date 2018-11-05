#include "oid_generator.h"

oid_generator::oid_generator(OID server_id) : m_server_id(server_id), m_last_oid(0)
{
}

size_t oid_generator::get_mask(size_t bits, size_t sn_bits)
{
    size_t res = 0;
    for (size_t count = 0; count < (bits - sn_bits); ++count) {
        res = res << 1 | 1;
    }
    return res;
}

OID oid_generator::get_server_id(size_t bits, size_t sn_bits)
{
    OID res = m_server_id;
    for (size_t count = 0; count < (bits - sn_bits); ++count) {
        res = res << 1;
    }
    return res;
}

OID oid_generator::oid(bool increment, size_t bits, size_t sn_bits)
{
    // Ограничение количества бит в OID (для корректной работы в JS)
    static const size_t mask = get_mask(bits, sn_bits);
    // В старшие биты зашиваем номер сервера
    OID server_id = get_server_id(bits, sn_bits);

    static std::mt19937_64 generator;
    static std::uniform_int_distribution<size_t> distribution(1, mask);

    while (true) {
        // Так делал, чтоб генерированные oid гарантированно хранились на текущем сервере
        /*
        OID res = increment ? (++m_last_oid * m_max_number) : distribution(generator);
        res |= server_id;
        res = res - (res % m_max_number) + m_server_id;
        */
        OID res = increment ? ++m_last_oid : distribution(generator);
        res |= server_id;
        if (check(res)) return res;
    }

    return 0;
}
