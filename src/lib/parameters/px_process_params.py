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

తః __future__ దిగుమతి print_function
దిగుమతి sys
దిగుమతి os
దిగుమతి argparse
తః px4params దిగుమతి srcscanner, srcparser, injectxmlparams, xmlout, markdownout, jsonout, readonly_config

దిగుమతి lzma #to create .xz file
దిగుమతి json

నిర్వచనం save_compressed(filename):
    #create lzma compressed version
    xz_filename=filename+'.xz'
    తోడుగా lzma.open(xz_filename, 'wt', preset=9) ఇవ f:
        తోడుగా open(filename, 'r') ఇవ content_file:
            f.లిఖ(content_file.పఠ())



నిర్వచనం ప్రధాన():
    # Parse command line arguments
    parser = argparse.వాదం_Parser(description="Process parameter documentation.")
    parser.కూడు_వాదం("-s", "--src-path",
                        అప్రమేయ=["../src"],
                        metavar="PATH",
                        nargs='*',
                        help="one or more paths to source files to scan for parameters")
    parser.కూడు_వాదం("-x", "--xml",
                        nargs='?',
                        స్థిరరాశి="parameters.xml",
                        metavar="FILENAME",
                        help="Create XML file"
                             " (default FILENAME: parameters.xml)")
    parser.కూడు_వాదం("-b", "--board",
                        nargs='?',
                        స్థిరరాశి="",
                        metavar="BOARD",
                        help="Board to create xml parameter xml for")
    parser.కూడు_వాదం("-m", "--markdown",
                        nargs='?',
                        స్థిరరాశి="parameters.md",
                        metavar="FILENAME",
                        help="Create Markdown file"
                             " (default FILENAME: parameters.md)")
    parser.కూడు_వాదం("-j", "--json",
                        nargs='?',
                        స్థిరరాశి="parameters.json",
                        metavar="FILENAME",
                        help="Create Json file"
                             " (default FILENAME: parameters.json)")
    parser.కూడు_వాదం('-v', '--verbose',
                        action='store_true',
                        help="verbose output")
    parser.కూడు_వాదం('-c', '--compress',
                        action='store_true',
                        help="compress parameter file")
    parser.కూడు_వాదం("-o", "--overrides",
                        అప్రమేయ="{}",
                        metavar="OVERRIDES",
                        help="a dict of overrides in the form of a json string")
    parser.కూడు_వాదం("--readonly-config",
                        అప్రమేయ=ఏమీలేదు,
                        metavar="FILENAME",
                        help="path to readonly_params.yaml")

    వాదనలు = parser.parse_వాదనలు()

    # Check for valid command
    ఒకవేళ కాదు (వాదనలు.xml లేదా వాదనలు.markdown లేదా వాదనలు.json):
        ముద్రించు("Error: You need to specify at least one output method!\n")
        parser.print_usage()
        sys.exit(1)

    # Initialize source scanner and parser
    scanner = srcscanner.ఆధారం_Scanner()
    parser = srcparser.ఆధారం_Parser()

    # Scan directories, and parse the files
    ఒకవేళ (వాదనలు.verbose):
        ముద్రించు("Scanning source path " + str(వాదనలు.src_మార్గం))

    ఒకవేళ కాదు scanner.పరిశీలన_Dir(వాదనలు.src_మార్గం, parser):
        sys.exit(1)

    ఒకవేళ కాదు parser.Validate():
        sys.exit(1)
    param_groups = parser.GetParamGroups()

    ఒకవేళ పొడవు(param_groups) == 0:
        ముద్రించు("Warning: no parameters found")


    #inject parameters at front of set
    cur_dir = os.మార్గం.dirname(os.మార్గం.realpath(__file__))

    override_dict = json.loads(వాదనలు.overrides)
    ఒకవేళ పొడవు(override_dict.కీలకాలు()) > 0:
        కొరకు సమూహం లో param_groups:
            కొరకు param లో సమూహం.GetParams():
                name = param.GetName()
                ఒకవేళ name లో override_dict.కీలకాలు():
                    val = str(override_dict[param.GetName()])
                    param.అప్రమేయ = val
                    ముద్రించు("OVERRIDING {:s} to {:s}!!!!!".format(name, val))

    # Mark readonly parameters
    ఒకవేళ వాదనలు.readonly_config:
        all_param_పేర్లు = సమితి()
        కొరకు సమూహం లో param_groups:
            కొరకు param లో సమూహం.GetParams():
                all_param_పేర్లు.కూడు(param.GetName())
        readonly_params = readonly_config.ఎక్కించు_readonly_params(వాదనలు.readonly_config, all_param_పేర్లు)
        కొరకు సమూహం లో param_groups:
            కొరకు param లో సమూహం.GetParams():
                ఒకవేళ param.GetName() లో readonly_params:
                    param.సమితి_Readonly()

    output_files = []

    # Output to XML file
    ఒకవేళ వాదనలు.xml:
        ఒకవేళ వాదనలు.verbose:
            ముద్రించు("Creating XML file " + వాదనలు.xml)
        బహిర్గతం = xmlout.XMLOutput(param_groups, వాదనలు.board)
        బహిర్గతం.Save(వాదనలు.xml)
        output_files.append(వాదనలు.xml)

    # Output to Markdown/HTML tables
    ఒకవేళ వాదనలు.markdown:
        ఒకవేళ వాదనలు.verbose:
            ముద్రించు("Creating markdown file " + వాదనలు.markdown)
        బహిర్గతం = markdownout.MarkdownTablesOutput(param_groups)
        బహిర్గతం.Save(వాదనలు.markdown)
        output_files.append(వాదనలు.markdown)

    # Output to JSON file
    ఒకవేళ వాదనలు.json:
        ఒకవేళ వాదనలు.verbose:
            ముద్రించు("Creating Json file " + వాదనలు.json)
        cur_dir = os.మార్గం.dirname(os.మార్గం.realpath(__file__))
        బహిర్గతం = jsonout.JsonOutput(param_groups, వాదనలు.board)
        బహిర్గతం.Save(వాదనలు.json)
        output_files.append(వాదనలు.json)

    ఒకవేళ వాదనలు.compress:
        కొరకు f లో output_files:
            ఒకవేళ వాదనలు.verbose:
                ముద్రించు("Compressing file " + f)
            save_compressed(f)


ఒకవేళ __name__ == "__main__":
    ప్రధాన()
