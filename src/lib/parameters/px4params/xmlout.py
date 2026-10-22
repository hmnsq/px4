আমদানি xml.etree.Element_বৃক্ষ ইব ET
আমদানি codecs

def indent(elem, level=0):
    i = "\n" + level*"  "
    যদি len(elem):
        যদি নয় elem.text অথবা নয় elem.text.strip():
            elem.text = i + "  "
        যদি নয় elem.লেজ অথবা নয় elem.লেজ.strip():
            elem.লেজ = i
        জন্য elem মধ্যে elem:
            indent(elem, level+1)
        যদি নয় elem.লেজ অথবা নয় elem.লেজ.strip():
            elem.লেজ = i
    অন্যথা:
        যদি level এবং (নয় elem.লেজ অথবা নয় elem.লেজ.strip()):
            elem.লেজ = i

শ্রেণী XMLOutput():

    def __init__(self, groups, board):
        xml_parameters = ET.Element("parameters")
        xml_version = ET.SubElement(xml_parameters, "version")
        xml_version.text = "3"
        xml_version = ET.SubElement(xml_parameters, "parameter_version_major")
        xml_version.text = "1"
        xml_version = ET.SubElement(xml_parameters, "parameter_version_minor")
        xml_version.text = "15"
        last_param_name = ""
        board_specific_param_set = মিথ্যা
        জন্য group মধ্যে groups:
            xml_group = ET.SubElement(xml_parameters, "group")
            xml_group.attrib["name"] = group.GetName()
            যদি group.no_code_generation:
                xml_group.attrib["no_code_generation"] = group.no_code_generation
            জন্য param মধ্যে group.GetParams():
                যদি (last_param_name == param.GetName() এবং নয় board_specific_param_set) অথবা last_param_name != param.GetName():
                    xml_param = ET.SubElement(xml_group, "parameter")
                    xml_param.attrib["name"] = param.GetName()
                    xml_param.attrib["default"] = param.GetDefault()
                    xml_param.attrib["type"] = param.GetType()
                    যদি param.GetVolatile():
                        xml_param.attrib["volatile"] = "true"
                    যদি param.GetBoolean():
                        xml_param.attrib["boolean"] = "true"
                    যদি param.GetReadonly():
                        xml_param.attrib["readonly"] = "true"
                    যদি (param.GetCategory()):
                        xml_param.attrib["category"] = param.GetCategory()
                    last_param_name = param.GetName()
                    জন্য code মধ্যে param.GetFieldCodes():
                        মান = param.Get_Field_মান(code)
                        যদি code == "board":
                            যদি মান == board:
                                board_specific_param_set = সত্য
                                xml_field = ET.SubElement(xml_param, code)
                                xml_field.text = মান
                            অন্যথা:
                                xml_group.remove(xml_param)
                        অন্যথা:
                            xml_field = ET.SubElement(xml_param, code)
                            xml_field.text = মান
                যদি last_param_name != param.GetName():
                    board_specific_param_set = মিথ্যা
                
                যদি len(param.GetEnumCodes()) > 0:
                    xml_মানসমূহ = ET.SubElement(xml_param, "values")
                    জন্য code মধ্যে param.GetEnumCodes():
                        xml_মান = ET.SubElement(xml_মানসমূহ, "value")
                        xml_মান.attrib["code"] = code;
                        xml_মান.text = param.Get_Enum_মান(code)

                যদি len(param.GetBitmaskList()) > 0:
                    xml_মানসমূহ = ET.SubElement(xml_param, "bitmask")
                    জন্য সূচক মধ্যে param.GetBitmaskList():
                        xml_মান = ET.SubElement(xml_মানসমূহ, "bit")
                        xml_মান.attrib["index"] = সূচক;
                        xml_মান.text = param.GetBitmaskBit(সূচক)

        indent(xml_parameters)
        self.xml_document = ET.Element_বৃক্ষ(xml_parameters)

    def Save(self, filename):
        self.xml_document.লিখ(filename, encoding="UTF-8")
