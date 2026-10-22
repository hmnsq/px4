ഇറക്കുമതി xml.etree.Element_വൃക്ഷം ഇവ ET
ഇറക്കുമതി codecs

def indent(elem, level=0):
    i = "\n" + level*"  "
    എങ്കിൽ len(elem):
        എങ്കിൽ അല്ല elem.text അഥവാ അല്ല elem.text.strip():
            elem.text = i + "  "
        എങ്കിൽ അല്ല elem.tail അഥവാ അല്ല elem.tail.strip():
            elem.tail = i
        വേണ്ടി elem ഇൽ elem:
            indent(elem, level+1)
        എങ്കിൽ അല്ല elem.tail അഥവാ അല്ല elem.tail.strip():
            elem.tail = i
    അല്ലെങ്കിൽ:
        എങ്കിൽ level ഒപ്പം (അല്ല elem.tail അഥവാ അല്ല elem.tail.strip()):
            elem.tail = i

വർഗ്ഗം XMLOutput():

    def __init__(self, groups, board):
        xml_parameters = ET.Element("parameters")
        xml_version = ET.SubElement(xml_parameters, "version")
        xml_version.text = "3"
        xml_version = ET.SubElement(xml_parameters, "parameter_version_major")
        xml_version.text = "1"
        xml_version = ET.SubElement(xml_parameters, "parameter_version_minor")
        xml_version.text = "15"
        last_param_name = ""
        board_specific_param_set = അസത്യം
        വേണ്ടി group ഇൽ groups:
            xml_group = ET.SubElement(xml_parameters, "group")
            xml_group.attrib["name"] = group.GetName()
            എങ്കിൽ group.no_code_generation:
                xml_group.attrib["no_code_generation"] = group.no_code_generation
            വേണ്ടി param ഇൽ group.GetParams():
                എങ്കിൽ (last_param_name == param.GetName() ഒപ്പം അല്ല board_specific_param_set) അഥവാ last_param_name != param.GetName():
                    xml_param = ET.SubElement(xml_group, "parameter")
                    xml_param.attrib["name"] = param.GetName()
                    xml_param.attrib["default"] = param.GetDefault()
                    xml_param.attrib["type"] = param.GetType()
                    എങ്കിൽ param.GetVolatile():
                        xml_param.attrib["volatile"] = "true"
                    എങ്കിൽ param.GetBoolean():
                        xml_param.attrib["boolean"] = "true"
                    എങ്കിൽ param.GetReadonly():
                        xml_param.attrib["readonly"] = "true"
                    എങ്കിൽ (param.GetCategory()):
                        xml_param.attrib["category"] = param.GetCategory()
                    last_param_name = param.GetName()
                    വേണ്ടി code ഇൽ param.GetFieldCodes():
                        മൂല്യം = param.Get_Field_മൂല്യം(code)
                        എങ്കിൽ code == "board":
                            എങ്കിൽ മൂല്യം == board:
                                board_specific_param_set = സത്യം
                                xml_field = ET.SubElement(xml_param, code)
                                xml_field.text = മൂല്യം
                            അല്ലെങ്കിൽ:
                                xml_group.remove(xml_param)
                        അല്ലെങ്കിൽ:
                            xml_field = ET.SubElement(xml_param, code)
                            xml_field.text = മൂല്യം
                എങ്കിൽ last_param_name != param.GetName():
                    board_specific_param_set = അസത്യം
                
                എങ്കിൽ len(param.GetEnumCodes()) > 0:
                    xml_മൂല്യങ്ങൾ = ET.SubElement(xml_param, "values")
                    വേണ്ടി code ഇൽ param.GetEnumCodes():
                        xml_മൂല്യം = ET.SubElement(xml_മൂല്യങ്ങൾ, "value")
                        xml_മൂല്യം.attrib["code"] = code;
                        xml_മൂല്യം.text = param.Get_Enum_മൂല്യം(code)

                എങ്കിൽ len(param.GetBitmaskList()) > 0:
                    xml_മൂല്യങ്ങൾ = ET.SubElement(xml_param, "bitmask")
                    വേണ്ടി index ഇൽ param.GetBitmaskList():
                        xml_മൂല്യം = ET.SubElement(xml_മൂല്യങ്ങൾ, "bit")
                        xml_മൂല്യം.attrib["index"] = index;
                        xml_മൂല്യം.text = param.GetBitmaskBit(index)

        indent(xml_parameters)
        self.xml_document = ET.Element_വൃക്ഷം(xml_parameters)

    def Save(self, filename):
        self.xml_document.ലിഖ(filename, encoding="UTF-8")
