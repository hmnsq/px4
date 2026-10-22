आनयन xml.etree.Element_रूख इव ET
आनयन codecs

def indent(elem, level=0):
    i = "\n" + level*"  "
    यदि len(elem):
        यदि होइन elem.text वा होइन elem.text.strip():
            elem.text = i + "  "
        यदि होइन elem.tail वा होइन elem.tail.strip():
            elem.tail = i
        लागि elem अन्तः elem:
            indent(elem, level+1)
        यदि होइन elem.tail वा होइन elem.tail.strip():
            elem.tail = i
    अन्यथा:
        यदि level र (होइन elem.tail वा होइन elem.tail.strip()):
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
        लागि group अन्तः groups:
            xml_group = ET.SubElement(xml_parameters, "group")
            xml_group.attrib["name"] = group.GetName()
            यदि group.no_code_generation:
                xml_group.attrib["no_code_generation"] = group.no_code_generation
            लागि param अन्तः group.GetParams():
                यदि (last_param_name == param.GetName() र होइन board_specific_param_set) वा last_param_name != param.GetName():
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
                    लागि code अन्तः param.GetFieldCodes():
                        मान = param.Get_Field_मान(code)
                        यदि code == "board":
                            यदि मान == board:
                                board_specific_param_set = सत्य
                                xml_field = ET.SubElement(xml_param, code)
                                xml_field.text = मान
                            अन्यथा:
                                xml_group.remove(xml_param)
                        अन्यथा:
                            xml_field = ET.SubElement(xml_param, code)
                            xml_field.text = मान
                यदि last_param_name != param.GetName():
                    board_specific_param_set = असत्य
                
                यदि len(param.GetEnumCodes()) > 0:
                    xml_मानहरू = ET.SubElement(xml_param, "values")
                    लागि code अन्तः param.GetEnumCodes():
                        xml_मान = ET.SubElement(xml_मानहरू, "value")
                        xml_मान.attrib["code"] = code;
                        xml_मान.text = param.Get_Enum_मान(code)

                यदि len(param.GetBitmaskList()) > 0:
                    xml_मानहरू = ET.SubElement(xml_param, "bitmask")
                    लागि index अन्तः param.GetBitmaskList():
                        xml_मान = ET.SubElement(xml_मानहरू, "bit")
                        xml_मान.attrib["index"] = index;
                        xml_मान.text = param.GetBitmaskBit(index)

        indent(xml_parameters)
        self.xml_document = ET.Element_रूख(xml_parameters)

    def Save(self, filename):
        self.xml_document.लिख(filename, encoding="UTF-8")
