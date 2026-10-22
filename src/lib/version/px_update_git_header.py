#!/usr/bin/env python

استورد argparse
استورد os
استورد sys
استورد subprocess
استورد re

parser = argparse.دلیل_Parser(description="""Extract version info from git and
generate a version header file. The working directory is expected to be
the root of Firmware.""")
parser.جمع_کړه_دلیل('filename', metavar='version.h', help='Header output file')
parser.جمع_کړه_دلیل('--git_tag', help='git tag string')
parser.جمع_کړه_دلیل('-v', '--verbose', dest='verbose', action='store_true',
                    help='Verbose output', افتراضي=خطأ)
parser.جمع_کړه_دلیل('--validate', dest='validate', action='store_true',
                    help='Validate the tag format', افتراضي=خطأ)

دلیلونه = parser.parse_دلیلونه()
filename = دلیلونه.filename
verbose = دلیلونه.verbose
validate = دلیلونه.validate

حاول:
    fp_سرلیک = open(filename, 'r')
    old_سرلیک = fp_سرلیک.اقرأ()
استثناء:
    old_سرلیک = ''


# Generate the header file content
سرلیک = """
/* Auto Magically Generated file */
/* Do not edit! */
#pragma once
"""


# PX4
إذا دلیلونه.git_نښان:
    git_نښان = دلیلونه.git_نښان
وإلا:
    git_describe_cmd = 'git describe --exclude ext/* --always --tags --dirty'
    git_نښان = subprocess.check_output(git_describe_cmd.وېشل(),
                                  stderr=subprocess.STDOUT).decode('utf-8').strip()

حاول:
    # get the tag if we're on a tagged commit
    نښان_or_branch = subprocess.check_output((git_describe_cmd+' --exact-match').وېشل(),
                                            stderr=subprocess.STDOUT).decode('utf-8').strip()
استثناء:
    نښان_or_branch = هیڅ

إذا validate:
    إذا verbose:
        چاپ("testing git tag: "+git_نښان)
    # remove optional '-dirty' at the end
    git_نښان_test = re.sub(r'-dirty$', '', git_نښان)
    # remove optional -<num_commits>-g<commit_hash> at the end (in case we are not on a tagged commit)
    git_نښان_test = re.sub(r'-[0-9]+-g[0-9a-fA-F]+$', '', git_نښان_test)
    # now check the version format
    m = re.سمون(r'v([0-9]+)\.([0-9]+)\.[0-9]+(((-dev)|(-alpha[0-9]+)|(-beta[0-9]+)|(-rc[0-9]+))|'\
                 r'(-[0-9]+\.[0-9]+\.[0-9]+((-dev)|(-alpha[0-9]+)|(-beta[0-9]+)|([-]?rc[0-9]+))?))?$', git_نښان_test)
    إذا ليس m:
        چاپ("")
        چاپ("Error: the git tag '{:}' does not match the expected format.".format(git_نښان_test))
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

git_نسخه = subprocess.check_output('git rev-parse --verify HEAD'.وېشل(),
                                      stderr=subprocess.STDOUT).decode('utf-8').strip()
حاول:
    git_branch_name = subprocess.check_output('git symbolic-ref -q --short HEAD'.وېشل(),
                                          stderr=subprocess.STDOUT).decode('utf-8').strip()
استثناء:
    git_branch_name = ''
git_نسخه_short = git_نسخه[0:16]

# OEM version
حاول:
    oem_نښان = subprocess.check_output('git describe --match ext/oem-* --tags'.وېشل(),
                                      stderr=subprocess.STDOUT).decode('utf-8').strip()
    oem_نښان = oem_نښان[8:]
استثناء:
    oem_نښان = ''

إذا نښان_or_branch دی هیڅ:
    # replace / so it can be used as directory name
    نښان_or_branch = git_branch_name.replace('/', '-')
    # either a release or master branch (used for metadata)
    # CI uploads to 'master/' on S3 for legacy QGC compatibility
    إذا ليس نښان_or_branch.startswith('release-'):
        نښان_or_branch = 'master'

