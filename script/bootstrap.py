#!/usr/bin/env python

import argparse
import os
import subprocess
import sys
import tarfile
import zipfile

SOURCE_ROOT = os.path.abspath(os.path.dirname(os.path.dirname(__file__)))
DOWNLOAD_DIR = os.path.join(SOURCE_ROOT, "download")

PLATFORM_KEY = {
  'cygwin': 'win',
  'darwin': 'osx',
  'linux2': 'linux',
  'win32': 'win',
}[sys.platform]

EXECUTE_PATH = {
  'darwin': 'nwjs.app/Contents/MacOS/nwjs',
  'cygwin': 'nwjs',
  'linux2': 'nwjs',
  'win32': 'nwjs',
}[sys.platform]


def execute(argv):
  print 'running', argv
  try:
    subprocess.check_call(argv)
  except subprocess.CalledProcessError as e:
    print e.output
    raise e


def EnsureBinaryExists(binary, args):
  file_extension = ".tar.gz" if PLATFORM_KEY== 'linux' else ".zip"
  binary_name_with_extension = binary + file_extension
  download_path = os.path.join(DOWNLOAD_DIR, binary_name_with_extension)
  if not os.path.exists(download_path):
    download_url = 'http://dl.nwjs.io/v{0}/{1}'.format(args.version,
        binary_name_with_extension)
    print 'Downloading ' + download_url
    execute(['wget', download_url, '-P', DOWNLOAD_DIR])
    if (file_extension == '.zip'):
      with zipfile.ZipFile(download_path) as z:
        z.extractall(DOWNLOAD_DIR)
    else:
      with tarfile.open(download_path) as t:
        t.extractall(DOWNLOAD_DIR)


def main():
  os.chdir(SOURCE_ROOT)
  args = parse_args()
  execute(['nw-gyp', 'clean'])
  execute(['rm', '-rf', os.path.join(SOURCE_ROOT, 'lib')])
  execute(['nw-gyp', 'configure', '--target='+args.version,
           '--arch='+args.arch])
  execute(['nw-gyp', 'rebuild', '--target='+args.version, '--arch='+args.arch])
  nwjs_binary = 'nwjs-v{0}-{1}-{2}'.format(args.version, PLATFORM_KEY,
                                           args.arch)
  EnsureBinaryExists(nwjs_binary, args)
  execute_path = os.path.join(DOWNLOAD_DIR, nwjs_binary, EXECUTE_PATH)
  # FIXME: figure out why there is no execute permission in the file extracted
  # by zipfile.
  execute(['chmod', '-R', '+x',
           os.path.join(DOWNLOAD_DIR, nwjs_binary, 'nwjs.app')])
  execute([execute_path, os.path.join(SOURCE_ROOT, 'samples/nw.js')])


def parse_args():
  parser = argparse.ArgumentParser(description='Build Greenworks')
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
