#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/platform/platform.hpp>

TEST_F(Storage, EmptyFile) {
    SCOPED_TEST(EmptyFile)

    using namespace mbgl;

    DefaultFileSource fs(nullptr, uv_default_loop());

    fs.request({ Resource::Unknown, "asset://TEST_DATA/fixtures/storage/empty" },
               uv_default_loop(),
               [&](const Response &res) {
        EXPECT_EQ(Response::Successful, res.status);
        EXPECT_EQ(0ul, res.data.size());
        EXPECT_EQ(0, res.expires);
        EXPECT_LT(1420000000, res.modified);
        EXPECT_NE("", res.etag);
        EXPECT_EQ("", res.message);
        EmptyFile.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

TEST_F(Storage, NonExistentFile) {
    SCOPED_TEST(NonExistentFile)

    using namespace mbgl;

    DefaultFileSource fs(nullptr, uv_default_loop());

    fs.request({ Resource::Unknown, "asset://TEST_DATA/fixtures/storage/does_not_exist" },
               uv_default_loop(),
               [&](const Response &res) {
        EXPECT_EQ(Response::Error, res.status);
        EXPECT_EQ(0ul, res.data.size());
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
        EXPECT_EQ("no such file or directory", res.message);
        NonExistentFile.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
