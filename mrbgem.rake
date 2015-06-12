MRuby::Gem::Specification.new('mruby-raspicam') do |spec|
  # Note that it needs WiringPI libraries (http://wiringpi.com)
  spec.license = 'MIT'
  spec.author  = 'Paolo Bosetti, University of Trento'
  spec.summary = 'Raspberry library for IO access'
  spec.version = 0.1
  spec.description = spec.summary
  spec.homepage = "Not yet defined"
  lib_dir =  File.expand_path("../lib", __FILE__)
  if not build.kind_of? MRuby::CrossBuild then
    spec.cc.command = 'gcc' # clang does not work!
    spec.cc.flags << %w||
    spec.cc.include_paths << "/usr/local/include"
    spec.cc.include_paths << lib_dir
    spec.linker.library_paths << "/usr/local/lib"
    spec.linker.libraries << %w[LaserCam]
  else
    # complete for your case scenario
  end
end
