#
# Copyright 2018 Google LLC
# Copyright 2018-present Open Networking Foundation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

licenses(["notice"])  # Apache v2

package(
    default_visibility = ["//visibility:public"],
)

load("//bazel:rules.bzl", "stratum_cc_binary", "HOST_ARCHES")

stratum_cc_binary(
    name = "stratum_bmv2",
    srcs = [
        "main.cc",
    ],
    arches = HOST_ARCHES,
    deps = [
        "@com_github_google_glog//:glog",
        "//stratum/glue:init_google",
        "//stratum/glue:logging",
        "//stratum/hal/lib/bmv2:bmv2_switch",
        "//stratum/hal/lib/common:hal",
        "//stratum/hal/lib/phal:phal_sim",
        "//stratum/hal/lib/pi:pi_node",
        "//stratum/lib/security:auth_policy_checker",
        "//stratum/lib/security:credentials_manager",
        "@local_bmv2_bin//:bmv2_headers",
        "@local_bmv2_bin//:bmv2_simple_switch",
        "@local_bmv2_bin//:bmv2_pi",
    ],
    data = ["dummy.json"],
)

load("@bazel_tools//tools/build_defs/pkg:pkg.bzl", "pkg_deb", "pkg_tar")

pkg_tar(
    name = "stratum_bmv2_bin",
    srcs = [":stratum_bmv2"],
    mode = "0755",
    package_dir = "/usr/bin",
    strip_prefix = "/stratum/hal/bin/bmv2",
)

pkg_tar(
    name = "stratum_bmv2_service",
    srcs = ["stratum_bmv2.service"],
    mode = "0644",
    package_dir = "/etc/systemd/system",
    strip_prefix = "/stratum/hal/bin/bmv2",
)

pkg_tar(
    name = "stratum_bmv2_config",
    srcs = ["stratum_bmv2.environment"],
    mode = "0644",
    package_dir = "/etc/stratum",
    strip_prefix = "/stratum/hal/bin/bmv2",
)

pkg_tar(
    name = "stratum_bmv2_pipeline",
    srcs = ["dummy.json"],
    mode = "0644",
    package_dir = "/etc/stratum",
    strip_prefix = "/stratum/hal/bin/bmv2",
)

pkg_tar(
    name = "stratum_bmv2_data",
    extension = "tar.gz",
    deps = [
        ":stratum_bmv2_bin",
        ":stratum_bmv2_config",
        ":stratum_bmv2_pipeline",
        ":stratum_bmv2_service",
    ],
)

pkg_deb(
    name = "stratum_bmv2_debian",
    architecture = "amd64",
    built_using = "bazel (0.14.1)",
    data = ":stratum_bmv2_data",
    depends = [
        "libjudy-dev",
        "libgmp-dev",
        "libpcap-dev",
        "libboost1.55-all-dev",
    ],
    description_file = "debian/description",
    homepage = "https://stratumproject.org/",
    maintainer = "https://stratumproject.org/",
    package = "stratum_bmv2",
    postinst = "debian/postinst",
    version = "0.1.1",
)
