ಆನಯನ xml.etree.Element_ವೃಕ್ಷ ಇವ ET
ಆನಯನ codecs

def indent(elem, level=0):
    i = "\n" + level*"  "
    ಯದಿ len(elem):
        ಯದಿ ನ elem.text ವಾ ನ elem.text.strip():
            elem.text = i + "  "
        ಯದಿ ನ elem.tail ವಾ ನ elem.tail.strip():
            elem.tail = i
        ಪ್ರತಿ elem ಅನ್ತಃ elem:
            indent(elem, level+1)
        ಯದಿ ನ elem.tail ವಾ ನ elem.tail.strip():
            elem.tail = i
    ಅನ್ಯಥಾ:
        ಯದಿ level ಚ (ನ elem.tail ವಾ ನ elem.tail.strip()):
            elem.tail = i

ವರ್ಗ XMLOutput():

    def __init__(self, groups, board):
        xml_parameters = ET.Element("parameters")
        xml_version = ET.SubElement(xml_parameters, "version")
        xml_version.text = "3"
        xml_version = ET.SubElement(xml_parameters, "parameter_version_major")
        xml_version.text = "1"
        xml_version = ET.SubElement(xml_parameters, "parameter_version_minor")
        xml_version.text = "15"
        last_param_name = ""
        board_specific_param_set = ಅಸತ್ಯ
        ಪ್ರತಿ group ಅನ್ತಃ groups:
            xml_group = ET.SubElement(xml_parameters, "group")
            xml_group.attrib["name"] = group.GetName()
            ಯದಿ group.no_code_generation:
                xml_group.attrib["no_code_generation"] = group.no_code_generation
            ಪ್ರತಿ param ಅನ್ತಃ group.GetParams():
                ಯದಿ (last_param_name == param.GetName() ಚ ನ board_specific_param_set) ವಾ last_param_name != param.GetName():
                    xml_param = ET.SubElement(xml_group, "parameter")
                    xml_param.attrib["name"] = param.GetName()
                    xml_param.attrib["default"] = param.GetDefault()
                    xml_param.attrib["type"] = param.GetType()
                    ಯದಿ param.GetVolatile():
                        xml_param.attrib["volatile"] = "true"
                    ಯದಿ param.GetBoolean():
                        xml_param.attrib["boolean"] = "true"
                    ಯದಿ param.GetReadonly():
                        xml_param.attrib["readonly"] = "true"
                    ಯದಿ (param.GetCategory()):
                        xml_param.attrib["category"] = param.GetCategory()
                    last_param_name = param.GetName()
                    ಪ್ರತಿ code ಅನ್ತಃ param.GetFieldCodes():
                        ಮೌಲ್ಯ = param.Get_Field_ಮೌಲ್ಯ(code)
                        ಯದಿ code == "board":
                            ಯದಿ ಮೌಲ್ಯ == board:
                                board_specific_param_set = ಸತ್ಯ
                                xml_field = ET.SubElement(xml_param, code)
                                xml_field.text = ಮೌಲ್ಯ
                            ಅನ್ಯಥಾ:
                                xml_group.remove(xml_param)
                        ಅನ್ಯಥಾ:
                            xml_field = ET.SubElement(xml_param, code)
                            xml_field.text = ಮೌಲ್ಯ
                ಯದಿ last_param_name != param.GetName():
                    board_specific_param_set = ಅಸತ್ಯ
                
                ಯದಿ len(param.GetEnumCodes()) > 0:
                    xml_ಮೌಲ್ಯಗಳು = ET.SubElement(xml_param, "values")
                    ಪ್ರತಿ code ಅನ್ತಃ param.GetEnumCodes():
                        xml_ಮೌಲ್ಯ = ET.SubElement(xml_ಮೌಲ್ಯಗಳು, "value")
                        xml_ಮೌಲ್ಯ.attrib["code"] = code;
                        xml_ಮೌಲ್ಯ.text = param.Get_Enum_ಮೌಲ್ಯ(code)

                ಯದಿ len(param.GetBitmaskList()) > 0:
                    xml_ಮೌಲ್ಯಗಳು = ET.SubElement(xml_param, "bitmask")
                    ಪ್ರತಿ index ಅನ್ತಃ param.GetBitmaskList():
                        xml_ಮೌಲ್ಯ = ET.SubElement(xml_ಮೌಲ್ಯಗಳು, "bit")
                        xml_ಮೌಲ್ಯ.attrib["index"] = index;
                        xml_ಮೌಲ್ಯ.text = param.GetBitmaskBit(index)

        indent(xml_parameters)
        self.xml_document = ET.Element_ವೃಕ್ಷ(xml_parameters)

    def Save(self, filename):
        self.xml_document.ಲಿಖ(filename, encoding="UTF-8")
