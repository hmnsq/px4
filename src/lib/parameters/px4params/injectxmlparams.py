#!/usr/bin/env python
# Injects params from XML file into a param_group
#

من __future__ استورد print_function
من px4params استورد srcscanner, srcparser, xmlout, markdownout
من .srcparser استورد Parameter_گروهه, Parameter

استورد xml.etree.Element_وڻ كـ ET
استورد sys


صنف XMLInject():
    وصف __init__(self, injected_xml_filename):
        self.groups=[]

        valid_parameter_خاصيتون = سيٽ(["category", "default", "name", "type", "volatile", "boolean"])
        valid_ميدان_ٽيگ_فهرست = سيٽ(["board","short_desc", "long_desc", "min", "max", "unit", "decimal", "increment", "reboot_required"])
        valid_other_چوٽي_level_ٽيگ_فهرست = سيٽ(["group","values"])

        importtree = ET.parse(injected_xml_filename)
        injectgroups = importtree.getroot().findall("group")
        not_handled_parameter_ٽيگ_فهرست=سيٽ()
        not_handled_parameter_خاصيتون=سيٽ()
        لكل igroup في injectgroups:
            گروهه_name=igroup.get('name')
            imported_گروهه = Parameter_گروهه(گروهه_name)
            no_ڪوڊ_generation=igroup.get('no_code_generation')
            إذا no_ڪوڊ_generation:
                imported_گروهه.no_ڪوڊ_generation=no_ڪوڊ_generation
            لكل iparam في igroup:
                param_name=iparam.get('name')
                param_type=iparam.get('type')
                new_param= Parameter(param_name, param_type)
                #get other param info stored as attributes
                لكل param_attrib في iparam.attrib:
                    إذا param_attrib ليس في valid_parameter_خاصيتون:
                        not_handled_parameter_خاصيتون.جوڙيو(param_attrib)
                    وإلاإذا param_attrib == 'category':
                        new_param.سيٽ_Category(iparam.get('category'))
                    وإلاإذا param_attrib == 'default':
                        new_param.افتراضي = iparam.get('default')
                    وإلاإذا param_attrib == 'volatile':
                        new_param.سيٽ_Volatile()
                    وإلاإذا param_attrib == "boolean":
                        new_param.سيٽ_Boolean()

                #get param info stored as child tags
                لكل ٻار في iparam:
                    إذا ٻار.ٽيگ في valid_ميدان_ٽيگ_فهرست:
                        new_param.سيٽ_ميدان_2(ٻار.ٽيگ, ٻار.متن)
                    وإلاإذا ٻار.ٽيگ == 'values':
                        لكل قدر في ٻار:
                            new_param.سيٽ_Enum_قدر(قدر.get('code'), قدر.متن)
                    وإلاإذا ٻار.ٽيگ == 'bitmask':
                        لكل ٻٽو_انگ في ٻار:
                            new_param.سيٽ_Bitmask_ٻٽو_انگ(ٻٽو_انگ.get('index'), ٻٽو_انگ.متن)
                    وإلا:
                        not_handled_parameter_ٽيگ_فهرست.جوڙيو(ٻار.ٽيگ)                      
            
                imported_گروهه.جوڙيو_Parameter(new_param)
            self.groups.append(imported_گروهه)

                    
        إذا not_handled_parameter_ٽيگ_فهرست أو not_handled_parameter_خاصيتون:
            ڇاپيو("WARNING: Injected file parameter has unhandled child tags: %s" % not_handled_parameter_ٽيگ_فهرست)
            ڇاپيو("WARNING: Injected file parameter has unhandled attributes: %s" % not_handled_parameter_خاصيتون)

        

    وصف injected(self):
        أرجع self.groups
