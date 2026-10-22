#!/usr/bin/env python
"""
Param source code generation script.
"""
ତଃ __future__ ଆମଦାନୀ print_function
ଆମଦାନୀ xml.etree.Element_ବୃକ୍ଷ ଇଵ ET
ଆମଦାନୀ argparse
ଆମଦାନୀ sys

ଚେଷ୍ଟା:
    ତଃ jinja2 ଆମଦାନୀ Environment, FileSystemLoader
ବ୍ୟତିକ୍ରମ ImportError ଇଵ e:
    print("Failed to import jinja2: " + str(e))
    print("")
    print("You may need to install it using:")
    print("    pip3 install --user jinja2")
    print("")
    sys.exit(1)

ଆମଦାନୀ os
ତଃ px4params.readonly_config ଆମଦାନୀ load_readonly_params

def generate(xml_file, dest='.', readonly_config=None):
    """
    Generate px4 param source from xml.

    @param xml_file: input parameter xml file
    @param dest: Destination directory for generated files
    @param readonly_config: path to readonly_params.yaml (optional)
        None means to scan everything.
    """
    # pylint: disable=broad-except
    ବୃକ୍ଷ = ET.parse(xml_file)
    ମୂଳ = ବୃକ୍ଷ.getroot()

    params = []
    ପାଇଁ group ଭିତରେ ମୂଳ:
        ଯଦି group.tag == "group" ଏବଂ "no_code_generation" ନୁହେଁ ଭିତରେ group.attrib:
            ପାଇଁ param ଭିତରେ group:
                params.append(param)

    params = sorted(params, ଚାବି=ଅନାମ name: name.attrib["name"])

    all_param_names = set(p.attrib["name"] ପାଇଁ p ଭିତରେ params)
    readonly_params = load_readonly_params(readonly_config, all_param_names)

    script_path = os.path.dirname(os.path.realpath(__file__))

    # for jinja docs see: http://jinja.pocoo.org/docs/2.9/api/
    env = Environment(
        loader=FileSystemLoader(os.path.join(script_path, 'templates')))

    ଯଦି ନୁହେଁ os.path.isdir(dest):
        os.path.mkdir(dest)

    template_files = [
        'px4_parameters.hpp.jinja',
    ]
    ପାଇଁ template_file ଭିତରେ template_files:
        template = env.get_template(template_file)
        with open(os.path.join(
                dest, template_file.replace('.jinja','')), 'w') ଇଵ fid:
            fid.ଲିଖ(template.render(params=params, readonly_params=readonly_params))

ଯଦି __name__ == "__main__":
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument("--xml", help="parameter xml file")
    arg_parser.add_argument("--dest", help="destination path", ପୂର୍ବନିର୍ଦ୍ଧାରିତ=os.path.curdir)
    arg_parser.add_argument("--readonly-config", help="path to readonly_params.yaml", ପୂର୍ବନିର୍ଦ୍ଧାରିତ=None)
    args = arg_parser.parse_args()
    generate(xml_file=args.xml, dest=args.dest, readonly_config=args.readonly_config)

#  vim: set et fenc=utf-8 ff=unix sts=4 sw=4 ts=4 :
