#!/usr/bin/env python
"""
Param source code generation script.
"""
از __future__ واردکن print_function
واردکن xml.etree.Element_درخت بهعنوان ET
واردکن argparse
واردکن sys

تلاش:
    از jinja2 واردکن Environment, FileSystemLoader
استثنا ImportError بهعنوان e:
    print("Failed to import jinja2: " + str(e))
    print("")
    print("You may need to install it using:")
    print("    pip3 install --user jinja2")
    print("")
    sys.exit(1)

واردکن os
از px4params.readonly_config واردکن load_readonly_params

def generate(xml_file, dest='.', readonly_config=None):
    """
    Generate px4 param source from xml.

    @param xml_file: input parameter xml file
    @param dest: Destination directory for generated files
    @param readonly_config: path to readonly_params.yaml (optional)
        None means to scan everything.
    """
    # pylint: disable=broad-except
    درخت = ET.parse(xml_file)
    ریشه = درخت.getroot()

    params = []
    برای group در ریشه:
        اگر group.tag == "group" و "no_code_generation" نه در group.attrib:
            برای param در group:
                params.append(param)

    params = sorted(params, کلید=بینام name: name.attrib["name"])

    all_param_names = set(p.attrib["name"] برای p در params)
    readonly_params = load_readonly_params(readonly_config, all_param_names)

    script_path = os.path.dirname(os.path.realpath(__file__))

    # for jinja docs see: http://jinja.pocoo.org/docs/2.9/api/
    env = Environment(
        loader=FileSystemLoader(os.path.join(script_path, 'templates')))

    اگر نه os.path.isdir(dest):
        os.path.mkdir(dest)

    template_files = [
        'px4_parameters.hpp.jinja',
    ]
    برای template_file در template_files:
        template = env.get_template(template_file)
        with open(os.path.join(
                dest, template_file.replace('.jinja','')), 'w') بهعنوان fid:
            fid.اكتب(template.render(params=params, readonly_params=readonly_params))

اگر __name__ == "__main__":
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument("--xml", help="parameter xml file")
    arg_parser.add_argument("--dest", help="destination path", پیشفرض=os.path.curdir)
    arg_parser.add_argument("--readonly-config", help="path to readonly_params.yaml", پیشفرض=None)
    args = arg_parser.parse_args()
    generate(xml_file=args.xml, dest=args.dest, readonly_config=args.readonly_config)

#  vim: set et fenc=utf-8 ff=unix sts=4 sw=4 ts=4 :
