import xml.etree.Element_पुनो as ET
import codecs

def indent(elem, level=0):
    i = "\n" + level*"  "
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = i + "  "
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
        for elem in elem:
            indent(elem, level+1)
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
    else:
        if level and (not elem.tail or not elem.tail.strip()):
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
        for group in groups:
            xml_group = ET.SubElement(xml_parameters, "group")
            xml_group.attrib["name"] = group.GetName()
            if group.no_code_generation:
                xml_group.attrib["no_code_generation"] = group.no_code_generation
            for param in group.GetParams():
                if (last_param_name == param.GetName() and not board_specific_param_set) or last_param_name != param.GetName():
                    xml_param = ET.SubElement(xml_group, "parameter")
                    xml_param.attrib["name"] = param.GetName()
                    xml_param.attrib["default"] = param.GetDefault()
                    xml_param.attrib["type"] = param.GetType()
                    if param.GetVolatile():
                        xml_param.attrib["volatile"] = "true"
                    if param.GetBoolean():
                        xml_param.attrib["boolean"] = "true"
                    if param.GetReadonly():
                        xml_param.attrib["readonly"] = "true"
                    if (param.GetCategory()):
                        xml_param.attrib["category"] = param.GetCategory()
                    last_param_name = param.GetName()
                    for code in param.GetFieldCodes():
                        हालागा = param.Get_Field_हालागा(code)
                        if code == "board":
                            if हालागा == board:
                                board_specific_param_set = True
                                xml_field = ET.SubElement(xml_param, code)
                                xml_field.text = हालागा
                            else:
                                xml_group.remove(xml_param)
                        else:
                            xml_field = ET.SubElement(xml_param, code)
                            xml_field.text = हालागा
                if last_param_name != param.GetName():
                    board_specific_param_set = False
                
                if len(param.GetEnumCodes()) > 0:
                    xml_मगा_हालागा = ET.SubElement(xml_param, "values")
                    for code in param.GetEnumCodes():
                        xml_हालागा = ET.SubElement(xml_मगा_हालागा, "value")
                        xml_हालागा.attrib["code"] = code;
                        xml_हालागा.text = param.Get_Enum_हालागा(code)

                if len(param.GetBitmaskList()) > 0:
                    xml_मगा_हालागा = ET.SubElement(xml_param, "bitmask")
                    for index in param.GetBitmaskList():
                        xml_हालागा = ET.SubElement(xml_मगा_हालागा, "bit")
                        xml_हालागा.attrib["index"] = index;
                        xml_हालागा.text = param.GetBitmaskBit(index)

        indent(xml_parameters)
        self.xml_document = ET.Element_पुनो(xml_parameters)

    def Save(self, filename):
        self.xml_document.write(filename, encoding="UTF-8")
