{
    'target_defaults': {
        'default_configuration': 'Release',
        'configurations': {
          'Debug': {},
          'Release': {}
        },
    },
    'targets': [{
        'target_name': 'gtest',
        'type': 'static_library',
        'sources': [
            'gtest-all.cc'
        ],
        'link_settings': {
            'xcode_settings': { 'OTHER_LDFLAGS': [ '-pthread' ] },
            'ldflags': [ '-pthread' ],
        },
        'direct_dependent_settings': {
            'include_dirs': [
              '.',
            ],
        },
    }]
}