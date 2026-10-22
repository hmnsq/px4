#!/usr/bin/env python

ಆಮದು argparse
ಆಮದು os
ಆಮದು sys
ಆಮದು subprocess
ಆಮದು re

parser = argparse.ವಾದ_Parser(description="""Extract version info from git and
generate a version header file. The working directory is expected to be
the root of Firmware.""")
parser.ಕೂಡಿಸು_ವಾದ('filename', metavar='version.h', help='Header output file')
parser.ಕೂಡಿಸು_ವಾದ('--git_tag', help='git tag string')
parser.ಕೂಡಿಸು_ವಾದ('-v', '--verbose', dest='verbose', action='store_true',
                    help='Verbose output', ಪೂರ್ವನಿಯೋಜಿತ=ಸುಳ್ಳು)
parser.ಕೂಡಿಸು_ವಾದ('--validate', dest='validate', action='store_true',
                    help='Validate the tag format', ಪೂರ್ವನಿಯೋಜಿತ=ಸುಳ್ಳು)

ವಾದಗಳು = parser.parse_ವಾದಗಳು()
filename = ವಾದಗಳು.filename
verbose = ವಾದಗಳು.verbose
validate = ವಾದಗಳು.validate

ಪ್ರಯತ್ನಿಸು:
    fp_ಶೀರ್ಷಿಕೆ = open(filename, 'r')
    old_ಶೀರ್ಷಿಕೆ = fp_ಶೀರ್ಷಿಕೆ.ಪಠ()
ಅಪವಾದ:
    old_ಶೀರ್ಷಿಕೆ = ''


# Generate the header file content
ಶೀರ್ಷಿಕೆ = """
/* Auto Magically Generated file */
/* Do not edit! */
#pragma once
"""


# PX4
ಆದರೆ ವಾದಗಳು.git_ಟ್ಯಾಗ್:
    git_ಟ್ಯಾಗ್ = ವಾದಗಳು.git_ಟ್ಯಾಗ್
ಇಲ್ಲದಿದ್ದರೆ:
    git_describe_cmd = 'git describe --exclude ext/* --always --tags --dirty'
    git_ಟ್ಯಾಗ್ = subprocess.check_output(git_describe_cmd.ವಿಭಜಿಸು(),
                                  stderr=subprocess.STDOUT).decode('utf-8').strip()

ಪ್ರಯತ್ನಿಸು:
    # get the tag if we're on a tagged commit
    ಟ್ಯಾಗ್_or_branch = subprocess.check_output((git_describe_cmd+' --exact-match').ವಿಭಜಿಸು(),
                                            stderr=subprocess.STDOUT).decode('utf-8').strip()
ಅಪವಾದ:
    ಟ್ಯಾಗ್_or_branch = ಏನೂಇಲ್ಲ

