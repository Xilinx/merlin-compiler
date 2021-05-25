# (C) Copyright 2016-2021 Xilinx, Inc.
# All Rights Reserved.
#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License. 


%parameters = (
"param0"=>"asdf",
);
sub param_define {
	($name, $value) = @_;
	if ($parameters{$name} eq "") {
		return $value;
	}
	else {
		return $parameters{$name};
	}
}
sub param_array_define {
	($name, @value) = @_;
  if("hw_module_list" eq $name ) {
    return ("dequant");
  }
  elsif("pcore_dir_list" eq $name ) {
    return (".././src/dequant/pcores");
  }
  else {
    return @value;
 }
}

use POSIX;
use XML::Simple;
sub param_tree_define {
  my ($name, $xml_file, $array_reserve) = @_;
  my $xml = new XML::Simple(KeyAttr=>'nan', ForceArray=>$array_reserve);
  my $test = $xml->XMLin("test.xml");
  foreach my $param_tree (@{$test->{configuration}->{param_tree}})
  {
    if( $name eq $param_tree->{name} )
    {
      return $param_tree;
    }
  }
  return $xml->XMLin($xml_file);
}

