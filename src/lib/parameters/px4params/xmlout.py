आनयन xml.etree.Element_वृक्षः इव ET
आनयन codecs

def indent(elem, level=0):
    i = "\n" + level*"  "
    सचे len(elem):
        सचे न elem.text वा न elem.text.strip():
            elem.text = i + "  "
        सचे न elem.tail वा न elem.tail.strip():
            elem.tail = i
        अत्थाय elem अन्तः elem:
            indent(elem, level+1)
        सचे न elem.tail वा न elem.tail.strip():
            elem.tail = i
    अञ्ञथा:
        सचे level च (न elem.tail वा न elem.tail.strip()):
            elem.tail = i

वग्ग XMLOutput():

    def __init__(self, groups, board):
        xml_parameters = ET.Element("parameters")
        xml_version = ET.SubElement(xml_parameters, "version")
        xml_version.text = "3"
        xml_version = ET.SubElement(xml_parameters, "parameter_version_major")
        xml_version.text = "1"
        xml_version = ET.SubElement(xml_parameters, "parameter_version_minor")
        xml_version.text = "15"
        last_param_name = ""
        board_specific_param_set = मिच्छा
        अत्थाय group अन्तः groups:
            xml_group = ET.SubElement(xml_parameters, "group")
            xml_group.attrib["name"] = group.GetName()
            सचे group.no_code_generation:
                xml_group.attrib["no_code_generation"] = group.no_code_generation
            अत्थाय param अन्तः group.GetParams():
                सचे (last_param_name == param.GetName() च न board_specific_param_set) वा last_param_name != param.GetName():
                    xml_param = ET.SubElement(xml_group, "parameter")
                    xml_param.attrib["name"] = param.GetName()
                    xml_param.attrib["default"] = param.GetDefault()
                    xml_param.attrib["type"] = param.GetType()
                    सचे param.GetVolatile():
                        xml_param.attrib["volatile"] = "true"
                    सचे param.GetBoolean():
                        xml_param.attrib["boolean"] = "true"
                    सचे param.GetReadonly():
                        xml_param.attrib["readonly"] = "true"
                    सचे (param.GetCategory()):
                        xml_param.attrib["category"] = param.GetCategory()
                    last_param_name = param.GetName()
                    अत्थाय code अन्तः param.GetFieldCodes():
                        मानम् = param.Get_Field_मानम्(code)
                        सचे code == "board":
                            सचे मानम् == board:
                                board_specific_param_set = सच्च
                                xml_field = ET.SubElement(xml_param, code)
                                xml_field.text = मानम्
                            अञ्ञथा:
                                xml_group.remove(xml_param)
                        अञ्ञथा:
                            xml_field = ET.SubElement(xml_param, code)
                            xml_field.text = मानम्
                सचे last_param_name != param.GetName():
                    board_specific_param_set = मिच्छा
                
                सचे len(param.GetEnumCodes()) > 0:
                    xml_मानानि = ET.SubElement(xml_param, "values")
                    अत्थाय code अन्तः param.GetEnumCodes():
                        xml_मानम् = ET.SubElement(xml_मानानि, "value")
                        xml_मानम्.attrib["code"] = code;
                        xml_मानम्.text = param.Get_Enum_मानम्(code)

                सचे len(param.GetBitmaskList()) > 0:
                    xml_मानानि = ET.SubElement(xml_param, "bitmask")
                    अत्थाय index अन्तः param.GetBitmaskList():
                        xml_मानम् = ET.SubElement(xml_मानानि, "bit")
                        xml_मानम्.attrib["index"] = index;
                        xml_मानम्.text = param.GetBitmaskBit(index)

        indent(xml_parameters)
        self.xml_document = ET.Element_वृक्षः(xml_parameters)

    def Save(self, filename):
        self.xml_document.लिख(filename, encoding="UTF-8")
