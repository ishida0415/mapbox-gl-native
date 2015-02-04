{
  'target_defaults': {
    'target_conditions': [
      ['_type == "static_library"', {
        'xcode_settings': {
          'SDKROOT': 'iphoneos',
          'SUPPORTED_PLATFORMS': 'iphonesimulator iphoneos',
          'CLANG_CXX_LIBRARY': 'libc++',
          'CLANG_CXX_LANGUAGE_STANDARD':'c++11',
          'IPHONEOS_DEPLOYMENT_TARGET':'7.0',
          'TARGETED_DEVICE_FAMILY': '1,2',
          'GCC_VERSION': 'com.apple.compilers.llvm.clang.1_0',
          'CLANG_ENABLE_OBJC_ARC': 'NO',
          'CODE_SIGN_IDENTITY': 'iPhone Developer',
          'SKIP_INSTALL': 'YES'
        },
        'configurations': {
          'Release': {
            'xcode_settings': {
              'ARCHS': [ "armv7", "armv7s", "arm64", "i386", "x86_64" ],
            },
          },
        },
      }],
    ],
  },
  'targets': [
    { 'target_name': 'mbgl-ios',
      'product_name': 'mbgl-ios',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,
      'dependencies': [
        'version',
      ],
      'sources': [
        '../platform/darwin/log_nslog.mm',
        '../platform/darwin/string_nsstring.mm',
        '../platform/darwin/http_request_cocoa.mm',
        '../platform/darwin/application_root.mm',
        '../platform/darwin/image.mm',
        '../platform/default/asset_request_libuv.cpp',
      ],
      'include_dirs': [
        '../include',
        '../src',
      ],
      'xcode_settings': {
        'OTHER_CPLUSPLUSFLAGS': [
          '<@(uv_cflags)',
          '-I<(boost_root)/include',
        ],
      },
      'direct_dependent_settings': {
        'include_dirs': [
          '../include',
        ],
        'xcode_settings': {
          'OTHER_LDFLAGS': [
            '-framework ImageIO',
            '-framework MobileCoreServices',
          ],
        },
      },
    },
  ],
}
