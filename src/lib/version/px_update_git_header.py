#!/usr/bin/env python

దిగుమతి argparse
దిగుమతి os
దిగుమతి sys
దిగుమతి subprocess
దిగుమతి re

parser = argparse.వాదం_Parser(description="""Extract version info from git and
generate a version header file. The working directory is expected to be
the root of Firmware.""")
parser.కూడు_వాదం('filename', metavar='version.h', help='Header output file')
parser.కూడు_వాదం('--git_tag', help='git tag string')
parser.కూడు_వాదం('-v', '--verbose', dest='verbose', action='store_true',
                    help='Verbose output', అప్రమేయ=అబద్ధం)
parser.కూడు_వాదం('--validate', dest='validate', action='store_true',
                    help='Validate the tag format', అప్రమేయ=అబద్ధం)

వాదనలు = parser.parse_వాదనలు()
filename = వాదనలు.filename
verbose = వాదనలు.verbose
validate = వాదనలు.validate

ప్రయత్నించు:
    fp_శీర్షిక = open(filename, 'r')
    old_శీర్షిక = fp_శీర్షిక.పఠ()
మినహాయింపు:
    old_శీర్షిక = ''


# Generate the header file content
శీర్షిక = """
/* Auto Magically Generated file */
/* Do not edit! */
#pragma once
"""


# PX4
ఒకవేళ వాదనలు.git_ట్యాగ్:
    git_ట్యాగ్ = వాదనలు.git_ట్యాగ్
లేకపోతే:
    git_describe_cmd = 'git describe --exclude ext/* --always --tags --dirty'
    git_ట్యాగ్ = subprocess.check_output(git_describe_cmd.విభజించు(),
                                  stderr=subprocess.STDOUT).decode('utf-8').strip()

ప్రయత్నించు:
    # get the tag if we're on a tagged commit
    ట్యాగ్_or_branch = subprocess.check_output((git_describe_cmd+' --exact-match').విభజించు(),
                                            stderr=subprocess.STDOUT).decode('utf-8').strip()
మినహాయింపు:
    ట్యాగ్_or_branch = ఏమీలేదు

ఒకవేళ validate:
    ఒకవేళ verbose:
        ముద్రించు("testing git tag: "+git_ట్యాగ్)
    # remove optional '-dirty' at the end
    git_ట్యాగ్_test = re.sub(r'-dirty$', '', git_ట్యాగ్)
    # remove optional -<num_commits>-g<commit_hash> at the end (in case we are not on a tagged commit)
    git_ట్యాగ్_test = re.sub(r'-[0-9]+-g[0-9a-fA-F]+$', '', git_ట్యాగ్_test)
    # now check the version format
    m = re.సరిపోలిక(r'v([0-9]+)\.([0-9]+)\.[0-9]+(((-dev)|(-alpha[0-9]+)|(-beta[0-9]+)|(-rc[0-9]+))|'\
                 r'(-[0-9]+\.[0-9]+\.[0-9]+((-dev)|(-alpha[0-9]+)|(-beta[0-9]+)|([-]?rc[0-9]+))?))?$', git_ట్యాగ్_test)
    ఒకవేళ కాదు m:
        ముద్రించు("")
        ముద్రించు("Error: the git tag '{:}' does not match the expected format.".format(git_ట్యాగ్_test))
        ముద్రించు("")
        ముద్రించు("The expected format is 'v<PX4 version>[-<custom version>]'")
        ముద్రించు("  <PX4 version>: v<major>.<minor>.<patch>[-rc<rc>|-beta<beta>|-alpha<alpha>|-dev]")
        ముద్రించు("  <custom version>: <major>.<minor>.<patch>[-rc<rc>|-beta<beta>|-alpha<alpha>|-dev]")
        ముద్రించు("Examples:")
        ముద్రించు("  v1.9.0-rc3 (preferred)")
        ముద్రించు("  v1.9.0-beta1")
        ముద్రించు("  v1.9.0-1.0.0")
        ముద్రించు("  v1.9.0-1.0.0-alpha2")
        ముద్రించు("See also https://docs.px4.io/main/en/dev_setup/building_px4.html#building-for-nuttx")
        ముద్రించు("")
        sys.exit(1)

git_సంస్కరణ = subprocess.check_output('git rev-parse --verify HEAD'.విభజించు(),
                                      stderr=subprocess.STDOUT).decode('utf-8').strip()
ప్రయత్నించు:
    git_branch_name = subprocess.check_output('git symbolic-ref -q --short HEAD'.విభజించు(),
                                          stderr=subprocess.STDOUT).decode('utf-8').strip()
మినహాయింపు:
    git_branch_name = ''
git_సంస్కరణ_short = git_సంస్కరణ[0:16]

