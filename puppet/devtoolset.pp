class merlin::devtoolset {
  package { 'devtoolset-7-libstdc++-devel'
    require => Package['centos-release-scl', 'devtoolset-7'],
    ensure => latest,
  }

  package { 'python27'
    require => Package['centos-release-scl'],
    ensure => latest,
  }

  exec { 'source /opt/rh/devtoolset-7/enable && source /opt/rh/python27/enable'
    require => [Package['devtoolset-7-libstdc++-devel'], Package['python27']]
  }
}
