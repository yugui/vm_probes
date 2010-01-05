require 'mkmf'
autoload :Tempfile, 'tempfile'
autoload :ERB, 'erb'

DTRACE = 'dtrace'
ARCH=$configure_args['with-arch']

def choose_mechanism
  case model = $configure_args['tracing-mechanism']
  when 'dtrace', nil
    model = 'dtrace'
    begin
      $objs = %w[ ruby-probes.o ]
      $objs << 'dtrace.o' if dtrace_needs_postprocessor?(ARCH)
    rescue
      message "something wrong in checking dtrace. see config.log\n"
      message $!.message
      exit false
    end

    create_dtrace_d
  else
    message "unknown tracing model #{model}"
    exit false
  end
end

def create_dtrace_d
  bitsize_of_value = try_constant("SIZEOF_VALUE * CHAR_BIT", %w[ limits.h ruby/ruby.h ])
  unless bitsize_of_value.kind_of?(Integer)
    message 'failed to check SIZEOF_VALUE'
    exit false
  end
  template = ERB.new(File.read( File.expand_path("../dtrace.d.erb", __FILE__) ))
  File.open("dtrace.d", "wb"){|f| f.write template.result(binding) }
end

def dtrace_needs_postprocessor?(arch)
  return $dtrace_needs_postprocessor unless $dtrace_needs_postprocessor.nil?
  arch_arg = "--arch=#{arch}" if arch
  Tempfile.open("conftest.d"){|f|
    f.puts <<-EOS
      provider conftest {
        probe fire();
      };
    EOS
    f.close
    unless xsystem("#{DTRACE} #{arch_arg} -h -o dtrace_conftest.h -s #{f.path}")
      message "failed to run dtrace -h" 
      exit false
    end
    unless try_compile(<<-EOS, "$(COUTFLAG)conftest.$(OBJEXT)")
#include "dtrace_conftest.h"
int main() {
  if (CONFTEST_FIRE_ENABLED()) {
      CONFTEST_FIRE();
  }
  return 0;
}
    EOS
      message "failed to compile a test program" 
      exit false
    end
    return $dtrace_needs_postprocessor = xsystem(RbConfig.expand("#{DTRACE} #{arch_arg} -G -o dtrace.o conftest.$(OBJEXT)"))
  }

  raise "never reached"
ensure
  rm_f 'dtrace_conftest.h'
  rm_f 'dtrace.o'
end

def append_dtrace_o
  if $objs.include?('dtrace.o')
    other_objs = $objs.select{|x| x != 'dtrace.o'}
    arch_arg = "--arch=#{ARCH}" if ARCH
    File.open("Makefile", "at"){|f|
      f.puts "%<dtrace_obj>s: %<objs>s\n\t%<dtrace> -G %<arch_arg> -o $@ -s %<dtrace_d> %<objs>" % {
        dtrace_obj: RbConfig.expand('dtrace.$(OBJEXT)'),
        objs: other_objs.join(' '),
        arch_arg: arch_arg,
        dtrace_d: 'dtrace.d',
      }
    }
  end
end

choose_mechanism
create_makefile('runtime-probes')
append_dtrace_o
