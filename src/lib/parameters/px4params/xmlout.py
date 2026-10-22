આયાત xml.etree.Element_વૃક્ષ ઇવ ET
આયાત codecs

def indent(elem, level=0):
    i = "\n" + level*"  "
    જો len(elem):
        જો નહીં elem.text અથવા નહીં elem.text.strip():
            elem.text = i + "  "
        જો નહીં elem.tail અથવા નહીં elem.tail.strip():
            elem.tail = i
        માટે elem માં elem:
            indent(elem, level+1)
        જો નહીં elem.tail અથવા નહીં elem.tail.strip():
            elem.tail = i
    નહીંતર:
        જો level અને (નહીં elem.tail અથવા નહીં elem.tail.strip()):
            elem.tail = i

વર્ગ XMLOutput():

    def __init__(self, groups, board):
        xml_parameters = ET.Element("parameters")
        xml_version = ET.SubElement(xml_parameters, "version")
        xml_version.text = "3"
        xml_version = ET.SubElement(xml_parameters, "parameter_version_major")
        xml_version.text = "1"
        xml_version = ET.SubElement(xml_parameters, "parameter_version_minor")
        xml_version.text = "15"
        last_param_name = ""
        board_specific_param_set = ખોટું
        માટે group માં groups:
            xml_group = ET.SubElement(xml_parameters, "group")
            xml_group.attrib["name"] = group.GetName()
            જો group.no_code_generation:
                xml_group.attrib["no_code_generation"] = group.no_code_generation
            માટે param માં group.GetParams():
                જો (last_param_name == param.GetName() અને નહીં board_specific_param_set) અથવા last_param_name != param.GetName():
                    xml_param = ET.SubElement(xml_group, "parameter")
                    xml_param.attrib["name"] = param.GetName()
                    xml_param.attrib["default"] = param.GetDefault()
                    xml_param.attrib["type"] = param.GetType()
                    જો param.GetVolatile():
                        xml_param.attrib["volatile"] = "true"
                    જો param.GetBoolean():
                        xml_param.attrib["boolean"] = "true"
                    જો param.GetReadonly():
                        xml_param.attrib["readonly"] = "true"
                    જો (param.GetCategory()):
                        xml_param.attrib["category"] = param.GetCategory()
                    last_param_name = param.GetName()
                    માટે code માં param.GetFieldCodes():
                        કિંમત = param.Get_Field_કિંમત(code)
                        જો code == "board":
                            જો કિંમત == board:
                                board_specific_param_set = સાચું
                                xml_field = ET.SubElement(xml_param, code)
                                xml_field.text = કિંમત
                            નહીંતર:
                                xml_group.remove(xml_param)
                        નહીંતર:
                            xml_field = ET.SubElement(xml_param, code)
                            xml_field.text = કિંમત
                જો last_param_name != param.GetName():
                    board_specific_param_set = ખોટું
                
                જો len(param.GetEnumCodes()) > 0:
                    xml_કિંમતો = ET.SubElement(xml_param, "values")
                    માટે code માં param.GetEnumCodes():
                        xml_કિંમત = ET.SubElement(xml_કિંમતો, "value")
                        xml_કિંમત.attrib["code"] = code;
                        xml_કિંમત.text = param.Get_Enum_કિંમત(code)

                જો len(param.GetBitmaskList()) > 0:
                    xml_કિંમતો = ET.SubElement(xml_param, "bitmask")
                    માટે index માં param.GetBitmaskList():
                        xml_કિંમત = ET.SubElement(xml_કિંમતો, "bit")
                        xml_કિંમત.attrib["index"] = index;
                        xml_કિંમત.text = param.GetBitmaskBit(index)

        indent(xml_parameters)
        self.xml_document = ET.Element_વૃક્ષ(xml_parameters)

    def Save(self, filename):
        self.xml_document.લિખ(filename, encoding="UTF-8")
