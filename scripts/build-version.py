#!/usr/bin/env python

import sys, os, errno


output_dir = sys.argv[1]
tag = map(int, sys.argv[2].split('.'))
rev = sys.argv[3]


def mkdir_p(path):
    try:
        os.makedirs(path)
    except OSError as exc: # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else: raise


header = """// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
#ifndef MBGL_UTIL_VERSION
#define MBGL_UTIL_VERSION

#define MBGL_VERSION 0x{major:02x}{minor:02x}{patch:02x}
#define MBGL_VERSION_STRING "{major}.{minor}.{patch}"
#define MBGL_VERSION_MAJOR {major}
#define MBGL_VERSION_MINOR {minor}
#define MBGL_VERSION_PATCH {patch}
#define MBGL_VERSION_REV "{rev}"

namespace mbgl {{
namespace version {{

extern const int major, minor, patch;
extern const char *revision;
extern const char *string;
extern const unsigned int number;

}}
}}

#endif
""".format(
    major = tag[0],
    minor = tag[1],
    patch = tag[2],
    rev = rev
)

header_path = os.path.join(output_dir, 'include/mbgl/util/version.hpp')
mkdir_p(os.path.dirname(header_path))
with open(header_path, 'w') as f: f.write(header)

source = """// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
#include <mbgl/util/version.hpp>

namespace mbgl {{
namespace version {{

const int major = {major};
const int minor = {minor};
const int patch = {patch};
const char *revision = "{rev}";
const char *string = "{major}.{minor}.{patch}";
const unsigned int number = 0x{major:02x}{minor:02x}{patch:02x};

}}
}}
""".format(
    major = tag[0],
    minor = tag[1],
    patch = tag[2],
    rev = rev
)

# Note: We can't use version.cpp since libuv already has a file named version.c.
# Having another one here would overwrite that file and cause missing symbols errors.
source_path = os.path.join(output_dir, 'src/mbgl/util/mbgl_version.cpp')
mkdir_p(os.path.dirname(source_path))
with open(source_path, 'w') as f: f.write(source)
