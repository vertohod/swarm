#ifndef DEFINES_FOR_FILTER_H
#define DEFINES_FOR_FILTER_H

#define FROM(OBJECT) auto& original_object = OBJECT::convert(obj);
#define WHERE(CONDITION) return CONDITION;
#define FL(NAME) original_object.get_##NAME()
#define INDEX(NAME) original_object.get_##NAME().value()

#endif
