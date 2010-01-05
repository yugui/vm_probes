require 'mkmf'
autoload :Tempfile, 'tempfile'
autoload :ERB, 'erb'

DTRACE = 'dtrace'
ARCH=$configure_args['with-arch']

def choose_mechanism
  case mechanism = $configure_args['tracing-mechanism']
  when 'dtrace', nil
    mechanism = 'dtrace'
    begin
      $objs = %w[ vm_probes.o ]
      $objs << 'dtrace.o' if dtrace_needs_postprocessor?(ARCH)
    rescue
      message "something wrong in checking dtrace. see mkmf.log\n"
      message $!.message
      exit false
    end
    create_dtrace_d
  else
    message "unknown tracing mechanism #{mechanism}"
    exit false
  end

  return mechanism
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
  return 0;
}
    EOS
      message "failed to compile a test program" 
      exit false
    end
    unless xsystem(RbConfig.expand("#{DTRACE} #{arch_arg} -G -o dtrace.o conftest.$(OBJEXT)"))
      return $dtrace_needs_postprocessor = false
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
    unless xsystem(RbConfig.expand("#{DTRACE} #{arch_arg} -G -o dtrace.o conftest.$(OBJEXT)"))
      message "dtrace USDT is broken"
      exit false
    end
    return $dtrace_needs_postprocessor = true
  }

  raise "never reached"
ensure
  rm_f 'dtrace_conftest.h'
  rm_f 'dtrace.o'
end

def prepend_variables(mechanism)
  makefile = File.read("Makefile")
  File.open("Makefile", "wt"){|f|
    f.puts "DTRACE=#{DTRACE}"
    f.puts "TRACING_MECHANISM=#{mechanism}"
    f.puts "ARCH_FLAGS=--arch #{ARCH}" if ARCH

    orig_objs = $objs.select{|x| x != "#{mechanism}.o"}
    f.puts "ORIG_OBJS=#{orig_objs.join(' ')}"

    f.puts(makefile)
  }
end

mechanism = choose_mechanism
create_makefile('vm_probes')
prepend_variables(mechanism)
