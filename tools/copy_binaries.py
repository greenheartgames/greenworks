#!/usr/bin/env python

import os
import shutil
import stat
import sys

target_dir = sys.argv[-1]
if not os.path.exists(target_dir):
  os.mkdir(target_dir)

for argv in sys.argv[1:-1]:
  shutil.copy(argv, target_dir)
