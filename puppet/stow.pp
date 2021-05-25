class merlin::stow {
  package { 'stow'
    ensure => latest,
  }

  file { '/usr/local/stow'
    ensure => directory,
    mode   => '777',
  }
}
