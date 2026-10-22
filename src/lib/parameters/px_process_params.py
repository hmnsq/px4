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

def save_compressed(filename):
    #create lzma compressed version
    xz_filename=filename+'.xz'
    with lzma.open(xz_filename, 'wt', preset=9) كـ f:
        with open(filename, 'r') كـ content_file:
            f.اكتب(content_file.اقرأ())



def main():
    # Parse command line arguments
    parser = argparse.ArgumentParser(description="Process parameter documentation.")
    parser.add_argument("-s", "--src-path",
                        افتراضي=["../src"],
                        metavar="PATH",
                        nargs='*',
                        help="one or more paths to source files to scan for parameters")
    parser.add_argument("-x", "--xml",
                        nargs='?',
                        ثابت="parameters.xml",
                        metavar="FILENAME",
                        help="Create XML file"
                             " (default FILENAME: parameters.xml)")
    parser.add_argument("-b", "--board",
                        nargs='?',
                        ثابت="",
                        metavar="BOARD",
                        help="Board to create xml parameter xml for")
    parser.add_argument("-m", "--markdown",
                        nargs='?',
                        ثابت="parameters.md",
                        metavar="FILENAME",
                        help="Create Markdown file"
                             " (default FILENAME: parameters.md)")
    parser.add_argument("-j", "--json",
                        nargs='?',
                        ثابت="parameters.json",
                        metavar="FILENAME",
                        help="Create Json file"
                             " (default FILENAME: parameters.json)")
    parser.add_argument('-v', '--verbose',
                        action='store_true',
                        help="verbose output")
    parser.add_argument('-c', '--compress',
                        action='store_true',
                        help="compress parameter file")
    parser.add_argument("-o", "--overrides",
                        افتراضي="{}",
                        metavar="OVERRIDES",
                        help="a dict of overrides in the form of a json string")
    parser.add_argument("--readonly-config",
                        افتراضي=None,
                        metavar="FILENAME",
                        help="path to readonly_params.yaml")

    args = parser.parse_args()

    # Check for valid command
    𐤀𐤌 𐤋𐤀 (args.xml 𐤀𐤅 args.markdown 𐤀𐤅 args.json):
        print("Error: You need to specify at least one output method!\n")
        parser.print_usage()
        sys.exit(1)

    # Initialize source scanner and parser
    scanner = srcscanner.SourceScanner()
    parser = srcparser.SourceParser()

    # Scan directories, and parse the files
    𐤀𐤌 (args.verbose):
        print("Scanning source path " + str(args.src_path))

    𐤀𐤌 𐤋𐤀 scanner.ScanDir(args.src_path, parser):
        sys.exit(1)

    𐤀𐤌 𐤋𐤀 parser.Validate():
        sys.exit(1)
    param_groups = parser.GetParamGroups()

    𐤀𐤌 len(param_groups) == 0:
        print("Warning: no parameters found")


    #inject parameters at front of set
    cur_dir = os.path.dirname(os.path.realpath(__file__))

    override_dict = json.loads(args.overrides)
    𐤀𐤌 len(override_dict.keys()) > 0:
        لكل group في param_groups:
            لكل param في group.GetParams():
                name = param.GetName()
                𐤀𐤌 name في override_dict.keys():
                    val = str(override_dict[param.GetName()])
                    param.افتراضي = val
                    print("OVERRIDING {:s} to {:s}!!!!!".format(name, val))

    # Mark readonly parameters
    𐤀𐤌 args.readonly_config:
        all_param_names = set()
        لكل group في param_groups:
            لكل param في group.GetParams():
                all_param_names.add(param.GetName())
        readonly_params = readonly_config.load_readonly_params(args.readonly_config, all_param_names)
        لكل group في param_groups:
            لكل param في group.GetParams():
                𐤀𐤌 param.GetName() في readonly_params:
                    param.SetReadonly()

    output_files = []

    # Output to XML file
    𐤀𐤌 args.xml:
        𐤀𐤌 args.verbose:
            print("Creating XML file " + args.xml)
        out = xmlout.XMLOutput(param_groups, args.board)
        out.Save(args.xml)
        output_files.append(args.xml)

    # Output to Markdown/HTML tables
    𐤀𐤌 args.markdown:
        𐤀𐤌 args.verbose:
            print("Creating markdown file " + args.markdown)
        out = markdownout.MarkdownTablesOutput(param_groups)
        out.Save(args.markdown)
        output_files.append(args.markdown)

    # Output to JSON file
    𐤀𐤌 args.json:
        𐤀𐤌 args.verbose:
            print("Creating Json file " + args.json)
        cur_dir = os.path.dirname(os.path.realpath(__file__))
        out = jsonout.JsonOutput(param_groups, args.board)
        out.Save(args.json)
        output_files.append(args.json)

    𐤀𐤌 args.compress:
        لكل f في output_files:
            𐤀𐤌 args.verbose:
                print("Compressing file " + f)
            save_compressed(f)


𐤀𐤌 __name__ == "__main__":
    main()