ಆದರೆ validate:
    ಆದರೆ verbose:
        ಮುದ್ರಿಸು("testing git tag: "+git_ಟ್ಯಾಗ್)
    # remove optional '-dirty' at the end
    git_ಟ್ಯಾಗ್_test = re.sub(r'-dirty$', '', git_ಟ್ಯಾಗ್)
    # remove optional -<num_commits>-g<commit_hash> at the end (in case we are not on a tagged commit)
    git_ಟ್ಯಾಗ್_test = re.sub(r'-[0-9]+-g[0-9a-fA-F]+$', '', git_ಟ್ಯಾಗ್_test)
    # now check the version format
    m = re.ಹೊಂದಿಕೆ(r'v([0-9]+)\.([0-9]+)\.[0-9]+(((-dev)|(-alpha[0-9]+)|(-beta[0-9]+)|(-rc[0-9]+))|'\
                 r'(-[0-9]+\.[0-9]+\.[0-9]+((-dev)|(-alpha[0-9]+)|(-beta[0-9]+)|([-]?rc[0-9]+))?))?$', git_ಟ್ಯಾಗ್_test)
    ಆದರೆ ಅಲ್ಲ m:
        ಮುದ್ರಿಸು("")
        ಮುದ್ರಿಸು("Error: the git tag '{:}' does not match the expected format.".format(git_ಟ್ಯಾಗ್_test))
        ಮುದ್ರಿಸು("")
        ಮುದ್ರಿಸು("The expected format is 'v<PX4 version>[-<custom version>]'")
        ಮುದ್ರಿಸು("  <PX4 version>: v<major>.<minor>.<patch>[-rc<rc>|-beta<beta>|-alpha<alpha>|-dev]")
        ಮುದ್ರಿಸು("  <custom version>: <major>.<minor>.<patch>[-rc<rc>|-beta<beta>|-alpha<alpha>|-dev]")
        ಮುದ್ರಿಸು("Examples:")
        ಮುದ್ರಿಸು("  v1.9.0-rc3 (preferred)")
        ಮುದ್ರಿಸು("  v1.9.0-beta1")
        ಮುದ್ರಿಸು("  v1.9.0-1.0.0")
        ಮುದ್ರಿಸು("  v1.9.0-1.0.0-alpha2")
        ಮುದ್ರಿಸು("See also https://docs.px4.io/main/en/dev_setup/building_px4.html#building-for-nuttx")
        ಮುದ್ರಿಸು("")
        sys.exit(1)

git_ಆವೃತ್ತಿ = subprocess.check_output('git rev-parse --verify HEAD'.ವಿಭಜಿಸು(),
                                      stderr=subprocess.STDOUT).decode('utf-8').strip()
ಪ್ರಯತ್ನಿಸು:
    git_branch_name = subprocess.check_output('git symbolic-ref -q --short HEAD'.ವಿಭಜಿಸು(),
                                          stderr=subprocess.STDOUT).decode('utf-8').strip()
ಅಪವಾದ:
    git_branch_name = ''
git_ಆವೃತ್ತಿ_short = git_ಆವೃತ್ತಿ[0:16]

# OEM version
ಪ್ರಯತ್ನಿಸು:
    oem_ಟ್ಯಾಗ್ = subprocess.check_output('git describe --match ext/oem-* --tags'.ವಿಭಜಿಸು(),
                                      stderr=subprocess.STDOUT).decode('utf-8').strip()
    oem_ಟ್ಯಾಗ್ = oem_ಟ್ಯಾಗ್[8:]
ಅಪವಾದ:
    oem_ಟ್ಯಾಗ್ = ''

ಆದರೆ ಟ್ಯಾಗ್_or_branch ಆಗಿರುವ ಏನೂಇಲ್ಲ:
    # replace / so it can be used as directory name
    ಟ್ಯಾಗ್_or_branch = git_branch_name.replace('/', '-')
    # either a release or master branch (used for metadata)
    # CI uploads to 'master/' on S3 for legacy QGC compatibility
    ಆದರೆ ಅಲ್ಲ ಟ್ಯಾಗ್_or_branch.startswith('release-'):
        ಟ್ಯಾಗ್_or_branch = 'master'

ಶೀರ್ಷಿಕೆ += f"""
#define PX4_GIT_VERSION_STR "{git_version}"
#define PX4_GIT_VERSION_BINARY 0x{git_version_short}
#define PX4_GIT_TAG_STR "{git_tag}"
#define PX4_GIT_BRANCH_NAME "{git_branch_name}"

#define PX4_GIT_OEM_VERSION_STR  "{oem_tag}"

#define PX4_GIT_TAG_OR_BRANCH_NAME "{tag_or_branch}" // special variable: git tag, release or master branch
"""


# Mavlink
ಆದರೆ (os.ಮಾರ್ಗ.exists('src/modules/mavlink/mavlink/.git')):
    mavlink_git_ಆವೃತ್ತಿ = subprocess.check_output('git rev-parse --verify HEAD'.ವಿಭಜಿಸು(),
                                      cwd='src/modules/mavlink/mavlink', stderr=subprocess.STDOUT).decode('utf-8').strip()
    mavlink_git_ಆವೃತ್ತಿ_short = mavlink_git_ಆವೃತ್ತಿ[0:16]

    ಶೀರ್ಷಿಕೆ += f"""
#define MAVLINK_LIB_GIT_VERSION_STR  "{mavlink_git_version}"
#define MAVLINK_LIB_GIT_VERSION_BINARY 0x{mavlink_git_version_short}
"""


