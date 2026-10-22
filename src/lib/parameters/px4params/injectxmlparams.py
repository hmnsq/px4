#!/usr/bin/env python
# Injects params from XML file into a param_group
#

من __future__ استورد print_function
من px4params استورد srcscanner, srcparser, xmlout, markdownout
من .srcparser استورد ParameterGroup, Parameter

استورد xml.etree.ElementTree كـ ET
استورد sys


מחלקה XMLInject():
    def __init__(self, injected_xml_filename):
        self.groups=[]

        valid_parameter_attributes = set(["category", "default", "name", "type", "volatile", "boolean"])
        valid_field_tags = set(["board","short_desc", "long_desc", "min", "max", "unit", "decimal", "increment", "reboot_required"])
        valid_other_top_level_tags = set(["group","values"])

        importtree = ET.parse(injected_xml_filename)
        injectgroups = importtree.getroot().findall("group")
        not_handled_parameter_tags=set()
        not_handled_parameter_attributes=set()
        עבור igroup في injectgroups:
            group_name=igroup.get('name')
            imported_group = ParameterGroup(group_name)
            no_code_generation=igroup.get('no_code_generation')
            אם no_code_generation:
                imported_group.no_code_generation=no_code_generation
            עבור iparam في igroup:
                param_name=iparam.get('name')
                param_type=iparam.get('type')
                new_param= Parameter(param_name, param_type)
                #get other param info stored as attributes
                עבור param_attrib في iparam.attrib:
                    אם param_attrib לא في valid_parameter_attributes:
                        not_handled_parameter_attributes.add(param_attrib)
                    وإلاإذا param_attrib == 'category':
                        new_param.SetCategory(iparam.get('category'))
                    وإلاإذا param_attrib == 'default':
                        new_param.افتراضي = iparam.get('default')
                    وإلاإذا param_attrib == 'volatile':
                        new_param.SetVolatile()
                    وإلاإذا param_attrib == "boolean":
                        new_param.SetBoolean()

                #get param info stored as child tags
                עבור child في iparam:
                    אם child.tag في valid_field_tags:
                        new_param.SetField(child.tag, child.text)
                    وإلاإذا child.tag == 'values':
                        עבור value في child:
                            new_param.SetEnumValue(value.get('code'), value.text)
                    وإلاإذا child.tag == 'bitmask':
                        עבור bit في child:
                            new_param.SetBitmaskBit(bit.get('index'), bit.text)
                    אחרת:
                        not_handled_parameter_tags.add(child.tag)                      
            
                imported_group.AddParameter(new_param)
            self.groups.append(imported_group)

                    
        אם not_handled_parameter_tags או not_handled_parameter_attributes:
            print("WARNING: Injected file parameter has unhandled child tags: %s" % not_handled_parameter_tags)
            print("WARNING: Injected file parameter has unhandled attributes: %s" % not_handled_parameter_attributes)

        

    def injected(self):
        החזר self.groups
