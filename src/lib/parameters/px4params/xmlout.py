आनयन xml.etree.Element_वृक्षः इव ET
आनयन codecs

def indent(elem, level=0):
    i = "\n" + level*"  "
    जइ len(elem):
        जइ ण elem.text वा ण elem.text.strip():
            elem.text = i + "  "
        जइ ण elem.tail वा ण elem.tail.strip():
            elem.tail = i
        प्रति elem अन्तः elem:
            indent(elem, level+1)
        जइ ण elem.tail वा ण elem.tail.strip():
            elem.tail = i
    अण्णहा:
        जइ level य (ण elem.tail वा ण elem.tail.strip()):
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
        board_specific_param_set = मिच्छ
        प्रति group अन्तः groups:
            xml_group = ET.SubElement(xml_parameters, "group")
            xml_group.attrib["name"] = group.GetName()
            जइ group.no_code_generation:
                xml_group.attrib["no_code_generation"] = group.no_code_generation
            प्रति param अन्तः group.GetParams():
                जइ (last_param_name == param.GetName() य ण board_specific_param_set) वा last_param_name != param.GetName():
                    xml_param = ET.SubElement(xml_group, "parameter")
                    xml_param.attrib["name"] = param.GetName()
                    xml_param.attrib["default"] = param.GetDefault()
                    xml_param.attrib["type"] = param.GetType()
                    जइ param.GetVolatile():
                        xml_param.attrib["volatile"] = "true"
                    जइ param.GetBoolean():
                        xml_param.attrib["boolean"] = "true"
                    जइ param.GetReadonly():
                        xml_param.attrib["readonly"] = "true"
                    जइ (param.GetCategory()):
                        xml_param.attrib["category"] = param.GetCategory()
                    last_param_name = param.GetName()
                    प्रति code अन्तः param.GetFieldCodes():
                        मानम् = param.Get_Field_मानम्(code)
                        जइ code == "board":
                            जइ मानम् == board:
                                board_specific_param_set = सच्च
                                xml_field = ET.SubElement(xml_param, code)
                                xml_field.text = मानम्
                            अण्णहा:
                                xml_group.remove(xml_param)
                        अण्णहा:
                            xml_field = ET.SubElement(xml_param, code)
                            xml_field.text = मानम्
                जइ last_param_name != param.GetName():
                    board_specific_param_set = मिच्छ
                
                जइ len(param.GetEnumCodes()) > 0:
                    xml_मानानि = ET.SubElement(xml_param, "values")
                    प्रति code अन्तः param.GetEnumCodes():
                        xml_मानम् = ET.SubElement(xml_मानानि, "value")
                        xml_मानम्.attrib["code"] = code;
                        xml_मानम्.text = param.Get_Enum_मानम्(code)

                जइ len(param.GetBitmaskList()) > 0:
                    xml_मानानि = ET.SubElement(xml_param, "bitmask")
                    प्रति index अन्तः param.GetBitmaskList():
                        xml_मानम् = ET.SubElement(xml_मानानि, "bit")
                        xml_मानम्.attrib["index"] = index;
                        xml_मानम्.text = param.GetBitmaskBit(index)

        indent(xml_parameters)
        self.xml_document = ET.Element_वृक्षः(xml_parameters)

    def Save(self, filename):
        self.xml_document.लिख(filename, encoding="UTF-8")
