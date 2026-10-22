#!/usr/bin/env python
# Injects params from XML file into a param_group
#

ਤਃ __future__ ਆਯਾਤ print_function
ਤਃ px4params ਆਯਾਤ srcscanner, srcparser, xmlout, markdownout
ਤਃ .srcparser ਆਯਾਤ Parameter_ਸਮੂਹ, Parameter

ਆਯਾਤ xml.etree.Element_ਰੁੱਖ ਇਵ ET
ਆਯਾਤ sys


ਵਰਗ XMLInject():
    ਪਰਿਭਾਸ਼ਾ __init__(self, injected_xml_filename):
        self.groups=[]

        valid_parameter_ਗੁਣ = ਸੈੱਟ(["category", "default", "name", "type", "volatile", "boolean"])
        valid_ਖੇਤਰ_ਟੈਗ_ਸੂਚੀ = ਸੈੱਟ(["board","short_desc", "long_desc", "min", "max", "unit", "decimal", "increment", "reboot_required"])
        valid_other_ਸਿਖਰ_level_ਟੈਗ_ਸੂਚੀ = ਸੈੱਟ(["group","values"])

        importtree = ET.parse(injected_xml_filename)
        injectgroups = importtree.getroot().findall("group")
        not_handled_parameter_ਟੈਗ_ਸੂਚੀ=ਸੈੱਟ()
        not_handled_parameter_ਗੁਣ=ਸੈੱਟ()
        ਲਈ igroup ਵਿੱਚ injectgroups:
            ਸਮੂਹ_name=igroup.get('name')
            imported_ਸਮੂਹ = Parameter_ਸਮੂਹ(ਸਮੂਹ_name)
            no_ਕੋਡ_generation=igroup.get('no_code_generation')
            ਜੇ no_ਕੋਡ_generation:
                imported_ਸਮੂਹ.no_ਕੋਡ_generation=no_ਕੋਡ_generation
            ਲਈ iparam ਵਿੱਚ igroup:
                param_name=iparam.get('name')
                param_type=iparam.get('type')
                new_param= Parameter(param_name, param_type)
                #get other param info stored as attributes
                ਲਈ param_attrib ਵਿੱਚ iparam.attrib:
                    ਜੇ param_attrib ਨਹੀਂ ਵਿੱਚ valid_parameter_ਗੁਣ:
                        not_handled_parameter_ਗੁਣ.ਜੋੜੋ(param_attrib)
                    ਜਾਂਜੇ param_attrib == 'category':
                        new_param.ਸੈੱਟ_Category(iparam.get('category'))
                    ਜਾਂਜੇ param_attrib == 'default':
                        new_param.ਮੂਲ = iparam.get('default')
                    ਜਾਂਜੇ param_attrib == 'volatile':
                        new_param.ਸੈੱਟ_Volatile()
                    ਜਾਂਜੇ param_attrib == "boolean":
                        new_param.ਸੈੱਟ_Boolean()

                #get param info stored as child tags
                ਲਈ ਬੱਚਾ ਵਿੱਚ iparam:
                    ਜੇ ਬੱਚਾ.ਟੈਗ ਵਿੱਚ valid_ਖੇਤਰ_ਟੈਗ_ਸੂਚੀ:
                        new_param.ਸੈੱਟ_ਖੇਤਰ_2(ਬੱਚਾ.ਟੈਗ, ਬੱਚਾ.ਪਾਠ)
                    ਜਾਂਜੇ ਬੱਚਾ.ਟੈਗ == 'values':
                        ਲਈ ਮੁੱਲ ਵਿੱਚ ਬੱਚਾ:
                            new_param.ਸੈੱਟ_Enum_ਮੁੱਲ(ਮੁੱਲ.get('code'), ਮੁੱਲ.ਪਾਠ)
                    ਜਾਂਜੇ ਬੱਚਾ.ਟੈਗ == 'bitmask':
                        ਲਈ ਦੋਆਧਾਰੀ_ਅੰਕ ਵਿੱਚ ਬੱਚਾ:
                            new_param.ਸੈੱਟ_Bitmask_ਦੋਆਧਾਰੀ_ਅੰਕ(ਦੋਆਧਾਰੀ_ਅੰਕ.get('index'), ਦੋਆਧਾਰੀ_ਅੰਕ.ਪਾਠ)
                    ਨਹੀਂਤਾਂ:
                        not_handled_parameter_ਟੈਗ_ਸੂਚੀ.ਜੋੜੋ(ਬੱਚਾ.ਟੈਗ)                      
            
                imported_ਸਮੂਹ.ਜੋੜੋ_Parameter(new_param)
            self.groups.append(imported_ਸਮੂਹ)

                    
        ਜੇ not_handled_parameter_ਟੈਗ_ਸੂਚੀ ਜਾਂ not_handled_parameter_ਗੁਣ:
            ਛਾਪੋ("WARNING: Injected file parameter has unhandled child tags: %s" % not_handled_parameter_ਟੈਗ_ਸੂਚੀ)
            ਛਾਪੋ("WARNING: Injected file parameter has unhandled attributes: %s" % not_handled_parameter_ਗੁਣ)

        

    ਪਰਿਭਾਸ਼ਾ injected(self):
        ਵਾਪਸ self.groups
