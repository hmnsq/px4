#!/usr/bin/env python

استورد argparse
استورد os
استورد sys
استورد subprocess
استورد re

parser = argparse.دلیل_Parser(description="""Extract version info from git and
generate a version header file. The working directory is expected to be
the root of Firmware.""")
parser.جوڑو_دلیل('filename', metavar='version.h', help='Header output file')
parser.جوڑو_دلیل('--git_tag', help='git tag string')
parser.جوڑو_دلیل('-v', '--verbose', dest='verbose', action='store_true',
                    help='Verbose output', افتراضي=خطأ)
parser.جوڑو_دلیل('--validate', dest='validate', action='store_true',
                    help='Validate the tag format', افتراضي=خطأ)

دلیلاں = parser.parse_دلیلاں()
filename = دلیلاں.filename
verbose = دلیلاں.verbose
validate = دلیلاں.validate

حاول:
    fp_سرخی = open(filename, 'r')
    old_سرخی = fp_سرخی.اقرأ()
استثناء:
    old_سرخی = ''


# Generate the header file content
سرخی = """
/* Auto Magically Generated file */
/* Do not edit! */
#pragma once
"""


# PX4
إذا دلیلاں.git_ٹیگ:
    git_ٹیگ = دلیلاں.git_ٹیگ
وإلا:
    git_describe_cmd = 'git describe --exclude ext/* --always --tags --dirty'
    git_ٹیگ = subprocess.check_output(git_describe_cmd.ونڈو(),
                                  stderr=subprocess.STDOUT).decode('utf-8').strip()

حاول:
    # get the tag if we're on a tagged commit
    ٹیگ_or_branch = subprocess.check_output((git_describe_cmd+' --exact-match').ونڈو(),
                                            stderr=subprocess.STDOUT).decode('utf-8').strip()
استثناء:
    ٹیگ_or_branch = کجھ_نئیں

إذا validate:
    إذا verbose:
        چھاپو("testing git tag: "+git_ٹیگ)
    # remove optional '-dirty' at the end
    git_ٹیگ_test = re.sub(r'-dirty$', '', git_ٹیگ)
    # remove optional -<num_commits>-g<commit_hash> at the end (in case we are not on a tagged commit)
    git_ٹیگ_test = re.sub(r'-[0-9]+-g[0-9a-fA-F]+$', '', git_ٹیگ_test)
    # now check the version format
    m = re.میل(r'v([0-9]+)\.([0-9]+)\.[0-9]+(((-dev)|(-alpha[0-9]+)|(-beta[0-9]+)|(-rc[0-9]+))|'\
                 r'(-[0-9]+\.[0-9]+\.[0-9]+((-dev)|(-alpha[0-9]+)|(-beta[0-9]+)|([-]?rc[0-9]+))?))?$', git_ٹیگ_test)
    إذا ليس m:
        چھاپو("")
        چھاپو("Error: the git tag '{:}' does not match the expected format.".format(git_ٹیگ_test))
        چھاپو("")
        چھاپو("The expected format is 'v<PX4 version>[-<custom version>]'")
        چھاپو("  <PX4 version>: v<major>.<minor>.<patch>[-rc<rc>|-beta<beta>|-alpha<alpha>|-dev]")
        چھاپو("  <custom version>: <major>.<minor>.<patch>[-rc<rc>|-beta<beta>|-alpha<alpha>|-dev]")
        چھاپو("Examples:")
        چھاپو("  v1.9.0-rc3 (preferred)")
        چھاپو("  v1.9.0-beta1")
        چھاپو("  v1.9.0-1.0.0")
        چھاپو("  v1.9.0-1.0.0-alpha2")
        چھاپو("See also https://docs.px4.io/main/en/dev_setup/building_px4.html#building-for-nuttx")
        چھاپو("")
        sys.exit(1)

git_نسخہ = subprocess.check_output('git rev-parse --verify HEAD'.ونڈو(),
                                      stderr=subprocess.STDOUT).decode('utf-8').strip()
حاول:
    git_branch_name = subprocess.check_output('git symbolic-ref -q --short HEAD'.ونڈو(),
                                          stderr=subprocess.STDOUT).decode('utf-8').strip()
استثناء:
    git_branch_name = ''
git_نسخہ_short = git_نسخہ[0:16]

