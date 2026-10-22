आनयन xml.etree.Element_वृक्ष इव ET
आनयन codecs

def indent(elem, level=0):
    i = "\n" + level*"  "
    यदि len(elem):
        यदि न elem.text वा न elem.text.strip():
            elem.text = i + "  "
        यदि न elem.tail वा न elem.tail.strip():
            elem.tail = i
        प्रति elem अन्तः elem:
            indent(elem, level+1)
        यदि न elem.tail वा न elem.tail.strip():
            elem.tail = i
    अन्यथा:
        यदि level च (न elem.tail वा न elem.tail.strip()):
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
        प्रति group अन्तः groups:
            xml_group = ET.SubElement(xml_parameters, "group")
            xml_group.attrib["name"] = group.GetName()
            यदि group.no_code_generation:
                xml_group.attrib["no_code_generation"] = group.no_code_generation
            प्रति param अन्तः group.GetParams():
                यदि (last_param_name == param.GetName() च न board_specific_param_set) वा last_param_name != param.GetName():
                    xml_param = ET.SubElement(xml_group, "parameter")
                    xml_param.attrib["name"] = param.GetName()
                    xml_param.attrib["default"] = param.GetDefault()
                    xml_param.attrib["type"] = param.GetType()
                    यदि param.GetVolatile():
                        xml_param.attrib["volatile"] = "true"
                    यदि param.GetBoolean():
                        xml_param.attrib["boolean"] = "true"
                    यदि param.GetReadonly():
                        xml_param.attrib["readonly"] = "true"
                    यदि (param.GetCategory()):
                        xml_param.attrib["category"] = param.GetCategory()
                    last_param_name = param.GetName()
                    प्रति code अन्तः param.GetFieldCodes():
                        मूल्य = param.Get_Field_मूल्य(code)
                        यदि code == "board":
                            यदि मूल्य == board:
                                board_specific_param_set = सत्य
                                xml_field = ET.SubElement(xml_param, code)
                                xml_field.text = मूल्य
                            अन्यथा:
                                xml_group.remove(xml_param)
                        अन्यथा:
                            xml_field = ET.SubElement(xml_param, code)
                            xml_field.text = मूल्य
                यदि last_param_name != param.GetName():
                    board_specific_param_set = असत्य
                
                यदि len(param.GetEnumCodes()) > 0:
                    xml_मूल्यावली = ET.SubElement(xml_param, "values")
                    प्रति code अन्तः param.GetEnumCodes():
                        xml_मूल्य = ET.SubElement(xml_मूल्यावली, "value")
                        xml_मूल्य.attrib["code"] = code;
                        xml_मूल्य.text = param.Get_Enum_मूल्य(code)

                यदि len(param.GetBitmaskList()) > 0:
                    xml_मूल्यावली = ET.SubElement(xml_param, "bitmask")
                    प्रति index अन्तः param.GetBitmaskList():
                        xml_मूल्य = ET.SubElement(xml_मूल्यावली, "bit")
                        xml_मूल्य.attrib["index"] = index;
                        xml_मूल्य.text = param.GetBitmaskBit(index)

        indent(xml_parameters)
        self.xml_document = ET.Element_वृक्ष(xml_parameters)

    def Save(self, filename):
        self.xml_document.लिख(filename, encoding="UTF-8")
