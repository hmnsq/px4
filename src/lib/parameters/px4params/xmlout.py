ଆମଦାନୀ xml.etree.Element_ବୃକ୍ଷ ଇଵ ET
ଆମଦାନୀ codecs

def indent(elem, level=0):
    i = "\n" + level*"  "
    ଯଦି len(elem):
        ଯଦି ନୁହେଁ elem.text କିମ୍ବା ନୁହେଁ elem.text.strip():
            elem.text = i + "  "
        ଯଦି ନୁହେଁ elem.tail କିମ୍ବା ନୁହେଁ elem.tail.strip():
            elem.tail = i
        ପାଇଁ elem ଭିତରେ elem:
            indent(elem, level+1)
        ଯଦି ନୁହେଁ elem.tail କିମ୍ବା ନୁହେଁ elem.tail.strip():
            elem.tail = i
    ଅନ୍ୟଥା:
        ଯଦି level ଏବଂ (ନୁହେଁ elem.tail କିମ୍ବା ନୁହେଁ elem.tail.strip()):
            elem.tail = i

ଶ୍ରେଣୀ XMLOutput():

    def __init__(self, groups, board):
        xml_parameters = ET.Element("parameters")
        xml_version = ET.SubElement(xml_parameters, "version")
        xml_version.text = "3"
        xml_version = ET.SubElement(xml_parameters, "parameter_version_major")
        xml_version.text = "1"
        xml_version = ET.SubElement(xml_parameters, "parameter_version_minor")
        xml_version.text = "15"
        last_param_name = ""
        board_specific_param_set = ମିଥ୍ୟା
        ପାଇଁ group ଭିତରେ groups:
            xml_group = ET.SubElement(xml_parameters, "group")
            xml_group.attrib["name"] = group.GetName()
            ଯଦି group.no_code_generation:
                xml_group.attrib["no_code_generation"] = group.no_code_generation
            ପାଇଁ param ଭିତରେ group.GetParams():
                ଯଦି (last_param_name == param.GetName() ଏବଂ ନୁହେଁ board_specific_param_set) କିମ୍ବା last_param_name != param.GetName():
                    xml_param = ET.SubElement(xml_group, "parameter")
                    xml_param.attrib["name"] = param.GetName()
                    xml_param.attrib["default"] = param.GetDefault()
                    xml_param.attrib["type"] = param.GetType()
                    ଯଦି param.GetVolatile():
                        xml_param.attrib["volatile"] = "true"
                    ଯଦି param.GetBoolean():
                        xml_param.attrib["boolean"] = "true"
                    ଯଦି param.GetReadonly():
                        xml_param.attrib["readonly"] = "true"
                    ଯଦି (param.GetCategory()):
                        xml_param.attrib["category"] = param.GetCategory()
                    last_param_name = param.GetName()
                    ପାଇଁ code ଭିତରେ param.GetFieldCodes():
                        ମୂଲ୍ୟ = param.Get_Field_ମୂଲ୍ୟ(code)
                        ଯଦି code == "board":
                            ଯଦି ମୂଲ୍ୟ == board:
                                board_specific_param_set = ସତ୍ୟ
                                xml_field = ET.SubElement(xml_param, code)
                                xml_field.text = ମୂଲ୍ୟ
                            ଅନ୍ୟଥା:
                                xml_group.remove(xml_param)
                        ଅନ୍ୟଥା:
                            xml_field = ET.SubElement(xml_param, code)
                            xml_field.text = ମୂଲ୍ୟ
                ଯଦି last_param_name != param.GetName():
                    board_specific_param_set = ମିଥ୍ୟା
                
                ଯଦି len(param.GetEnumCodes()) > 0:
                    xml_ମୂଲ୍ୟଗୁଡ଼ିକ = ET.SubElement(xml_param, "values")
                    ପାଇଁ code ଭିତରେ param.GetEnumCodes():
                        xml_ମୂଲ୍ୟ = ET.SubElement(xml_ମୂଲ୍ୟଗୁଡ଼ିକ, "value")
                        xml_ମୂଲ୍ୟ.attrib["code"] = code;
                        xml_ମୂଲ୍ୟ.text = param.Get_Enum_ମୂଲ୍ୟ(code)

                ଯଦି len(param.GetBitmaskList()) > 0:
                    xml_ମୂଲ୍ୟଗୁଡ଼ିକ = ET.SubElement(xml_param, "bitmask")
                    ପାଇଁ index ଭିତରେ param.GetBitmaskList():
                        xml_ମୂଲ୍ୟ = ET.SubElement(xml_ମୂଲ୍ୟଗୁଡ଼ିକ, "bit")
                        xml_ମୂଲ୍ୟ.attrib["index"] = index;
                        xml_ମୂଲ୍ୟ.text = param.GetBitmaskBit(index)

        indent(xml_parameters)
        self.xml_document = ET.Element_ବୃକ୍ଷ(xml_parameters)

    def Save(self, filename):
        self.xml_document.ଲିଖ(filename, encoding="UTF-8")
