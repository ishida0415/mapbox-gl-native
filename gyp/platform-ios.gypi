{
  'targets': [
    { 'target_name': 'platform-ios',
      'product_name': 'mbgl-platform-ios',
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
        '../platform/default/sqlite_cache.cpp',
        '../platform/default/sqlite3.hpp',
        '../platform/default/sqlite3.cpp',
        '../platform/darwin/log_nslog.mm',
        '../platform/darwin/string_nsstring.mm',
        '../platform/darwin/application_root.mm',
        '../platform/darwin/asset_root.mm',
        '../platform/darwin/image.mm',
        '../platform/darwin/nsthread.mm',
        '../platform/darwin/reachability.m',
        '../platform/darwin/NSException+MGLAdditions.h',
        '../platform/darwin/NSString+MGLAdditions.h',
        '../platform/darwin/NSString+MGLAdditions.m',
        '../include/mbgl/darwin/MGLTypes.h',
        '../platform/darwin/MGLTypes.m',
        '../include/mbgl/darwin/MGLStyle.h',
        '../platform/darwin/MGLStyle.mm',
        '../include/mbgl/darwin/MGLGeometry.h',
        '../platform/darwin/MGLGeometry_Private.h',
        '../platform/darwin/MGLGeometry.mm',
        '../include/mbgl/darwin/MGLAnnotation.h',
        '../include/mbgl/darwin/MGLShape.h',
        '../platform/darwin/MGLShape.m',
        '../include/mbgl/darwin/MGLMultiPoint.h',
        '../platform/darwin/MGLMultiPoint_Private.h',
        '../platform/darwin/MGLMultiPoint.mm',
        '../include/mbgl/darwin/MGLOverlay.h',
        '../include/mbgl/darwin/MGLPointAnnotation.h',
        '../platform/darwin/MGLPointAnnotation.m',
        '../include/mbgl/darwin/MGLPolyline.h',
        '../platform/darwin/MGLPolyline.mm',
        '../include/mbgl/darwin/MGLPolygon.h',
        '../platform/darwin/MGLPolygon.mm',
        '../include/mbgl/darwin/MGLMapCamera.h',
        '../platform/darwin/MGLMapCamera.mm',
        '../include/mbgl/ios/Mapbox.h',
        '../platform/ios/src/MGLMapboxEvents.h',
        '../platform/ios/src/MGLMapboxEvents.m',
        '../include/mbgl/ios/MGLMapView.h',
        '../include/mbgl/ios/MGLMapView+IBAdditions.h',
        '../include/mbgl/ios/MGLMapView+MGLCustomStyleLayerAdditions.h',
        '../platform/ios/src/MGLMapView.mm',
        '../include/mbgl/ios/MGLAccountManager.h',
        '../platform/ios/src/MGLAccountManager_Private.h',
        '../platform/ios/src/MGLAccountManager.m',
        '../include/mbgl/ios/MGLUserLocation.h',
        '../platform/ios/src/MGLUserLocation_Private.h',
        '../platform/ios/src/MGLUserLocation.m',
        '../platform/ios/src/MGLUserLocationAnnotationView.h',
        '../platform/ios/src/MGLUserLocationAnnotationView.m',
        '../include/mbgl/ios/MGLAnnotationImage.h',
        '../platform/ios/src/MGLAnnotationImage_Private.h',
        '../platform/ios/src/MGLAnnotationImage.m',
        '../include/mbgl/ios/MGLCalloutView.h',
        '../platform/ios/src/MGLCompactCalloutView.h',
        '../platform/ios/src/MGLCompactCalloutView.m',
        '../platform/ios/src/MGLCategoryLoader.h',
        '../platform/ios/src/MGLCategoryLoader.m',
        '../platform/ios/src/NSBundle+MGLAdditions.h',
        '../platform/ios/src/NSBundle+MGLAdditions.m',
        '../platform/ios/src/NSProcessInfo+MGLAdditions.h',
        '../platform/ios/src/NSProcessInfo+MGLAdditions.m',
        '../platform/ios/vendor/SMCalloutView/SMCalloutView.h',
        '../platform/ios/vendor/SMCalloutView/SMCalloutView.m',
        '../platform/ios/vendor/Fabric/FABAttributes.h',
        '../platform/ios/vendor/Fabric/FABKitProtocol.h',
        '../platform/ios/vendor/Fabric/Fabric.h',
        '../platform/ios/vendor/Fabric/Fabric+FABKits.h',
        '../platform/ios/resources/',
      ],

      'variables': {
        'cflags_cc': [
          '<@(libuv_cflags)',
          '<@(boost_cflags)',
          '<@(variant_cflags)',
          '<@(sqlite_cflags)',
          '<@(zlib_cflags)',
        ],
        'libraries': [
          '<@(libuv_static_libs)',
          '<@(sqlite_static_libs)',
          '<@(zlib_static_libs)',
        ],
        'ldflags': [
          '<@(sqlite_ldflags)',
          '<@(zlib_ldflags)',
          '-framework CoreLocation',
          '-framework GLKit',
          '-framework ImageIO',
          '-framework MobileCoreServices',
          '-framework QuartzCore',
          '-framework SystemConfiguration',
          '-ObjC',
        ],
      },

      'include_dirs': [
        '../include/mbgl/ios',
        '../include/mbgl/darwin',
        '../include',
        '../src',
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
          '../include/mbgl/ios',
          '../include/mbgl/darwin',
          '../include',
        ],
        'mac_bundle_resources': [
          '<!@(find ../platform/ios/resources -type f \! -name "README")',
        ],
      },
    },
  ],
}
