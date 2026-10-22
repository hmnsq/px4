استورد xml.etree.Element_ونه كـ ET
استورد codecs

تعریف indent(elem, level=0):
    i = "\n" + level*"  "
    إذا اوږدوالی(elem):
        إذا ليس elem.متن أو ليس elem.متن.strip():
            elem.متن = i + "  "
        إذا ليس elem.لکۍ أو ليس elem.لکۍ.strip():
            elem.لکۍ = i
        لكل elem في elem:
            indent(elem, level+1)
        إذا ليس elem.لکۍ أو ليس elem.لکۍ.strip():
            elem.لکۍ = i
    وإلا:
        إذا level و (ليس elem.لکۍ أو ليس elem.لکۍ.strip()):
            elem.لکۍ = i

صنف XMLOutput():

    تعریف __init__(self, groups, board):
        xml_parameters = ET.Element("parameters")
        xml_نسخه = ET.SubElement(xml_parameters, "version")
        xml_نسخه.متن = "3"
        xml_نسخه = ET.SubElement(xml_parameters, "parameter_version_major")
        xml_نسخه.متن = "1"
        xml_نسخه = ET.SubElement(xml_parameters, "parameter_version_minor")
        xml_نسخه.متن = "15"
        وروستی_param_name = ""
        board_specific_param_سېټ = خطأ
        لكل ډله في groups:
            xml_ډله = ET.SubElement(xml_parameters, "group")
            xml_ډله.attrib["name"] = ډله.GetName()
            إذا ډله.no_کوډ_generation:
                xml_ډله.attrib["no_code_generation"] = ډله.no_کوډ_generation
            لكل param في ډله.GetParams():
                إذا (وروستی_param_name == param.GetName() و ليس board_specific_param_سېټ) أو وروستی_param_name != param.GetName():
                    xml_param = ET.SubElement(xml_ډله, "parameter")
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
                    وروستی_param_name = param.GetName()
                    لكل کوډ في param.Get_ډګر_Codes():
                        ارزښت = param.Get_ډګر_ارزښت(کوډ)
                        إذا کوډ == "board":
                            إذا ارزښت == board:
                                board_specific_param_سېټ = ریښتیا
                                xml_ډګر = ET.SubElement(xml_param, کوډ)
                                xml_ډګر.متن = ارزښت
                            وإلا:
                                xml_ډله.remove(xml_param)
                        وإلا:
                            xml_ډګر = ET.SubElement(xml_param, کوډ)
                            xml_ډګر.متن = ارزښت
                إذا وروستی_param_name != param.GetName():
                    board_specific_param_سېټ = خطأ
                
                إذا اوږدوالی(param.GetEnumCodes()) > 0:
                    xml_ارزښتونه = ET.SubElement(xml_param, "values")
                    لكل کوډ في param.GetEnumCodes():
                        xml_ارزښت = ET.SubElement(xml_ارزښتونه, "value")
                        xml_ارزښت.attrib["code"] = کوډ;
                        xml_ارزښت.متن = param.Get_Enum_ارزښت(کوډ)

                إذا اوږدوالی(param.GetBitmaskList()) > 0:
                    xml_ارزښتونه = ET.SubElement(xml_param, "bitmask")
                    لكل شاخص في param.GetBitmaskList():
                        xml_ارزښت = ET.SubElement(xml_ارزښتونه, "bit")
                        xml_ارزښت.attrib["index"] = شاخص;
                        xml_ارزښت.متن = param.Get_Bitmask_بټ(شاخص)

        indent(xml_parameters)
        self.xml_document = ET.Element_ونه(xml_parameters)

    تعریف Save(self, filename):
        self.xml_document.اكتب(filename, encoding="UTF-8")
