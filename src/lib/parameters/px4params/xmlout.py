استورد xml.etree.ElementTree كـ ET
استورد codecs

def indent(elem, level=0):
    i = "\n" + level*"  "
    𐤀𐤌 len(elem):
        𐤀𐤌 𐤋𐤀 elem.text 𐤀𐤅 𐤋𐤀 elem.text.strip():
            elem.text = i + "  "
        𐤀𐤌 𐤋𐤀 elem.tail 𐤀𐤅 𐤋𐤀 elem.tail.strip():
            elem.tail = i
        لكل elem في elem:
            indent(elem, level+1)
        𐤀𐤌 𐤋𐤀 elem.tail 𐤀𐤅 𐤋𐤀 elem.tail.strip():
            elem.tail = i
    وإلا:
        𐤀𐤌 level 𐤅 (𐤋𐤀 elem.tail 𐤀𐤅 𐤋𐤀 elem.tail.strip()):
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
        board_specific_param_set = خطأ
        لكل group في groups:
            xml_group = ET.SubElement(xml_parameters, "group")
            xml_group.attrib["name"] = group.GetName()
            𐤀𐤌 group.no_code_generation:
                xml_group.attrib["no_code_generation"] = group.no_code_generation
            لكل param في group.GetParams():
                𐤀𐤌 (last_param_name == param.GetName() 𐤅 𐤋𐤀 board_specific_param_set) 𐤀𐤅 last_param_name != param.GetName():
                    xml_param = ET.SubElement(xml_group, "parameter")
                    xml_param.attrib["name"] = param.GetName()
                    xml_param.attrib["default"] = param.GetDefault()
                    xml_param.attrib["type"] = param.GetType()
                    𐤀𐤌 param.GetVolatile():
                        xml_param.attrib["volatile"] = "true"
                    𐤀𐤌 param.GetBoolean():
                        xml_param.attrib["boolean"] = "true"
                    𐤀𐤌 param.GetReadonly():
                        xml_param.attrib["readonly"] = "true"
                    𐤀𐤌 (param.GetCategory()):
                        xml_param.attrib["category"] = param.GetCategory()
                    last_param_name = param.GetName()
                    لكل code في param.GetFieldCodes():
                        value = param.GetFieldValue(code)
                        𐤀𐤌 code == "board":
                            𐤀𐤌 value == board:
                                board_specific_param_set = صحيح
                                xml_field = ET.SubElement(xml_param, code)
                                xml_field.text = value
                            وإلا:
                                xml_group.remove(xml_param)
                        وإلا:
                            xml_field = ET.SubElement(xml_param, code)
                            xml_field.text = value
                𐤀𐤌 last_param_name != param.GetName():
                    board_specific_param_set = خطأ
                
                𐤀𐤌 len(param.GetEnumCodes()) > 0:
                    xml_values = ET.SubElement(xml_param, "values")
                    لكل code في param.GetEnumCodes():
                        xml_value = ET.SubElement(xml_values, "value")
                        xml_value.attrib["code"] = code;
                        xml_value.text = param.GetEnumValue(code)

                𐤀𐤌 len(param.GetBitmaskList()) > 0:
                    xml_values = ET.SubElement(xml_param, "bitmask")
                    لكل index في param.GetBitmaskList():
                        xml_value = ET.SubElement(xml_values, "bit")
                        xml_value.attrib["index"] = index;
                        xml_value.text = param.GetBitmaskBit(index)

        indent(xml_parameters)
        self.xml_document = ET.ElementTree(xml_parameters)

    def Save(self, filename):
        self.xml_document.اكتب(filename, encoding="UTF-8")
