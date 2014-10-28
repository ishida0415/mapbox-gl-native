{
  'targets': [
    { 'target_name': 'mbgl',
      'product_name': 'mbgl',
      'type': 'static_library',
      'hard_dependency': 1,
      'dependencies': [
          'shaders',
      ],
      'variables': {
        'cflags_cc': [
          '<@(png_cflags)',
          '<@(uv_cflags)',
          '<@(sqlite3_cflags)',
          '<@(zlib_cflags)',
          '-I<(boost_root)/include',
        ],
        'cflags': [
          '<@(uv_cflags)',
        ],
        'ldflags': [
          '<@(png_ldflags)',
          '<@(uv_ldflags)',
          '<@(sqlite3_ldflags)',
          '<@(zlib_ldflags)',
        ],
      },
      'sources': [
        '<!@(find src -name "*.cpp")',
        '<!@(test -f "config/constants_local.cpp" && echo "config/constants_local.cpp" || echo "config/constants.cpp")',
        '<!@(find src -name "*.c")',
        '<!@(find src -name "*.h")',
        '<!@(find include -name "*.hpp")',
        '<!@(find include -name "*.h")',
        '<!@(find src -name "*.glsl")',
        'bin/style.json'
      ],
      'include_dirs': [
        '../include',
      ],
      'libraries': [
        '<@(png_static_libs)',
        '<@(uv_static_libs)',
        '<@(sqlite3_static_libs)',
        '<@(zlib_static_libs)',
      ],
      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
            'OTHER_CFLAGS': [ '<@(cflags)' ],
            # Makefile builds don't respect 'libraries' on OS X.
            'OTHER_LDFLAGS': [ '>@(_libraries)' ],
          },
        }, {
          'cflags_cc': [ '<@(cflags_cc)' ],
          'cflags': [ '<@(cflags)' ],
        }]
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '../include',
        ],
        'conditions': [
          ['OS == "mac"', {
            'xcode_settings': {
              'OTHER_LDFLAGS': [ '<@(ldflags)' ]
            }
          }, {
            'ldflags': [ '<@(ldflags)' ]
          }]
        ]
      }
    }
  ]
}
