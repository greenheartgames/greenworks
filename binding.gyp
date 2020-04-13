{
  'variables': {
    'steamworks_sdk_dir': '<!(node tools/steamworks_sdk_dir.js)',
    'target_dir': 'lib'
  },

  'conditions': [
    ['OS=="win"', {
      'conditions': [
        ['target_arch=="ia32"', {
          'variables': {
            'project_name': 'greenworks-win32',
            'redist_bin_dir': '',
            'public_lib_dir': 'win32',
            'lib_steam': 'steam_api.lib',
            'lib_encryptedappticket': 'sdkencryptedappticket.lib',
            'lib_dll_steam': 'steam_api.dll',
            'lib_dll_encryptedappticket': 'sdkencryptedappticket.dll',
          },
        }],
        ['target_arch=="x64"', {
          'variables': {
            'project_name': 'greenworks-win64',
            'redist_bin_dir': 'win64',
            'public_lib_dir': 'win64',
            'lib_steam': 'steam_api64.lib',
            'lib_encryptedappticket': 'sdkencryptedappticket64.lib',
            'lib_dll_steam': 'steam_api64.dll',
            'lib_dll_encryptedappticket': 'sdkencryptedappticket64.dll',
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
        'public_lib_dir': 'osx32',
        'lib_steam': 'libsteam_api.dylib',
        'lib_encryptedappticket': 'libsdkencryptedappticket.dylib',
      },
    }],
    ['OS=="linux"', {
      'variables': {
        'lib_steam': 'libsteam_api.so',
        'lib_encryptedappticket': 'libsdkencryptedappticket.so',
      },
      'conditions': [
        ['target_arch=="ia32"', {
          'variables': {
            'project_name': 'greenworks-linux32',
            'redist_bin_dir': 'linux32',
            'public_lib_dir': 'linux32',
          }
        }],
        ['target_arch=="x64"', {
          'variables': {
            'project_name': 'greenworks-linux64',
            'redist_bin_dir': 'linux64',
            'public_lib_dir': 'linux64',
          }
        }],
      ],
    }],
  ],

  'targets': [
    {
      'target_name': '<(project_name)',
      'sources': [
        'src/api/greenworks_api_utils.cc',
        'src/api/steam_api_achievement.cc',
        'src/api/steam_api_auth.cc',
        'src/api/steam_api_cloud.cc',
        'src/api/steam_api_dlc.cc',
        'src/api/steam_api_friends.cc',
        'src/api/steam_api_matchmaking.cc',
        'src/api/steam_api_registry.h',
        'src/api/steam_api_settings.cc',
        'src/api/steam_api_stats.cc',
        'src/api/steam_api_workshop.cc',
        'src/greenworks_api.cc',
        'src/greenworks_async_workers.cc',
        'src/greenworks_async_workers.h',
        'src/greenworks_unzip.cc',
        'src/greenworks_unzip.h',
        'src/greenworks_utils.cc',
        'src/greenworks_utils.h',
        'src/greenworks_version.h',
        'src/greenworks_workshop_workers.cc',
        'src/greenworks_workshop_workers.h',
        'src/greenworks_zip.cc',
        'src/greenworks_zip.h',
        'src/steam_async_worker.cc',
        'src/steam_async_worker.h',
        'src/steam_client.cc',
        'src/steam_client.h',
        'src/steam_event.cc',
        'src/steam_event.h',
        'src/steam_id.cc',
        'src/steam_id.h',
      ],
      'include_dirs': [
        'deps',
        'src',
        '<(steamworks_sdk_dir)/public',
        '<!(node -e "require(\'nan\')")'
      ],
      'dependencies': [ 'deps/zlib/zlib.gyp:minizip' ],
      'link_settings': {
        'library_dirs': [
          '<(steamworks_sdk_dir)/redistributable_bin/<(redist_bin_dir)/',
          '<(steamworks_sdk_dir)/public/steam/lib/<(public_lib_dir)/',
        ],
        'conditions': [
          ['OS=="linux" or OS=="mac"', {
            'libraries': ['-lsteam_api', '-lsdkencryptedappticket'],
          }],
          ['OS=="win"', {
            'libraries': ['-l<(lib_steam)', '-l<(lib_encryptedappticket)'],
          }],
        ],
      },
      'cflags': [ '-std=c++11' ],
      'conditions': [
        ['OS== "linux"',
          {
            'ldflags': [
              '-Wl,-rpath,\$$ORIGIN',
            ],
            'cflags': [
              # Disable compilation warnings on nw.js custom node_buffer.h
              '-Wno-unknown-pragmas',
              '-Wno-attributes',
            ],
          },
        ],
        ['OS== "win" and target_arch=="x64"',
          {
            'defines': [
              '_AMD64_',
            ],
          },
        ],
        # For zlib.gyp::minizip library.
        ['OS=="mac" or OS=="ios" or OS=="android"', {
          # Mac, Android and the BSDs don't have fopen64, ftello64, or
          # fseeko64. We use fopen, ftell, and fseek instead on these
          # systems.
          'defines': [
            'USE_FILE32API'
          ],
        }],
      ],
      'xcode_settings': {
        'WARNING_CFLAGS':  [
          '-Wno-deprecated-declarations',
        ],
        'OTHER_CPLUSPLUSFLAGS' : [
          '-std=c++11',
          '-stdlib=libc++'
        ],
        'OTHER_LDFLAGS': [
          '-stdlib=libc++'
        ],
      },
      'msvs_disabled_warnings': [
        4068,  # disable unknown pragma warnings from nw.js custom node_buffer.h.
        4267,  # conversion from 'size_t' to 'int', popssible loss of data
      ],
    },
    {
      'target_name': 'copy_binaries',
      'dependencies': [
        '<(project_name)'
      ],
      'type': 'none',
      'actions': [
        {
          'action_name': 'Copy Binaries',
          'variables': {
            'conditions': [
              ['OS=="win"', {
                'lib_steam_path': '<(steamworks_sdk_dir)/redistributable_bin/<(redist_bin_dir)/<(lib_dll_steam)',
                'lib_encryptedappticket_path': '<(steamworks_sdk_dir)/public/steam/lib/<(public_lib_dir)/<(lib_dll_encryptedappticket)',
              }],
              ['OS=="mac" or OS=="linux"', {
                'lib_steam_path': '<(steamworks_sdk_dir)/redistributable_bin/<(redist_bin_dir)/<(lib_steam)',
                'lib_encryptedappticket_path': '<(steamworks_sdk_dir)/public/steam/lib/<(public_lib_dir)/<(lib_encryptedappticket)',
              }],
            ]
          },
          'inputs': [
            '<(lib_steam_path)',
            '<(lib_encryptedappticket_path)',
            '<(PRODUCT_DIR)/<(project_name).node',
          ],
          'outputs': [
            '<(target_dir)',
          ],
          'action': [
            'python',
            'tools/copy_binaries.py',
            '<@(_inputs)',
            '<@(_outputs)',
          ],
        }
      ],
    },
  ]
}
