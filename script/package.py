#!/usr/bin/env python

import argparse
import os
import shutil
import subprocess
import sys
import json
import zipfile

SOURCE_ROOT = os.path.abspath(os.path.dirname(os.path.dirname(__file__)))
DIST_DIR = os.path.join(SOURCE_ROOT, 'dist')

PLATFORM_KEY = {
  'cygwin': 'win',
  'darwin': 'osx',
  'linux2': 'linux',
  'win32': 'win',
}[sys.platform]


def make_zip(zip_file_path, files):
  zip_file = zipfile.ZipFile(zip_file_path, "w", zipfile.ZIP_DEFLATED)
  for filename in files:
    zip_file.write(filename, filename)
  zip_file.close()


def get_greenworks_version():
  with open(os.path.join(SOURCE_ROOT, 'package.json')) as f:
    data = json.load(f)
    return data['version']


def main():
  args = parse_args()
  shutil.rmtree(DIST_DIR, ignore_errors=True)
  os.makedirs(DIST_DIR)

  want = [
      'lib/greenworks-{0}{1}.node'.format(PLATFORM_KEY,
        '64' if args.arch == 'x64' else '32'),
      'greenworks.js'
  ]
  # greenworks-v0.6.0-nw-v0.16.0-win-ia32.zip
  dist_name = 'greenworks-v{0}-nw-v{1}-{2}-{3}.zip'.format(
      get_greenworks_version(), args.version, PLATFORM_KEY, args.arch)

  zip_file = os.path.join(SOURCE_ROOT, 'dist', dist_name)
  print 'Creating ', zip_file
  make_zip(zip_file, want)


def parse_args():
  parser = argparse.ArgumentParser(description='Build project')
  parser.add_argument('-a', '--arch',
                      help='x64 or ia32',
                      default='x64',
                      required=False)
  parser.add_argument('-v', '--version',
                      default='',
                      help='NW.js version',
                      required=True)
  return parser.parse_args()

if __name__ == '__main__':
  sys.exit(main())