# NuttX
ಆದರೆ (os.ಮಾರ್ಗ.exists('platforms/nuttx/NuttX/nuttx/.git')):
    # The PX4/NuttX fork branch name recorded in .gitmodules is the
    # authoritative kernel version: px4_firmware_nuttx-X.Y.Z+ is updated
    # in the same commit as every kernel upgrade. Fall back to git
    # describe for checkouts that do not follow the fork convention
    # (e.g. upstream apache/nuttx directly, which carries release tags).
    nuttx_git_ಟ್ಯಾಗ್ = "v0.0.0"
    ಪ್ರಯತ್ನಿಸು:
        nuttx_branch = subprocess.check_output(
            'git config -f .gitmodules submodule.platforms/nuttx/NuttX/nuttx.branch'.ವಿಭಜಿಸು(),
            stderr=subprocess.STDOUT).decode('utf-8').strip()
        branch_ಆವೃತ್ತಿ = re.ಹೊಂದಿಕೆ(r'^px4_firmware_nuttx-([0-9]+\.[0-9]+\.[0-9]+)\+?$', nuttx_branch)
    ಅಪವಾದ subprocess.CalledProcessError:
        branch_ಆವೃತ್ತಿ = ಏನೂಇಲ್ಲ
    ಆದರೆ branch_ಆವೃತ್ತಿ:
        nuttx_git_ಟ್ಯಾಗ್ = "v" + branch_ಆವೃತ್ತಿ.ಗುಂಪು(1)
    ಇಲ್ಲದಿದ್ದರೆ:
        ಪ್ರಯತ್ನಿಸು:
            nuttx_describe = subprocess.check_output(
                'git describe --tags --match nuttx-*'.ವಿಭಜಿಸು(),
                cwd='platforms/nuttx/NuttX/nuttx', stderr=subprocess.STDOUT).decode('utf-8').strip()
            describe_ಆವೃತ್ತಿ = re.ಹೊಂದಿಕೆ(r'^nuttx-([0-9]+\.[0-9]+\.[0-9]+)', nuttx_describe)
            ಆದರೆ describe_ಆವೃತ್ತಿ:
                nuttx_git_ಟ್ಯಾಗ್ = "v" + describe_ಆವೃತ್ತಿ.ಗುಂಪು(1)
        ಅಪವಾದ subprocess.CalledProcessError:
            ದಾಟು
    nuttx_git_ಆವೃತ್ತಿ = subprocess.check_output('git rev-parse --verify HEAD'.ವಿಭಜಿಸು(),
                                      cwd='platforms/nuttx/NuttX/nuttx', stderr=subprocess.STDOUT).decode('utf-8').strip()
    nuttx_git_ಆವೃತ್ತಿ_short = nuttx_git_ಆವೃತ್ತಿ[0:16]

    ಶೀರ್ಷಿಕೆ += f"""
#define NUTTX_GIT_VERSION_STR  "{nuttx_git_version}"
#define NUTTX_GIT_VERSION_BINARY 0x{nuttx_git_version_short}
#define NUTTX_GIT_TAG_STR  "{nuttx_git_tag}"
"""


ಆದರೆ old_ಶೀರ್ಷಿಕೆ != ಶೀರ್ಷಿಕೆ:
    ಆದರೆ verbose:
        ಮುದ್ರಿಸು('Updating header {}'.format(filename))
    fp_ಶೀರ್ಷಿಕೆ = open(filename, 'w')
    fp_ಶೀರ್ಷಿಕೆ.ಲಿಖ(ಶೀರ್ಷಿಕೆ)
