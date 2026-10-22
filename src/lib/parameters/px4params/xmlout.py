ಆಮದು xml.etree.Element_ವೃಕ್ಷ ಇವ ET
ಆಮದು codecs

ವ್ಯಾಖ್ಯಾನ indent(elem, level=0):
    i = "\n" + level*"  "
    ಆದರೆ ಉದ್ದ(elem):
        ಆದರೆ ಅಲ್ಲ elem.ಪಠ್ಯ ಅಥವಾ ಅಲ್ಲ elem.ಪಠ್ಯ.strip():
            elem.ಪಠ್ಯ = i + "  "
        ಆದರೆ ಅಲ್ಲ elem.ಬಾಲ ಅಥವಾ ಅಲ್ಲ elem.ಬಾಲ.strip():
            elem.ಬಾಲ = i
        ಗಾಗಿ elem ಒಳಗೆ elem:
            indent(elem, level+1)
        ಆದರೆ ಅಲ್ಲ elem.ಬಾಲ ಅಥವಾ ಅಲ್ಲ elem.ಬಾಲ.strip():
            elem.ಬಾಲ = i
    ಇಲ್ಲದಿದ್ದರೆ:
        ಆದರೆ level ಮತ್ತು (ಅಲ್ಲ elem.ಬಾಲ ಅಥವಾ ಅಲ್ಲ elem.ಬಾಲ.strip()):
            elem.ಬಾಲ = i

ವರ್ಗ XMLOutput():

    ವ್ಯಾಖ್ಯಾನ __init__(self, groups, board):
        xml_parameters = ET.Element("parameters")
        xml_ಆವೃತ್ತಿ = ET.SubElement(xml_parameters, "version")
        xml_ಆವೃತ್ತಿ.ಪಠ್ಯ = "3"
        xml_ಆವೃತ್ತಿ = ET.SubElement(xml_parameters, "parameter_version_major")
        xml_ಆವೃತ್ತಿ.ಪಠ್ಯ = "1"
        xml_ಆವೃತ್ತಿ = ET.SubElement(xml_parameters, "parameter_version_minor")
        xml_ಆವೃತ್ತಿ.ಪಠ್ಯ = "15"
        ಕೊನೆಯ_param_name = ""
        board_specific_param_ಗಣ = ಸುಳ್ಳು
        ಗಾಗಿ ಗುಂಪು ಒಳಗೆ groups:
            xml_ಗುಂಪು = ET.SubElement(xml_parameters, "group")
            xml_ಗುಂಪು.attrib["name"] = ಗುಂಪು.GetName()
            ಆದರೆ ಗುಂಪು.no_ಸಂಕೇತ_generation:
                xml_ಗುಂಪು.attrib["no_code_generation"] = ಗುಂಪು.no_ಸಂಕೇತ_generation
            ಗಾಗಿ param ಒಳಗೆ ಗುಂಪು.GetParams():
                ಆದರೆ (ಕೊನೆಯ_param_name == param.GetName() ಮತ್ತು ಅಲ್ಲ board_specific_param_ಗಣ) ಅಥವಾ ಕೊನೆಯ_param_name != param.GetName():
                    xml_param = ET.SubElement(xml_ಗುಂಪು, "parameter")
                    xml_param.attrib["name"] = param.GetName()
                    xml_param.attrib["default"] = param.GetDefault()
                    xml_param.attrib["type"] = param.GetType()
                    ಆದರೆ param.GetVolatile():
                        xml_param.attrib["volatile"] = "true"
                    ಆದರೆ param.GetBoolean():
                        xml_param.attrib["boolean"] = "true"
                    ಆದರೆ param.GetReadonly():
                        xml_param.attrib["readonly"] = "true"
                    ಆದರೆ (param.GetCategory()):
                        xml_param.attrib["category"] = param.GetCategory()
                    ಕೊನೆಯ_param_name = param.GetName()
                    ಗಾಗಿ ಸಂಕೇತ_2 ಒಳಗೆ param.Get_ಕ್ಷೇತ್ರ_Codes():
                        ಮೌಲ್ಯ = param.Get_ಕ್ಷೇತ್ರ_ಮೌಲ್ಯ(ಸಂಕೇತ_2)
                        ಆದರೆ ಸಂಕೇತ_2 == "board":
                            ಆದರೆ ಮೌಲ್ಯ == board:
                                board_specific_param_ಗಣ = ನಿಜ
                                xml_ಕ್ಷೇತ್ರ = ET.SubElement(xml_param, ಸಂಕೇತ_2)
                                xml_ಕ್ಷೇತ್ರ.ಪಠ್ಯ = ಮೌಲ್ಯ
                            ಇಲ್ಲದಿದ್ದರೆ:
                                xml_ಗುಂಪು.remove(xml_param)
                        ಇಲ್ಲದಿದ್ದರೆ:
                            xml_ಕ್ಷೇತ್ರ = ET.SubElement(xml_param, ಸಂಕೇತ_2)
                            xml_ಕ್ಷೇತ್ರ.ಪಠ್ಯ = ಮೌಲ್ಯ
                ಆದರೆ ಕೊನೆಯ_param_name != param.GetName():
                    board_specific_param_ಗಣ = ಸುಳ್ಳು
                
                ಆದರೆ ಉದ್ದ(param.GetEnumCodes()) > 0:
                    xml_ಮೌಲ್ಯಗಳು = ET.SubElement(xml_param, "values")
                    ಗಾಗಿ ಸಂಕೇತ_2 ಒಳಗೆ param.GetEnumCodes():
                        xml_ಮೌಲ್ಯ = ET.SubElement(xml_ಮೌಲ್ಯಗಳು, "value")
                        xml_ಮೌಲ್ಯ.attrib["code"] = ಸಂಕೇತ_2;
                        xml_ಮೌಲ್ಯ.ಪಠ್ಯ = param.Get_Enum_ಮೌಲ್ಯ(ಸಂಕೇತ_2)

                ಆದರೆ ಉದ್ದ(param.GetBitmaskList()) > 0:
                    xml_ಮೌಲ್ಯಗಳು = ET.SubElement(xml_param, "bitmask")
                    ಗಾಗಿ ಸೂಚ್ಯಂಕ ಒಳಗೆ param.GetBitmaskList():
                        xml_ಮೌಲ್ಯ = ET.SubElement(xml_ಮೌಲ್ಯಗಳು, "bit")
                        xml_ಮೌಲ್ಯ.attrib["index"] = ಸೂಚ್ಯಂಕ;
                        xml_ಮೌಲ್ಯ.ಪಠ್ಯ = param.Get_Bitmask_ಬಿಟ್(ಸೂಚ್ಯಂಕ)

        indent(xml_parameters)
        self.xml_document = ET.Element_ವೃಕ್ಷ(xml_parameters)

    ವ್ಯಾಖ್ಯಾನ Save(self, filename):
        self.xml_document.ಲಿಖ(filename, encoding="UTF-8")
