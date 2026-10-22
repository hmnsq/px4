استورد xml.etree.Element_درخت كـ ET
استورد codecs

تعریف indent(elem, level=0):
    i = "\n" + level*"  "
    إذا طول(elem):
        إذا ليس elem.متن أو ليس elem.متن.strip():
            elem.متن = i + "  "
        إذا ليس elem.دم أو ليس elem.دم.strip():
            elem.دم = i
        لكل elem في elem:
            indent(elem, level+1)
        إذا ليس elem.دم أو ليس elem.دم.strip():
            elem.دم = i
    وإلا:
        إذا level و (ليس elem.دم أو ليس elem.دم.strip()):
            elem.دم = i

صنف XMLOutput():

    تعریف __init__(self, groups, board):
        xml_parameters = ET.Element("parameters")
        xml_نسخه = ET.SubElement(xml_parameters, "version")
        xml_نسخه.متن = "3"
        xml_نسخه = ET.SubElement(xml_parameters, "parameter_version_major")
        xml_نسخه.متن = "1"
        xml_نسخه = ET.SubElement(xml_parameters, "parameter_version_minor")
        xml_نسخه.متن = "15"
        آخرین_param_name = ""
        board_specific_param_مجموعه = خطأ
        لكل گروه في groups:
            xml_گروه = ET.SubElement(xml_parameters, "group")
            xml_گروه.attrib["name"] = گروه.GetName()
            إذا گروه.no_کد_generation:
                xml_گروه.attrib["no_code_generation"] = گروه.no_کد_generation
            لكل param في گروه.GetParams():
                إذا (آخرین_param_name == param.GetName() و ليس board_specific_param_مجموعه) أو آخرین_param_name != param.GetName():
                    xml_param = ET.SubElement(xml_گروه, "parameter")
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
                    آخرین_param_name = param.GetName()
                    لكل کد في param.Get_فیلد_Codes():
                        مقدار = param.Get_فیلد_مقدار(کد)
                        إذا کد == "board":
                            إذا مقدار == board:
                                board_specific_param_مجموعه = درست
                                xml_فیلد = ET.SubElement(xml_param, کد)
                                xml_فیلد.متن = مقدار
                            وإلا:
                                xml_گروه.remove(xml_param)
                        وإلا:
                            xml_فیلد = ET.SubElement(xml_param, کد)
                            xml_فیلد.متن = مقدار
                إذا آخرین_param_name != param.GetName():
                    board_specific_param_مجموعه = خطأ
                
                إذا طول(param.GetEnumCodes()) > 0:
                    xml_مقادیر = ET.SubElement(xml_param, "values")
                    لكل کد في param.GetEnumCodes():
                        xml_مقدار = ET.SubElement(xml_مقادیر, "value")
                        xml_مقدار.attrib["code"] = کد;
                        xml_مقدار.متن = param.Get_Enum_مقدار(کد)

                إذا طول(param.GetBitmaskList()) > 0:
                    xml_مقادیر = ET.SubElement(xml_param, "bitmask")
                    لكل شاخص في param.GetBitmaskList():
                        xml_مقدار = ET.SubElement(xml_مقادیر, "bit")
                        xml_مقدار.attrib["index"] = شاخص;
                        xml_مقدار.متن = param.Get_Bitmask_بیت(شاخص)

        indent(xml_parameters)
        self.xml_document = ET.Element_درخت(xml_parameters)

    تعریف Save(self, filename):
        self.xml_document.اكتب(filename, encoding="UTF-8")
