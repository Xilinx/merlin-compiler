#(C) Copyright 2016-2021 Xilinx, Inc.
#All Rights Reserved.
#
#Licensed to the Apache Software Foundation (ASF) under one
#or more contributor license agreements.  See the NOTICE file
#distributed with this work for additional information
#regarding copyright ownership.  The ASF licenses this file
#to you under the Apache License, Version 2.0 (the
#"License"); you may not use this file except in compliance
#with the License.  You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
#Unless required by applicable law or agreed to in writing,
#software distributed under the License is distributed on an
#"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#KIND, either express or implied.  See the License for the
#specific language governing permissions and limitations
#under the License. (edited)
#!/usr/bin/env python

import sys
import numpy as np

points = np.array( [(float(x),float(y),float(z)) for (x,y,z) in map(lambda L: L.strip().split(), sys.stdin.readlines())] )

N = points.shape[0]

# Johan Philip. "The Probability Distribution of the Distance between two Random Points in a Box", numerically estimated with Mathematica
# an underestimate, actually, because we set a lower bound at the vdW threshold
expected_dist = 12.69

dists = np.zeros((N,N))
for (i,A) in enumerate(points):
  for (j,B) in enumerate(points):
    if i!=j:
      dists[i,j] = np.linalg.norm(A-B)
    else:
      dists[i,j] = expected_dist;

print 'Distance mean:',np.mean(dists)
print 'Distance variance:',np.var(dists)
minpair = np.unravel_index(np.argmin(dists), (N,N))
print 'Closest pair:',minpair, np.min(dists)
print '  p0',points[minpair[0]]
print '  p1',points[minpair[1]]
maxpair = np.unravel_index(np.argmax(dists), (N,N))
print 'Furthest pair:',maxpair, np.max(dists),'( max',20.0*np.sqrt(3.),')'
print '  p0',points[maxpair[0]]
print '  p1',points[maxpair[1]]
