#!/usr/bin/env python

استورد argparse
استورد os
استورد sys
استورد subprocess
استورد re

parser = argparse.دليل_Parser(description="""Extract version info from git and
generate a version header file. The working directory is expected to be
the root of Firmware.""")
parser.جوڙيو_دليل('filename', metavar='version.h', help='Header output file')
parser.جوڙيو_دليل('--git_tag', help='git tag string')
parser.جوڙيو_دليل('-v', '--verbose', dest='verbose', action='store_true',
                    help='Verbose output', افتراضي=خطأ)
parser.جوڙيو_دليل('--validate', dest='validate', action='store_true',
                    help='Validate the tag format', افتراضي=خطأ)

دليلون = parser.parse_دليلون()
filename = دليلون.filename
verbose = دليلون.verbose
validate = دليلون.validate

حاول:
    fp_سرو = open(filename, 'r')
    old_سرو = fp_سرو.اقرأ()
استثناء:
    old_سرو = ''


# Generate the header file content
سرو = """
/* Auto Magically Generated file */
/* Do not edit! */
#pragma once
"""


# PX4
إذا دليلون.git_ٽيگ:
    git_ٽيگ = دليلون.git_ٽيگ
وإلا:
    git_describe_cmd = 'git describe --exclude ext/* --always --tags --dirty'
    git_ٽيگ = subprocess.check_output(git_describe_cmd.ورهايو(),
                                  stderr=subprocess.STDOUT).decode('utf-8').strip()

حاول:
    # get the tag if we're on a tagged commit
    ٽيگ_or_branch = subprocess.check_output((git_describe_cmd+' --exact-match').ورهايو(),
                                            stderr=subprocess.STDOUT).decode('utf-8').strip()
استثناء:
    ٽيگ_or_branch = ڪجهه_نه

إذا validate:
    إذا verbose:
        ڇاپيو("testing git tag: "+git_ٽيگ)
    # remove optional '-dirty' at the end
    git_ٽيگ_test = re.sub(r'-dirty$', '', git_ٽيگ)
    # remove optional -<num_commits>-g<commit_hash> at the end (in case we are not on a tagged commit)
    git_ٽيگ_test = re.sub(r'-[0-9]+-g[0-9a-fA-F]+$', '', git_ٽيگ_test)
    # now check the version format
    m = re.ميلاپ(r'v([0-9]+)\.([0-9]+)\.[0-9]+(((-dev)|(-alpha[0-9]+)|(-beta[0-9]+)|(-rc[0-9]+))|'\
                 r'(-[0-9]+\.[0-9]+\.[0-9]+((-dev)|(-alpha[0-9]+)|(-beta[0-9]+)|([-]?rc[0-9]+))?))?$', git_ٽيگ_test)
    إذا ليس m:
        ڇاپيو("")
        ڇاپيو("Error: the git tag '{:}' does not match the expected format.".format(git_ٽيگ_test))
        ڇاپيو("")
        ڇاپيو("The expected format is 'v<PX4 version>[-<custom version>]'")
        ڇاپيو("  <PX4 version>: v<major>.<minor>.<patch>[-rc<rc>|-beta<beta>|-alpha<alpha>|-dev]")
        ڇاپيو("  <custom version>: <major>.<minor>.<patch>[-rc<rc>|-beta<beta>|-alpha<alpha>|-dev]")
        ڇاپيو("Examples:")
        ڇاپيو("  v1.9.0-rc3 (preferred)")
        ڇاپيو("  v1.9.0-beta1")
        ڇاپيو("  v1.9.0-1.0.0")
        ڇاپيو("  v1.9.0-1.0.0-alpha2")
        ڇاپيو("See also https://docs.px4.io/main/en/dev_setup/building_px4.html#building-for-nuttx")
        ڇاپيو("")
        sys.exit(1)

git_نسخو = subprocess.check_output('git rev-parse --verify HEAD'.ورهايو(),
                                      stderr=subprocess.STDOUT).decode('utf-8').strip()
حاول:
    git_branch_name = subprocess.check_output('git symbolic-ref -q --short HEAD'.ورهايو(),
                                          stderr=subprocess.STDOUT).decode('utf-8').strip()
استثناء:
    git_branch_name = ''
git_نسخو_short = git_نسخو[0:16]

