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

ತಃ __future__ ಆಮದು print_function
ಆಮದು sys
ಆಮದು os
ಆಮದು argparse
ತಃ px4params ಆಮದು srcscanner, srcparser, injectxmlparams, xmlout, markdownout, jsonout, readonly_config

ಆಮದು lzma #to create .xz file
ಆಮದು json

ವ್ಯಾಖ್ಯಾನ save_compressed(filename):
    #create lzma compressed version
    xz_filename=filename+'.xz'
    ಜೊತೆ lzma.open(xz_filename, 'wt', preset=9) ಇವ f:
        ಜೊತೆ open(filename, 'r') ಇವ content_file:
            f.ಲಿಖ(content_file.ಪಠ())



ವ್ಯಾಖ್ಯಾನ ಮುಖ್ಯ():
    # Parse command line arguments
    parser = argparse.ವಾದ_Parser(description="Process parameter documentation.")
    parser.ಕೂಡಿಸು_ವಾದ("-s", "--src-path",
                        ಪೂರ್ವನಿಯೋಜಿತ=["../src"],
                        metavar="PATH",
                        nargs='*',
                        help="one or more paths to source files to scan for parameters")
    parser.ಕೂಡಿಸು_ವಾದ("-x", "--xml",
                        nargs='?',
                        ಸ್ಥಿರಾಂಕ="parameters.xml",
                        metavar="FILENAME",
                        help="Create XML file"
                             " (default FILENAME: parameters.xml)")
    parser.ಕೂಡಿಸು_ವಾದ("-b", "--board",
                        nargs='?',
                        ಸ್ಥಿರಾಂಕ="",
                        metavar="BOARD",
                        help="Board to create xml parameter xml for")
    parser.ಕೂಡಿಸು_ವಾದ("-m", "--markdown",
                        nargs='?',
                        ಸ್ಥಿರಾಂಕ="parameters.md",
                        metavar="FILENAME",
                        help="Create Markdown file"
                             " (default FILENAME: parameters.md)")
    parser.ಕೂಡಿಸು_ವಾದ("-j", "--json",
                        nargs='?',
                        ಸ್ಥಿರಾಂಕ="parameters.json",
                        metavar="FILENAME",
                        help="Create Json file"
                             " (default FILENAME: parameters.json)")
    parser.ಕೂಡಿಸು_ವಾದ('-v', '--verbose',
                        action='store_true',
                        help="verbose output")
    parser.ಕೂಡಿಸು_ವಾದ('-c', '--compress',
                        action='store_true',
                        help="compress parameter file")
    parser.ಕೂಡಿಸು_ವಾದ("-o", "--overrides",
                        ಪೂರ್ವನಿಯೋಜಿತ="{}",
                        metavar="OVERRIDES",
                        help="a dict of overrides in the form of a json string")
    parser.ಕೂಡಿಸು_ವಾದ("--readonly-config",
                        ಪೂರ್ವನಿಯೋಜಿತ=ಏನೂಇಲ್ಲ,
                        metavar="FILENAME",
                        help="path to readonly_params.yaml")

    ವಾದಗಳು = parser.parse_ವಾದಗಳು()

    # Check for valid command
    ಆದರೆ ಅಲ್ಲ (ವಾದಗಳು.xml ಅಥವಾ ವಾದಗಳು.markdown ಅಥವಾ ವಾದಗಳು.json):
        ಮುದ್ರಿಸು("Error: You need to specify at least one output method!\n")
        parser.print_usage()
        sys.exit(1)

    # Initialize source scanner and parser
    scanner = srcscanner.ಮೂಲ_Scanner()
    parser = srcparser.ಮೂಲ_Parser()

    # Scan directories, and parse the files
    ಆದರೆ (ವಾದಗಳು.verbose):
        ಮುದ್ರಿಸು("Scanning source path " + str(ವಾದಗಳು.src_ಮಾರ್ಗ))

    ಆದರೆ ಅಲ್ಲ scanner.ಪರಿಶೀಲನೆ_Dir(ವಾದಗಳು.src_ಮಾರ್ಗ, parser):
        sys.exit(1)

    ಆದರೆ ಅಲ್ಲ parser.Validate():
        sys.exit(1)
    param_groups = parser.GetParamGroups()

    ಆದರೆ ಉದ್ದ(param_groups) == 0:
        ಮುದ್ರಿಸು("Warning: no parameters found")


    #inject parameters at front of set
    cur_dir = os.ಮಾರ್ಗ.dirname(os.ಮಾರ್ಗ.realpath(__file__))

    override_dict = json.loads(ವಾದಗಳು.overrides)
    ಆದರೆ ಉದ್ದ(override_dict.ಕೀಲಿಗಳು()) > 0:
        ಗಾಗಿ ಗುಂಪು ಒಳಗೆ param_groups:
            ಗಾಗಿ param ಒಳಗೆ ಗುಂಪು.GetParams():
                name = param.GetName()
                ಆದರೆ name ಒಳಗೆ override_dict.ಕೀಲಿಗಳು():
                    val = str(override_dict[param.GetName()])
                    param.ಪೂರ್ವನಿಯೋಜಿತ = val
                    ಮುದ್ರಿಸು("OVERRIDING {:s} to {:s}!!!!!".format(name, val))

    # Mark readonly parameters
    ಆದರೆ ವಾದಗಳು.readonly_config:
        all_param_ಹೆಸರುಗಳು = ಗಣ()
        ಗಾಗಿ ಗುಂಪು ಒಳಗೆ param_groups:
            ಗಾಗಿ param ಒಳಗೆ ಗುಂಪು.GetParams():
                all_param_ಹೆಸರುಗಳು.ಕೂಡಿಸು(param.GetName())
        readonly_params = readonly_config.ತುಂಬು_readonly_params(ವಾದಗಳು.readonly_config, all_param_ಹೆಸರುಗಳು)
        ಗಾಗಿ ಗುಂಪು ಒಳಗೆ param_groups:
            ಗಾಗಿ param ಒಳಗೆ ಗುಂಪು.GetParams():
                ಆದರೆ param.GetName() ಒಳಗೆ readonly_params:
                    param.ಗಣ_Readonly()

    output_files = []

    # Output to XML file
    ಆದರೆ ವಾದಗಳು.xml:
        ಆದರೆ ವಾದಗಳು.verbose:
            ಮುದ್ರಿಸು("Creating XML file " + ವಾದಗಳು.xml)
        ಹೊರಹೋಗು = xmlout.XMLOutput(param_groups, ವಾದಗಳು.board)
        ಹೊರಹೋಗು.Save(ವಾದಗಳು.xml)
        output_files.append(ವಾದಗಳು.xml)

    # Output to Markdown/HTML tables
    ಆದರೆ ವಾದಗಳು.markdown:
        ಆದರೆ ವಾದಗಳು.verbose:
            ಮುದ್ರಿಸು("Creating markdown file " + ವಾದಗಳು.markdown)
        ಹೊರಹೋಗು = markdownout.MarkdownTablesOutput(param_groups)
        ಹೊರಹೋಗು.Save(ವಾದಗಳು.markdown)
        output_files.append(ವಾದಗಳು.markdown)

    # Output to JSON file
    ಆದರೆ ವಾದಗಳು.json:
        ಆದರೆ ವಾದಗಳು.verbose:
            ಮುದ್ರಿಸು("Creating Json file " + ವಾದಗಳು.json)
        cur_dir = os.ಮಾರ್ಗ.dirname(os.ಮಾರ್ಗ.realpath(__file__))
        ಹೊರಹೋಗು = jsonout.JsonOutput(param_groups, ವಾದಗಳು.board)
        ಹೊರಹೋಗು.Save(ವಾದಗಳು.json)
        output_files.append(ವಾದಗಳು.json)

    ಆದರೆ ವಾದಗಳು.compress:
        ಗಾಗಿ f ಒಳಗೆ output_files:
            ಆದರೆ ವಾದಗಳು.verbose:
                ಮುದ್ರಿಸು("Compressing file " + f)
            save_compressed(f)


ಆದರೆ __name__ == "__main__":
    ಮುಖ್ಯ()
