#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "event.h"
class analysis {
    // private variables
    public:
        analysis();
        ~analysis();
        void new_event(const event&);
        void begin_analysis();
};

#endif