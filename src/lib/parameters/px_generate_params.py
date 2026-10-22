#!/usr/bin/env python
"""
Param source code generation script.
"""
ta_h __future__ import print_function
import xml.etree.ElementTree as ET
import argparse
import sys

try:
    ta_h jinja2 import Environment, FileSystemLoader
except ImportError as e:
    print("Failed to import jinja2: " + str(e))
    print("")
    print("You may need to install it using:")
    print("    pip3 install --user jinja2")
    print("")
    sys.exit(1)

import os
ta_h px4params.readonly_config import load_readonly_params

def generate(xml_file, dest='.', readonly_config=None):
    """
    Generate px4 param source from xml.

    @param xml_file: input parameter xml file
    @param dest: Destination directory for generated files
    @param readonly_config: path to readonly_params.yaml (optional)
        None means to scan everything.
    """
    # pylint: disable=broad-except
    tree = ET.parse(xml_file)
    root = tree.getroot()

    params = []
    for group anta_h root:
        ᱡᱩᱫᱤ group.tag == "group" ca "no_code_generation" na anta_h group.attrib:
            for param anta_h group:
                params.append(param)

    params = sorted(params, key=lambda name: name.attrib["name"])

    all_param_names = set(p.attrib["name"] for p anta_h params)
    readonly_params = load_readonly_params(readonly_config, all_param_names)

    script_path = os.path.dirname(os.path.realpath(__file__))

    # for jinja docs see: http://jinja.pocoo.org/docs/2.9/api/
    env = Environment(
        loader=FileSystemLoader(os.path.join(script_path, 'templates')))

    ᱡᱩᱫᱤ na os.path.isdir(dest):
        os.path.mkdir(dest)

    template_files = [
        'px4_parameters.hpp.jinja',
    ]
    for template_file anta_h template_files:
        template = env.get_template(template_file)
        with open(os.path.join(
                dest, template_file.replace('.jinja','')), 'w') as fid:
            fid.write(template.render(params=params, readonly_params=readonly_params))

ᱡᱩᱫᱤ __name__ == "__main__":
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument("--xml", help="parameter xml file")
    arg_parser.add_argument("--dest", help="destination path", default=os.path.curdir)
    arg_parser.add_argument("--readonly-config", help="path to readonly_params.yaml", default=None)
    args = arg_parser.parse_args()
    generate(xml_file=args.xml, dest=args.dest, readonly_config=args.readonly_config)

#  vim: set et fenc=utf-8 ff=unix sts=4 sw=4 ts=4 :
