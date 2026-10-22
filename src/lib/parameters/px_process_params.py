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
    با lzma.open(xz_filename, 'wt', preset=9) كـ f:
        با open(filename, 'r') كـ content_file:
            f.اكتب(content_file.اقرأ())



تعریف اصلی():
    # Parse command line arguments
    parser = argparse.آرگومان_Parser(description="Process parameter documentation.")
    parser.جمع_کن_آرگومان("-s", "--src-path",
                        افتراضي=["../src"],
                        metavar="PATH",
                        nargs='*',
                        help="one or more paths to source files to scan for parameters")
    parser.جمع_کن_آرگومان("-x", "--xml",
                        nargs='?',
                        ثابت="parameters.xml",
                        metavar="FILENAME",
                        help="Create XML file"
                             " (default FILENAME: parameters.xml)")
    parser.جمع_کن_آرگومان("-b", "--board",
                        nargs='?',
                        ثابت="",
                        metavar="BOARD",
                        help="Board to create xml parameter xml for")
    parser.جمع_کن_آرگومان("-m", "--markdown",
                        nargs='?',
                        ثابت="parameters.md",
                        metavar="FILENAME",
                        help="Create Markdown file"
                             " (default FILENAME: parameters.md)")
    parser.جمع_کن_آرگومان("-j", "--json",
                        nargs='?',
                        ثابت="parameters.json",
                        metavar="FILENAME",
                        help="Create Json file"
                             " (default FILENAME: parameters.json)")
    parser.جمع_کن_آرگومان('-v', '--verbose',
                        action='store_true',
                        help="verbose output")
    parser.جمع_کن_آرگومان('-c', '--compress',
                        action='store_true',
                        help="compress parameter file")
    parser.جمع_کن_آرگومان("-o", "--overrides",
                        افتراضي="{}",
                        metavar="OVERRIDES",
                        help="a dict of overrides in the form of a json string")
    parser.جمع_کن_آرگومان("--readonly-config",
                        افتراضي=هیچ,
                        metavar="FILENAME",
                        help="path to readonly_params.yaml")

    آرگومانها = parser.parse_آرگومانها()

    # Check for valid command
    إذا ليس (آرگومانها.xml أو آرگومانها.markdown أو آرگومانها.json):
        چاپ("Error: You need to specify at least one output method!\n")
        parser.print_usage()
        sys.exit(1)

    # Initialize source scanner and parser
    scanner = srcscanner.منبع_Scanner()
    parser = srcparser.منبع_Parser()

    # Scan directories, and parse the files
    إذا (آرگومانها.verbose):
        چاپ("Scanning source path " + str(آرگومانها.src_مسیر))

    إذا ليس scanner.پیمایش_Dir(آرگومانها.src_مسیر, parser):
        sys.exit(1)

    إذا ليس parser.Validate():
        sys.exit(1)
    param_groups = parser.GetParamGroups()

    إذا طول(param_groups) == 0:
        چاپ("Warning: no parameters found")


    #inject parameters at front of set
    cur_dir = os.مسیر.dirname(os.مسیر.realpath(__file__))

    override_dict = json.loads(آرگومانها.overrides)
    إذا طول(override_dict.کلیدها()) > 0:
        لكل گروه في param_groups:
            لكل param في گروه.GetParams():
                name = param.GetName()
                إذا name في override_dict.کلیدها():
                    val = str(override_dict[param.GetName()])
                    param.افتراضي = val
                    چاپ("OVERRIDING {:s} to {:s}!!!!!".format(name, val))

    # Mark readonly parameters
    إذا آرگومانها.readonly_config:
        all_param_نامها = مجموعه()
        لكل گروه في param_groups:
            لكل param في گروه.GetParams():
                all_param_نامها.جمع_کن(param.GetName())
        readonly_params = readonly_config.بارگیری_readonly_params(آرگومانها.readonly_config, all_param_نامها)
        لكل گروه في param_groups:
            لكل param في گروه.GetParams():
                إذا param.GetName() في readonly_params:
                    param.مجموعه_Readonly()

    output_files = []

    # Output to XML file
    إذا آرگومانها.xml:
        إذا آرگومانها.verbose:
            چاپ("Creating XML file " + آرگومانها.xml)
        بیرون = xmlout.XMLOutput(param_groups, آرگومانها.board)
        بیرون.Save(آرگومانها.xml)
        output_files.append(آرگومانها.xml)

    # Output to Markdown/HTML tables
    إذا آرگومانها.markdown:
        إذا آرگومانها.verbose:
            چاپ("Creating markdown file " + آرگومانها.markdown)
        بیرون = markdownout.MarkdownTablesOutput(param_groups)
        بیرون.Save(آرگومانها.markdown)
        output_files.append(آرگومانها.markdown)

    # Output to JSON file
    إذا آرگومانها.json:
        إذا آرگومانها.verbose:
            چاپ("Creating Json file " + آرگومانها.json)
        cur_dir = os.مسیر.dirname(os.مسیر.realpath(__file__))
        بیرون = jsonout.JsonOutput(param_groups, آرگومانها.board)
        بیرون.Save(آرگومانها.json)
        output_files.append(آرگومانها.json)

    إذا آرگومانها.compress:
        لكل f في output_files:
            إذا آرگومانها.verbose:
                چاپ("Compressing file " + f)
            save_compressed(f)


إذا __name__ == "__main__":
    اصلی()
