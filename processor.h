#ifndef PROCESSOR_H
#define PROCESSOR_H

namespace swarm
{

enum EVENT int {
    NONE    = 0,
    SELECT  = 1,
    INSERT  = 2,
    UPDATE  = 3,
    REMOVE  = 4,
    RESULT  = 5
};


} // end of namespace

#endif