سرلیک += f"""
#define PX4_GIT_VERSION_STR "{git_version}"
#define PX4_GIT_VERSION_BINARY 0x{git_version_short}
#define PX4_GIT_TAG_STR "{git_tag}"
#define PX4_GIT_BRANCH_NAME "{git_branch_name}"

#define PX4_GIT_OEM_VERSION_STR  "{oem_tag}"

#define PX4_GIT_TAG_OR_BRANCH_NAME "{tag_or_branch}" // special variable: git tag, release or master branch
"""


# Mavlink
إذا (os.لار.exists('src/modules/mavlink/mavlink/.git')):
    mavlink_git_نسخه = subprocess.check_output('git rev-parse --verify HEAD'.وېشل(),
                                      cwd='src/modules/mavlink/mavlink', stderr=subprocess.STDOUT).decode('utf-8').strip()
    mavlink_git_نسخه_short = mavlink_git_نسخه[0:16]

    سرلیک += f"""
#define MAVLINK_LIB_GIT_VERSION_STR  "{mavlink_git_version}"
#define MAVLINK_LIB_GIT_VERSION_BINARY 0x{mavlink_git_version_short}
"""


# NuttX
إذا (os.لار.exists('platforms/nuttx/NuttX/nuttx/.git')):
    # The PX4/NuttX fork branch name recorded in .gitmodules is the
    # authoritative kernel version: px4_firmware_nuttx-X.Y.Z+ is updated
    # in the same commit as every kernel upgrade. Fall back to git
    # describe for checkouts that do not follow the fork convention
    # (e.g. upstream apache/nuttx directly, which carries release tags).
    nuttx_git_نښان = "v0.0.0"
    حاول:
        nuttx_branch = subprocess.check_output(
            'git config -f .gitmodules submodule.platforms/nuttx/NuttX/nuttx.branch'.وېشل(),
            stderr=subprocess.STDOUT).decode('utf-8').strip()
        branch_نسخه = re.سمون(r'^px4_firmware_nuttx-([0-9]+\.[0-9]+\.[0-9]+)\+?$', nuttx_branch)
    استثناء subprocess.CalledProcessError:
        branch_نسخه = هیڅ
    إذا branch_نسخه:
        nuttx_git_نښان = "v" + branch_نسخه.ډله(1)
    وإلا:
        حاول:
            nuttx_describe = subprocess.check_output(
                'git describe --tags --match nuttx-*'.وېشل(),
                cwd='platforms/nuttx/NuttX/nuttx', stderr=subprocess.STDOUT).decode('utf-8').strip()
            describe_نسخه = re.سمون(r'^nuttx-([0-9]+\.[0-9]+\.[0-9]+)', nuttx_describe)
            إذا describe_نسخه:
                nuttx_git_نښان = "v" + describe_نسخه.ډله(1)
        استثناء subprocess.CalledProcessError:
            تیرشه
    nuttx_git_نسخه = subprocess.check_output('git rev-parse --verify HEAD'.وېشل(),
                                      cwd='platforms/nuttx/NuttX/nuttx', stderr=subprocess.STDOUT).decode('utf-8').strip()
    nuttx_git_نسخه_short = nuttx_git_نسخه[0:16]

    سرلیک += f"""
#define NUTTX_GIT_VERSION_STR  "{nuttx_git_version}"
#define NUTTX_GIT_VERSION_BINARY 0x{nuttx_git_version_short}
#define NUTTX_GIT_TAG_STR  "{nuttx_git_tag}"
"""


إذا old_سرلیک != سرلیک:
    إذا verbose:
        چاپ('Updating header {}'.format(filename))
    fp_سرلیک = open(filename, 'w')
    fp_سرلیک.اكتب(سرلیک)