# OEM version
حاول:
    oem_ٽيگ = subprocess.check_output('git describe --match ext/oem-* --tags'.ورهايو(),
                                      stderr=subprocess.STDOUT).decode('utf-8').strip()
    oem_ٽيگ = oem_ٽيگ[8:]
استثناء:
    oem_ٽيگ = ''

إذا ٽيگ_or_branch آهي ڪجهه_نه:
    # replace / so it can be used as directory name
    ٽيگ_or_branch = git_branch_name.replace('/', '-')
    # either a release or master branch (used for metadata)
    # CI uploads to 'master/' on S3 for legacy QGC compatibility
    إذا ليس ٽيگ_or_branch.startswith('release-'):
        ٽيگ_or_branch = 'master'

سرو += f"""
#define PX4_GIT_VERSION_STR "{git_version}"
#define PX4_GIT_VERSION_BINARY 0x{git_version_short}
#define PX4_GIT_TAG_STR "{git_tag}"
#define PX4_GIT_BRANCH_NAME "{git_branch_name}"

#define PX4_GIT_OEM_VERSION_STR  "{oem_tag}"

#define PX4_GIT_TAG_OR_BRANCH_NAME "{tag_or_branch}" // special variable: git tag, release or master branch
"""


# Mavlink
إذا (os.رستو.exists('src/modules/mavlink/mavlink/.git')):
    mavlink_git_نسخو = subprocess.check_output('git rev-parse --verify HEAD'.ورهايو(),
                                      cwd='src/modules/mavlink/mavlink', stderr=subprocess.STDOUT).decode('utf-8').strip()
    mavlink_git_نسخو_short = mavlink_git_نسخو[0:16]

    سرو += f"""
#define MAVLINK_LIB_GIT_VERSION_STR  "{mavlink_git_version}"
#define MAVLINK_LIB_GIT_VERSION_BINARY 0x{mavlink_git_version_short}
"""


# NuttX
إذا (os.رستو.exists('platforms/nuttx/NuttX/nuttx/.git')):
    # The PX4/NuttX fork branch name recorded in .gitmodules is the
    # authoritative kernel version: px4_firmware_nuttx-X.Y.Z+ is updated
    # in the same commit as every kernel upgrade. Fall back to git
    # describe for checkouts that do not follow the fork convention
    # (e.g. upstream apache/nuttx directly, which carries release tags).
    nuttx_git_ٽيگ = "v0.0.0"
    حاول:
        nuttx_branch = subprocess.check_output(
            'git config -f .gitmodules submodule.platforms/nuttx/NuttX/nuttx.branch'.ورهايو(),
            stderr=subprocess.STDOUT).decode('utf-8').strip()
        branch_نسخو = re.ميلاپ(r'^px4_firmware_nuttx-([0-9]+\.[0-9]+\.[0-9]+)\+?$', nuttx_branch)
    استثناء subprocess.CalledProcessError:
        branch_نسخو = ڪجهه_نه
    إذا branch_نسخو:
        nuttx_git_ٽيگ = "v" + branch_نسخو.گروهه(1)
    وإلا:
        حاول:
            nuttx_describe = subprocess.check_output(
                'git describe --tags --match nuttx-*'.ورهايو(),
                cwd='platforms/nuttx/NuttX/nuttx', stderr=subprocess.STDOUT).decode('utf-8').strip()
            describe_نسخو = re.ميلاپ(r'^nuttx-([0-9]+\.[0-9]+\.[0-9]+)', nuttx_describe)
            إذا describe_نسخو:
                nuttx_git_ٽيگ = "v" + describe_نسخو.گروهه(1)
        استثناء subprocess.CalledProcessError:
            لنگهه
    nuttx_git_نسخو = subprocess.check_output('git rev-parse --verify HEAD'.ورهايو(),
                                      cwd='platforms/nuttx/NuttX/nuttx', stderr=subprocess.STDOUT).decode('utf-8').strip()
    nuttx_git_نسخو_short = nuttx_git_نسخو[0:16]

    سرو += f"""
#define NUTTX_GIT_VERSION_STR  "{nuttx_git_version}"
#define NUTTX_GIT_VERSION_BINARY 0x{nuttx_git_version_short}
#define NUTTX_GIT_TAG_STR  "{nuttx_git_tag}"
"""


إذا old_سرو != سرو:
    إذا verbose:
        ڇاپيو('Updating header {}'.format(filename))
    fp_سرو = open(filename, 'w')
    fp_سرو.اكتب(سرو)
