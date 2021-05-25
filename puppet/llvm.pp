class merlin::llvm {
  require merlin::llvm-deps

  file { '/curr/software'
    ensure => directory
  }

  exec { 'Clone LLVM'
    cwd => '/curr/software',
    requires => File['/curr/software'],
    command => 'rm -rf llvm-sources && git clone git@github.com:llvm-mirror/llvm llvm-sources',
  }

  exec { 'Clone LLVM tools'
    cwd => '/curr/software/llvm-sources/tools',
    command => 'git clone git@github.com:llvm-mirror/clang',
    requires => Exec['Clone LLVM'],
  }

  exec { 'Clone LLVM projects'
    cwd => '/curr/software/llvm-sources/projects',
    command => 'git clone git@github.com:llvm-mirror/libcxxabi && \
                git clone git@github.com:llvm-mirror/libcxx && \
                git clone git@github.com:llvm-mirror/compiler-rt',
    requires => Exec['Clone LLVM'],
  }

  file { '/curr/software/llvm-sources/build'
    ensure => directory,
    requires => Exec['Clone LLVM'],
  }

  exec { 'Build and install LLVM Suite in /usr/local/stow'
    cwd => '/curr/software/llvm-sources/build',
    command => 'cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON \
                  -DCMAKE_INSTALL_PREFIX=/usr/local/stow/llvm \
                  -DLLVM_TARGETS_TO_BUILD="host;X86" .. && ninja install',
    requires => [Exec['Clone LLVM Projects'], Exec['Clone LLVM Tools'], File['/curr/software/llvm-sources/build']],
  }

  exec { 'Get LLVM Suite in PATH'
    cwd => '/usr/local/stow',
    command => 'stow -R llvm',
    requires => Exec['Build and install LLVM Suite in /usr/local/stow']
  }

  exec { 'Build and install LLVM Suite linked to libc++ in /curr/software'
    cwd => '/curr/software/llvm-sources/build',
    command => 'find . -name CMakeCache.txt -delete && \
                  CC=`which clang` CXX=`which clang++` cmake -GNinja -DCMAKE_BUILD_TYPE=Release \
                  -DLLVM_ENABLE_LIBCXX=ON -DCMAKE_INSTALL_PREFIX=/curr/software/llvm \
                  -DLIBCXX_USE_COMPILER_RT=YES -DLLVM_TARGETS_TO_BUILD="host;X86" .. \
                  && ninja install',
    requires => Exec['Get LLVM Suite in PATH']
  }
}
