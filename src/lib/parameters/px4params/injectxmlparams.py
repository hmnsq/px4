#!/usr/bin/env python
# Injects params from XML file into a param_group
#

من __future__ استورد print_function
من px4params استورد srcscanner, srcparser, xmlout, markdownout
من .srcparser استورد ParameterGroup, Parameter

استورد xml.etree.Element_ڤوکوق كـ ET
استورد sys


صنف XMLInject():
    def __init__(self, injected_xml_filename):
        self.groups=[]

        valid_parameter_attributes = set(["category", "default", "name", "type", "volatile", "boolean"])
        valid_field_tags = set(["board","short_desc", "long_desc", "min", "max", "unit", "decimal", "increment", "reboot_required"])
        valid_other_اتس_level_tags = set(["group","values"])

        importtree = ET.parse(injected_xml_filename)
        injectgroups = importtree.getroot().findall("group")
        not_handled_parameter_tags=set()
        not_handled_parameter_attributes=set()
        لكل igroup في injectgroups:
            group_name=igroup.get('name')
            imported_group = ParameterGroup(group_name)
            no_code_generation=igroup.get('no_code_generation')
            إذا no_code_generation:
                imported_group.no_code_generation=no_code_generation
            لكل iparam في igroup:
                param_name=iparam.get('name')
                param_type=iparam.get('type')
                new_param= Parameter(param_name, param_type)
                #get other param info stored as attributes
                لكل param_attrib في iparam.attrib:
                    إذا param_attrib ليس في valid_parameter_attributes:
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
                لكل child في iparam:
                    إذا child.tag في valid_field_tags:
                        new_param.SetField(child.tag, child.text)
                    وإلاإذا child.tag == 'values':
                        لكل نيلاي في child:
                            new_param.Set_Enum_نيلاي(نيلاي.get('code'), نيلاي.text)
                    وإلاإذا child.tag == 'bitmask':
                        لكل bit في child:
                            new_param.SetBitmaskBit(bit.get('index'), bit.text)
                    وإلا:
                        not_handled_parameter_tags.add(child.tag)                      
            
                imported_group.AddParameter(new_param)
            self.groups.append(imported_group)

                    
        إذا not_handled_parameter_tags أو not_handled_parameter_attributes:
            print("WARNING: Injected file parameter has unhandled child tags: %s" % not_handled_parameter_tags)
            print("WARNING: Injected file parameter has unhandled attributes: %s" % not_handled_parameter_attributes)

        

    def injected(self):
        أرجع self.groups