# OEM version
حاول:
    oem_ٹیگ = subprocess.check_output('git describe --match ext/oem-* --tags'.ونڈو(),
                                      stderr=subprocess.STDOUT).decode('utf-8').strip()
    oem_ٹیگ = oem_ٹیگ[8:]
استثناء:
    oem_ٹیگ = ''

إذا ٹیگ_or_branch اے کجھ_نئیں:
    # replace / so it can be used as directory name
    ٹیگ_or_branch = git_branch_name.replace('/', '-')
    # either a release or master branch (used for metadata)
    # CI uploads to 'master/' on S3 for legacy QGC compatibility
    إذا ليس ٹیگ_or_branch.startswith('release-'):
        ٹیگ_or_branch = 'master'

سرخی += f"""
#define PX4_GIT_VERSION_STR "{git_version}"
#define PX4_GIT_VERSION_BINARY 0x{git_version_short}
#define PX4_GIT_TAG_STR "{git_tag}"
#define PX4_GIT_BRANCH_NAME "{git_branch_name}"

#define PX4_GIT_OEM_VERSION_STR  "{oem_tag}"

#define PX4_GIT_TAG_OR_BRANCH_NAME "{tag_or_branch}" // special variable: git tag, release or master branch
"""


# Mavlink
إذا (os.رستہ.exists('src/modules/mavlink/mavlink/.git')):
    mavlink_git_نسخہ = subprocess.check_output('git rev-parse --verify HEAD'.ونڈو(),
                                      cwd='src/modules/mavlink/mavlink', stderr=subprocess.STDOUT).decode('utf-8').strip()
    mavlink_git_نسخہ_short = mavlink_git_نسخہ[0:16]

    سرخی += f"""
#define MAVLINK_LIB_GIT_VERSION_STR  "{mavlink_git_version}"
#define MAVLINK_LIB_GIT_VERSION_BINARY 0x{mavlink_git_version_short}
"""


# NuttX
إذا (os.رستہ.exists('platforms/nuttx/NuttX/nuttx/.git')):
    # The PX4/NuttX fork branch name recorded in .gitmodules is the
    # authoritative kernel version: px4_firmware_nuttx-X.Y.Z+ is updated
    # in the same commit as every kernel upgrade. Fall back to git
    # describe for checkouts that do not follow the fork convention
    # (e.g. upstream apache/nuttx directly, which carries release tags).
    nuttx_git_ٹیگ = "v0.0.0"
    حاول:
        nuttx_branch = subprocess.check_output(
            'git config -f .gitmodules submodule.platforms/nuttx/NuttX/nuttx.branch'.ونڈو(),
            stderr=subprocess.STDOUT).decode('utf-8').strip()
        branch_نسخہ = re.میل(r'^px4_firmware_nuttx-([0-9]+\.[0-9]+\.[0-9]+)\+?$', nuttx_branch)
    استثناء subprocess.CalledProcessError:
        branch_نسخہ = کجھ_نئیں
    إذا branch_نسخہ:
        nuttx_git_ٹیگ = "v" + branch_نسخہ.گروہ(1)
    وإلا:
        حاول:
            nuttx_describe = subprocess.check_output(
                'git describe --tags --match nuttx-*'.ونڈو(),
                cwd='platforms/nuttx/NuttX/nuttx', stderr=subprocess.STDOUT).decode('utf-8').strip()
            describe_نسخہ = re.میل(r'^nuttx-([0-9]+\.[0-9]+\.[0-9]+)', nuttx_describe)
            إذا describe_نسخہ:
                nuttx_git_ٹیگ = "v" + describe_نسخہ.گروہ(1)
        استثناء subprocess.CalledProcessError:
            لنگھو
    nuttx_git_نسخہ = subprocess.check_output('git rev-parse --verify HEAD'.ونڈو(),
                                      cwd='platforms/nuttx/NuttX/nuttx', stderr=subprocess.STDOUT).decode('utf-8').strip()
    nuttx_git_نسخہ_short = nuttx_git_نسخہ[0:16]

    سرخی += f"""
#define NUTTX_GIT_VERSION_STR  "{nuttx_git_version}"
#define NUTTX_GIT_VERSION_BINARY 0x{nuttx_git_version_short}
#define NUTTX_GIT_TAG_STR  "{nuttx_git_tag}"
"""


إذا old_سرخی != سرخی:
    إذا verbose:
        چھاپو('Updating header {}'.format(filename))
    fp_سرخی = open(filename, 'w')
    fp_سرخی.اكتب(سرخی)
