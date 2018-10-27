#ifndef DEFINES_KEY
#define DEFINES_KEY

#include "defines_properties_methods.h"
#include "key_interface.h"

#define DECLARATION_KEY(TABLE_OBJECT, KEY_NAME, KEY_FIELDS) \
class KEY_NAME : public swarm::object, public swarm::factory<KEY_NAME>, public swarm::key_interface \
{ \
private: \
    typedef TABLE_OBJECT table_object_t; \
    static const std::string m_table_name; \
public: \
    static const std::string& stable_name(); \
    virtual const std::string& table_name() const override; \
\
    DECLARATION_FIELDS_FUNCTIONS_STORES \
    DECLARATION_CONSTS \
    DECLARATION_SPECIAL_MEMBERS \
\
    KEY_FIELDS \
\
public:\
    KEY_NAME(); \
    KEY_NAME(std::shared_ptr<swarm::record>& record_ptr); \
\
    virtual const swarm::object* get_object() const override; \
    virtual size_t hash() const override; \
    virtual bool operator==(const key_interface& obj) const override; \
    virtual bool operator<(const key_interface& obj) const override; \
\
private: \
    KEY_NAME(const KEY_NAME&); \
    KEY_NAME& operator=(const KEY_NAME&); \
};

#define INIT(NAME) \
{ \
    FIELD(NAME) = std::dynamic_pointer_cast<const table_object_t>(record_ptr->get_payload())->get_##NAME(); \
}

#define DEFINITION_KEY(KEY_NAME, INIT_FIELDS, EQUALS, LESSES) \
DEFINITION_FIELDS_FUNCTIONS_STORES(KEY_NAME) \
DEFINITION_CONSTS(KEY_NAME, #KEY_NAME, true, 0) \
DEFINITION_SPECIAL_MEMBERS(KEY_NAME) \
const std::string KEY_NAME::m_table_name = table_object_t::stp(); \
KEY_NAME::KEY_NAME() {} \
KEY_NAME::KEY_NAME(std::shared_ptr<swarm::record>& record_ptr) : swarm::key_interface(record_ptr) \
{ \
    INIT_FIELDS \
} \
const swarm::object* KEY_NAME::get_object() const \
{ \
    return this; \
} \
const std::string& KEY_NAME::table_name() const \
{ \
    return m_table_name; \
} \
bool KEY_NAME::operator==(const key_interface& key) const \
{ \
	return EQUALS; \
} \
bool KEY_NAME::operator<(const key_interface& key) const \
{ \
	return LESSES; \
}

#define EQUAL(NAME) \
    FIELD(NAME) == dynamic_cast<const decltype(this)>(&key)->get_##NAME()

#define LESS(NAME) \
    FIELD(NAME) < dynamic_cast<const decltype(this)>(&key)->get_##NAME()

#define MORE(NAME) \
    dynamic_cast<const decltype(this)>(&key)->get_##NAME() < FIELD(NAME)

#endif
