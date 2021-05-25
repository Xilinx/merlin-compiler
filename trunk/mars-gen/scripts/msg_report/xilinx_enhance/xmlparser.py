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

"""
Wrapper of underlying XML Parser Library
"""

import xml.etree.ElementTree as ET
#pylint: disable=super-init-not-called, invalid-name

class XMLParseError(Exception):
    """ xml file parse error """
    def __init__(self, msg):
        self.msg = msg
    def __str__(self):
        return self.msg

def parse_xml(path):
    """ Returns representation of the root node in the XML file. """
    try:
        return XMLElement(ET.parse(path).getroot())
    except ET.ParseError as e:
        raise XMLParseError(str(e))

class XMLElement:
    """ Representation of node in XML document """
    def __init__(self, node):
        self._node = node

    def __iter__(self):
        return (XMLElement(i) for i in self._node)

    def __len__(self):
        return len(self._node)

    def __getitem__(self, k):
        return XMLElement(self._node[k])

    def __repr__(self):
        return repr(self._node)

    def tag(self):
        """ Returns the tag name of the node. """
        return self._node.tag

    def attr(self, name):
        """ Returns the value of the attribute of the node. """
        return self._node.attrib.get(name, "")

    def text(self):
        """ Returns the text of the node. """
        if self._node.text is None:
            return ""
        else:
            return self._node.text
