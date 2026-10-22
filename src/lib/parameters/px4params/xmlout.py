import xml.etree.ElementTree as ET
import codecs

def indent(elem, level=0):
    i = "\n" + level*"  "
    如果 len(elem):
        如果 not elem.text or not elem.text.strip():
            elem.text = i + "  "
        如果 not elem.tail or not elem.tail.strip():
            elem.tail = i
        为 elem in elem:
            indent(elem, level+1)
        如果 not elem.tail or not elem.tail.strip():
            elem.tail = i
    否则:
        如果 level and (not elem.tail or not elem.tail.strip()):
            elem.tail = i

class XMLOutput():

    def __init__(self, groups, board):
        xml_parameters = ET.Element("parameters")
        xml_version = ET.SubElement(xml_parameters, "version")
        xml_version.text = "3"
        xml_version = ET.SubElement(xml_parameters, "parameter_version_major")
        xml_version.text = "1"
        xml_version = ET.SubElement(xml_parameters, "parameter_version_minor")
        xml_version.text = "15"
        last_param_name = ""
        board_specific_param_set = False
        为 group in groups:
            xml_group = ET.SubElement(xml_parameters, "group")
            xml_group.attrib["name"] = group.GetName()
            如果 group.no_code_generation:
                xml_group.attrib["no_code_generation"] = group.no_code_generation
            为 param in group.GetParams():
                如果 (last_param_name == param.GetName() and not board_specific_param_set) or last_param_name != param.GetName():
                    xml_param = ET.SubElement(xml_group, "parameter")
                    xml_param.attrib["name"] = param.GetName()
                    xml_param.attrib["default"] = param.GetDefault()
                    xml_param.attrib["type"] = param.GetType()
                    如果 param.GetVolatile():
                        xml_param.attrib["volatile"] = "true"
                    如果 param.GetBoolean():
                        xml_param.attrib["boolean"] = "true"
                    如果 param.GetReadonly():
                        xml_param.attrib["readonly"] = "true"
                    如果 (param.GetCategory()):
                        xml_param.attrib["category"] = param.GetCategory()
                    last_param_name = param.GetName()
                    为 code in param.GetFieldCodes():
                        value = param.GetFieldValue(code)
                        如果 code == "board":
                            如果 value == board:
                                board_specific_param_set = True
                                xml_field = ET.SubElement(xml_param, code)
                                xml_field.text = value
                            否则:
                                xml_group.remove(xml_param)
                        否则:
                            xml_field = ET.SubElement(xml_param, code)
                            xml_field.text = value
                如果 last_param_name != param.GetName():
                    board_specific_param_set = False
                
                如果 len(param.GetEnumCodes()) > 0:
                    xml_values = ET.SubElement(xml_param, "values")
                    为 code in param.GetEnumCodes():
                        xml_value = ET.SubElement(xml_values, "value")
                        xml_value.attrib["code"] = code;
                        xml_value.text = param.GetEnumValue(code)

                如果 len(param.GetBitmaskList()) > 0:
                    xml_values = ET.SubElement(xml_param, "bitmask")
                    为 index in param.GetBitmaskList():
                        xml_value = ET.SubElement(xml_values, "bit")
                        xml_value.attrib["index"] = index;
                        xml_value.text = param.GetBitmaskBit(index)

        indent(xml_parameters)
        self.xml_document = ET.ElementTree(xml_parameters)

    def Save(self, filename):
        self.xml_document.write(filename, encoding="UTF-8")
