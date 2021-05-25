class merlin {
  if $operatingsystemrelease =~ /^6.*/ {
      require merlin::devtoolset
  }
  require merlin::llvm
}
