#!/usr/bin/env python3

import os
import re
import sys

import shlex
import shutil

BUNDLE_NAME = "SSH Commander.app"

INFO_PLIST_CONTENTS = """<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleDevelopmentRegion</key>
    <string>English</string>
    <key>CFBundleDisplayName</key>
    <string>SSH Commander</string>
    <key>CFBundleExecutable</key>
    <string>SSH Commander</string>
    <key>CFBundleGetInfoString</key>
    <string>1.0, Copyright 2016-2018 Andreas Heck</string>
    <key>CFBundleIconFile</key>
    <string>sshcommander.icns</string>
    <key>CFBundleInfoDictionaryVersion</key>
    <string>6.0</string>
    <key>CFBundleName</key>
    <string>SSH Commander</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>1.0</string>
    <key>CFBundleVersion</key>
    <string>1.0</string>
    <key>NSHumanReadableCopyright</key>
    <string>Copyright 2016-2018 Andreas Heck</string>
    <key>NSPrincipalClass</key>
    <string>NSApplication</string>
    <key>CGDisableCoalescedUpdates</key>
    <true/>
</dict>
</plist>
"""

def find_macdeployqt():
    print("Searching for Qt tool macdeployqt...")

    qtdir = os.path.join(os.getenv("HOME"), "Qt")

    if not os.path.exists(qtdir):
        return None

    pattern = re.compile(r"^5\.(\d+)\.(\d+)$")
    found_dir = None
    found_minor = 0
    found_bugfix = 0

    dirs = os.listdir(qtdir)
    for name in dirs:
        if not os.path.isdir(os.path.join(qtdir, name)): continue

        match = pattern.search(name)

        if match:
            minor = int(match.group(1))
            bugfix = int(match.group(2))

            if found_dir is None or (minor > found_minor or (minor == found_minor and bugfix > found_bugfix)):
                found_dir = name
                found_minor = minor
                found_bugfix = bugfix

    if found_dir is None:
        return None

    path = os.path.join(qtdir, found_dir, "clang_64", "bin", "macdeployqt")
    if os.path.exists(path):
        return path

    return None

def create_bundle_dirs():
    if os.path.exists(BUNDLE_NAME):
        print("Removing old bundle directory " + BUNDLE_NAME + "...")
        shutil.rmtree(BUNDLE_NAME)

    print("Creating bundle directories...")
    os.makedirs(os.path.join(BUNDLE_NAME, "Contents", "MacOS"))
    os.makedirs(os.path.join(BUNDLE_NAME, "Contents", "Resources"))

def create_info_plist():
    print("Creating file Info.plist...")
    f = open(os.path.join(BUNDLE_NAME, "Contents", "Info.plist"), "w")
    f.write(INFO_PLIST_CONTENTS)
    f.close()

def create_pkg_info():
    pass

def copy_binary():
    print("Copying binary...")
    targetfile = os.path.join(BUNDLE_NAME, "Contents", "MacOS", "SSH Commander")
    shutil.copyfile("src/sshcommander", targetfile)
    os.chmod(targetfile, 0o775)

def copy_icon():
    print("Copying icon...")
    targetfile = os.path.join(BUNDLE_NAME, "Contents", "Resources",
            "sshcommander.icns")
    shutil.copyfile("src/images/sshcommander.icns", targetfile)

def copy_framework():
    macdeployqt_path = find_macdeployqt()
    if macdeployqt_path is None:
        print("Fatal: Couldn't find macdeployqt", file=sys.stderr)
        print("Qt framework files were not copied to app bundle", file=sys.stderr)
        exit(1)

    if os.system(macdeployqt_path + " " + shlex.quote(BUNDLE_NAME)) == 0:
        print("Copied Qt framework files to app bundle")
    else:
        print("Failed to copy Qt framework files to app bundle", file=sys.stderr)

if os.system("make -j2") != 0:
    print("Failed to build", file=sys.stderr)
    sys.exit(1)

print()
print("Starting to create bundle " + BUNDLE_NAME + "...")
create_bundle_dirs()
create_info_plist()
create_pkg_info()
copy_binary()
copy_icon()
copy_framework()

print("Finished creating bundle " + BUNDLE_NAME)
