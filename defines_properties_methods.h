#ifndef DEFINES_PROPERTIES_METHODS_H 
#define DEFINES_PROPERTIES_METHODS_H 

#define FIELD_OBJECT_TYPE "tp"

#define BIT_NULL        0
#define BIT_INCREMENT   1
#define BIT_SYNCHRONIZE 2

//--------------------
#define DESER(OBJECT, TYPE, NAME) \
[](swarm::object* ptr, rapidjson::Value& obj)\
{\
    TYPE val;\
    swarm::converter(obj, val);\
    dynamic_cast<OBJECT*>(ptr)->set_##NAME(val);\
}


//--------------------
#define SER(OBJECT, NAME) \
[](const swarm::object* ptr, rapidjson::Value& obj, rapidjson::MemoryPoolAllocator<>& al)\
{\
    swarm::pusher(obj, al, #NAME, dynamic_cast<const OBJECT*>(ptr)->get_##NAME());\
}


//--------------------
#define ADD_FUNCTION(OBJECT, TYPE, NAME) \
struct add_function_into_map_##OBJECT##_##NAME\
{\
    add_function_into_map_##OBJECT##_##NAME()\
    {\
        OBJECT::m_serialization_functions.push_back(SER(OBJECT, NAME));\
        OBJECT::m_deserialization_functions.insert(std::make_pair(#NAME, DESER(OBJECT, TYPE, NAME)));\
    }\
};\
add_function_into_map_##OBJECT##_##NAME object_for_add_function_##OBJECT##_##NAME;\
\
template <typename T>\
struct add_index_field_##OBJECT##_##NAME\
{\
    add_index_field_##OBJECT##_##NAME() {}\
};\
template <>\
struct add_index_field_##OBJECT##_##NAME<swarm::index>\
{\
    add_index_field_##OBJECT##_##NAME()\
    {\
        OBJECT::m_index_fields.push_back([](swarm::object* ptr, OID id){\
            TYPE val;\
            swarm::convert_to_oid(id, val);\
            dynamic_cast<OBJECT*>(ptr)->set_##NAME(val);\
        });\
    }\
};\
add_index_field_##OBJECT##_##NAME<TYPE> object_for_add_index_field_##OBJECT##_##NAME;

// ------ добавление полей в объекты -----------------------------------------------------------
#define DECLARATION_FIELD(TYPE, NAME) \
private:\
    TYPE m_field_##NAME;\
public:\
    TYPE get_##NAME() const;\
    void set_##NAME(const TYPE& val);


//--------------------
#define DEFINITION_FIELD(OBJECT, TYPE, NAME) \
TYPE OBJECT::get_##NAME() const\
{\
    return m_field_##NAME;\
}\
void OBJECT::set_##NAME(const TYPE& val)\
{\
    m_field_##NAME = val;\
}\
ADD_FUNCTION(OBJECT, TYPE, NAME)


//--------------------
#define FIELD(NAME) m_field_##NAME


//--------------------
#define DECLARATION_CONSTS \
private:\
    static const std::string m_tp;\
    static const size_t m_settings;\
    static const int m_vr;\
public:\
    const std::string& get_tp() const;\
    int get_vr() const;\
    void set_tp(const std::string&);\
    void set_vr(int);\
    static const std::string& stp();\
    static size_t ssettings();\
    virtual const std::string& tp() const override;\
    virtual size_t settings() const override;\
    virtual int vr() const override;


//--------------------
#define DEFINITION_CONSTS(OBJECT, TP, SETTINGS, VR) \
const std::string OBJECT::m_tp = TP;\
const size_t OBJECT::m_settings = SETTINGS;\
const int OBJECT::m_vr = VR;\
const std::string& OBJECT::get_tp() const {return m_tp;}\
int OBJECT::get_vr() const {return m_vr;}\
void OBJECT::set_tp(const std::string& val){}\
void OBJECT::set_vr(int val){}\
const std::string& OBJECT::stp() {return OBJECT::m_tp;}\
size_t OBJECT::ssettings() {return OBJECT::m_settings;}\
const std::string& OBJECT::tp() const {return OBJECT::m_tp;}\
size_t OBJECT::settings() const {return OBJECT::m_settings;}\
int OBJECT::vr() const {return OBJECT::m_vr;}\
ADD_FUNCTION(OBJECT, const std::string, tp)\
ADD_FUNCTION(OBJECT, int, vr)


//--------------------
#define DECLARATION_FIELDS_FUNCTIONS_STORES \
public:\
    static deserialization_functions_t m_deserialization_functions;\
    static serialization_functions_t m_serialization_functions;\
    static index_fields_t m_index_fields;\


//--------------------
#define DEFINITION_FIELDS_FUNCTIONS_STORES(OBJECT) \
OBJECT::deserialization_functions_t OBJECT::m_deserialization_functions;\
OBJECT::serialization_functions_t OBJECT::m_serialization_functions;\
OBJECT::index_fields_t OBJECT::m_index_fields;\
struct add_function_create_for_##OBJECT\
{\
    add_function_create_for_##OBJECT()\
    {\
        swarm::object::registration_object(#OBJECT, &OBJECT::create_interface);\
    }\
};\
add_function_create_for_##OBJECT object_add_function_create_##OBJECT;

// ---------------------------------------------------------------------------------------------

#define DECLARATION_SPECIAL_MEMBERS \
public: \
    static bool init_keys(swarm::keys_stores_t&); \
    virtual void serialization(rapidjson::Value& obj, rapidjson::MemoryPoolAllocator<>& al) const override; \
    virtual void deserialization(rapidjson::Document& obj) override; \
    virtual void deserialization(rapidjson::Value& obj) override; \
    virtual std::shared_ptr<swarm::object> make_copy() const override; \
    virtual void set_index_fields(OID) override; \
private: \
    static bool unique_keys_exist_flag; \
    static void set_unique_keys_flag(bool flag); \
public: \
    static bool get_unique_keys_flag();

#define DEFINITION_SERIALIZATION(OBJECT) \
void OBJECT::serialization(rapidjson::Value& obj, rapidjson::MemoryPoolAllocator<>& al) const \
{ \
    object::serialization(obj, al); \
    for (auto& func : m_serialization_functions) func(this, obj, al); \
}

#define DEFINITION_DESERIALIZATION_1(OBJECT) \
void OBJECT::deserialization(rapidjson::Document& obj) \
{ \
    object::deserialization(obj); \
    for (auto& member : obj.GetObject()) { \
        auto it = m_deserialization_functions.find(member.name.GetString()); \
        if (it != m_deserialization_functions.end()) { \
            it->second(this, member.value); \
        } \
    } \
}

#define DEFINITION_DESERIALIZATION_2(OBJECT) \
void OBJECT::deserialization(rapidjson::Value& obj) \
{ \
    object::deserialization(obj); \
    for (auto& member : obj.GetObject()) { \
        auto it = m_deserialization_functions.find(member.name.GetString()); \
        if (it != m_deserialization_functions.end()) { \
            it->second(this, member.value); \
        } \
    } \
}

#define DEFINITION_MAKE_COPY(OBJECT) \
std::shared_ptr<swarm::object> OBJECT::make_copy() const \
{ \
    auto json_ptr = object::serialization(); \
    auto object_new = object::create_object(object::get_field(json_ptr, FIELD_OBJECT_TYPE)); \
    object_new->deserialization(json_ptr); \
    return object_new; \
}

#define DEFINITION_SET_INDEX_FIELDS(OBJECT) \
void OBJECT::set_index_fields(OID id) \
{ \
    for (auto& fn : OBJECT::m_index_fields) { fn(this, id); } \
}

#define DEFINITION_KEY_STORE_EMPTY

#define DEFINITION_INIT_KEYS(OBJECT, KEY_STORE) \
bool OBJECT::init_keys(swarm::keys_stores_t& keys_stores) \
{ \
    KEY_STORE \
    return get_unique_keys_flag(); \
}

#define DEFINITION_UNIQUE_KYES(OBJECT) \
bool OBJECT::unique_keys_exist_flag = false; \
void OBJECT::set_unique_keys_flag(bool flag) \
{ \
    unique_keys_exist_flag = unique_keys_exist_flag || flag; \
} \
bool OBJECT::get_unique_keys_flag() \
{ \
    return unique_keys_exist_flag; \
}

#define DEFINITION_SPECIAL_MEMBERS(OBJECT) \
    DEFINITION_UNIQUE_KYES(OBJECT) \
    DEFINITION_INIT_KEYS(OBJECT, DEFINITION_KEY_STORE_EMPTY) \
    DEFINITION_SERIALIZATION(OBJECT) \
    DEFINITION_DESERIALIZATION_1(OBJECT) \
    DEFINITION_DESERIALIZATION_2(OBJECT) \
    DEFINITION_MAKE_COPY(OBJECT) \
    DEFINITION_SET_INDEX_FIELDS(OBJECT)

#endif
