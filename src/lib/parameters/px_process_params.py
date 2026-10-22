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

ਤਃ __future__ ਆਯਾਤ print_function
ਆਯਾਤ sys
ਆਯਾਤ os
ਆਯਾਤ argparse
ਤਃ px4params ਆਯਾਤ srcscanner, srcparser, injectxmlparams, xmlout, markdownout, jsonout, readonly_config

ਆਯਾਤ lzma #to create .xz file
ਆਯਾਤ json

ਪਰਿਭਾਸ਼ਾ save_compressed(filename):
    #create lzma compressed version
    xz_filename=filename+'.xz'
    ਨਾਲ lzma.open(xz_filename, 'wt', preset=9) ਇਵ f:
        ਨਾਲ open(filename, 'r') ਇਵ content_file:
            f.ਲਿਖ(content_file.ਪਠ())



ਪਰਿਭਾਸ਼ਾ ਮੁੱਖ():
    # Parse command line arguments
    parser = argparse.ਦਲੀਲ_Parser(description="Process parameter documentation.")
    parser.ਜੋੜੋ_ਦਲੀਲ("-s", "--src-path",
                        ਮੂਲ=["../src"],
                        metavar="PATH",
                        nargs='*',
                        help="one or more paths to source files to scan for parameters")
    parser.ਜੋੜੋ_ਦਲੀਲ("-x", "--xml",
                        nargs='?',
                        ਅਚਲ="parameters.xml",
                        metavar="FILENAME",
                        help="Create XML file"
                             " (default FILENAME: parameters.xml)")
    parser.ਜੋੜੋ_ਦਲੀਲ("-b", "--board",
                        nargs='?',
                        ਅਚਲ="",
                        metavar="BOARD",
                        help="Board to create xml parameter xml for")
    parser.ਜੋੜੋ_ਦਲੀਲ("-m", "--markdown",
                        nargs='?',
                        ਅਚਲ="parameters.md",
                        metavar="FILENAME",
                        help="Create Markdown file"
                             " (default FILENAME: parameters.md)")
    parser.ਜੋੜੋ_ਦਲੀਲ("-j", "--json",
                        nargs='?',
                        ਅਚਲ="parameters.json",
                        metavar="FILENAME",
                        help="Create Json file"
                             " (default FILENAME: parameters.json)")
    parser.ਜੋੜੋ_ਦਲੀਲ('-v', '--verbose',
                        action='store_true',
                        help="verbose output")
    parser.ਜੋੜੋ_ਦਲੀਲ('-c', '--compress',
                        action='store_true',
                        help="compress parameter file")
    parser.ਜੋੜੋ_ਦਲੀਲ("-o", "--overrides",
                        ਮੂਲ="{}",
                        metavar="OVERRIDES",
                        help="a dict of overrides in the form of a json string")
    parser.ਜੋੜੋ_ਦਲੀਲ("--readonly-config",
                        ਮੂਲ=ਕੁਝ_ਨਹੀਂ,
                        metavar="FILENAME",
                        help="path to readonly_params.yaml")

    ਦਲੀਲਾਂ = parser.parse_ਦਲੀਲਾਂ()

    # Check for valid command
    ਜੇ ਨਹੀਂ (ਦਲੀਲਾਂ.xml ਜਾਂ ਦਲੀਲਾਂ.markdown ਜਾਂ ਦਲੀਲਾਂ.json):
        ਛਾਪੋ("Error: You need to specify at least one output method!\n")
        parser.print_usage()
        sys.exit(1)

    # Initialize source scanner and parser
    scanner = srcscanner.ਸਰੋਤ_Scanner()
    parser = srcparser.ਸਰੋਤ_Parser()

    # Scan directories, and parse the files
    ਜੇ (ਦਲੀਲਾਂ.verbose):
        ਛਾਪੋ("Scanning source path " + str(ਦਲੀਲਾਂ.src_ਰਸਤਾ))

    ਜੇ ਨਹੀਂ scanner.ਜਾਂਚ_Dir(ਦਲੀਲਾਂ.src_ਰਸਤਾ, parser):
        sys.exit(1)

    ਜੇ ਨਹੀਂ parser.Validate():
        sys.exit(1)
    param_groups = parser.GetParamGroups()

    ਜੇ ਲੰਬਾਈ(param_groups) == 0:
        ਛਾਪੋ("Warning: no parameters found")


    #inject parameters at front of set
    cur_dir = os.ਰਸਤਾ.dirname(os.ਰਸਤਾ.realpath(__file__))

    override_dict = json.loads(ਦਲੀਲਾਂ.overrides)
    ਜੇ ਲੰਬਾਈ(override_dict.ਕੁੰਜੀਆਂ()) > 0:
        ਲਈ ਸਮੂਹ ਵਿੱਚ param_groups:
            ਲਈ param ਵਿੱਚ ਸਮੂਹ.GetParams():
                name = param.GetName()
                ਜੇ name ਵਿੱਚ override_dict.ਕੁੰਜੀਆਂ():
                    val = str(override_dict[param.GetName()])
                    param.ਮੂਲ = val
                    ਛਾਪੋ("OVERRIDING {:s} to {:s}!!!!!".format(name, val))

    # Mark readonly parameters
    ਜੇ ਦਲੀਲਾਂ.readonly_config:
        all_param_ਨਾਂ = ਸੈੱਟ()
        ਲਈ ਸਮੂਹ ਵਿੱਚ param_groups:
            ਲਈ param ਵਿੱਚ ਸਮੂਹ.GetParams():
                all_param_ਨਾਂ.ਜੋੜੋ(param.GetName())
        readonly_params = readonly_config.ਲੱਦੋ_readonly_params(ਦਲੀਲਾਂ.readonly_config, all_param_ਨਾਂ)
        ਲਈ ਸਮੂਹ ਵਿੱਚ param_groups:
            ਲਈ param ਵਿੱਚ ਸਮੂਹ.GetParams():
                ਜੇ param.GetName() ਵਿੱਚ readonly_params:
                    param.ਸੈੱਟ_Readonly()

    output_files = []

    # Output to XML file
    ਜੇ ਦਲੀਲਾਂ.xml:
        ਜੇ ਦਲੀਲਾਂ.verbose:
            ਛਾਪੋ("Creating XML file " + ਦਲੀਲਾਂ.xml)
        ਬਾਹਰਲਾ = xmlout.XMLOutput(param_groups, ਦਲੀਲਾਂ.board)
        ਬਾਹਰਲਾ.Save(ਦਲੀਲਾਂ.xml)
        output_files.append(ਦਲੀਲਾਂ.xml)

    # Output to Markdown/HTML tables
    ਜੇ ਦਲੀਲਾਂ.markdown:
        ਜੇ ਦਲੀਲਾਂ.verbose:
            ਛਾਪੋ("Creating markdown file " + ਦਲੀਲਾਂ.markdown)
        ਬਾਹਰਲਾ = markdownout.MarkdownTablesOutput(param_groups)
        ਬਾਹਰਲਾ.Save(ਦਲੀਲਾਂ.markdown)
        output_files.append(ਦਲੀਲਾਂ.markdown)

    # Output to JSON file
    ਜੇ ਦਲੀਲਾਂ.json:
        ਜੇ ਦਲੀਲਾਂ.verbose:
            ਛਾਪੋ("Creating Json file " + ਦਲੀਲਾਂ.json)
        cur_dir = os.ਰਸਤਾ.dirname(os.ਰਸਤਾ.realpath(__file__))
        ਬਾਹਰਲਾ = jsonout.JsonOutput(param_groups, ਦਲੀਲਾਂ.board)
        ਬਾਹਰਲਾ.Save(ਦਲੀਲਾਂ.json)
        output_files.append(ਦਲੀਲਾਂ.json)

    ਜੇ ਦਲੀਲਾਂ.compress:
        ਲਈ f ਵਿੱਚ output_files:
            ਜੇ ਦਲੀਲਾਂ.verbose:
                ਛਾਪੋ("Compressing file " + f)
            save_compressed(f)


ਜੇ __name__ == "__main__":
    ਮੁੱਖ()
