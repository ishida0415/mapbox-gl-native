HEADLESS = none
PLATFORM ?= ios
ASSET ?= fs
HTTP ?= nsurl
CACHE ?= sqlite

HOST_VERSION = all

PROVISIONING_PROFILE ?= 19324a54-7455-4f0b-8e1c-e6957c718ebc

export MASON_PLATFORM=$(HOST)
export MASON_PLATFORM_VERSION=$(HOST_VERSION)
