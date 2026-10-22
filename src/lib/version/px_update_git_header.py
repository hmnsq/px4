#!/usr/bin/env python

استورد argparse
استورد os
استورد sys
استورد subprocess
استورد re

parser = argparse.آرگومان_Parser(description="""Extract version info from git and
generate a version header file. The working directory is expected to be
the root of Firmware.""")
parser.جمع_کن_آرگومان('filename', metavar='version.h', help='Header output file')
parser.جمع_کن_آرگومان('--git_tag', help='git tag string')
parser.جمع_کن_آرگومان('-v', '--verbose', dest='verbose', action='store_true',
                    help='Verbose output', افتراضي=خطأ)
parser.جمع_کن_آرگومان('--validate', dest='validate', action='store_true',
                    help='Validate the tag format', افتراضي=خطأ)

آرگومانها = parser.parse_آرگومانها()
filename = آرگومانها.filename
verbose = آرگومانها.verbose
validate = آرگومانها.validate

حاول:
    fp_سرآیند = open(filename, 'r')
    old_سرآیند = fp_سرآیند.اقرأ()
استثناء:
    old_سرآیند = ''


# Generate the header file content
سرآیند = """
/* Auto Magically Generated file */
/* Do not edit! */
#pragma once
"""


# PX4
إذا آرگومانها.git_برچسب:
    git_برچسب = آرگومانها.git_برچسب
وإلا:
    git_describe_cmd = 'git describe --exclude ext/* --always --tags --dirty'
    git_برچسب = subprocess.check_output(git_describe_cmd.تقسیم(),
                                  stderr=subprocess.STDOUT).decode('utf-8').strip()

حاول:
    # get the tag if we're on a tagged commit
    برچسب_or_branch = subprocess.check_output((git_describe_cmd+' --exact-match').تقسیم(),
                                            stderr=subprocess.STDOUT).decode('utf-8').strip()
استثناء:
    برچسب_or_branch = هیچ

إذا validate:
    إذا verbose:
        چاپ("testing git tag: "+git_برچسب)
    # remove optional '-dirty' at the end
    git_برچسب_test = re.sub(r'-dirty$', '', git_برچسب)
    # remove optional -<num_commits>-g<commit_hash> at the end (in case we are not on a tagged commit)
    git_برچسب_test = re.sub(r'-[0-9]+-g[0-9a-fA-F]+$', '', git_برچسب_test)
    # now check the version format
    m = re.تطابق(r'v([0-9]+)\.([0-9]+)\.[0-9]+(((-dev)|(-alpha[0-9]+)|(-beta[0-9]+)|(-rc[0-9]+))|'\
                 r'(-[0-9]+\.[0-9]+\.[0-9]+((-dev)|(-alpha[0-9]+)|(-beta[0-9]+)|([-]?rc[0-9]+))?))?$', git_برچسب_test)
    إذا ليس m:
        چاپ("")
        چاپ("Error: the git tag '{:}' does not match the expected format.".format(git_برچسب_test))
        چاپ("")
        چاپ("The expected format is 'v<PX4 version>[-<custom version>]'")
        چاپ("  <PX4 version>: v<major>.<minor>.<patch>[-rc<rc>|-beta<beta>|-alpha<alpha>|-dev]")
        چاپ("  <custom version>: <major>.<minor>.<patch>[-rc<rc>|-beta<beta>|-alpha<alpha>|-dev]")
        چاپ("Examples:")
        چاپ("  v1.9.0-rc3 (preferred)")
        چاپ("  v1.9.0-beta1")
        چاپ("  v1.9.0-1.0.0")
        چاپ("  v1.9.0-1.0.0-alpha2")
        چاپ("See also https://docs.px4.io/main/en/dev_setup/building_px4.html#building-for-nuttx")
        چاپ("")
        sys.exit(1)

git_نسخه = subprocess.check_output('git rev-parse --verify HEAD'.تقسیم(),
                                      stderr=subprocess.STDOUT).decode('utf-8').strip()
حاول:
    git_branch_name = subprocess.check_output('git symbolic-ref -q --short HEAD'.تقسیم(),
                                          stderr=subprocess.STDOUT).decode('utf-8').strip()
استثناء:
    git_branch_name = ''
git_نسخه_short = git_نسخه[0:16]

