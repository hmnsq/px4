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
    سره lzma.open(xz_filename, 'wt', preset=9) كـ f:
        سره open(filename, 'r') كـ content_file:
            f.اكتب(content_file.اقرأ())



تعریف اصلي():
    # Parse command line arguments
    parser = argparse.دلیل_Parser(description="Process parameter documentation.")
    parser.جمع_کړه_دلیل("-s", "--src-path",
                        افتراضي=["../src"],
                        metavar="PATH",
                        nargs='*',
                        help="one or more paths to source files to scan for parameters")
    parser.جمع_کړه_دلیل("-x", "--xml",
                        nargs='?',
                        ثابت="parameters.xml",
                        metavar="FILENAME",
                        help="Create XML file"
                             " (default FILENAME: parameters.xml)")
    parser.جمع_کړه_دلیل("-b", "--board",
                        nargs='?',
                        ثابت="",
                        metavar="BOARD",
                        help="Board to create xml parameter xml for")
    parser.جمع_کړه_دلیل("-m", "--markdown",
                        nargs='?',
                        ثابت="parameters.md",
                        metavar="FILENAME",
                        help="Create Markdown file"
                             " (default FILENAME: parameters.md)")
    parser.جمع_کړه_دلیل("-j", "--json",
                        nargs='?',
                        ثابت="parameters.json",
                        metavar="FILENAME",
                        help="Create Json file"
                             " (default FILENAME: parameters.json)")
    parser.جمع_کړه_دلیل('-v', '--verbose',
                        action='store_true',
                        help="verbose output")
    parser.جمع_کړه_دلیل('-c', '--compress',
                        action='store_true',
                        help="compress parameter file")
    parser.جمع_کړه_دلیل("-o", "--overrides",
                        افتراضي="{}",
                        metavar="OVERRIDES",
                        help="a dict of overrides in the form of a json string")
    parser.جمع_کړه_دلیل("--readonly-config",
                        افتراضي=هیڅ,
                        metavar="FILENAME",
                        help="path to readonly_params.yaml")

    دلیلونه = parser.parse_دلیلونه()

    # Check for valid command
    إذا ليس (دلیلونه.xml أو دلیلونه.markdown أو دلیلونه.json):
        چاپ("Error: You need to specify at least one output method!\n")
        parser.print_usage()
        sys.exit(1)

    # Initialize source scanner and parser
    scanner = srcscanner.سرچینه_Scanner()
    parser = srcparser.سرچینه_Parser()

    # Scan directories, and parse the files
    إذا (دلیلونه.verbose):
        چاپ("Scanning source path " + str(دلیلونه.src_لار))

    إذا ليس scanner.لټون_Dir(دلیلونه.src_لار, parser):
        sys.exit(1)

    إذا ليس parser.Validate():
        sys.exit(1)
    param_groups = parser.GetParamGroups()

    إذا اوږدوالی(param_groups) == 0:
        چاپ("Warning: no parameters found")


    #inject parameters at front of set
    cur_dir = os.لار.dirname(os.لار.realpath(__file__))

    override_dict = json.loads(دلیلونه.overrides)
    إذا اوږدوالی(override_dict.کلیګانې()) > 0:
        لكل ډله في param_groups:
            لكل param في ډله.GetParams():
                name = param.GetName()
                إذا name في override_dict.کلیګانې():
                    val = str(override_dict[param.GetName()])
                    param.افتراضي = val
                    چاپ("OVERRIDING {:s} to {:s}!!!!!".format(name, val))

    # Mark readonly parameters
    إذا دلیلونه.readonly_config:
        all_param_نومونه = سېټ()
        لكل ډله في param_groups:
            لكل param في ډله.GetParams():
                all_param_نومونه.جمع_کړه(param.GetName())
        readonly_params = readonly_config.بارول_readonly_params(دلیلونه.readonly_config, all_param_نومونه)
        لكل ډله في param_groups:
            لكل param في ډله.GetParams():
                إذا param.GetName() في readonly_params:
                    param.سېټ_Readonly()

    output_files = []

    # Output to XML file
    إذا دلیلونه.xml:
        إذا دلیلونه.verbose:
            چاپ("Creating XML file " + دلیلونه.xml)
        بهر = xmlout.XMLOutput(param_groups, دلیلونه.board)
        بهر.Save(دلیلونه.xml)
        output_files.append(دلیلونه.xml)

    # Output to Markdown/HTML tables
    إذا دلیلونه.markdown:
        إذا دلیلونه.verbose:
            چاپ("Creating markdown file " + دلیلونه.markdown)
        بهر = markdownout.MarkdownTablesOutput(param_groups)
        بهر.Save(دلیلونه.markdown)
        output_files.append(دلیلونه.markdown)

    # Output to JSON file
    إذا دلیلونه.json:
        إذا دلیلونه.verbose:
            چاپ("Creating Json file " + دلیلونه.json)
        cur_dir = os.لار.dirname(os.لار.realpath(__file__))
        بهر = jsonout.JsonOutput(param_groups, دلیلونه.board)
        بهر.Save(دلیلونه.json)
        output_files.append(دلیلونه.json)

    إذا دلیلونه.compress:
        لكل f في output_files:
            إذا دلیلونه.verbose:
                چاپ("Compressing file " + f)
            save_compressed(f)


إذا __name__ == "__main__":
    اصلي()
