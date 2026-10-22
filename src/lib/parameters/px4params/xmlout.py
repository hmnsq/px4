आयात xml.etree.Element_वृक्ष इव ET
आयात codecs

def indent(elem, level=0):
    i = "\n" + level*"  "
    जर len(elem):
        जर नाही elem.text किंवा नाही elem.text.strip():
            elem.text = i + "  "
        जर नाही elem.tail किंवा नाही elem.tail.strip():
            elem.tail = i
        साठी elem मध्ये elem:
            indent(elem, level+1)
        जर नाही elem.tail किंवा नाही elem.tail.strip():
            elem.tail = i
    अन्यथा:
        जर level आणि (नाही elem.tail किंवा नाही elem.tail.strip()):
            elem.tail = i

वर्ग XMLOutput():

    def __init__(self, groups, board):
        xml_parameters = ET.Element("parameters")
        xml_version = ET.SubElement(xml_parameters, "version")
        xml_version.text = "3"
        xml_version = ET.SubElement(xml_parameters, "parameter_version_major")
        xml_version.text = "1"
        xml_version = ET.SubElement(xml_parameters, "parameter_version_minor")
        xml_version.text = "15"
        last_param_name = ""
        board_specific_param_set = असत्य
        साठी group मध्ये groups:
            xml_group = ET.SubElement(xml_parameters, "group")
            xml_group.attrib["name"] = group.GetName()
            जर group.no_code_generation:
                xml_group.attrib["no_code_generation"] = group.no_code_generation
            साठी param मध्ये group.GetParams():
                जर (last_param_name == param.GetName() आणि नाही board_specific_param_set) किंवा last_param_name != param.GetName():
                    xml_param = ET.SubElement(xml_group, "parameter")
                    xml_param.attrib["name"] = param.GetName()
                    xml_param.attrib["default"] = param.GetDefault()
                    xml_param.attrib["type"] = param.GetType()
                    जर param.GetVolatile():
                        xml_param.attrib["volatile"] = "true"
                    जर param.GetBoolean():
                        xml_param.attrib["boolean"] = "true"
                    जर param.GetReadonly():
                        xml_param.attrib["readonly"] = "true"
                    जर (param.GetCategory()):
                        xml_param.attrib["category"] = param.GetCategory()
                    last_param_name = param.GetName()
                    साठी code मध्ये param.GetFieldCodes():
                        मूल्य = param.Get_Field_मूल्य(code)
                        जर code == "board":
                            जर मूल्य == board:
                                board_specific_param_set = सत्य
                                xml_field = ET.SubElement(xml_param, code)
                                xml_field.text = मूल्य
                            अन्यथा:
                                xml_group.remove(xml_param)
                        अन्यथा:
                            xml_field = ET.SubElement(xml_param, code)
                            xml_field.text = मूल्य
                जर last_param_name != param.GetName():
                    board_specific_param_set = असत्य
                
                जर len(param.GetEnumCodes()) > 0:
                    xml_मूल्ये = ET.SubElement(xml_param, "values")
                    साठी code मध्ये param.GetEnumCodes():
                        xml_मूल्य = ET.SubElement(xml_मूल्ये, "value")
                        xml_मूल्य.attrib["code"] = code;
                        xml_मूल्य.text = param.Get_Enum_मूल्य(code)

                जर len(param.GetBitmaskList()) > 0:
                    xml_मूल्ये = ET.SubElement(xml_param, "bitmask")
                    साठी index मध्ये param.GetBitmaskList():
                        xml_मूल्य = ET.SubElement(xml_मूल्ये, "bit")
                        xml_मूल्य.attrib["index"] = index;
                        xml_मूल्य.text = param.GetBitmaskBit(index)

        indent(xml_parameters)
        self.xml_document = ET.Element_वृक्ष(xml_parameters)

    def Save(self, filename):
        self.xml_document.लिख(filename, encoding="UTF-8")
