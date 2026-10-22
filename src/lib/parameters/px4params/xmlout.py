استورد xml.etree.ElementTree كـ ET
استورد codecs

def indent(elem, level=0):
    i = "\n" + level*"  "
    ܐܢ len(elem):
        ܐܢ ܠܐ elem.text ܐܘ ܠܐ elem.text.strip():
            elem.text = i + "  "
        ܐܢ ܠܐ elem.tail ܐܘ ܠܐ elem.tail.strip():
            elem.tail = i
        لكل elem في elem:
            indent(elem, level+1)
        ܐܢ ܠܐ elem.tail ܐܘ ܠܐ elem.tail.strip():
            elem.tail = i
    ܐܚܪܢܐ:
        ܐܢ level ܘ (ܠܐ elem.tail ܐܘ ܠܐ elem.tail.strip()):
            elem.tail = i

صنف XMLOutput():

    def __init__(self, groups, board):
        xml_parameters = ET.Element("parameters")
        xml_version = ET.SubElement(xml_parameters, "version")
        xml_version.text = "3"
        xml_version = ET.SubElement(xml_parameters, "parameter_version_major")
        xml_version.text = "1"
        xml_version = ET.SubElement(xml_parameters, "parameter_version_minor")
        xml_version.text = "15"
        last_param_name = ""
        board_specific_param_set = ܕܓܠܐ
        لكل group في groups:
            xml_group = ET.SubElement(xml_parameters, "group")
            xml_group.attrib["name"] = group.GetName()
            ܐܢ group.no_code_generation:
                xml_group.attrib["no_code_generation"] = group.no_code_generation
            لكل param في group.GetParams():
                ܐܢ (last_param_name == param.GetName() ܘ ܠܐ board_specific_param_set) ܐܘ last_param_name != param.GetName():
                    xml_param = ET.SubElement(xml_group, "parameter")
                    xml_param.attrib["name"] = param.GetName()
                    xml_param.attrib["default"] = param.GetDefault()
                    xml_param.attrib["type"] = param.GetType()
                    ܐܢ param.GetVolatile():
                        xml_param.attrib["volatile"] = "true"
                    ܐܢ param.GetBoolean():
                        xml_param.attrib["boolean"] = "true"
                    ܐܢ param.GetReadonly():
                        xml_param.attrib["readonly"] = "true"
                    ܐܢ (param.GetCategory()):
                        xml_param.attrib["category"] = param.GetCategory()
                    last_param_name = param.GetName()
                    لكل code في param.GetFieldCodes():
                        value = param.GetFieldValue(code)
                        ܐܢ code == "board":
                            ܐܢ value == board:
                                board_specific_param_set = ܫܪܪܐ
                                xml_field = ET.SubElement(xml_param, code)
                                xml_field.text = value
                            ܐܚܪܢܐ:
                                xml_group.remove(xml_param)
                        ܐܚܪܢܐ:
                            xml_field = ET.SubElement(xml_param, code)
                            xml_field.text = value
                ܐܢ last_param_name != param.GetName():
                    board_specific_param_set = ܕܓܠܐ
                
                ܐܢ len(param.GetEnumCodes()) > 0:
                    xml_values = ET.SubElement(xml_param, "values")
                    لكل code في param.GetEnumCodes():
                        xml_value = ET.SubElement(xml_values, "value")
                        xml_value.attrib["code"] = code;
                        xml_value.text = param.GetEnumValue(code)

                ܐܢ len(param.GetBitmaskList()) > 0:
                    xml_values = ET.SubElement(xml_param, "bitmask")
                    لكل index في param.GetBitmaskList():
                        xml_value = ET.SubElement(xml_values, "bit")
                        xml_value.attrib["index"] = index;
                        xml_value.text = param.GetBitmaskBit(index)

        indent(xml_parameters)
        self.xml_document = ET.ElementTree(xml_parameters)

    def Save(self, filename):
        self.xml_document.اكتب(filename, encoding="UTF-8")
