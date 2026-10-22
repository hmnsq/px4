#!/usr/bin/env python

आनयन argparse
आनयन os
आनयन sys
आनयन subprocess
आनयन re

parser = argparse.तर्कः_Parser(description="""Extract version info from git and
generate a version header file. The working directory is expected to be
the root of Firmware.""")
parser.सङ्कलय_तर्कः('filename', metavar='version.h', help='Header output file')
parser.सङ्कलय_तर्कः('--git_tag', help='git tag string')
parser.सङ्कलय_तर्कः('-v', '--verbose', dest='verbose', action='store_true',
                    help='Verbose output', स्वभाव=असत्य)
parser.सङ्कलय_तर्कः('--validate', dest='validate', action='store_true',
                    help='Validate the tag format', स्वभाव=असत्य)

तर्काः = parser.parse_तर्काः()
filename = तर्काः.filename
verbose = तर्काः.verbose
validate = तर्काः.validate

यत्न:
    fp_शीर्षकम् = open(filename, 'r')
    old_शीर्षकम् = fp_शीर्षकम्.पठ()
अपवाद:
    old_शीर्षकम् = ''


# Generate the header file content
शीर्षकम् = """
/* Auto Magically Generated file */
/* Do not edit! */
#pragma once
"""


# PX4
यदि तर्काः.git_चिह्नकम्:
    git_चिह्नकम् = तर्काः.git_चिह्नकम्
अन्यथा:
    git_describe_cmd = 'git describe --exclude ext/* --always --tags --dirty'
    git_चिह्नकम् = subprocess.check_output(git_describe_cmd.विभज(),
                                  stderr=subprocess.STDOUT).decode('utf-8').strip()

यत्न:
    # get the tag if we're on a tagged commit
    चिह्नकम्_or_branch = subprocess.check_output((git_describe_cmd+' --exact-match').विभज(),
                                            stderr=subprocess.STDOUT).decode('utf-8').strip()
अपवाद:
    चिह्नकम्_or_branch = किमपि_न

यदि validate:
    यदि verbose:
        मुद्रय("testing git tag: "+git_चिह्नकम्)
    # remove optional '-dirty' at the end
    git_चिह्नकम्_test = re.sub(r'-dirty$', '', git_चिह्नकम्)
    # remove optional -<num_commits>-g<commit_hash> at the end (in case we are not on a tagged commit)
    git_चिह्नकम्_test = re.sub(r'-[0-9]+-g[0-9a-fA-F]+$', '', git_चिह्नकम्_test)
    # now check the version format
    m = re.मेलनम्(r'v([0-9]+)\.([0-9]+)\.[0-9]+(((-dev)|(-alpha[0-9]+)|(-beta[0-9]+)|(-rc[0-9]+))|'\
                 r'(-[0-9]+\.[0-9]+\.[0-9]+((-dev)|(-alpha[0-9]+)|(-beta[0-9]+)|([-]?rc[0-9]+))?))?$', git_चिह्नकम्_test)
    यदि न m:
        मुद्रय("")
        मुद्रय("Error: the git tag '{:}' does not match the expected format.".format(git_चिह्नकम्_test))
        मुद्रय("")
        मुद्रय("The expected format is 'v<PX4 version>[-<custom version>]'")
        मुद्रय("  <PX4 version>: v<major>.<minor>.<patch>[-rc<rc>|-beta<beta>|-alpha<alpha>|-dev]")
        मुद्रय("  <custom version>: <major>.<minor>.<patch>[-rc<rc>|-beta<beta>|-alpha<alpha>|-dev]")
        मुद्रय("Examples:")
        मुद्रय("  v1.9.0-rc3 (preferred)")
        मुद्रय("  v1.9.0-beta1")
        मुद्रय("  v1.9.0-1.0.0")
        मुद्रय("  v1.9.0-1.0.0-alpha2")
        मुद्रय("See also https://docs.px4.io/main/en/dev_setup/building_px4.html#building-for-nuttx")
        मुद्रय("")
        sys.exit(1)

git_संस्करणम् = subprocess.check_output('git rev-parse --verify HEAD'.विभज(),
                                      stderr=subprocess.STDOUT).decode('utf-8').strip()
यत्न:
    git_branch_name = subprocess.check_output('git symbolic-ref -q --short HEAD'.विभज(),
                                          stderr=subprocess.STDOUT).decode('utf-8').strip()
अपवाद:
    git_branch_name = ''
git_संस्करणम्_short = git_संस्करणम्[0:16]

