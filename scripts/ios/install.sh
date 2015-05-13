#!/usr/bin/env bash

set -e
set -o pipefail

mapbox_time "checkout_mason" \
git submodule update --init .mason

mapbox_time "install_xcpretty" \
gem install xcpretty --no-rdoc --no-ri --no-document --quiet

mapbox_time "install_recent_git" \
brew install git

mapbox_time "install_awscli" \
brew install awscli

mkdir -p ${KIF_SCREENSHOTS}
