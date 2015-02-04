{
  'conditions': [
    ['install_prefix != ""', {
      'targets': [
        { 'target_name': 'install',
          'type': 'none',
          'hard_dependency': 1,
          'dependencies': [
            'core',
            'platform-<(platform_lib)',
            'http-<(http_lib)',
            'asset-<(asset_lib)',
            'cache-<(cache_lib)',
            'headless-<(headless_lib)',
            'standalone',
          ],

          'copies': [
            { 'files': [ '<(PRODUCT_DIR)/libmbgl.a' ], 'destination': '<(install_prefix)/lib' },
            { 'files': [ '../include/mbgl' ], 'destination': '<(install_prefix)/include' },
            { 'files': [ '<(SHARED_INTERMEDIATE_DIR)/include/mbgl/util/version.hpp' ], 'destination': '<(install_prefix)/include/mbgl/util' },
          ],

          'actions': [
            { 'action_name': 'mbgl-config',
              'inputs': [
                '../utils/mbgl-config/mbgl-config.template.sh',
                '../utils/mbgl-config/build.sh',
              ],
              'outputs': [
                '<(install_prefix)/bin/mbgl-config',
              ],
              'action': [
                './utils/mbgl-config/build.sh',
                '<(install_prefix)',
                '<(PRODUCT_DIR)/libmbgl.a.ldflags',
              ]
            }
          ]
        },
      ],
    }],
  ],
}
