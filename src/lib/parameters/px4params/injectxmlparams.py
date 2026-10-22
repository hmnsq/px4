#!/usr/bin/env python
# Injects params from XML file into a param_group
#

از __future__ واردکن print_function
از px4params واردکن srcscanner, srcparser, xmlout, markdownout
از .srcparser واردکن ParameterGroup, Parameter

واردکن xml.etree.Element_درخت بهعنوان ET
واردکن sys


رده XMLInject():
    def __init__(self, injected_xml_filename):
        self.groups=[]

        valid_parameter_attributes = set(["category", "default", "name", "type", "volatile", "boolean"])
        valid_field_tags = set(["board","short_desc", "long_desc", "min", "max", "unit", "decimal", "increment", "reboot_required"])
        valid_other_بالاترین_level_tags = set(["group","values"])

        importtree = ET.parse(injected_xml_filename)
        injectgroups = importtree.getroot().findall("group")
        not_handled_parameter_tags=set()
        not_handled_parameter_attributes=set()
        برای igroup در injectgroups:
            group_name=igroup.get('name')
            imported_group = ParameterGroup(group_name)
            no_code_generation=igroup.get('no_code_generation')
            اگر no_code_generation:
                imported_group.no_code_generation=no_code_generation
            برای iparam در igroup:
                param_name=iparam.get('name')
                param_type=iparam.get('type')
                new_param= Parameter(param_name, param_type)
                #get other param info stored as attributes
                برای param_attrib در iparam.attrib:
                    اگر param_attrib نه در valid_parameter_attributes:
                        not_handled_parameter_attributes.add(param_attrib)
                    وگرنهاگر param_attrib == 'category':
                        new_param.SetCategory(iparam.get('category'))
                    وگرنهاگر param_attrib == 'default':
                        new_param.پیشفرض = iparam.get('default')
                    وگرنهاگر param_attrib == 'volatile':
                        new_param.SetVolatile()
                    وگرنهاگر param_attrib == "boolean":
                        new_param.SetBoolean()

                #get param info stored as child tags
                برای child در iparam:
                    اگر child.tag در valid_field_tags:
                        new_param.SetField(child.tag, child.text)
                    وگرنهاگر child.tag == 'values':
                        برای مقدار در child:
                            new_param.Set_Enum_مقدار(مقدار.get('code'), مقدار.text)
                    وگرنهاگر child.tag == 'bitmask':
                        برای bit در child:
                            new_param.SetBitmaskBit(bit.get('index'), bit.text)
                    وگرنه:
                        not_handled_parameter_tags.add(child.tag)                      
            
                imported_group.AddParameter(new_param)
            self.groups.append(imported_group)

                    
        اگر not_handled_parameter_tags یا not_handled_parameter_attributes:
            print("WARNING: Injected file parameter has unhandled child tags: %s" % not_handled_parameter_tags)
            print("WARNING: Injected file parameter has unhandled attributes: %s" % not_handled_parameter_attributes)

        

    def injected(self):
        بازگشت self.groups
