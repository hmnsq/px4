استورد xml.etree.Element_دەرەخ كـ ET
استورد codecs

def indent(elem, level=0):
    i = "\n" + level*"  "
    إذا len(elem):
        إذا ليس elem.text أو ليس elem.text.strip():
            elem.text = i + "  "
        إذا ليس elem.tail أو ليس elem.tail.strip():
            elem.tail = i
        لكل elem في elem:
            indent(elem, level+1)
        إذا ليس elem.tail أو ليس elem.tail.strip():
            elem.tail = i
    وإلا:
        إذا level و (ليس elem.tail أو ليس elem.tail.strip()):
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
            إذا group.no_code_generation:
                xml_group.attrib["no_code_generation"] = group.no_code_generation
            لكل param في group.GetParams():
                إذا (last_param_name == param.GetName() و ليس board_specific_param_set) أو last_param_name != param.GetName():
                    xml_param = ET.SubElement(xml_group, "parameter")
                    xml_param.attrib["name"] = param.GetName()
                    xml_param.attrib["default"] = param.GetDefault()
                    xml_param.attrib["type"] = param.GetType()
                    إذا param.GetVolatile():
                        xml_param.attrib["volatile"] = "true"
                    إذا param.GetBoolean():
                        xml_param.attrib["boolean"] = "true"
                    إذا param.GetReadonly():
                        xml_param.attrib["readonly"] = "true"
                    إذا (param.GetCategory()):
                        xml_param.attrib["category"] = param.GetCategory()
                    last_param_name = param.GetName()
                    لكل code في param.GetFieldCodes():
                        قىممەت = param.Get_Field_قىممەت(code)
                        إذا code == "board":
                            إذا قىممەت == board:
                                board_specific_param_set = صحيح
                                xml_field = ET.SubElement(xml_param, code)
                                xml_field.text = قىممەت
                            وإلا:
                                xml_group.remove(xml_param)
                        وإلا:
                            xml_field = ET.SubElement(xml_param, code)
                            xml_field.text = قىممەت
                إذا last_param_name != param.GetName():
                    board_specific_param_set = خطأ
                
                إذا len(param.GetEnumCodes()) > 0:
                    xml_قىممەتلەر = ET.SubElement(xml_param, "values")
                    لكل code في param.GetEnumCodes():
                        xml_قىممەت = ET.SubElement(xml_قىممەتلەر, "value")
                        xml_قىممەت.attrib["code"] = code;
                        xml_قىممەت.text = param.Get_Enum_قىممەت(code)

                إذا len(param.GetBitmaskList()) > 0:
                    xml_قىممەتلەر = ET.SubElement(xml_param, "bitmask")
                    لكل index في param.GetBitmaskList():
                        xml_قىممەت = ET.SubElement(xml_قىممەتلەر, "bit")
                        xml_قىممەت.attrib["index"] = index;
                        xml_قىممەت.text = param.GetBitmaskBit(index)

        indent(xml_parameters)
        self.xml_document = ET.Element_دەرەخ(xml_parameters)

    def Save(self, filename):
        self.xml_document.اكتب(filename, encoding="UTF-8")
