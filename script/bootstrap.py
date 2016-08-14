#!/usr/bin/env python

import argparse
import os
import subprocess
import sys
import tarfile
import zipfile

SOURCE_ROOT = os.path.abspath(os.path.dirname(os.path.dirname(__file__)))
DOWNLOAD_DIR = os.path.join(SOURCE_ROOT, 'download')

PLATFORM_KEY = {
  'cygwin': 'win',
  'darwin': 'osx',
  'linux2': 'linux',
  'win32': 'win',
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


def download_and_extract(download_url, binary_name):
  local_path = os.path.join(DOWNLOAD_DIR, binary_name)
  # Skip if downloaded.
  if os.path.exists(local_path):
    return
  print 'Downloading ' + download_url
  execute(['wget', download_url, '-P', DOWNLOAD_DIR])
  if (binary_name.endswith('.zip')):
    with zipfile.ZipFile(local_path) as z:
      z.extractall(DOWNLOAD_DIR)
  else:
    with tarfile.open(local_path) as t:
      t.extractall(DOWNLOAD_DIR)


def ensure_electron_exists(electron_binary, args):
  download_url = 'https://github.com/electron/electron/releases/download/' \
                 'v{0}/{1}'.format(args.version, electron_binary)
  download_and_extract(download_url, electron_binary)
  #if not os.path.exists(download_path):
    #download_url = 'https://github.com/electron/electron/releases/download/
                    #v{0}/{1}.zip'.format(args.version, electron_binary)
    #print 'Downloading ' + download_url
    #execute(['wget', download_url, '-P', DOWNLOAD_DIR])
    #with zipfile.ZipFile(download_path) as z:
      #z.extractall(DOWNLOAD_DIR)


def ensure_nwjs_exists(nwjs_binary, args):
  download_url = 'http://dl.nwjs.io/v{0}/{1}'.format(args.version,
                                                     nwjs_binary)
  download_and_extract(download_url, nwjs_binary)
  #if not os.path.exists(download_path):
    #download_url = 'http://dl.nwjs.io/v{0}/{1}'.format(args.version,
        #binary_name_with_extension)
    #print 'Downloading ' + download_url
    #execute(['wget', download_url, '-P', DOWNLOAD_DIR])
    #if (file_extension == '.zip'):
      #with zipfile.ZipFile(download_path) as z:
        #z.extractall(DOWNLOAD_DIR)
    #else:
      #with tarfile.open(download_path) as t:
        #t.extractall(DOWNLOAD_DIR)


def main():
  os.chdir(SOURCE_ROOT)
  args = parse_args()
  execute(['rm', '-rf', os.path.join(SOURCE_ROOT, 'lib')])
  if args.target == 'nw.js':
    execute(['nw-gyp', 'clean'])
    execute(['nw-gyp', 'configure', '--target='+args.version,
             '--arch='+args.arch])
    execute(['nw-gyp', 'rebuild', '--target='+args.version, '--arch='+args.arch])
    nwjs_binary = 'nwjs-v{0}-{1}-{2}'.format(args.version, PLATFORM_KEY,
                                             args.arch)
    execute_path = os.path.join(DOWNLOAD_DIR, nwjs_binary, NW_EXECUTE_PATH)
    ensure_nwjs_exists(
        nwjs_binary + ('.tar.gz' if PLATFORM_KEY == 'linux' else '.zip'),
        args)
    # FIXME: figure out why there is no execute permission in the file extracted
    # by zipfile.
    if PLATFORM_KEY == 'osx':
      execute(['chmod', '-R', '+x',
               os.path.join(DOWNLOAD_DIR, nwjs_binary, 'nwjs.app')])
    execute([execute_path, os.path.join(SOURCE_ROOT, 'samples/nw.js')])
  elif args.target == 'electron':
    execute(['node-gyp', 'clean'])
    env = os.environ.copy()
    env['HOME'] = os.path.join(env['HOME'], '.electron-gyp')
    execute(['node-gyp', 'rebuild', '--target='+args.version,
             '--arch='+args.arch,
             '--dist-url=https://atom.io/download/atom-shell'], env)
    electron_binary = 'electron-v{0}-{1}-{2}'.format(args.version,
                                                     sys.platform,
                                                     args.arch)
    ensure_electron_exists(electron_binary + '.zip', args)
    execute_path = os.path.join(DOWNLOAD_DIR, electron_binary,
                                ELECTRON_EXECUTE_PATH)
    print execute_path
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
