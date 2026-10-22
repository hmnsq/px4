আনযন xml.etree.Element_বৃক্ষ ইৱ ET
আনযন codecs

def indent(elem, level=0):
    i = "\n" + level*"  "
    যদি len(elem):
        যদি নহয় elem.text বা নহয় elem.text.strip():
            elem.text = i + "  "
        যদি নহয় elem.tail বা নহয় elem.tail.strip():
            elem.tail = i
        বাবে elem অন্তঃ elem:
            indent(elem, level+1)
        যদি নহয় elem.tail বা নহয় elem.tail.strip():
            elem.tail = i
    অন্যথা:
        যদি level আৰু (নহয় elem.tail বা নহয় elem.tail.strip()):
            elem.tail = i

শ্ৰেণী XMLOutput():

    def __init__(self, groups, board):
        xml_parameters = ET.Element("parameters")
        xml_version = ET.SubElement(xml_parameters, "version")
        xml_version.text = "3"
        xml_version = ET.SubElement(xml_parameters, "parameter_version_major")
        xml_version.text = "1"
        xml_version = ET.SubElement(xml_parameters, "parameter_version_minor")
        xml_version.text = "15"
        last_param_name = ""
        board_specific_param_set = মিছা
        বাবে group অন্তঃ groups:
            xml_group = ET.SubElement(xml_parameters, "group")
            xml_group.attrib["name"] = group.GetName()
            যদি group.no_code_generation:
                xml_group.attrib["no_code_generation"] = group.no_code_generation
            বাবে param অন্তঃ group.GetParams():
                যদি (last_param_name == param.GetName() আৰু নহয় board_specific_param_set) বা last_param_name != param.GetName():
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
                    বাবে code অন্তঃ param.GetFieldCodes():
                        মান = param.Get_Field_মান(code)
                        যদি code == "board":
                            যদি মান == board:
                                board_specific_param_set = সঁচা
                                xml_field = ET.SubElement(xml_param, code)
                                xml_field.text = মান
                            অন্যথা:
                                xml_group.remove(xml_param)
                        অন্যথা:
                            xml_field = ET.SubElement(xml_param, code)
                            xml_field.text = মান
                যদি last_param_name != param.GetName():
                    board_specific_param_set = মিছা
                
                যদি len(param.GetEnumCodes()) > 0:
                    xml_মানসমূহ = ET.SubElement(xml_param, "values")
                    বাবে code অন্তঃ param.GetEnumCodes():
                        xml_মান = ET.SubElement(xml_মানসমূহ, "value")
                        xml_মান.attrib["code"] = code;
                        xml_মান.text = param.Get_Enum_মান(code)

                যদি len(param.GetBitmaskList()) > 0:
                    xml_মানসমূহ = ET.SubElement(xml_param, "bitmask")
                    বাবে index অন্তঃ param.GetBitmaskList():
                        xml_মান = ET.SubElement(xml_মানসমূহ, "bit")
                        xml_মান.attrib["index"] = index;
                        xml_মান.text = param.GetBitmaskBit(index)

        indent(xml_parameters)
        self.xml_document = ET.Element_বৃক্ষ(xml_parameters)

    def Save(self, filename):
        self.xml_document.লিখ(filename, encoding="UTF-8")
