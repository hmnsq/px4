#!/usr/bin/env python

ਆਯਾਤ argparse
ਆਯਾਤ os
ਆਯਾਤ sys
ਆਯਾਤ subprocess
ਆਯਾਤ re

parser = argparse.ਦਲੀਲ_Parser(description="""Extract version info from git and
generate a version header file. The working directory is expected to be
the root of Firmware.""")
parser.ਜੋੜੋ_ਦਲੀਲ('filename', metavar='version.h', help='Header output file')
parser.ਜੋੜੋ_ਦਲੀਲ('--git_tag', help='git tag string')
parser.ਜੋੜੋ_ਦਲੀਲ('-v', '--verbose', dest='verbose', action='store_true',
                    help='Verbose output', ਮੂਲ=ਝੂਠ)
parser.ਜੋੜੋ_ਦਲੀਲ('--validate', dest='validate', action='store_true',
                    help='Validate the tag format', ਮੂਲ=ਝੂਠ)

ਦਲੀਲਾਂ = parser.parse_ਦਲੀਲਾਂ()
filename = ਦਲੀਲਾਂ.filename
verbose = ਦਲੀਲਾਂ.verbose
validate = ਦਲੀਲਾਂ.validate

ਕੋਸ਼ਿਸ਼:
    fp_ਸਿਰਲੇਖ = open(filename, 'r')
    old_ਸਿਰਲੇਖ = fp_ਸਿਰਲੇਖ.ਪਠ()
ਅਪਵਾਦ:
    old_ਸਿਰਲੇਖ = ''


# Generate the header file content
ਸਿਰਲੇਖ = """
/* Auto Magically Generated file */
/* Do not edit! */
#pragma once
"""


# PX4
ਜੇ ਦਲੀਲਾਂ.git_ਟੈਗ:
    git_ਟੈਗ = ਦਲੀਲਾਂ.git_ਟੈਗ
ਨਹੀਂਤਾਂ:
    git_describe_cmd = 'git describe --exclude ext/* --always --tags --dirty'
    git_ਟੈਗ = subprocess.check_output(git_describe_cmd.ਵੰਡੋ(),
                                  stderr=subprocess.STDOUT).decode('utf-8').strip()

ਕੋਸ਼ਿਸ਼:
    # get the tag if we're on a tagged commit
    ਟੈਗ_or_branch = subprocess.check_output((git_describe_cmd+' --exact-match').ਵੰਡੋ(),
                                            stderr=subprocess.STDOUT).decode('utf-8').strip()
ਅਪਵਾਦ:
    ਟੈਗ_or_branch = ਕੁਝ_ਨਹੀਂ

ਜੇ validate:
    ਜੇ verbose:
        ਛਾਪੋ("testing git tag: "+git_ਟੈਗ)
    # remove optional '-dirty' at the end
    git_ਟੈਗ_test = re.sub(r'-dirty$', '', git_ਟੈਗ)
    # remove optional -<num_commits>-g<commit_hash> at the end (in case we are not on a tagged commit)
    git_ਟੈਗ_test = re.sub(r'-[0-9]+-g[0-9a-fA-F]+$', '', git_ਟੈਗ_test)
    # now check the version format
    m = re.ਮੇਲ(r'v([0-9]+)\.([0-9]+)\.[0-9]+(((-dev)|(-alpha[0-9]+)|(-beta[0-9]+)|(-rc[0-9]+))|'\
                 r'(-[0-9]+\.[0-9]+\.[0-9]+((-dev)|(-alpha[0-9]+)|(-beta[0-9]+)|([-]?rc[0-9]+))?))?$', git_ਟੈਗ_test)
    ਜੇ ਨਹੀਂ m:
        ਛਾਪੋ("")
        ਛਾਪੋ("Error: the git tag '{:}' does not match the expected format.".format(git_ਟੈਗ_test))
        ਛਾਪੋ("")
        ਛਾਪੋ("The expected format is 'v<PX4 version>[-<custom version>]'")
        ਛਾਪੋ("  <PX4 version>: v<major>.<minor>.<patch>[-rc<rc>|-beta<beta>|-alpha<alpha>|-dev]")
        ਛਾਪੋ("  <custom version>: <major>.<minor>.<patch>[-rc<rc>|-beta<beta>|-alpha<alpha>|-dev]")
        ਛਾਪੋ("Examples:")
        ਛਾਪੋ("  v1.9.0-rc3 (preferred)")
        ਛਾਪੋ("  v1.9.0-beta1")
        ਛਾਪੋ("  v1.9.0-1.0.0")
        ਛਾਪੋ("  v1.9.0-1.0.0-alpha2")
        ਛਾਪੋ("See also https://docs.px4.io/main/en/dev_setup/building_px4.html#building-for-nuttx")
        ਛਾਪੋ("")
        sys.exit(1)

git_ਸੰਸਕਰਣ = subprocess.check_output('git rev-parse --verify HEAD'.ਵੰਡੋ(),
                                      stderr=subprocess.STDOUT).decode('utf-8').strip()
ਕੋਸ਼ਿਸ਼:
    git_branch_name = subprocess.check_output('git symbolic-ref -q --short HEAD'.ਵੰਡੋ(),
                                          stderr=subprocess.STDOUT).decode('utf-8').strip()
ਅਪਵਾਦ:
    git_branch_name = ''
git_ਸੰਸਕਰਣ_short = git_ਸੰਸਕਰਣ[0:16]