# OEM version
ప్రయత్నించు:
    oem_ట్యాగ్ = subprocess.check_output('git describe --match ext/oem-* --tags'.విభజించు(),
                                      stderr=subprocess.STDOUT).decode('utf-8').strip()
    oem_ట్యాగ్ = oem_ట్యాగ్[8:]
మినహాయింపు:
    oem_ట్యాగ్ = ''

ఒకవేళ ట్యాగ్_or_branch అయిన ఏమీలేదు:
    # replace / so it can be used as directory name
    ట్యాగ్_or_branch = git_branch_name.replace('/', '-')
    # either a release or master branch (used for metadata)
    # CI uploads to 'master/' on S3 for legacy QGC compatibility
    ఒకవేళ కాదు ట్యాగ్_or_branch.startswith('release-'):
        ట్యాగ్_or_branch = 'master'

శీర్షిక += f"""
#define PX4_GIT_VERSION_STR "{git_version}"
#define PX4_GIT_VERSION_BINARY 0x{git_version_short}
#define PX4_GIT_TAG_STR "{git_tag}"
#define PX4_GIT_BRANCH_NAME "{git_branch_name}"

#define PX4_GIT_OEM_VERSION_STR  "{oem_tag}"

#define PX4_GIT_TAG_OR_BRANCH_NAME "{tag_or_branch}" // special variable: git tag, release or master branch
"""


# Mavlink
ఒకవేళ (os.మార్గం.exists('src/modules/mavlink/mavlink/.git')):
    mavlink_git_సంస్కరణ = subprocess.check_output('git rev-parse --verify HEAD'.విభజించు(),
                                      cwd='src/modules/mavlink/mavlink', stderr=subprocess.STDOUT).decode('utf-8').strip()
    mavlink_git_సంస్కరణ_short = mavlink_git_సంస్కరణ[0:16]

    శీర్షిక += f"""
#define MAVLINK_LIB_GIT_VERSION_STR  "{mavlink_git_version}"
#define MAVLINK_LIB_GIT_VERSION_BINARY 0x{mavlink_git_version_short}
"""


# NuttX
ఒకవేళ (os.మార్గం.exists('platforms/nuttx/NuttX/nuttx/.git')):
    # The PX4/NuttX fork branch name recorded in .gitmodules is the
    # authoritative kernel version: px4_firmware_nuttx-X.Y.Z+ is updated
    # in the same commit as every kernel upgrade. Fall back to git
    # describe for checkouts that do not follow the fork convention
    # (e.g. upstream apache/nuttx directly, which carries release tags).
    nuttx_git_ట్యాగ్ = "v0.0.0"
    ప్రయత్నించు:
        nuttx_branch = subprocess.check_output(
            'git config -f .gitmodules submodule.platforms/nuttx/NuttX/nuttx.branch'.విభజించు(),
            stderr=subprocess.STDOUT).decode('utf-8').strip()
        branch_సంస్కరణ = re.సరిపోలిక(r'^px4_firmware_nuttx-([0-9]+\.[0-9]+\.[0-9]+)\+?$', nuttx_branch)
    మినహాయింపు subprocess.CalledProcessError:
        branch_సంస్కరణ = ఏమీలేదు
    ఒకవేళ branch_సంస్కరణ:
        nuttx_git_ట్యాగ్ = "v" + branch_సంస్కరణ.సమూహం(1)
    లేకపోతే:
        ప్రయత్నించు:
            nuttx_describe = subprocess.check_output(
                'git describe --tags --match nuttx-*'.విభజించు(),
                cwd='platforms/nuttx/NuttX/nuttx', stderr=subprocess.STDOUT).decode('utf-8').strip()
            describe_సంస్కరణ = re.సరిపోలిక(r'^nuttx-([0-9]+\.[0-9]+\.[0-9]+)', nuttx_describe)
            ఒకవేళ describe_సంస్కరణ:
                nuttx_git_ట్యాగ్ = "v" + describe_సంస్కరణ.సమూహం(1)
        మినహాయింపు subprocess.CalledProcessError:
            దాటు
    nuttx_git_సంస్కరణ = subprocess.check_output('git rev-parse --verify HEAD'.విభజించు(),
                                      cwd='platforms/nuttx/NuttX/nuttx', stderr=subprocess.STDOUT).decode('utf-8').strip()
    nuttx_git_సంస్కరణ_short = nuttx_git_సంస్కరణ[0:16]

    శీర్షిక += f"""
#define NUTTX_GIT_VERSION_STR  "{nuttx_git_version}"
#define NUTTX_GIT_VERSION_BINARY 0x{nuttx_git_version_short}
#define NUTTX_GIT_TAG_STR  "{nuttx_git_tag}"
"""


ఒకవేళ old_శీర్షిక != శీర్షిక:
    ఒకవేళ verbose:
        ముద్రించు('Updating header {}'.format(filename))
    fp_శీర్షిక = open(filename, 'w')
    fp_శీర్షిక.లిఖ(శీర్షిక)
