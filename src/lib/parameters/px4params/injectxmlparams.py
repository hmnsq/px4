#!/usr/bin/env python
# Injects params from XML file into a param_group
#

من __future__ استورد print_function
من px4params استورد srcscanner, srcparser, xmlout, markdownout
من .srcparser استورد Parameter_ډله, Parameter

استورد xml.etree.Element_ونه كـ ET
استورد sys


صنف XMLInject():
    تعریف __init__(self, injected_xml_filename):
        self.groups=[]

        valid_parameter_ځانګړتیاوې = سېټ(["category", "default", "name", "type", "volatile", "boolean"])
        valid_ډګر_نښانونه = سېټ(["board","short_desc", "long_desc", "min", "max", "unit", "decimal", "increment", "reboot_required"])
        valid_other_سر_level_نښانونه = سېټ(["group","values"])

        importtree = ET.parse(injected_xml_filename)
        injectgroups = importtree.getroot().findall("group")
        not_handled_parameter_نښانونه=سېټ()
        not_handled_parameter_ځانګړتیاوې=سېټ()
        لكل igroup في injectgroups:
            ډله_name=igroup.get('name')
            imported_ډله = Parameter_ډله(ډله_name)
            no_کوډ_generation=igroup.get('no_code_generation')
            إذا no_کوډ_generation:
                imported_ډله.no_کوډ_generation=no_کوډ_generation
            لكل iparam في igroup:
                param_name=iparam.get('name')
                param_type=iparam.get('type')
                new_param= Parameter(param_name, param_type)
                #get other param info stored as attributes
                لكل param_attrib في iparam.attrib:
                    إذا param_attrib ليس في valid_parameter_ځانګړتیاوې:
                        not_handled_parameter_ځانګړتیاوې.جمع_کړه(param_attrib)
                    وإلاإذا param_attrib == 'category':
                        new_param.سېټ_Category(iparam.get('category'))
                    وإلاإذا param_attrib == 'default':
                        new_param.افتراضي = iparam.get('default')
                    وإلاإذا param_attrib == 'volatile':
                        new_param.سېټ_Volatile()
                    وإلاإذا param_attrib == "boolean":
                        new_param.سېټ_Boolean()

                #get param info stored as child tags
                لكل ماشوم في iparam:
                    إذا ماشوم.نښان في valid_ډګر_نښانونه:
                        new_param.سېټ_ډګر_2(ماشوم.نښان, ماشوم.متن)
                    وإلاإذا ماشوم.نښان == 'values':
                        لكل ارزښت في ماشوم:
                            new_param.سېټ_Enum_ارزښت(ارزښت.get('code'), ارزښت.متن)
                    وإلاإذا ماشوم.نښان == 'bitmask':
                        لكل بټ في ماشوم:
                            new_param.سېټ_Bitmask_بټ(بټ.get('index'), بټ.متن)
                    وإلا:
                        not_handled_parameter_نښانونه.جمع_کړه(ماشوم.نښان)                      
            
                imported_ډله.جمع_کړه_Parameter(new_param)
            self.groups.append(imported_ډله)

                    
        إذا not_handled_parameter_نښانونه أو not_handled_parameter_ځانګړتیاوې:
            چاپ("WARNING: Injected file parameter has unhandled child tags: %s" % not_handled_parameter_نښانونه)
            چاپ("WARNING: Injected file parameter has unhandled attributes: %s" % not_handled_parameter_ځانګړتیاوې)

        

    تعریف injected(self):
        أرجع self.groups
