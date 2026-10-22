استورد xml.etree.Element_وڻ كـ ET
استورد codecs

وصف indent(elem, level=0):
    i = "\n" + level*"  "
    إذا ڊيگهه(elem):
        إذا ليس elem.متن أو ليس elem.متن.strip():
            elem.متن = i + "  "
        إذا ليس elem.پڇ أو ليس elem.پڇ.strip():
            elem.پڇ = i
        لكل elem في elem:
            indent(elem, level+1)
        إذا ليس elem.پڇ أو ليس elem.پڇ.strip():
            elem.پڇ = i
    وإلا:
        إذا level و (ليس elem.پڇ أو ليس elem.پڇ.strip()):
            elem.پڇ = i

صنف XMLOutput():

    وصف __init__(self, groups, board):
        xml_parameters = ET.Element("parameters")
        xml_نسخو = ET.SubElement(xml_parameters, "version")
        xml_نسخو.متن = "3"
        xml_نسخو = ET.SubElement(xml_parameters, "parameter_version_major")
        xml_نسخو.متن = "1"
        xml_نسخو = ET.SubElement(xml_parameters, "parameter_version_minor")
        xml_نسخو.متن = "15"
        آخري_param_name = ""
        board_specific_param_سيٽ = خطأ
        لكل گروهه في groups:
            xml_گروهه = ET.SubElement(xml_parameters, "group")
            xml_گروهه.attrib["name"] = گروهه.GetName()
            إذا گروهه.no_ڪوڊ_generation:
                xml_گروهه.attrib["no_code_generation"] = گروهه.no_ڪوڊ_generation
            لكل param في گروهه.GetParams():
                إذا (آخري_param_name == param.GetName() و ليس board_specific_param_سيٽ) أو آخري_param_name != param.GetName():
                    xml_param = ET.SubElement(xml_گروهه, "parameter")
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
                    آخري_param_name = param.GetName()
                    لكل ڪوڊ في param.Get_ميدان_Codes():
                        قدر = param.Get_ميدان_قدر(ڪوڊ)
                        إذا ڪوڊ == "board":
                            إذا قدر == board:
                                board_specific_param_سيٽ = سچ
                                xml_ميدان = ET.SubElement(xml_param, ڪوڊ)
                                xml_ميدان.متن = قدر
                            وإلا:
                                xml_گروهه.remove(xml_param)
                        وإلا:
                            xml_ميدان = ET.SubElement(xml_param, ڪوڊ)
                            xml_ميدان.متن = قدر
                إذا آخري_param_name != param.GetName():
                    board_specific_param_سيٽ = خطأ
                
                إذا ڊيگهه(param.GetEnumCodes()) > 0:
                    xml_قدرون = ET.SubElement(xml_param, "values")
                    لكل ڪوڊ في param.GetEnumCodes():
                        xml_قدر = ET.SubElement(xml_قدرون, "value")
                        xml_قدر.attrib["code"] = ڪوڊ;
                        xml_قدر.متن = param.Get_Enum_قدر(ڪوڊ)

                إذا ڊيگهه(param.GetBitmaskList()) > 0:
                    xml_قدرون = ET.SubElement(xml_param, "bitmask")
                    لكل اشاريو في param.GetBitmaskList():
                        xml_قدر = ET.SubElement(xml_قدرون, "bit")
                        xml_قدر.attrib["index"] = اشاريو;
                        xml_قدر.متن = param.Get_Bitmask_ٻٽو_انگ(اشاريو)

        indent(xml_parameters)
        self.xml_document = ET.Element_وڻ(xml_parameters)

    وصف Save(self, filename):
        self.xml_document.اكتب(filename, encoding="UTF-8")
