#ifndef DTRACE_PROBES_H
#define DTRACE_PROBES_H

# include "dtrace.h"
# define TRACE_METHOD_ENTRY_ENABLED()  RUBY_METHOD_ENTRY_ENABLED()
# define TRACE_METHOD_RETURN_ENABLED() RUBY_METHOD_RETURN_ENABLED()
# define TRACE_RAISE_ENABLED()         RUBY_RAISE_ENABLED()
# define TRACE_RESCUE_ENABLED()        RUBY_RESCUE_ENABLED()
# define TRACE_LINE_ENABLED()          RUBY_LINE_ENABLED()
# define TRACE_GC_START_ENABLED()      RUBY_GC_START_ENABLED()
# define TRACE_GC_END_ENABLED()        RUBY_GC_END_ENABLED()
# define TRACE_THREAD_INIT_ENABLED()   RUBY_THREAD_INIT_ENABLED()
# define TRACE_THREAD_TERM_ENABLED()   RUBY_THREAD_TERM_ENABLED()
# define TRACE_THREAD_LEAVE_ENABLED()  RUBY_THREAD_LEAVE_ENABLED()
# define TRACE_THREAD_ENTER_ENABLED()  RUBY_THREAD_ENTER_ENABLED()
# define TRACE_OBJECT_CREATE_ENABLED() RUBY_OBJECT_CREATE_ENABLED()
# define TRACE_OBJECT_FREE_ENABLED()   RUBY_OBJECT_FREE_ENABLED()

# define FIRE_METHOD_ENTRY(receiver, classname, methodname, sourcefile, sourceline) \
   RUBY_METHOD_ENTRY(receiver, (char*)classname, (char*)methodname, (char*)sourcefile, sourceline)
# define FIRE_METHOD_RETURN(receiver, classname, methodname, sourcefile, sourceline) \
   RUBY_METHOD_RETURN(receiver, (char*)classname, (char*)methodname, (char*)sourcefile, sourceline)
# define FIRE_RAISE(exception, classname, sourcename, sourceline) \
   RUBY_RAISE(exception, (char*)classname, (char*)sourcename, sourceline)
# define FIRE_RESCUE(exception, classname, sourcename, sourceline) \
   RUBY_RESCUE(exception, (char*)classname, (char*)sourcename, sourceline)
# define FIRE_LINE(sourcename, sourceline) \
   RUBY_LINE((char*)sourcename, sourceline)
# define FIRE_GC_START()     RUBY_GC_START()
# define FIRE_GC_END()       RUBY_GC_END()
# define FIRE_THREAD_INIT(th, sourcefile, sourceline) \
   RUBY_THREAD_INIT(th, (char*)sourcefile, sourceline)
# define FIRE_THREAD_TERM(th, sourcefile, sourceline) \
   RUBY_THREAD_TERM(th, (char*)sourcefile, sourceline)
# define FIRE_THREAD_LEAVE(th, sourcefile, sourceline) \
   RUBY_THREAD_LEAVE(th, (char*)sourcefile, sourceline)
# define FIRE_THREAD_ENTER(th, sourcefile, sourceline) \
   RUBY_THREAD_ENTER(th, (char*)sourcefile, sourceline)
# define FIRE_OBJECT_CREATE(obj, sourcefile, sourceline) \
   RUBY_OBJECT_CREATE(obj, (char*)sourcefile, sourceline)
# define FIRE_OBJECT_FREE(obj) \
   RUBY_OBJECT_FREE(obj)

#endif
