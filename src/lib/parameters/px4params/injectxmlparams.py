#!/usr/bin/env python
# Injects params from XML file into a param_group
#

ତଃ __future__ ଆମଦାନୀ print_function
ତଃ px4params ଆମଦାନୀ srcscanner, srcparser, xmlout, markdownout
ତଃ .srcparser ଆମଦାନୀ ParameterGroup, Parameter

ଆମଦାନୀ xml.etree.Element_ବୃକ୍ଷ ଇଵ ET
ଆମଦାନୀ sys


ଶ୍ରେଣୀ XMLInject():
    def __init__(self, injected_xml_filename):
        self.groups=[]

        valid_parameter_attributes = set(["category", "default", "name", "type", "volatile", "boolean"])
        valid_field_tags = set(["board","short_desc", "long_desc", "min", "max", "unit", "decimal", "increment", "reboot_required"])
        valid_other_ଶୀର୍ଷ_level_tags = set(["group","values"])

        importtree = ET.parse(injected_xml_filename)
        injectgroups = importtree.getroot().findall("group")
        not_handled_parameter_tags=set()
        not_handled_parameter_attributes=set()
        ପାଇଁ igroup ଭିତରେ injectgroups:
            group_name=igroup.get('name')
            imported_group = ParameterGroup(group_name)
            no_code_generation=igroup.get('no_code_generation')
            ଯଦି no_code_generation:
                imported_group.no_code_generation=no_code_generation
            ପାଇଁ iparam ଭିତରେ igroup:
                param_name=iparam.get('name')
                param_type=iparam.get('type')
                new_param= Parameter(param_name, param_type)
                #get other param info stored as attributes
                ପାଇଁ param_attrib ଭିତରେ iparam.attrib:
                    ଯଦି param_attrib ନୁହେଁ ଭିତରେ valid_parameter_attributes:
                        not_handled_parameter_attributes.add(param_attrib)
                    ନଚେତଯଦି param_attrib == 'category':
                        new_param.SetCategory(iparam.get('category'))
                    ନଚେତଯଦି param_attrib == 'default':
                        new_param.ପୂର୍ବନିର୍ଦ୍ଧାରିତ = iparam.get('default')
                    ନଚେତଯଦି param_attrib == 'volatile':
                        new_param.SetVolatile()
                    ନଚେତଯଦି param_attrib == "boolean":
                        new_param.SetBoolean()

                #get param info stored as child tags
                ପାଇଁ child ଭିତରେ iparam:
                    ଯଦି child.tag ଭିତରେ valid_field_tags:
                        new_param.SetField(child.tag, child.text)
                    ନଚେତଯଦି child.tag == 'values':
                        ପାଇଁ ମୂଲ୍ୟ ଭିତରେ child:
                            new_param.Set_Enum_ମୂଲ୍ୟ(ମୂଲ୍ୟ.get('code'), ମୂଲ୍ୟ.text)
                    ନଚେତଯଦି child.tag == 'bitmask':
                        ପାଇଁ bit ଭିତରେ child:
                            new_param.SetBitmaskBit(bit.get('index'), bit.text)
                    ଅନ୍ୟଥା:
                        not_handled_parameter_tags.add(child.tag)                      
            
                imported_group.AddParameter(new_param)
            self.groups.append(imported_group)

                    
        ଯଦି not_handled_parameter_tags କିମ୍ବା not_handled_parameter_attributes:
            print("WARNING: Injected file parameter has unhandled child tags: %s" % not_handled_parameter_tags)
            print("WARNING: Injected file parameter has unhandled attributes: %s" % not_handled_parameter_attributes)

        

    def injected(self):
        ଫେରନ୍ତୁ self.groups
