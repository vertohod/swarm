#ifndef DEFINES_OBJECT_H
#define DEFINES_OBJECT_H

#include "defines_properties_methods.h"
#include "object.h"

#define DECLARATION_OBJECT(OBJECT, FIELDS) \
class OBJECT : public swarm::object, public swarm::factory<OBJECT> \
{ \
    DECLARATION_FIELDS_FUNCTIONS_STORES \
    DECLARATION_CONSTS \
    DECLARATION_SPECIAL_MEMBERS \
    FIELDS \
public: \
    OBJECT(); \
};

#define NO_FIELDS

#endif
