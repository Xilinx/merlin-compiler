class merlin::llvm-deps {
  require merlin::stow

  package { 'zlib-devel'
    ensure => latest,
  }

  package { 'ncurses-devel'
    ensure => latest,
  }

  package { 'libcurl-devel'
    ensure => latest,
  }

  package { 'expat-devel'
    ensure => latest,
  }

  package { 'openssl-devel'
    ensure => latest,
  }

  package { 'python-virtualenv'
    ensure => latest,
  }

  exec { 'stow -R git'
    cwd => '/curr/software',
  }

  exec { 'stow -R cmake'
    cwd => '/curr/software',
  }

  exec { 'stow -R ninja'
    cwd => '/curr/software',
  }
}
