.. _rtc_module:

Module for working with RTC
########################################

Overview
********

This module work with DS3231 and DS3231 driver for Zephyr
************************

Module shuold clone to $zephyrproject/zephyr/software and
build with command west build -p always -b nucleo_f411re software/rtc_module

Project use device tree overlay for work with RTC and EEPROM memory
Also project have an shell commands

you should add remote to your west.yml to remotes section:

- name: shutdown_tracking_module
      url-base: https://github.com/2345lug

and add to project section:
- name: rtc_shutdown_tracking_module
      remote: shutdown_tracking_module
      repo-path: rtc_module.git
      revision: develop
      path: modules/rtc_module

and after that do "west update".

you can start test project from you "zephyr" folder by command:

west build -p always -b nucleo_f411re ../modules/rtc_module/sample
