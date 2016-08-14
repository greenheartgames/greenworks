#!/usr/bin/env python

import argparse
import os
import subprocess
import sys
import tarfile
import zipfile

SOURCE_ROOT = os.path.abspath(os.path.dirname(os.path.dirname(__file__)))
DOWNLOAD_DIR = os.path.join(SOURCE_ROOT, 'download')

NW_PLATFORM_KEY = {
  'cygwin': 'win',
  'darwin': 'osx',
  'linux2': 'linux',
  'win32': 'win',
}[sys.platform]

ELECTRON_PLATFORM_KEY = {
  'cygwin': 'win32',
  'darwin': 'darwin',
  'linux2': 'linux',
  'win32': 'win32',
}[sys.platform]


NW_EXECUTE_PATH = {
  'darwin': 'nwjs.app/Contents/MacOS/nwjs',
  'cygwin': 'nw',
  'linux2': 'nw',
  'win32': 'nw',
}[sys.platform]


ELECTRON_EXECUTE_PATH = {
  'darwin': 'Electron.app/Contents/MacOS/Electron',
  'cygwin': 'electron',
  'linux2': 'electron',
  'win32': 'electron',
}[sys.platform]


def execute(argv, env=os.environ):
  print 'running', argv
  try:
    subprocess.check_call(argv, env=env)
  except subprocess.CalledProcessError as e:
    print e.output
    raise e


def download_and_extract(download_url, binary_name, target_dir):
  local_path = os.path.join(DOWNLOAD_DIR, binary_name)
  # Skip if downloaded.
  if os.path.exists(local_path):
    return
  print 'Downloading ' + download_url
  execute(['wget', download_url, '-P', DOWNLOAD_DIR])
  if (binary_name.endswith('.zip')):
    print (os.path.splitext(local_path)[0])
    with zipfile.ZipFile(local_path) as z:
      z.extractall(target_dir)
  else:
    with tarfile.open(local_path) as t:
      t.extractall(target_dir)


def ensure_electron_exists(electron_binary, args):
  download_url = 'https://github.com/electron/electron/releases/download/' \
                 'v{0}/{1}'.format(args.version, electron_binary)
  electron_dir = os.path.splitext(os.path.basename(electron_binary))[0]
  download_and_extract(download_url, electron_binary,
                       os.path.join(DOWNLOAD_DIR, electron_dir))


def ensure_nwjs_exists(nwjs_binary, args):
  download_url = 'http://dl.nwjs.io/v{0}/{1}'.format(args.version,
                                                     nwjs_binary)
  download_and_extract(download_url, nwjs_binary, DOWNLOAD_DIR)


def main():
  os.chdir(SOURCE_ROOT)
  args = parse_args()
  execute(['rm', '-rf', os.path.join(SOURCE_ROOT, 'lib')])
  if args.target == 'nw.js':
    execute(['nw-gyp', 'clean'])
    execute(['nw-gyp', 'configure', '--target='+args.version,
             '--arch='+args.arch])
    execute(['nw-gyp', 'rebuild', '--target='+args.version, '--arch='+args.arch])
    nwjs_binary = 'nwjs-v{0}-{1}-{2}'.format(args.version, NW_PLATFORM_KEY,
                                             args.arch)
    ensure_nwjs_exists(
        nwjs_binary + ('.tar.gz' if NW_PLATFORM_KEY == 'linux' else '.zip'),
        args)

    nwjs_dir = os.path.join(DOWNLOAD_DIR, nwjs_binary)
    # FIXME: figure out why there is no execute permission in the file extracted
    # by zipfile.
    if NW_PLATFORM_KEY == 'osx':
      execute(['chmod', '-R', '+x',
               os.path.join(nwjs_dir, 'nwjs.app')])
    execute([os.path.join(nwjs_dir, NW_EXECUTE_PATH),
             os.path.join(SOURCE_ROOT, 'samples/nw.js')])
  elif args.target == 'electron':
    execute(['node-gyp', 'clean'])
    env = os.environ.copy()
    env['HOME'] = os.path.join(env['HOME'], '.electron-gyp')
    execute(['node-gyp', 'rebuild', '--target='+args.version,
             '--arch='+args.arch,
             '--dist-url=https://atom.io/download/atom-shell'], env)
    electron_binary = 'electron-v{0}-{1}-{2}'.format(args.version,
                                                     ELECTRON_PLATFORM_KEY,
                                                     args.arch)
    ensure_electron_exists(electron_binary + '.zip', args)
    execute_path = os.path.join(DOWNLOAD_DIR, electron_binary,
                                ELECTRON_EXECUTE_PATH)
    if ELECTRON_PLATFORM_KEY == 'linux':
      execute(['chmod', '+x', execute_path])
    execute([execute_path, os.path.join(SOURCE_ROOT, 'samples/electron')])


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
  parser.add_argument('-t', '--target',
                      default='nw.js',
                      help='Build for nw.js or electron',
                      required=True)
  return parser.parse_args()


if __name__ == '__main__':
  sys.exit(main())
