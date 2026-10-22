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

سے __future__ درآمد print_function
درآمد sys
درآمد os
درآمد argparse
سے px4params درآمد srcscanner, srcparser, injectxmlparams, xmlout, markdownout, jsonout, readonly_config

درآمد lzma #to create .xz file
درآمد json

تعریف save_compressed(filename):
    #create lzma compressed version
    xz_filename=filename+'.xz'
    ساتھ lzma.open(xz_filename, 'wt', preset=9) بطور f:
        ساتھ open(filename, 'r') بطور content_file:
            f.اكتب(content_file.اقرأ())



تعریف مین():
    # Parse command line arguments
    parser = argparse.دلیل_Parser(description="Process parameter documentation.")
    parser.جوڑو_دلیل("-s", "--src-path",
                        طےشدہ=["../src"],
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
                        طےشدہ="{}",
                        metavar="OVERRIDES",
                        help="a dict of overrides in the form of a json string")
    parser.جوڑو_دلیل("--readonly-config",
                        طےشدہ=کچھ_نہیں,
                        metavar="FILENAME",
                        help="path to readonly_params.yaml")

    دلائل = parser.parse_دلائل()

    # Check for valid command
    اگر نہیں (دلائل.xml یا دلائل.markdown یا دلائل.json):
        چھاپو("Error: You need to specify at least one output method!\n")
        parser.print_usage()
        sys.exit(1)

    # Initialize source scanner and parser
    scanner = srcscanner.ماخذ_Scanner()
    parser = srcparser.ماخذ_Parser()

    # Scan directories, and parse the files
    اگر (دلائل.verbose):
        چھاپو("Scanning source path " + str(دلائل.src_راستہ))

    اگر نہیں scanner.جائزہ_Dir(دلائل.src_راستہ, parser):
        sys.exit(1)

    اگر نہیں parser.Validate():
        sys.exit(1)
    param_groups = parser.GetParamGroups()

    اگر لمبائی(param_groups) == 0:
        چھاپو("Warning: no parameters found")


    #inject parameters at front of set
    cur_dir = os.راستہ.dirname(os.راستہ.realpath(__file__))

    override_dict = json.loads(دلائل.overrides)
    اگر لمبائی(override_dict.کلیدیں()) > 0:
        برائے گروہ میں param_groups:
            برائے param میں گروہ.GetParams():
                name = param.GetName()
                اگر name میں override_dict.کلیدیں():
                    val = str(override_dict[param.GetName()])
                    param.طےشدہ = val
                    چھاپو("OVERRIDING {:s} to {:s}!!!!!".format(name, val))

    # Mark readonly parameters
    اگر دلائل.readonly_config:
        all_param_نام = سیٹ()
        برائے گروہ میں param_groups:
            برائے param میں گروہ.GetParams():
                all_param_نام.جوڑو(param.GetName())
        readonly_params = readonly_config.بھرو_readonly_params(دلائل.readonly_config, all_param_نام)
        برائے گروہ میں param_groups:
            برائے param میں گروہ.GetParams():
                اگر param.GetName() میں readonly_params:
                    param.سیٹ_Readonly()

    output_files = []

    # Output to XML file
    اگر دلائل.xml:
        اگر دلائل.verbose:
            چھاپو("Creating XML file " + دلائل.xml)
        برآمد = xmlout.XMLOutput(param_groups, دلائل.board)
        برآمد.Save(دلائل.xml)
        output_files.append(دلائل.xml)

    # Output to Markdown/HTML tables
    اگر دلائل.markdown:
        اگر دلائل.verbose:
            چھاپو("Creating markdown file " + دلائل.markdown)
        برآمد = markdownout.MarkdownTablesOutput(param_groups)
        برآمد.Save(دلائل.markdown)
        output_files.append(دلائل.markdown)

    # Output to JSON file
    اگر دلائل.json:
        اگر دلائل.verbose:
            چھاپو("Creating Json file " + دلائل.json)
        cur_dir = os.راستہ.dirname(os.راستہ.realpath(__file__))
        برآمد = jsonout.JsonOutput(param_groups, دلائل.board)
        برآمد.Save(دلائل.json)
        output_files.append(دلائل.json)

    اگر دلائل.compress:
        برائے f میں output_files:
            اگر دلائل.verbose:
                چھاپو("Compressing file " + f)
            save_compressed(f)


اگر __name__ == "__main__":
    مین()