# OEM version
ਕੋਸ਼ਿਸ਼:
    oem_ਟੈਗ = subprocess.check_output('git describe --match ext/oem-* --tags'.ਵੰਡੋ(),
                                      stderr=subprocess.STDOUT).decode('utf-8').strip()
    oem_ਟੈਗ = oem_ਟੈਗ[8:]
ਅਪਵਾਦ:
    oem_ਟੈਗ = ''

ਜੇ ਟੈਗ_or_branch ਹੈ ਕੁਝ_ਨਹੀਂ:
    # replace / so it can be used as directory name
    ਟੈਗ_or_branch = git_branch_name.replace('/', '-')
    # either a release or master branch (used for metadata)
    # CI uploads to 'master/' on S3 for legacy QGC compatibility
    ਜੇ ਨਹੀਂ ਟੈਗ_or_branch.startswith('release-'):
        ਟੈਗ_or_branch = 'master'

ਸਿਰਲੇਖ += f"""
#define PX4_GIT_VERSION_STR "{git_version}"
#define PX4_GIT_VERSION_BINARY 0x{git_version_short}
#define PX4_GIT_TAG_STR "{git_tag}"
#define PX4_GIT_BRANCH_NAME "{git_branch_name}"

#define PX4_GIT_OEM_VERSION_STR  "{oem_tag}"

#define PX4_GIT_TAG_OR_BRANCH_NAME "{tag_or_branch}" // special variable: git tag, release or master branch
"""


# Mavlink
ਜੇ (os.ਰਸਤਾ.exists('src/modules/mavlink/mavlink/.git')):
    mavlink_git_ਸੰਸਕਰਣ = subprocess.check_output('git rev-parse --verify HEAD'.ਵੰਡੋ(),
                                      cwd='src/modules/mavlink/mavlink', stderr=subprocess.STDOUT).decode('utf-8').strip()
    mavlink_git_ਸੰਸਕਰਣ_short = mavlink_git_ਸੰਸਕਰਣ[0:16]

    ਸਿਰਲੇਖ += f"""
#define MAVLINK_LIB_GIT_VERSION_STR  "{mavlink_git_version}"
#define MAVLINK_LIB_GIT_VERSION_BINARY 0x{mavlink_git_version_short}
"""


# NuttX
ਜੇ (os.ਰਸਤਾ.exists('platforms/nuttx/NuttX/nuttx/.git')):
    # The PX4/NuttX fork branch name recorded in .gitmodules is the
    # authoritative kernel version: px4_firmware_nuttx-X.Y.Z+ is updated
    # in the same commit as every kernel upgrade. Fall back to git
    # describe for checkouts that do not follow the fork convention
    # (e.g. upstream apache/nuttx directly, which carries release tags).
    nuttx_git_ਟੈਗ = "v0.0.0"
    ਕੋਸ਼ਿਸ਼:
        nuttx_branch = subprocess.check_output(
            'git config -f .gitmodules submodule.platforms/nuttx/NuttX/nuttx.branch'.ਵੰਡੋ(),
            stderr=subprocess.STDOUT).decode('utf-8').strip()
        branch_ਸੰਸਕਰਣ = re.ਮੇਲ(r'^px4_firmware_nuttx-([0-9]+\.[0-9]+\.[0-9]+)\+?$', nuttx_branch)
    ਅਪਵਾਦ subprocess.CalledProcessError:
        branch_ਸੰਸਕਰਣ = ਕੁਝ_ਨਹੀਂ
    ਜੇ branch_ਸੰਸਕਰਣ:
        nuttx_git_ਟੈਗ = "v" + branch_ਸੰਸਕਰਣ.ਸਮੂਹ(1)
    ਨਹੀਂਤਾਂ:
        ਕੋਸ਼ਿਸ਼:
            nuttx_describe = subprocess.check_output(
                'git describe --tags --match nuttx-*'.ਵੰਡੋ(),
                cwd='platforms/nuttx/NuttX/nuttx', stderr=subprocess.STDOUT).decode('utf-8').strip()
            describe_ਸੰਸਕਰਣ = re.ਮੇਲ(r'^nuttx-([0-9]+\.[0-9]+\.[0-9]+)', nuttx_describe)
            ਜੇ describe_ਸੰਸਕਰਣ:
                nuttx_git_ਟੈਗ = "v" + describe_ਸੰਸਕਰਣ.ਸਮੂਹ(1)
        ਅਪਵਾਦ subprocess.CalledProcessError:
            ਲੰਘੋ
    nuttx_git_ਸੰਸਕਰਣ = subprocess.check_output('git rev-parse --verify HEAD'.ਵੰਡੋ(),
                                      cwd='platforms/nuttx/NuttX/nuttx', stderr=subprocess.STDOUT).decode('utf-8').strip()
    nuttx_git_ਸੰਸਕਰਣ_short = nuttx_git_ਸੰਸਕਰਣ[0:16]

    ਸਿਰਲੇਖ += f"""
#define NUTTX_GIT_VERSION_STR  "{nuttx_git_version}"
#define NUTTX_GIT_VERSION_BINARY 0x{nuttx_git_version_short}
#define NUTTX_GIT_TAG_STR  "{nuttx_git_tag}"
"""


ਜੇ old_ਸਿਰਲੇਖ != ਸਿਰਲੇਖ:
    ਜੇ verbose:
        ਛਾਪੋ('Updating header {}'.format(filename))
    fp_ਸਿਰਲੇਖ = open(filename, 'w')
    fp_ਸਿਰਲੇਖ.ਲਿਖ(ਸਿਰਲੇਖ)
