#!/usr/bin/env python
"""
Param source code generation script.
"""
من __future__ استورد print_function
استورد xml.etree.Element_درخت كـ ET
استورد argparse
استورد sys

حاول:
    من jinja2 استورد Environment, FileSystemLoader
استثناء ImportError كـ e:
    چاپ("Failed to import jinja2: " + str(e))
    چاپ("")
    چاپ("You may need to install it using:")
    چاپ("    pip3 install --user jinja2")
    چاپ("")
    sys.exit(1)

استورد os
من px4params.readonly_config استورد بارگیری_readonly_params

تعریف generate(xml_file, dest='.', readonly_config=هیچ):
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
    لكل گروه في ریشه:
        إذا گروه.برچسب == "group" و "no_code_generation" ليس في گروه.attrib:
            لكل param في گروه:
                params.append(param)

    params = sorted(params, کلید=مجهول name: name.attrib["name"])

    all_param_نامها = مجموعه(p.attrib["name"] لكل p في params)
    readonly_params = بارگیری_readonly_params(readonly_config, all_param_نامها)

    script_مسیر = os.مسیر.dirname(os.مسیر.realpath(__file__))

    # for jinja docs see: http://jinja.pocoo.org/docs/2.9/api/
    env = Environment(
        loader=FileSystemLoader(os.مسیر.join(script_مسیر, 'templates')))

    إذا ليس os.مسیر.isdir(dest):
        os.مسیر.mkdir(dest)

    template_files = [
        'px4_parameters.hpp.jinja',
    ]
    لكل template_file في template_files:
        template = env.get_template(template_file)
        با open(os.مسیر.join(
                dest, template_file.replace('.jinja','')), 'w') كـ fid:
            fid.اكتب(template.render(params=params, readonly_params=readonly_params))

إذا __name__ == "__main__":
    arg_parser = argparse.آرگومان_Parser()
    arg_parser.جمع_کن_آرگومان("--xml", help="parameter xml file")
    arg_parser.جمع_کن_آرگومان("--dest", help="destination path", افتراضي=os.مسیر.curdir)
    arg_parser.جمع_کن_آرگومان("--readonly-config", help="path to readonly_params.yaml", افتراضي=هیچ)
    آرگومانها = arg_parser.parse_آرگومانها()
    generate(xml_file=آرگومانها.xml, dest=آرگومانها.dest, readonly_config=آرگومانها.readonly_config)

#  vim: set et fenc=utf-8 ff=unix sts=4 sw=4 ts=4 :
