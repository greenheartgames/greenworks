#!/usr/bin/env python

import argparse
import contextlib
import tempfile
import os
import subprocess
import sys
import tarfile
import urllib2
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

NW_GYP = 'nw-gyp'
NODE_GYP = 'node-gyp'
if sys.platform in ['win32', 'cygwin']:
  NW_GYP += '.cmd'
  NODE_GYP += '.cmd'


def log(message):
  sys.stderr.write(message)
  sys.stderr.flush()


def execute(argv, env=os.environ):
  log('Running: {0}.\n'.format(' '.join(argv)))
  try:
    subprocess.check_call(argv, env=env)
  except subprocess.CalledProcessError as e:
    print e.output
    raise e


def ensure_dir_exists(path):
  if not os.path.exists(path):
    log ("Creating directory {0}.\n".format(path))
    os.makedirs(path)


def download_and_extract(download_url, binary_name, target_dir):
  for ext in ['.zip', '.tar.gz']:
    if binary_name.endswith(ext):
      local_path = os.path.join(DOWNLOAD_DIR, binary_name[:-len(ext)])
  # Skip if downloaded.
  if os.path.exists(local_path):
    log('{0} exists, skipping download.\n'.format(local_path))
    return
  log('Downloading {0}\n'.format(download_url))
  with tempfile.NamedTemporaryFile(delete=False) as t:
    with contextlib.closing(urllib2.urlopen(download_url)) as u:
      while True:
        chunk = u.read(1024*1024)
        if not len(chunk):
          break
        sys.stderr.write('.')
        sys.stderr.flush()
        t.write(chunk)

    log('\nExtracting {0} to {1}.\n'.format(t.name, target_dir))
    if (binary_name.endswith('.zip')):
      with zipfile.ZipFile(t) as z:
        z.extractall(target_dir)
    else:
      t.close()
      with tarfile.open(t.name) as tf:
        tf.extractall(target_dir)


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
  ensure_dir_exists(DOWNLOAD_DIR)
  args = parse_args()
  if not args.run_only:
    execute(['rm', '-rf', os.path.join(SOURCE_ROOT, 'lib')])
  if args.target == 'nw.js':
    if not args.run_only:
      execute([NW_GYP, 'clean'])
      execute([NW_GYP, 'configure', '--target='+args.version,
               '--arch='+args.arch])
      execute([NW_GYP, 'rebuild', '--target='+args.version,
               '--arch='+args.arch])
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
    if not args.run_only:
      execute([NODE_GYP, 'clean'])
      env = os.environ.copy()
      env['HOME'] = os.path.join(env['HOME'], '.electron-gyp')
      execute([NODE_GYP, 'rebuild', '--target='+args.version,
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
  parser.add_argument('-r', '--run-only',
                      help='Only Run Greenworks on a specific target',
                      action='store_true',
                      required=False)
  parser.add_argument('-v', '--version',
                      default='',
                      help='nw.js/electron version',
                      required=True)
  parser.add_argument('-t', '--target',
                      default='nw.js',
                      help='Build for nw.js or electron',
                      required=True)
  return parser.parse_args()


if __name__ == '__main__':
  sys.exit(main())
