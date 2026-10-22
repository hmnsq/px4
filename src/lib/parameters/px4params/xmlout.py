استورد xml.etree.Element_درخت كـ ET
استورد codecs

تعریف indent(elem, level=0):
    i = "\n" + level*"  "
    إذا لمبائی(elem):
        إذا ليس elem.متن أو ليس elem.متن.strip():
            elem.متن = i + "  "
        إذا ليس elem.پوچھ أو ليس elem.پوچھ.strip():
            elem.پوچھ = i
        لكل elem في elem:
            indent(elem, level+1)
        إذا ليس elem.پوچھ أو ليس elem.پوچھ.strip():
            elem.پوچھ = i
    وإلا:
        إذا level و (ليس elem.پوچھ أو ليس elem.پوچھ.strip()):
            elem.پوچھ = i

صنف XMLOutput():

    تعریف __init__(self, groups, board):
        xml_parameters = ET.Element("parameters")
        xml_نسخہ = ET.SubElement(xml_parameters, "version")
        xml_نسخہ.متن = "3"
        xml_نسخہ = ET.SubElement(xml_parameters, "parameter_version_major")
        xml_نسخہ.متن = "1"
        xml_نسخہ = ET.SubElement(xml_parameters, "parameter_version_minor")
        xml_نسخہ.متن = "15"
        آخری_param_name = ""
        board_specific_param_سیٹ = خطأ
        لكل گروہ في groups:
            xml_گروہ = ET.SubElement(xml_parameters, "group")
            xml_گروہ.attrib["name"] = گروہ.GetName()
            إذا گروہ.no_کوڈ_generation:
                xml_گروہ.attrib["no_code_generation"] = گروہ.no_کوڈ_generation
            لكل param في گروہ.GetParams():
                إذا (آخری_param_name == param.GetName() و ليس board_specific_param_سیٹ) أو آخری_param_name != param.GetName():
                    xml_param = ET.SubElement(xml_گروہ, "parameter")
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
                    آخری_param_name = param.GetName()
                    لكل کوڈ في param.Get_میدان_Codes():
                        قدر = param.Get_میدان_قدر(کوڈ)
                        إذا کوڈ == "board":
                            إذا قدر == board:
                                board_specific_param_سیٹ = سچ
                                xml_میدان = ET.SubElement(xml_param, کوڈ)
                                xml_میدان.متن = قدر
                            وإلا:
                                xml_گروہ.remove(xml_param)
                        وإلا:
                            xml_میدان = ET.SubElement(xml_param, کوڈ)
                            xml_میدان.متن = قدر
                إذا آخری_param_name != param.GetName():
                    board_specific_param_سیٹ = خطأ
                
                إذا لمبائی(param.GetEnumCodes()) > 0:
                    xml_اقدار = ET.SubElement(xml_param, "values")
                    لكل کوڈ في param.GetEnumCodes():
                        xml_قدر = ET.SubElement(xml_اقدار, "value")
                        xml_قدر.attrib["code"] = کوڈ;
                        xml_قدر.متن = param.Get_Enum_قدر(کوڈ)

                إذا لمبائی(param.GetBitmaskList()) > 0:
                    xml_اقدار = ET.SubElement(xml_param, "bitmask")
                    لكل اشاریہ في param.GetBitmaskList():
                        xml_قدر = ET.SubElement(xml_اقدار, "bit")
                        xml_قدر.attrib["index"] = اشاریہ;
                        xml_قدر.متن = param.Get_Bitmask_ثنائی_ہندسہ(اشاریہ)

        indent(xml_parameters)
        self.xml_document = ET.Element_درخت(xml_parameters)

    تعریف Save(self, filename):
        self.xml_document.اكتب(filename, encoding="UTF-8")
