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

وصف save_compressed(filename):
    #create lzma compressed version
    xz_filename=filename+'.xz'
    سان lzma.open(xz_filename, 'wt', preset=9) كـ f:
        سان open(filename, 'r') كـ content_file:
            f.اكتب(content_file.اقرأ())



وصف مکيه():
    # Parse command line arguments
    parser = argparse.دليل_Parser(description="Process parameter documentation.")
    parser.جوڙيو_دليل("-s", "--src-path",
                        افتراضي=["../src"],
                        metavar="PATH",
                        nargs='*',
                        help="one or more paths to source files to scan for parameters")
    parser.جوڙيو_دليل("-x", "--xml",
                        nargs='?',
                        ثابت="parameters.xml",
                        metavar="FILENAME",
                        help="Create XML file"
                             " (default FILENAME: parameters.xml)")
    parser.جوڙيو_دليل("-b", "--board",
                        nargs='?',
                        ثابت="",
                        metavar="BOARD",
                        help="Board to create xml parameter xml for")
    parser.جوڙيو_دليل("-m", "--markdown",
                        nargs='?',
                        ثابت="parameters.md",
                        metavar="FILENAME",
                        help="Create Markdown file"
                             " (default FILENAME: parameters.md)")
    parser.جوڙيو_دليل("-j", "--json",
                        nargs='?',
                        ثابت="parameters.json",
                        metavar="FILENAME",
                        help="Create Json file"
                             " (default FILENAME: parameters.json)")
    parser.جوڙيو_دليل('-v', '--verbose',
                        action='store_true',
                        help="verbose output")
    parser.جوڙيو_دليل('-c', '--compress',
                        action='store_true',
                        help="compress parameter file")
    parser.جوڙيو_دليل("-o", "--overrides",
                        افتراضي="{}",
                        metavar="OVERRIDES",
                        help="a dict of overrides in the form of a json string")
    parser.جوڙيو_دليل("--readonly-config",
                        افتراضي=ڪجهه_نه,
                        metavar="FILENAME",
                        help="path to readonly_params.yaml")

    دليلون = parser.parse_دليلون()

    # Check for valid command
    إذا ليس (دليلون.xml أو دليلون.markdown أو دليلون.json):
        ڇاپيو("Error: You need to specify at least one output method!\n")
        parser.print_usage()
        sys.exit(1)

    # Initialize source scanner and parser
    scanner = srcscanner.ذريعو_Scanner()
    parser = srcparser.ذريعو_Parser()

    # Scan directories, and parse the files
    إذا (دليلون.verbose):
        ڇاپيو("Scanning source path " + str(دليلون.src_رستو))

    إذا ليس scanner.جاچ_Dir(دليلون.src_رستو, parser):
        sys.exit(1)

    إذا ليس parser.Validate():
        sys.exit(1)
    param_groups = parser.GetParamGroups()

    إذا ڊيگهه(param_groups) == 0:
        ڇاپيو("Warning: no parameters found")


    #inject parameters at front of set
    cur_dir = os.رستو.dirname(os.رستو.realpath(__file__))

    override_dict = json.loads(دليلون.overrides)
    إذا ڊيگهه(override_dict.ڪنجيون()) > 0:
        لكل گروهه في param_groups:
            لكل param في گروهه.GetParams():
                name = param.GetName()
                إذا name في override_dict.ڪنجيون():
                    val = str(override_dict[param.GetName()])
                    param.افتراضي = val
                    ڇاپيو("OVERRIDING {:s} to {:s}!!!!!".format(name, val))

    # Mark readonly parameters
    إذا دليلون.readonly_config:
        all_param_نالا = سيٽ()
        لكل گروهه في param_groups:
            لكل param في گروهه.GetParams():
                all_param_نالا.جوڙيو(param.GetName())
        readonly_params = readonly_config.ڀريو_readonly_params(دليلون.readonly_config, all_param_نالا)
        لكل گروهه في param_groups:
            لكل param في گروهه.GetParams():
                إذا param.GetName() في readonly_params:
                    param.سيٽ_Readonly()

    output_files = []

    # Output to XML file
    إذا دليلون.xml:
        إذا دليلون.verbose:
            ڇاپيو("Creating XML file " + دليلون.xml)
        ٻاهريون = xmlout.XMLOutput(param_groups, دليلون.board)
        ٻاهريون.Save(دليلون.xml)
        output_files.append(دليلون.xml)

    # Output to Markdown/HTML tables
    إذا دليلون.markdown:
        إذا دليلون.verbose:
            ڇاپيو("Creating markdown file " + دليلون.markdown)
        ٻاهريون = markdownout.MarkdownTablesOutput(param_groups)
        ٻاهريون.Save(دليلون.markdown)
        output_files.append(دليلون.markdown)

    # Output to JSON file
    إذا دليلون.json:
        إذا دليلون.verbose:
            ڇاپيو("Creating Json file " + دليلون.json)
        cur_dir = os.رستو.dirname(os.رستو.realpath(__file__))
        ٻاهريون = jsonout.JsonOutput(param_groups, دليلون.board)
        ٻاهريون.Save(دليلون.json)
        output_files.append(دليلون.json)

    إذا دليلون.compress:
        لكل f في output_files:
            إذا دليلون.verbose:
                ڇاپيو("Compressing file " + f)
            save_compressed(f)


إذا __name__ == "__main__":
    مکيه()
