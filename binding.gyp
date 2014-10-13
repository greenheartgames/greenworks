{
  'variables': {
    'source_root_dir': '<!(python tools/source_root_dir.py)',
    'steamworks_sdk_dir': 'deps/steamworks_sdk'
  },

  'conditions': [
    ['OS=="win"', {
      'conditions': [
        ['target_arch=="ia32"', {
          'variables': {
            'project_name': 'greenworks-win32',
            'redist_bin_dir': '',
            'lib_steam': 'steam_api.lib',
            'lib_dll_steam': 'steam_api.dll',
          },
        }],
        ['target_arch=="x64"', {
          'variables': {
            'project_name': 'greenworks-win64',
            'redist_bin_dir': 'win64',
            'lib_steam': 'steam_api64.lib',
            'lib_dll_steam': 'steam_api64.dll',
          },
        }],
      ],
    }],
    ['OS=="mac"', {
      'conditions': [
        ['target_arch=="ia32"', {
          'variables': {
            'project_name': 'greenworks-osx32',
          },
        }],
        ['target_arch=="x64"', {
          'variables': {
            'project_name': 'greenworks-osx64',
          },
        }],
      ],
      'variables': {
        'redist_bin_dir': 'osx32',
        'lib_steam': 'libsteam_api.dylib'
      },
    }],
    ['OS=="linux"', {
      'conditions': [
        ['target_arch=="ia32"', {
          'variables': {
            'project_name': 'greenworks-linux32',
            'redist_bin_dir': 'linux32',
            'lib_steam': 'libsteam_api.so'
          }
        }],
        ['target_arch=="x64"', {
          'variables': {
            'project_name': 'greenworks-linux64',
            'redist_bin_dir': 'linux64',
            'lib_steam': 'libsteam_api.so'
          }
        }],
      ],
    }],
  ],

  'targets': [
    {
      'target_name': '<(project_name)',
      'sources': [
        'src/greenworks_api.cc',
        'src/greenworks_async_workers.cc',
        'src/greenworks_async_workers.h',
        'src/greenworks_workshop_workers.cc',
        'src/greenworks_workshop_workers.h',
        'src/greenworks_utils.cc',
        'src/greenworks_utils.h',
        'src/steam_async_worker.cc',
        'src/steam_async_worker.h',
      ],
      'include_dirs': [
        '<(steamworks_sdk_dir)/public',
        '<!(node -e "require(\'nan\')")'
      ],
      'link_settings': {
        'libraries': [
          '<(source_root_dir)/<(steamworks_sdk_dir)/redistributable_bin/<(redist_bin_dir)/<(lib_steam)'
        ]
      },
      'conditions': [
        ['OS== "linux"',
          {
            'ldflags': [
              '-Wl,-rpath,\$$ORIGIN',
            ],
          },
        ],
      ],
      'xcode_settings': {
        'WARNING_CFLAGS':  [
          '-Wno-deprecated-declarations',
        ],
      },
      'copies': [
        {
          'destination': '<(PRODUCT_DIR)',
          'conditions': [
            ['OS=="win"', {
              'files': [
                '<(source_root_dir)/<(steamworks_sdk_dir)/redistributable_bin/<(redist_bin_dir)/<(lib_dll_steam)'
              ],
            }],
            ['OS=="mac" or OS=="linux"', {
              'files': [
                '<(source_root_dir)/<(steamworks_sdk_dir)/redistributable_bin/<(redist_bin_dir)/<(lib_steam)'
              ],
            }],
          ],
        }
      ],
    },
  ]
}
