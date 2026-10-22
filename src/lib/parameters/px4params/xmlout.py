import xml.etree.ElementTree as ET
import codecs

def indent(elem, level=0):
    i = "\n" + level*"  "
    ᬬᬾᬦ᭄ len(elem):
        ᬬᬾᬦ᭄ na elem.text or na elem.text.strip():
            elem.text = i + "  "
        ᬬᬾᬦ᭄ na elem.tail or na elem.tail.strip():
            elem.tail = i
        for elem anta_h elem:
            indent(elem, level+1)
        ᬬᬾᬦ᭄ na elem.tail or na elem.tail.strip():
            elem.tail = i
    ᬮᬶᬬ:
        ᬬᬾᬦ᭄ level ca (na elem.tail or na elem.tail.strip()):
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
        for group anta_h groups:
            xml_group = ET.SubElement(xml_parameters, "group")
            xml_group.attrib["name"] = group.GetName()
            ᬬᬾᬦ᭄ group.no_code_generation:
                xml_group.attrib["no_code_generation"] = group.no_code_generation
            for param anta_h group.GetParams():
                ᬬᬾᬦ᭄ (last_param_name == param.GetName() ca na board_specific_param_set) or last_param_name != param.GetName():
                    xml_param = ET.SubElement(xml_group, "parameter")
                    xml_param.attrib["name"] = param.GetName()
                    xml_param.attrib["default"] = param.GetDefault()
                    xml_param.attrib["type"] = param.GetType()
                    ᬬᬾᬦ᭄ param.GetVolatile():
                        xml_param.attrib["volatile"] = "true"
                    ᬬᬾᬦ᭄ param.GetBoolean():
                        xml_param.attrib["boolean"] = "true"
                    ᬬᬾᬦ᭄ param.GetReadonly():
                        xml_param.attrib["readonly"] = "true"
                    ᬬᬾᬦ᭄ (param.GetCategory()):
                        xml_param.attrib["category"] = param.GetCategory()
                    last_param_name = param.GetName()
                    for code anta_h param.GetFieldCodes():
                        value = param.GetFieldValue(code)
                        ᬬᬾᬦ᭄ code == "board":
                            ᬬᬾᬦ᭄ value == board:
                                board_specific_param_set = True
                                xml_field = ET.SubElement(xml_param, code)
                                xml_field.text = value
                            ᬮᬶᬬ:
                                xml_group.remove(xml_param)
                        ᬮᬶᬬ:
                            xml_field = ET.SubElement(xml_param, code)
                            xml_field.text = value
                ᬬᬾᬦ᭄ last_param_name != param.GetName():
                    board_specific_param_set = False
                
                ᬬᬾᬦ᭄ len(param.GetEnumCodes()) > 0:
                    xml_values = ET.SubElement(xml_param, "values")
                    for code anta_h param.GetEnumCodes():
                        xml_value = ET.SubElement(xml_values, "value")
                        xml_value.attrib["code"] = code;
                        xml_value.text = param.GetEnumValue(code)

                ᬬᬾᬦ᭄ len(param.GetBitmaskList()) > 0:
                    xml_values = ET.SubElement(xml_param, "bitmask")
                    for index anta_h param.GetBitmaskList():
                        xml_value = ET.SubElement(xml_values, "bit")
                        xml_value.attrib["index"] = index;
                        xml_value.text = param.GetBitmaskBit(index)

        indent(xml_parameters)
        self.xml_document = ET.ElementTree(xml_parameters)

    def Save(self, filename):
        self.xml_document.write(filename, encoding="UTF-8")