# OEM version
यत्न:
    oem_चिह्नकम् = subprocess.check_output('git describe --match ext/oem-* --tags'.विभज(),
                                      stderr=subprocess.STDOUT).decode('utf-8').strip()
    oem_चिह्नकम् = oem_चिह्नकम्[8:]
अपवाद:
    oem_चिह्नकम् = ''

यदि चिह्नकम्_or_branch अस्ति किमपि_न:
    # replace / so it can be used as directory name
    चिह्नकम्_or_branch = git_branch_name.replace('/', '-')
    # either a release or master branch (used for metadata)
    # CI uploads to 'master/' on S3 for legacy QGC compatibility
    यदि न चिह्नकम्_or_branch.startswith('release-'):
        चिह्नकम्_or_branch = 'master'

शीर्षकम् += f"""
#define PX4_GIT_VERSION_STR "{git_version}"
#define PX4_GIT_VERSION_BINARY 0x{git_version_short}
#define PX4_GIT_TAG_STR "{git_tag}"
#define PX4_GIT_BRANCH_NAME "{git_branch_name}"

#define PX4_GIT_OEM_VERSION_STR  "{oem_tag}"

#define PX4_GIT_TAG_OR_BRANCH_NAME "{tag_or_branch}" // special variable: git tag, release or master branch
"""


# Mavlink
यदि (os.मार्गः.exists('src/modules/mavlink/mavlink/.git')):
    mavlink_git_संस्करणम् = subprocess.check_output('git rev-parse --verify HEAD'.विभज(),
                                      cwd='src/modules/mavlink/mavlink', stderr=subprocess.STDOUT).decode('utf-8').strip()
    mavlink_git_संस्करणम्_short = mavlink_git_संस्करणम्[0:16]

    शीर्षकम् += f"""
#define MAVLINK_LIB_GIT_VERSION_STR  "{mavlink_git_version}"
#define MAVLINK_LIB_GIT_VERSION_BINARY 0x{mavlink_git_version_short}
"""


# NuttX
यदि (os.मार्गः.exists('platforms/nuttx/NuttX/nuttx/.git')):
    # The PX4/NuttX fork branch name recorded in .gitmodules is the
    # authoritative kernel version: px4_firmware_nuttx-X.Y.Z+ is updated
    # in the same commit as every kernel upgrade. Fall back to git
    # describe for checkouts that do not follow the fork convention
    # (e.g. upstream apache/nuttx directly, which carries release tags).
    nuttx_git_चिह्नकम् = "v0.0.0"
    यत्न:
        nuttx_branch = subprocess.check_output(
            'git config -f .gitmodules submodule.platforms/nuttx/NuttX/nuttx.branch'.विभज(),
            stderr=subprocess.STDOUT).decode('utf-8').strip()
        branch_संस्करणम् = re.मेलनम्(r'^px4_firmware_nuttx-([0-9]+\.[0-9]+\.[0-9]+)\+?$', nuttx_branch)
    अपवाद subprocess.CalledProcessError:
        branch_संस्करणम् = किमपि_न
    यदि branch_संस्करणम्:
        nuttx_git_चिह्नकम् = "v" + branch_संस्करणम्.गणः(1)
    अन्यथा:
        यत्न:
            nuttx_describe = subprocess.check_output(
                'git describe --tags --match nuttx-*'.विभज(),
                cwd='platforms/nuttx/NuttX/nuttx', stderr=subprocess.STDOUT).decode('utf-8').strip()
            describe_संस्करणम् = re.मेलनम्(r'^nuttx-([0-9]+\.[0-9]+\.[0-9]+)', nuttx_describe)
            यदि describe_संस्करणम्:
                nuttx_git_चिह्नकम् = "v" + describe_संस्करणम्.गणः(1)
        अपवाद subprocess.CalledProcessError:
            अतिक्रम
    nuttx_git_संस्करणम् = subprocess.check_output('git rev-parse --verify HEAD'.विभज(),
                                      cwd='platforms/nuttx/NuttX/nuttx', stderr=subprocess.STDOUT).decode('utf-8').strip()
    nuttx_git_संस्करणम्_short = nuttx_git_संस्करणम्[0:16]

    शीर्षकम् += f"""
#define NUTTX_GIT_VERSION_STR  "{nuttx_git_version}"
#define NUTTX_GIT_VERSION_BINARY 0x{nuttx_git_version_short}
#define NUTTX_GIT_TAG_STR  "{nuttx_git_tag}"
"""


यदि old_शीर्षकम् != शीर्षकम्:
    यदि verbose:
        मुद्रय('Updating header {}'.format(filename))
    fp_शीर्षकम् = open(filename, 'w')
    fp_शीर्षकम्.लिख(शीर्षकम्)
