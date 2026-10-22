استورد xml.etree.ElementTree كـ ET
استورد codecs

def indent(elem, level=0):
    i = "\n" + level*"  "
    אם len(elem):
        אם לא elem.text או לא elem.text.strip():
            elem.text = i + "  "
        אם לא elem.tail או לא elem.tail.strip():
            elem.tail = i
        עבור elem في elem:
            indent(elem, level+1)
        אם לא elem.tail או לא elem.tail.strip():
            elem.tail = i
    אחרת:
        אם level וגם (לא elem.tail או לא elem.tail.strip()):
            elem.tail = i

מחלקה XMLOutput():

    def __init__(self, groups, board):
        xml_parameters = ET.Element("parameters")
        xml_version = ET.SubElement(xml_parameters, "version")
        xml_version.text = "3"
        xml_version = ET.SubElement(xml_parameters, "parameter_version_major")
        xml_version.text = "1"
        xml_version = ET.SubElement(xml_parameters, "parameter_version_minor")
        xml_version.text = "15"
        last_param_name = ""
        board_specific_param_set = שקר
        עבור group في groups:
            xml_group = ET.SubElement(xml_parameters, "group")
            xml_group.attrib["name"] = group.GetName()
            אם group.no_code_generation:
                xml_group.attrib["no_code_generation"] = group.no_code_generation
            עבור param في group.GetParams():
                אם (last_param_name == param.GetName() וגם לא board_specific_param_set) או last_param_name != param.GetName():
                    xml_param = ET.SubElement(xml_group, "parameter")
                    xml_param.attrib["name"] = param.GetName()
                    xml_param.attrib["default"] = param.GetDefault()
                    xml_param.attrib["type"] = param.GetType()
                    אם param.GetVolatile():
                        xml_param.attrib["volatile"] = "true"
                    אם param.GetBoolean():
                        xml_param.attrib["boolean"] = "true"
                    אם param.GetReadonly():
                        xml_param.attrib["readonly"] = "true"
                    אם (param.GetCategory()):
                        xml_param.attrib["category"] = param.GetCategory()
                    last_param_name = param.GetName()
                    עבור code في param.GetFieldCodes():
                        value = param.GetFieldValue(code)
                        אם code == "board":
                            אם value == board:
                                board_specific_param_set = אמת
                                xml_field = ET.SubElement(xml_param, code)
                                xml_field.text = value
                            אחרת:
                                xml_group.remove(xml_param)
                        אחרת:
                            xml_field = ET.SubElement(xml_param, code)
                            xml_field.text = value
                אם last_param_name != param.GetName():
                    board_specific_param_set = שקר
                
                אם len(param.GetEnumCodes()) > 0:
                    xml_values = ET.SubElement(xml_param, "values")
                    עבור code في param.GetEnumCodes():
                        xml_value = ET.SubElement(xml_values, "value")
                        xml_value.attrib["code"] = code;
                        xml_value.text = param.GetEnumValue(code)

                אם len(param.GetBitmaskList()) > 0:
                    xml_values = ET.SubElement(xml_param, "bitmask")
                    עבור index في param.GetBitmaskList():
                        xml_value = ET.SubElement(xml_values, "bit")
                        xml_value.attrib["index"] = index;
                        xml_value.text = param.GetBitmaskBit(index)

        indent(xml_parameters)
        self.xml_document = ET.ElementTree(xml_parameters)

    def Save(self, filename):
        self.xml_document.اكتب(filename, encoding="UTF-8")
