#!/usr/bin/env python

import os
import shutil
import stat
import sys

lib_steam_file = sys.argv[1]
greenworks_node_file = sys.argv[2];
target_dir = sys.argv[3];

if not os.path.exists(target_dir):
  os.mkdir(target_dir)

shutil.copy(lib_steam_file, target_dir)
shutil.copy(greenworks_node_file, target_dir)
