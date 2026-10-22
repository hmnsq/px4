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

तः __future__ आनयन print_function
आनयन sys
आनयन os
आनयन argparse
तः px4params आनयन srcscanner, srcparser, injectxmlparams, xmlout, markdownout, jsonout, readonly_config

आनयन lzma #to create .xz file
आनयन json

परिभाषा save_compressed(filename):
    #create lzma compressed version
    xz_filename=filename+'.xz'
    सह lzma.open(xz_filename, 'wt', preset=9) इव f:
        सह open(filename, 'r') इव content_file:
            f.लिख(content_file.पठ())



परिभाषा मुख्यम्():
    # Parse command line arguments
    parser = argparse.तर्कः_Parser(description="Process parameter documentation.")
    parser.सङ्कलय_तर्कः("-s", "--src-path",
                        स्वभाव=["../src"],
                        metavar="PATH",
                        nargs='*',
                        help="one or more paths to source files to scan for parameters")
    parser.सङ्कलय_तर्कः("-x", "--xml",
                        nargs='?',
                        ध्रुव="parameters.xml",
                        metavar="FILENAME",
                        help="Create XML file"
                             " (default FILENAME: parameters.xml)")
    parser.सङ्कलय_तर्कः("-b", "--board",
                        nargs='?',
                        ध्रुव="",
                        metavar="BOARD",
                        help="Board to create xml parameter xml for")
    parser.सङ्कलय_तर्कः("-m", "--markdown",
                        nargs='?',
                        ध्रुव="parameters.md",
                        metavar="FILENAME",
                        help="Create Markdown file"
                             " (default FILENAME: parameters.md)")
    parser.सङ्कलय_तर्कः("-j", "--json",
                        nargs='?',
                        ध्रुव="parameters.json",
                        metavar="FILENAME",
                        help="Create Json file"
                             " (default FILENAME: parameters.json)")
    parser.सङ्कलय_तर्कः('-v', '--verbose',
                        action='store_true',
                        help="verbose output")
    parser.सङ्कलय_तर्कः('-c', '--compress',
                        action='store_true',
                        help="compress parameter file")
    parser.सङ्कलय_तर्कः("-o", "--overrides",
                        स्वभाव="{}",
                        metavar="OVERRIDES",
                        help="a dict of overrides in the form of a json string")
    parser.सङ्कलय_तर्कः("--readonly-config",
                        स्वभाव=किमपि_न,
                        metavar="FILENAME",
                        help="path to readonly_params.yaml")

    तर्काः = parser.parse_तर्काः()

    # Check for valid command
    यदि न (तर्काः.xml वा तर्काः.markdown वा तर्काः.json):
        मुद्रय("Error: You need to specify at least one output method!\n")
        parser.print_usage()
        sys.exit(1)

    # Initialize source scanner and parser
    scanner = srcscanner.SourceScanner()
    parser = srcparser.SourceParser()

    # Scan directories, and parse the files
    यदि (तर्काः.verbose):
        मुद्रय("Scanning source path " + str(तर्काः.src_मार्गः))

    यदि न scanner.अवलोकनम्_Dir(तर्काः.src_मार्गः, parser):
        sys.exit(1)

    यदि न parser.Validate():
        sys.exit(1)
    param_groups = parser.GetParamGroups()

    यदि दीर्घता(param_groups) == 0:
        मुद्रय("Warning: no parameters found")


    #inject parameters at front of set
    cur_dir = os.मार्गः.dirname(os.मार्गः.realpath(__file__))

    override_dict = json.loads(तर्काः.overrides)
    यदि दीर्घता(override_dict.कुञ्चिकाः()) > 0:
        प्रति गणः अन्तः param_groups:
            प्रति param अन्तः गणः.GetParams():
                name = param.GetName()
                यदि name अन्तः override_dict.कुञ्चिकाः():
                    val = str(override_dict[param.GetName()])
                    param.स्वभाव = val
                    मुद्रय("OVERRIDING {:s} to {:s}!!!!!".format(name, val))

    # Mark readonly parameters
    यदि तर्काः.readonly_config:
        all_param_नामानि = समुच्चयः()
        प्रति गणः अन्तः param_groups:
            प्रति param अन्तः गणः.GetParams():
                all_param_नामानि.सङ्कलय(param.GetName())
        readonly_params = readonly_config.आरोपय_readonly_params(तर्काः.readonly_config, all_param_नामानि)
        प्रति गणः अन्तः param_groups:
            प्रति param अन्तः गणः.GetParams():
                यदि param.GetName() अन्तः readonly_params:
                    param.समुच्चयः_Readonly()

    output_files = []

    # Output to XML file
    यदि तर्काः.xml:
        यदि तर्काः.verbose:
            मुद्रय("Creating XML file " + तर्काः.xml)
        बहिः = xmlout.XMLOutput(param_groups, तर्काः.board)
        बहिः.Save(तर्काः.xml)
        output_files.append(तर्काः.xml)

    # Output to Markdown/HTML tables
    यदि तर्काः.markdown:
        यदि तर्काः.verbose:
            मुद्रय("Creating markdown file " + तर्काः.markdown)
        बहिः = markdownout.MarkdownTablesOutput(param_groups)
        बहिः.Save(तर्काः.markdown)
        output_files.append(तर्काः.markdown)

    # Output to JSON file
    यदि तर्काः.json:
        यदि तर्काः.verbose:
            मुद्रय("Creating Json file " + तर्काः.json)
        cur_dir = os.मार्गः.dirname(os.मार्गः.realpath(__file__))
        बहिः = jsonout.JsonOutput(param_groups, तर्काः.board)
        बहिः.Save(तर्काः.json)
        output_files.append(तर्काः.json)

    यदि तर्काः.compress:
        प्रति f अन्तः output_files:
            यदि तर्काः.verbose:
                मुद्रय("Compressing file " + f)
            save_compressed(f)


यदि __name__ == "__main__":
    मुख्यम्()
