درآمد xml.etree.Element_درخت بطور ET
درآمد codecs

تعریف indent(elem, level=0):
    i = "\n" + level*"  "
    اگر لمبائی(elem):
        اگر نہیں elem.متن یا نہیں elem.متن.strip():
            elem.متن = i + "  "
        اگر نہیں elem.دم یا نہیں elem.دم.strip():
            elem.دم = i
        برائے elem میں elem:
            indent(elem, level+1)
        اگر نہیں elem.دم یا نہیں elem.دم.strip():
            elem.دم = i
    ورنہ:
        اگر level اور (نہیں elem.دم یا نہیں elem.دم.strip()):
            elem.دم = i

جماعت XMLOutput():

    تعریف __init__(self, groups, board):
        xml_parameters = ET.Element("parameters")
        xml_نسخہ = ET.SubElement(xml_parameters, "version")
        xml_نسخہ.متن = "3"
        xml_نسخہ = ET.SubElement(xml_parameters, "parameter_version_major")
        xml_نسخہ.متن = "1"
        xml_نسخہ = ET.SubElement(xml_parameters, "parameter_version_minor")
        xml_نسخہ.متن = "15"
        آخری_param_name = ""
        board_specific_param_سیٹ = جھوٹ
        برائے گروہ میں groups:
            xml_گروہ = ET.SubElement(xml_parameters, "group")
            xml_گروہ.attrib["name"] = گروہ.GetName()
            اگر گروہ.no_رمز_generation:
                xml_گروہ.attrib["no_code_generation"] = گروہ.no_رمز_generation
            برائے param میں گروہ.GetParams():
                اگر (آخری_param_name == param.GetName() اور نہیں board_specific_param_سیٹ) یا آخری_param_name != param.GetName():
                    xml_param = ET.SubElement(xml_گروہ, "parameter")
                    xml_param.attrib["name"] = param.GetName()
                    xml_param.attrib["default"] = param.GetDefault()
                    xml_param.attrib["type"] = param.GetType()
                    اگر param.GetVolatile():
                        xml_param.attrib["volatile"] = "true"
                    اگر param.GetBoolean():
                        xml_param.attrib["boolean"] = "true"
                    اگر param.GetReadonly():
                        xml_param.attrib["readonly"] = "true"
                    اگر (param.GetCategory()):
                        xml_param.attrib["category"] = param.GetCategory()
                    آخری_param_name = param.GetName()
                    برائے رمز میں param.Get_میدان_Codes():
                        قدر = param.Get_میدان_قدر(رمز)
                        اگر رمز == "board":
                            اگر قدر == board:
                                board_specific_param_سیٹ = سچ
                                xml_میدان = ET.SubElement(xml_param, رمز)
                                xml_میدان.متن = قدر
                            ورنہ:
                                xml_گروہ.remove(xml_param)
                        ورنہ:
                            xml_میدان = ET.SubElement(xml_param, رمز)
                            xml_میدان.متن = قدر
                اگر آخری_param_name != param.GetName():
                    board_specific_param_سیٹ = جھوٹ
                
                اگر لمبائی(param.GetEnumCodes()) > 0:
                    xml_اقدار = ET.SubElement(xml_param, "values")
                    برائے رمز میں param.GetEnumCodes():
                        xml_قدر = ET.SubElement(xml_اقدار, "value")
                        xml_قدر.attrib["code"] = رمز;
                        xml_قدر.متن = param.Get_Enum_قدر(رمز)

                اگر لمبائی(param.GetBitmaskList()) > 0:
                    xml_اقدار = ET.SubElement(xml_param, "bitmask")
                    برائے اشاریہ میں param.GetBitmaskList():
                        xml_قدر = ET.SubElement(xml_اقدار, "bit")
                        xml_قدر.attrib["index"] = اشاریہ;
                        xml_قدر.متن = param.Get_Bitmask_ثنائی_ہندسہ(اشاریہ)

        indent(xml_parameters)
        self.xml_document = ET.Element_درخت(xml_parameters)

    تعریف Save(self, filename):
        self.xml_document.اكتب(filename, encoding="UTF-8")
