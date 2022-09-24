cc_library(
    name = "counting_quotient_filter",
    hdrs = ["counting_quotient_filter.h"],
    deps = [":cqf"],
)

cc_library(
    name = "cqf",
    srcs = [
        "src/gqf.c",
        "src/hashutil.c",
    ],
    hdrs = [
        "include/gqf.h",
        "include/gqf_int.h",
        "include/hashutil.h",
    ],
)

cc_library(
    name = "cqf_test",
    testonly = True,
    srcs = ["src/test.c"],
    hdrs = ["include/test.h"],
    deps = [
        ":cqf",
        "//third_party/openssl:crypto",
    ],
)

cc_test(
    name = "counting_quotient_filter_test",
    srcs = [
        "counting_quotient_filter_test.cc",
    ],
    deps = [
        ":counting_quotient_filter",
        ":cqf_test",
        "//testing/base/public:gunit_main",
    ],
)
