واردکن xml.etree.Element_درخت بهعنوان ET
واردکن codecs

def indent(elem, level=0):
    i = "\n" + level*"  "
    اگر len(elem):
        اگر نه elem.text یا نه elem.text.strip():
            elem.text = i + "  "
        اگر نه elem.tail یا نه elem.tail.strip():
            elem.tail = i
        برای elem در elem:
            indent(elem, level+1)
        اگر نه elem.tail یا نه elem.tail.strip():
            elem.tail = i
    وگرنه:
        اگر level و (نه elem.tail یا نه elem.tail.strip()):
            elem.tail = i

رده XMLOutput():

    def __init__(self, groups, board):
        xml_parameters = ET.Element("parameters")
        xml_version = ET.SubElement(xml_parameters, "version")
        xml_version.text = "3"
        xml_version = ET.SubElement(xml_parameters, "parameter_version_major")
        xml_version.text = "1"
        xml_version = ET.SubElement(xml_parameters, "parameter_version_minor")
        xml_version.text = "15"
        last_param_name = ""
        board_specific_param_set = نادرست
        برای group در groups:
            xml_group = ET.SubElement(xml_parameters, "group")
            xml_group.attrib["name"] = group.GetName()
            اگر group.no_code_generation:
                xml_group.attrib["no_code_generation"] = group.no_code_generation
            برای param در group.GetParams():
                اگر (last_param_name == param.GetName() و نه board_specific_param_set) یا last_param_name != param.GetName():
                    xml_param = ET.SubElement(xml_group, "parameter")
                    xml_param.attrib["name"] = param.GetName()
                    xml_param.attrib["default"] = param.GetDefault()
                    xml_param.attrib["type"] = param.GetType()
                    اگر param.GetVolatile():
                        xml_param.attrib["volatile"] = "true"
                    اگر param.GetBoolean():
                        xml_param.attrib["boolean"] = "true"
                    اگر param.GetReadonly():
                        xml_param.attrib["readonly"] = "true"
                    اگر (param.GetCategory()):
                        xml_param.attrib["category"] = param.GetCategory()
                    last_param_name = param.GetName()
                    برای code در param.GetFieldCodes():
                        مقدار = param.Get_Field_مقدار(code)
                        اگر code == "board":
                            اگر مقدار == board:
                                board_specific_param_set = درست
                                xml_field = ET.SubElement(xml_param, code)
                                xml_field.text = مقدار
                            وگرنه:
                                xml_group.remove(xml_param)
                        وگرنه:
                            xml_field = ET.SubElement(xml_param, code)
                            xml_field.text = مقدار
                اگر last_param_name != param.GetName():
                    board_specific_param_set = نادرست
                
                اگر len(param.GetEnumCodes()) > 0:
                    xml_مقادیر = ET.SubElement(xml_param, "values")
                    برای code در param.GetEnumCodes():
                        xml_مقدار = ET.SubElement(xml_مقادیر, "value")
                        xml_مقدار.attrib["code"] = code;
                        xml_مقدار.text = param.Get_Enum_مقدار(code)

                اگر len(param.GetBitmaskList()) > 0:
                    xml_مقادیر = ET.SubElement(xml_param, "bitmask")
                    برای index در param.GetBitmaskList():
                        xml_مقدار = ET.SubElement(xml_مقادیر, "bit")
                        xml_مقدار.attrib["index"] = index;
                        xml_مقدار.text = param.GetBitmaskBit(index)

        indent(xml_parameters)
        self.xml_document = ET.Element_درخت(xml_parameters)

    def Save(self, filename):
        self.xml_document.اكتب(filename, encoding="UTF-8")
