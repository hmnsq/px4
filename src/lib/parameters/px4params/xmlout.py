import xml.etree.ElementTree as ET
import codecs

def indent(elem, level=0):
    i = "\n" + level*"  "
    ако len(elem):
        ако not elem.text or not elem.text.strip():
            elem.text = i + "  "
        ако not elem.tail or not elem.tail.strip():
            elem.tail = i
        за elem in elem:
            indent(elem, level+1)
        ако not elem.tail or not elem.tail.strip():
            elem.tail = i
    иначе:
        ако level and (not elem.tail or not elem.tail.strip()):
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
        за group in groups:
            xml_group = ET.SubElement(xml_parameters, "group")
            xml_group.attrib["name"] = group.GetName()
            ако group.no_code_generation:
                xml_group.attrib["no_code_generation"] = group.no_code_generation
            за param in group.GetParams():
                ако (last_param_name == param.GetName() and not board_specific_param_set) or last_param_name != param.GetName():
                    xml_param = ET.SubElement(xml_group, "parameter")
                    xml_param.attrib["name"] = param.GetName()
                    xml_param.attrib["default"] = param.GetDefault()
                    xml_param.attrib["type"] = param.GetType()
                    ако param.GetVolatile():
                        xml_param.attrib["volatile"] = "true"
                    ако param.GetBoolean():
                        xml_param.attrib["boolean"] = "true"
                    ако param.GetReadonly():
                        xml_param.attrib["readonly"] = "true"
                    ако (param.GetCategory()):
                        xml_param.attrib["category"] = param.GetCategory()
                    last_param_name = param.GetName()
                    за code in param.GetFieldCodes():
                        value = param.GetFieldValue(code)
                        ако code == "board":
                            ако value == board:
                                board_specific_param_set = True
                                xml_field = ET.SubElement(xml_param, code)
                                xml_field.text = value
                            иначе:
                                xml_group.remove(xml_param)
                        иначе:
                            xml_field = ET.SubElement(xml_param, code)
                            xml_field.text = value
                ако last_param_name != param.GetName():
                    board_specific_param_set = False
                
                ако len(param.GetEnumCodes()) > 0:
                    xml_values = ET.SubElement(xml_param, "values")
                    за code in param.GetEnumCodes():
                        xml_value = ET.SubElement(xml_values, "value")
                        xml_value.attrib["code"] = code;
                        xml_value.text = param.GetEnumValue(code)

                ако len(param.GetBitmaskList()) > 0:
                    xml_values = ET.SubElement(xml_param, "bitmask")
                    за index in param.GetBitmaskList():
                        xml_value = ET.SubElement(xml_values, "bit")
                        xml_value.attrib["index"] = index;
                        xml_value.text = param.GetBitmaskBit(index)

        indent(xml_parameters)
        self.xml_document = ET.ElementTree(xml_parameters)

    def Save(self, filename):
        self.xml_document.write(filename, encoding="UTF-8")