# OEM version
حاول:
    oem_برچسب = subprocess.check_output('git describe --match ext/oem-* --tags'.تقسیم(),
                                      stderr=subprocess.STDOUT).decode('utf-8').strip()
    oem_برچسب = oem_برچسب[8:]
استثناء:
    oem_برچسب = ''

إذا برچسب_or_branch است هیچ:
    # replace / so it can be used as directory name
    برچسب_or_branch = git_branch_name.replace('/', '-')
    # either a release or master branch (used for metadata)
    # CI uploads to 'master/' on S3 for legacy QGC compatibility
    إذا ليس برچسب_or_branch.startswith('release-'):
        برچسب_or_branch = 'master'

سرآیند += f"""
#define PX4_GIT_VERSION_STR "{git_version}"
#define PX4_GIT_VERSION_BINARY 0x{git_version_short}
#define PX4_GIT_TAG_STR "{git_tag}"
#define PX4_GIT_BRANCH_NAME "{git_branch_name}"

#define PX4_GIT_OEM_VERSION_STR  "{oem_tag}"

#define PX4_GIT_TAG_OR_BRANCH_NAME "{tag_or_branch}" // special variable: git tag, release or master branch
"""


# Mavlink
إذا (os.مسیر.exists('src/modules/mavlink/mavlink/.git')):
    mavlink_git_نسخه = subprocess.check_output('git rev-parse --verify HEAD'.تقسیم(),
                                      cwd='src/modules/mavlink/mavlink', stderr=subprocess.STDOUT).decode('utf-8').strip()
    mavlink_git_نسخه_short = mavlink_git_نسخه[0:16]

    سرآیند += f"""
#define MAVLINK_LIB_GIT_VERSION_STR  "{mavlink_git_version}"
#define MAVLINK_LIB_GIT_VERSION_BINARY 0x{mavlink_git_version_short}
"""


# NuttX
إذا (os.مسیر.exists('platforms/nuttx/NuttX/nuttx/.git')):
    # The PX4/NuttX fork branch name recorded in .gitmodules is the
    # authoritative kernel version: px4_firmware_nuttx-X.Y.Z+ is updated
    # in the same commit as every kernel upgrade. Fall back to git
    # describe for checkouts that do not follow the fork convention
    # (e.g. upstream apache/nuttx directly, which carries release tags).
    nuttx_git_برچسب = "v0.0.0"
    حاول:
        nuttx_branch = subprocess.check_output(
            'git config -f .gitmodules submodule.platforms/nuttx/NuttX/nuttx.branch'.تقسیم(),
            stderr=subprocess.STDOUT).decode('utf-8').strip()
        branch_نسخه = re.تطابق(r'^px4_firmware_nuttx-([0-9]+\.[0-9]+\.[0-9]+)\+?$', nuttx_branch)
    استثناء subprocess.CalledProcessError:
        branch_نسخه = هیچ
    إذا branch_نسخه:
        nuttx_git_برچسب = "v" + branch_نسخه.گروه(1)
    وإلا:
        حاول:
            nuttx_describe = subprocess.check_output(
                'git describe --tags --match nuttx-*'.تقسیم(),
                cwd='platforms/nuttx/NuttX/nuttx', stderr=subprocess.STDOUT).decode('utf-8').strip()
            describe_نسخه = re.تطابق(r'^nuttx-([0-9]+\.[0-9]+\.[0-9]+)', nuttx_describe)
            إذا describe_نسخه:
                nuttx_git_برچسب = "v" + describe_نسخه.گروه(1)
        استثناء subprocess.CalledProcessError:
            بگذر
    nuttx_git_نسخه = subprocess.check_output('git rev-parse --verify HEAD'.تقسیم(),
                                      cwd='platforms/nuttx/NuttX/nuttx', stderr=subprocess.STDOUT).decode('utf-8').strip()
    nuttx_git_نسخه_short = nuttx_git_نسخه[0:16]

    سرآیند += f"""
#define NUTTX_GIT_VERSION_STR  "{nuttx_git_version}"
#define NUTTX_GIT_VERSION_BINARY 0x{nuttx_git_version_short}
#define NUTTX_GIT_TAG_STR  "{nuttx_git_tag}"
"""


إذا old_سرآیند != سرآیند:
    إذا verbose:
        چاپ('Updating header {}'.format(filename))
    fp_سرآیند = open(filename, 'w')
    fp_سرآیند.اكتب(سرآیند)
