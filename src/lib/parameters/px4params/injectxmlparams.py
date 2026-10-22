#!/usr/bin/env python
# Injects params from XML file into a param_group
#

من __future__ استورد print_function
من px4params استورد srcscanner, srcparser, xmlout, markdownout
من .srcparser استورد Parameter_گروه, Parameter

استورد xml.etree.Element_درخت كـ ET
استورد sys


صنف XMLInject():
    تعریف __init__(self, injected_xml_filename):
        self.groups=[]

        valid_parameter_صفات = مجموعه(["category", "default", "name", "type", "volatile", "boolean"])
        valid_فیلد_برچسبها = مجموعه(["board","short_desc", "long_desc", "min", "max", "unit", "decimal", "increment", "reboot_required"])
        valid_other_بالاترین_level_برچسبها = مجموعه(["group","values"])

        importtree = ET.parse(injected_xml_filename)
        injectgroups = importtree.getroot().findall("group")
        not_handled_parameter_برچسبها=مجموعه()
        not_handled_parameter_صفات=مجموعه()
        لكل igroup في injectgroups:
            گروه_name=igroup.get('name')
            imported_گروه = Parameter_گروه(گروه_name)
            no_کد_generation=igroup.get('no_code_generation')
            إذا no_کد_generation:
                imported_گروه.no_کد_generation=no_کد_generation
            لكل iparam في igroup:
                param_name=iparam.get('name')
                param_type=iparam.get('type')
                new_param= Parameter(param_name, param_type)
                #get other param info stored as attributes
                لكل param_attrib في iparam.attrib:
                    إذا param_attrib ليس في valid_parameter_صفات:
                        not_handled_parameter_صفات.جمع_کن(param_attrib)
                    وإلاإذا param_attrib == 'category':
                        new_param.مجموعه_Category(iparam.get('category'))
                    وإلاإذا param_attrib == 'default':
                        new_param.افتراضي = iparam.get('default')
                    وإلاإذا param_attrib == 'volatile':
                        new_param.مجموعه_Volatile()
                    وإلاإذا param_attrib == "boolean":
                        new_param.مجموعه_Boolean()

                #get param info stored as child tags
                لكل فرزند في iparam:
                    إذا فرزند.برچسب في valid_فیلد_برچسبها:
                        new_param.مجموعه_فیلد_2(فرزند.برچسب, فرزند.متن)
                    وإلاإذا فرزند.برچسب == 'values':
                        لكل مقدار في فرزند:
                            new_param.مجموعه_Enum_مقدار(مقدار.get('code'), مقدار.متن)
                    وإلاإذا فرزند.برچسب == 'bitmask':
                        لكل بیت في فرزند:
                            new_param.مجموعه_Bitmask_بیت(بیت.get('index'), بیت.متن)
                    وإلا:
                        not_handled_parameter_برچسبها.جمع_کن(فرزند.برچسب)                      
            
                imported_گروه.جمع_کن_Parameter(new_param)
            self.groups.append(imported_گروه)

                    
        إذا not_handled_parameter_برچسبها أو not_handled_parameter_صفات:
            چاپ("WARNING: Injected file parameter has unhandled child tags: %s" % not_handled_parameter_برچسبها)
            چاپ("WARNING: Injected file parameter has unhandled attributes: %s" % not_handled_parameter_صفات)

        

    تعریف injected(self):
        أرجع self.groups
