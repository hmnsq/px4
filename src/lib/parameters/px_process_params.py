#!/usr/bin/env python
############################################################################
#
#   Copyright (C) 2013-2017 PX4 Development Team. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
# 3. Neither the name PX4 nor the names of its contributors may be
#    used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
# OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
# AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
############################################################################

#
# PX4 parameters processor (main executable file)
#
# This tool scans the PX4 source code for declarations of tunable parameters
# and outputs the list in various formats.
#
# Currently supported formats are:
#   * XML for the parametric UI generator
#   * Human-readable description in Markdown page format for the PX4 dev guide
#

من __future__ استورد print_function
استورد sys
استورد os
استورد argparse
من px4params استورد srcscanner, srcparser, injectxmlparams, xmlout, markdownout, jsonout, readonly_config

استورد lzma #to create .xz file
استورد json

تعریف save_compressed(filename):
    #create lzma compressed version
    xz_filename=filename+'.xz'
    نال lzma.open(xz_filename, 'wt', preset=9) كـ f:
        نال open(filename, 'r') كـ content_file:
            f.اكتب(content_file.اقرأ())



تعریف مکھ():
    # Parse command line arguments
    parser = argparse.دلیل_Parser(description="Process parameter documentation.")
    parser.جوڑو_دلیل("-s", "--src-path",
                        افتراضي=["../src"],
                        metavar="PATH",
                        nargs='*',
                        help="one or more paths to source files to scan for parameters")
    parser.جوڑو_دلیل("-x", "--xml",
                        nargs='?',
                        ثابت="parameters.xml",
                        metavar="FILENAME",
                        help="Create XML file"
                             " (default FILENAME: parameters.xml)")
    parser.جوڑو_دلیل("-b", "--board",
                        nargs='?',
                        ثابت="",
                        metavar="BOARD",
                        help="Board to create xml parameter xml for")
    parser.جوڑو_دلیل("-m", "--markdown",
                        nargs='?',
                        ثابت="parameters.md",
                        metavar="FILENAME",
                        help="Create Markdown file"
                             " (default FILENAME: parameters.md)")
    parser.جوڑو_دلیل("-j", "--json",
                        nargs='?',
                        ثابت="parameters.json",
                        metavar="FILENAME",
                        help="Create Json file"
                             " (default FILENAME: parameters.json)")
    parser.جوڑو_دلیل('-v', '--verbose',
                        action='store_true',
                        help="verbose output")
    parser.جوڑو_دلیل('-c', '--compress',
                        action='store_true',
                        help="compress parameter file")
    parser.جوڑو_دلیل("-o", "--overrides",
                        افتراضي="{}",
                        metavar="OVERRIDES",
                        help="a dict of overrides in the form of a json string")
    parser.جوڑو_دلیل("--readonly-config",
                        افتراضي=کجھ_نئیں,
                        metavar="FILENAME",
                        help="path to readonly_params.yaml")

    دلیلاں = parser.parse_دلیلاں()

    # Check for valid command
    إذا ليس (دلیلاں.xml أو دلیلاں.markdown أو دلیلاں.json):
        چھاپو("Error: You need to specify at least one output method!\n")
        parser.print_usage()
        sys.exit(1)

    # Initialize source scanner and parser
    scanner = srcscanner.سروت_Scanner()
    parser = srcparser.سروت_Parser()

    # Scan directories, and parse the files
    إذا (دلیلاں.verbose):
        چھاپو("Scanning source path " + str(دلیلاں.src_رستہ))

    إذا ليس scanner.جانچ_Dir(دلیلاں.src_رستہ, parser):
        sys.exit(1)

    إذا ليس parser.Validate():
        sys.exit(1)
    param_groups = parser.GetParamGroups()

    إذا لمبائی(param_groups) == 0:
        چھاپو("Warning: no parameters found")


    #inject parameters at front of set
    cur_dir = os.رستہ.dirname(os.رستہ.realpath(__file__))

    override_dict = json.loads(دلیلاں.overrides)
    إذا لمبائی(override_dict.کلیدیں()) > 0:
        لكل گروہ في param_groups:
            لكل param في گروہ.GetParams():
                name = param.GetName()
                إذا name في override_dict.کلیدیں():
                    val = str(override_dict[param.GetName()])
                    param.افتراضي = val
                    چھاپو("OVERRIDING {:s} to {:s}!!!!!".format(name, val))

    # Mark readonly parameters
    إذا دلیلاں.readonly_config:
        all_param_ناں = سیٹ()
        لكل گروہ في param_groups:
            لكل param في گروہ.GetParams():
                all_param_ناں.جوڑو(param.GetName())
        readonly_params = readonly_config.لدو_readonly_params(دلیلاں.readonly_config, all_param_ناں)
        لكل گروہ في param_groups:
            لكل param في گروہ.GetParams():
                إذا param.GetName() في readonly_params:
                    param.سیٹ_Readonly()

    output_files = []

    # Output to XML file
    إذا دلیلاں.xml:
        إذا دلیلاں.verbose:
            چھاپو("Creating XML file " + دلیلاں.xml)
        باہرلا = xmlout.XMLOutput(param_groups, دلیلاں.board)
        باہرلا.Save(دلیلاں.xml)
        output_files.append(دلیلاں.xml)

    # Output to Markdown/HTML tables
    إذا دلیلاں.markdown:
        إذا دلیلاں.verbose:
            چھاپو("Creating markdown file " + دلیلاں.markdown)
        باہرلا = markdownout.MarkdownTablesOutput(param_groups)
        باہرلا.Save(دلیلاں.markdown)
        output_files.append(دلیلاں.markdown)

    # Output to JSON file
    إذا دلیلاں.json:
        إذا دلیلاں.verbose:
            چھاپو("Creating Json file " + دلیلاں.json)
        cur_dir = os.رستہ.dirname(os.رستہ.realpath(__file__))
        باہرلا = jsonout.JsonOutput(param_groups, دلیلاں.board)
        باہرلا.Save(دلیلاں.json)
        output_files.append(دلیلاں.json)

    إذا دلیلاں.compress:
        لكل f في output_files:
            إذا دلیلاں.verbose:
                چھاپو("Compressing file " + f)
            save_compressed(f)


إذا __name__ == "__main__":
    مکھ()
