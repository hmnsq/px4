இறக்குமதி xml.etree.Element_மரம் இவ ET
இறக்குமதி codecs

def indent(elem, level=0):
    i = "\n" + level*"  "
    எனில் len(elem):
        எனில் இல்லை elem.text அல்லது இல்லை elem.text.strip():
            elem.text = i + "  "
        எனில் இல்லை elem.tail அல்லது இல்லை elem.tail.strip():
            elem.tail = i
        க்கு elem உள் elem:
            indent(elem, level+1)
        எனில் இல்லை elem.tail அல்லது இல்லை elem.tail.strip():
            elem.tail = i
    இல்லையெனில்:
        எனில் level மற்றும் (இல்லை elem.tail அல்லது இல்லை elem.tail.strip()):
            elem.tail = i

வகுப்பு XMLOutput():

    def __init__(self, groups, board):
        xml_parameters = ET.Element("parameters")
        xml_version = ET.SubElement(xml_parameters, "version")
        xml_version.text = "3"
        xml_version = ET.SubElement(xml_parameters, "parameter_version_major")
        xml_version.text = "1"
        xml_version = ET.SubElement(xml_parameters, "parameter_version_minor")
        xml_version.text = "15"
        last_param_name = ""
        board_specific_param_set = பொய்
        க்கு group உள் groups:
            xml_group = ET.SubElement(xml_parameters, "group")
            xml_group.attrib["name"] = group.GetName()
            எனில் group.no_code_generation:
                xml_group.attrib["no_code_generation"] = group.no_code_generation
            க்கு param உள் group.GetParams():
                எனில் (last_param_name == param.GetName() மற்றும் இல்லை board_specific_param_set) அல்லது last_param_name != param.GetName():
                    xml_param = ET.SubElement(xml_group, "parameter")
                    xml_param.attrib["name"] = param.GetName()
                    xml_param.attrib["default"] = param.GetDefault()
                    xml_param.attrib["type"] = param.GetType()
                    எனில் param.GetVolatile():
                        xml_param.attrib["volatile"] = "true"
                    எனில் param.GetBoolean():
                        xml_param.attrib["boolean"] = "true"
                    எனில் param.GetReadonly():
                        xml_param.attrib["readonly"] = "true"
                    எனில் (param.GetCategory()):
                        xml_param.attrib["category"] = param.GetCategory()
                    last_param_name = param.GetName()
                    க்கு code உள் param.GetFieldCodes():
                        மதிப்பு = param.Get_Field_மதிப்பு(code)
                        எனில் code == "board":
                            எனில் மதிப்பு == board:
                                board_specific_param_set = உண்மை
                                xml_field = ET.SubElement(xml_param, code)
                                xml_field.text = மதிப்பு
                            இல்லையெனில்:
                                xml_group.remove(xml_param)
                        இல்லையெனில்:
                            xml_field = ET.SubElement(xml_param, code)
                            xml_field.text = மதிப்பு
                எனில் last_param_name != param.GetName():
                    board_specific_param_set = பொய்
                
                எனில் len(param.GetEnumCodes()) > 0:
                    xml_மதிப்புகள் = ET.SubElement(xml_param, "values")
                    க்கு code உள் param.GetEnumCodes():
                        xml_மதிப்பு = ET.SubElement(xml_மதிப்புகள், "value")
                        xml_மதிப்பு.attrib["code"] = code;
                        xml_மதிப்பு.text = param.Get_Enum_மதிப்பு(code)

                எனில் len(param.GetBitmaskList()) > 0:
                    xml_மதிப்புகள் = ET.SubElement(xml_param, "bitmask")
                    க்கு index உள் param.GetBitmaskList():
                        xml_மதிப்பு = ET.SubElement(xml_மதிப்புகள், "bit")
                        xml_மதிப்பு.attrib["index"] = index;
                        xml_மதிப்பு.text = param.GetBitmaskBit(index)

        indent(xml_parameters)
        self.xml_document = ET.Element_மரம்(xml_parameters)

    def Save(self, filename):
        self.xml_document.லிக(filename, encoding="UTF-8")
