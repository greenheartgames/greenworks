#!/usr/bin/env python

import argparse
import os
import shutil
import subprocess
import sys
import json
import zipfile

from github import GitHub

GITHUB_REPO = 'greenheartgames/greenworks'
SOURCE_ROOT = os.path.abspath(os.path.dirname(os.path.dirname(__file__)))
DIST_DIR = os.path.join(SOURCE_ROOT, 'dist')

PLATFORM_KEY = {
  'cygwin': 'win',
  'darwin': 'osx',
  'linux2': 'linux',
  'win32': 'win',
}[sys.platform]


def create_or_get_release_draft(github, releases, nwjs_version):
  # Search for existing draft.
  for release in releases:
    if release['draft']:
      return release
  return create_release_draft(github, nwjs_version)


def create_release_draft(github, nwjs_version):
  name = 'Greenworks v{0} for NW.js v{1}'.format(get_greenworks_version(),
                                                 nwjs_version)
  body = '[PlaceHolder]'
  data = dict(tag_name='v'+get_greenworks_version(), name=name, body=body,
              draft=True)
  r = github.repos(GITHUB_REPO).releases.post(data=data)
  return r


def auth_token():
  token = os.environ.get('GREENWORKS_GITHUB_TOKEN')
  message = ('Error: Please set the $GREENWORKS_GITHUB_TOKEN '
             'environment variable, which is your personal token.')
  assert token, message
  return token


def upload_to_github(file_path, nwjs_version):
  github = GitHub(auth_token())
  releases = github.repos(GITHUB_REPO).releases.get()
  release = create_or_get_release_draft(github, releases, nwjs_version)
  params = {'name':  os.path.basename(file_path) }
  headers = {'Content-Type': 'application/zip'}
  with open(file_path, 'rb') as f:
    github.repos(GITHUB_REPO).releases(release['id']).assets.post(
        params=params, headers=headers, data=f, verify=False)


def make_zip(zip_file_path, files):
  zip_file = zipfile.ZipFile(zip_file_path, 'w', zipfile.ZIP_DEFLATED)
  for filename in files:
    zip_file.write(filename, filename)
  zip_file.close()


def get_greenworks_version():
  with open(os.path.join(SOURCE_ROOT, 'package.json')) as f:
    data = json.load(f)
    return data['version']


def execute(argv, env=os.environ):
  print 'running', argv
  try:
    subprocess.check_call(argv, env=env)
  except subprocess.CalledProcessError as e:
    print e.output
    raise e


def force_build(args):
  build = os.path.join(SOURCE_ROOT, 'script', 'bootstrap.py')
  execute([sys.executable, build, '--target='+args.target,
           '--version='+args.version, '--arch='+args.arch])


def main():
  args = parse_args()
  shutil.rmtree(DIST_DIR, ignore_errors=True)
  os.makedirs(DIST_DIR)

  want = [
      'lib/greenworks-{0}{1}.node'.format(PLATFORM_KEY,
        '64' if args.arch == 'x64' else '32'),
      'greenworks.js'
  ]
  if args.target == 'nw.js':
    # The name is like greenworks-v0.6.0-nw-v0.16.0-win-ia32.zip.
    dist_name = 'greenworks-v{0}-nw-v{1}-{2}-{3}.zip'.format(
        get_greenworks_version(), args.version, PLATFORM_KEY, args.arch)
  elif args.target == 'electron':
    dist_name = 'greenworks-v{0}-electron-v{1}-{2}-{3}.zip'.format(
        get_greenworks_version(), args.version, PLATFORM_KEY,
        args.arch)
  else:
     raise 'Unknown target.'

  force_build(args)
  zip_file = os.path.join(SOURCE_ROOT, 'dist', dist_name)
  print 'Creating ', zip_file
  make_zip(zip_file, want)
  print 'Uploading {0} to GitHub.'.format(zip_file)
  upload_to_github(zip_file, args.version)


def parse_args():
  parser = argparse.ArgumentParser(
      description='Create dist and upload to GitHub')
  parser.add_argument('-a', '--arch',
                      help='x64 or ia32',
                      default='x64',
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
