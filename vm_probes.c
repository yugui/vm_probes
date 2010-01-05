#include <ruby/ruby.h>
#include "tracing_mechanism.h"

static void
event_hook(rb_event_flag_t flags, VALUE data, VALUE self, VALUE id, VALUE klass)
{
    if ((flags & RUBY_EVENT_LINE) && TRACE_LINE_ENABLED()) {
        const char *filename = rb_sourcefile();
        int line = rb_sourceline();
        FIRE_LINE(filename, line);
    }
    if ((flags & (RUBY_EVENT_CALL|RUBY_EVENT_C_CALL)) && TRACE_METHOD_ENTRY_ENABLED()) {
        const char *filename = rb_sourcefile();
        int line = rb_sourceline();
        FIRE_METHOD_ENTRY(self, rb_class2name(klass), rb_id2name(id), filename, line);
    }
    if ((flags & (RUBY_EVENT_RETURN|RUBY_EVENT_C_RETURN)) && TRACE_METHOD_RETURN_ENABLED()) {
        const char *filename = rb_sourcefile();
        int line = rb_sourceline();
        FIRE_METHOD_RETURN(self, rb_class2name(klass), rb_id2name(id), filename, line);
    }
    if ((flags & (RUBY_EVENT_RAISE)) && TRACE_RAISE_ENABLED()) {
        const char *filename = rb_sourcefile();
        int line = rb_sourceline();
        FIRE_RAISE(self, rb_class2name(klass), filename, line);
    }
    if ((flags & (RUBY_EVENT_SWITCH)) && TRACE_THREAD_LEAVE_ENABLED()) {
        const char *filename = rb_sourcefile();
        int line = rb_sourceline();
        FIRE_THREAD_LEAVE(self, filename, line);

    }
    if ((flags & (RUBY_EVENT_TH_INIT)) && TRACE_THREAD_INIT_ENABLED()) {
        const char *filename = rb_sourcefile();
        int line = rb_sourceline();
        FIRE_THREAD_INIT(self, filename, line);
    }
    if ((flags & (RUBY_EVENT_TH_TERM)) && TRACE_THREAD_TERM_ENABLED()) {
        const char *filename = rb_sourcefile();
        int line = rb_sourceline();
        FIRE_THREAD_TERM(self, filename, line);
    }
    if ((flags & (RUBY_EVENT_RESCUE)) && TRACE_RESCUE_ENABLED()) {
        const char *filename = rb_sourcefile();
        int line = rb_sourceline();
        FIRE_RESCUE(self, rb_class2name(CLASS_OF(self)), filename, line);
    }
    if ((flags & (RUBY_EVENT_OBJ_ALLOC)) && TRACE_OBJECT_CREATE_ENABLED()) {
        const char *filename = rb_sourcefile();
        int line = rb_sourceline();
        FIRE_OBJECT_CREATE(self, filename, line);
    }
    if ((flags & (RUBY_EVENT_OBJ_FREE)) && TRACE_OBJECT_FREE_ENABLED()) {
        FIRE_OBJECT_FREE(self);
    }
    if ((flags & (RUBY_EVENT_GC_START)) && TRACE_GC_START_ENABLED()) {
        FIRE_GC_START();
    }
    if ((flags & (RUBY_EVENT_GC_END)) && TRACE_GC_END_ENABLED()) {
        FIRE_GC_END();
    }
}

void Init_vm_probes()
{
    rb_add_event_hook(event_hook, 
            RUBY_EVENT_LINE 
            | RUBY_EVENT_CALL | RUBY_EVENT_RETURN | RUBY_EVENT_C_CALL | RUBY_EVENT_C_RETURN
            | RUBY_EVENT_RAISE
            | RUBY_EVENT_SWITCH | RUBY_EVENT_TH_INIT | RUBY_EVENT_TH_TERM
            | RUBY_EVENT_RESCUE 
            | RUBY_EVENT_OBJ_ALLOC | RUBY_EVENT_OBJ_FREE
            | RUBY_EVENT_GC_START | RUBY_EVENT_GC_END,
            Qnil);
}

