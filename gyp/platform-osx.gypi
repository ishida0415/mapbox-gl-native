{
  'targets': [
    { 'target_name': 'platform-osx',
      'product_name': 'mbgl-platform-osx',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,
      'dependencies': [
        'version',
      ],

      'sources': [
        '../platform/default/async_task.cpp',
        '../platform/default/run_loop.cpp',
        '../platform/default/timer.cpp',
        '../platform/default/default_file_source.cpp',
        '../platform/default/online_file_source.cpp',
        '../platform/default/mbgl/storage/offline.hpp',
        '../platform/default/mbgl/storage/offline.cpp',
        '../platform/default/mbgl/storage/offline_database.hpp',
        '../platform/default/mbgl/storage/offline_database.cpp',
        '../platform/default/mbgl/storage/offline_download.hpp',
        '../platform/default/mbgl/storage/offline_download.cpp',
        '../platform/default/sqlite3.hpp',
        '../platform/default/sqlite3.cpp',
        '../platform/darwin/src/log_nslog.mm',
        '../platform/darwin/src/string_nsstring.mm',
        '../platform/darwin/src/application_root.mm',
        '../platform/darwin/src/image.mm',
        '../platform/darwin/src/nsthread.mm',
        '../platform/darwin/src/reachability.m',
        '../platform/darwin/src/NSException+MGLAdditions.h',
        '../platform/darwin/src/NSString+MGLAdditions.h',
        '../platform/darwin/src/NSString+MGLAdditions.m',
        '../platform/darwin/src/MGLTypes.m',
        '../platform/darwin/src/MGLStyle.mm',
        '../platform/darwin/src/MGLGeometry_Private.h',
        '../platform/darwin/src/MGLGeometry.mm',
        '../platform/darwin/src/MGLShape.m',
        '../platform/darwin/src/MGLMultiPoint_Private.h',
        '../platform/darwin/src/MGLMultiPoint.mm',
        '../platform/darwin/src/MGLPointAnnotation.m',
        '../platform/darwin/src/MGLPolyline.mm',
        '../platform/darwin/src/MGLPolygon.mm',
        '../platform/darwin/src/MGLMapCamera.mm',
        '../platform/darwin/src/MGLDownloadable.mm',
        '../platform/darwin/src/MGLDownloadable_Private.h',
        '../platform/darwin/src/MGLDownloadController.mm',
        '../platform/darwin/src/MGLDownloadController_Private.h',
        '../platform/darwin/src/MGLDownloadRegion_Private.h',
        '../platform/darwin/src/MGLTilePyramidDownloadRegion.mm',
        '../platform/darwin/src/MGLAccountManager_Private.h',
        '../platform/darwin/src/MGLAccountManager.m',
        '../platform/darwin/src/NSBundle+MGLAdditions.h',
        '../platform/darwin/src/NSBundle+MGLAdditions.m',
        '../platform/darwin/src/NSProcessInfo+MGLAdditions.h',
        '../platform/darwin/src/NSProcessInfo+MGLAdditions.m',
        '../platform/osx/src/MGLMapView_Private.h',
        '../platform/osx/src/MGLMapView.mm',
        '../platform/osx/src/MGLMapView+IBAdditions.m',
        '../platform/osx/src/MGLOpenGLLayer.h',
        '../platform/osx/src/MGLOpenGLLayer.mm',
        '../platform/osx/src/MGLCompassCell.h',
        '../platform/osx/src/MGLCompassCell.m',
        '../platform/osx/src/MGLAttributionButton.h',
        '../platform/osx/src/MGLAttributionButton.m',
        '../platform/osx/src/MGLAnnotationImage.m',
      ],

      'variables': {
        'cflags_cc': [
          '<@(libuv_cflags)',
          '<@(boost_cflags)',
          '<@(sqlite_cflags)',
          '<@(zlib_cflags)',
          '<@(rapidjson_cflags)',
          '<@(variant_cflags)',
        ],
        'ldflags': [
          '<@(zlib_ldflags)',
        ],
        'libraries': [
          '<@(libuv_static_libs)',
          '<@(sqlite_static_libs)',
          '<@(zlib_static_libs)',
          '$(SDKROOT)/System/Library/Frameworks/Cocoa.framework',
          '$(SDKROOT)/System/Library/Frameworks/CoreLocation.framework',
          '$(SDKROOT)/System/Library/Frameworks/OpenGL.framework',
          '$(SDKROOT)/System/Library/Frameworks/QuartzCore.framework',
          '$(SDKROOT)/System/Library/Frameworks/SystemConfiguration.framework',
        ],
      },

      'include_dirs': [
        '../platform/osx/include',
        '../platform/darwin/include',
        '../include',
        '../src',
        '../platform/default',
      ],

      'xcode_settings': {
        'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
        'CLANG_ENABLE_OBJC_ARC': 'YES',
        'CLANG_ENABLE_MODULES': 'YES',
      },

      'link_settings': {
        'libraries': [ '<@(libraries)' ],
        'xcode_settings': {
          'OTHER_LDFLAGS': [ '<@(ldflags)' ],
        },
      },

      'direct_dependent_settings': {
        'include_dirs': [
          '../platform/osx/include',
          '../platform/darwin/include',
          '../include',
        ],
        'mac_bundle_resources': [
          '<!@(find ../platform/osx/src/resources -type f \! -name \'.*\')',
        ],
      },
    },
  ],
}
