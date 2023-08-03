# SPDX-License-Identifier: Apache-2.0

zephyr_include_directories(.)

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})
