{
'conditions': [
	['OS=="win"', {
			'variables': {
				'copy_command%': 'copy',
				'module_name%': 'greenworks-win',
				'redist_bin_dir%': '',
				'lib_steam': 'steam_api',
				'lib_extension%': 'lib',
				'dir': '$(CURDIR)../'
			},
		}
	],
	
	['OS=="mac"', {
		'variables': {
			'copy_command%': 'cp',
			'module_name%': 'greenworks-osx',
			'redist_bin_dir%': 'osx32',
			'lib_steam': 'libsteam_api',
			'lib_extension%': 'dylib',
			'dir': '$(CURDIR)/../'
		},
	}
	],

	['OS=="linux"', {
		'conditions': [
            ['target_arch=="ia32"', {
							'variables': {
								'copy_command%': 'cp',
								'module_name%': 'greenworks-linux32',
								'redist_bin_dir%': 'linux32',
								'lib_steam': 'libsteam_api',
								'lib_extension%': 'so',
								'dir': '../'
							}
            }],
            ['target_arch=="x64"', {
							'variables': {
								'copy_command%': 'cp',
								'module_name%': 'greenworks-linux64',
								'redist_bin_dir%': 'linux64',
								'lib_steam': 'libsteam_api',
								'lib_extension%': 'so',
								'dir': '../'
							}
            }]
          ]
      }],
],

'targets': [
{
'cflags!': ['-fexceptions','-m32'],
'cflags_cc!': [ '-fno-exceptions','-m32' ],
'CXXFLAGS!': ['-m32'],
'LDFLAGS!': ['-m32'],
'target_name': 'greenworks',
'sources': [
'Includes.h',
'CSteamCallable.h',
'CSteamCallable.cpp',
'CSteamWorkshop.h',
'CSteamWorkshop.cpp',
'CUtils.h',
'CUtils.cpp',
'Greenworks.h',
'Greenworks.cpp',
'Greenutils.h',
'Greenutils.cpp',
'CSteamWorkshopItem.h',
'CSteamWorkshopItem.cpp',
'misc/dirent.h',
'zlib/zlib.h',
'zlib/adler32.c',
'zlib/adler32.c',
'zlib/compress.c',
'zlib/crc32.h',
'zlib/crc32.c',
'zlib/deflate.h',
'zlib/deflate.c',
'zlib/gzclose.c',
'zlib/gzguts.h',
'zlib/gzlib.c',
'zlib/gzread.c',
'zlib/gzwrite.c',
'zlib/infback.c',
'zlib/inffast.h',
'zlib/inffast.c',
'zlib/inffixed.h',
'zlib/inflate.h',
'zlib/inflate.c',
'zlib/inftrees.h',
'zlib/inftrees.c',
'zlib/trees.h',
'zlib/trees.c',
'zlib/uncompr.c',
'zlib/zconf.h',
'zlib/zutil.h',
'zlib/zutil.c',
'minizip/crypt.h',
'minizip/ioapi.h',
'minizip/ioapi.c',
'minizip/iowin32.h',
'minizip/iowin32.c',
'minizip/mztools.h',
'minizip/mztools.c',
'minizip/unzip.h',
'minizip/unzip.c',
'minizip/zip.h',
'minizip/zip.c',
'CUnzip.h',
'CUnzip.cpp',
'CZip.h',
'CZip.cpp'
],
'link_settings': {
'ldflags': ['-L$(CURDIR)steamworks-sdk/redistributable_bin/<(redist_bin_dir)'],
'libraries': [
'<(dir)steamworks-sdk/redistributable_bin/<(redist_bin_dir)/<(lib_steam).<(lib_extension)'
]
}
},

{
'target_name': 'action_after_build',
'type': 'none',
'actions': [
{
'action_name': 'move_node_module',

'inputs': [
'<@(PRODUCT_DIR)/greenworks.node'
],

'outputs': [
'deploy/<@(module_name).node'
],

'action': ['<@(copy_command)', '<@(PRODUCT_DIR)/greenworks.node', 'deploy/<@(module_name).node']
}
]
}
]
}