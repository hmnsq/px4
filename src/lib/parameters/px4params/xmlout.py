आनयन xml.etree.Element_वृक्षः इव ET
आनयन codecs

परिभाषा indent(elem, level=0):
    i = "\n" + level*"  "
    यदि दीर्घता(elem):
        यदि न elem.text वा न elem.text.strip():
            elem.text = i + "  "
        यदि न elem.पुच्छ वा न elem.पुच्छ.strip():
            elem.पुच्छ = i
        प्रति elem अन्तः elem:
            indent(elem, level+1)
        यदि न elem.पुच्छ वा न elem.पुच्छ.strip():
            elem.पुच्छ = i
    अन्यथा:
        यदि level च (न elem.पुच्छ वा न elem.पुच्छ.strip()):
            elem.पुच्छ = i

वर्ग XMLOutput():

    परिभाषा __init__(self, groups, board):
        xml_parameters = ET.Element("parameters")
        xml_संस्करणम् = ET.SubElement(xml_parameters, "version")
        xml_संस्करणम्.text = "3"
        xml_संस्करणम् = ET.SubElement(xml_parameters, "parameter_version_major")
        xml_संस्करणम्.text = "1"
        xml_संस्करणम् = ET.SubElement(xml_parameters, "parameter_version_minor")
        xml_संस्करणम्.text = "15"
        चरमः_param_name = ""
        board_specific_param_समुच्चयः = असत्य
        प्रति गणः अन्तः groups:
            xml_गणः = ET.SubElement(xml_parameters, "group")
            xml_गणः.attrib["name"] = गणः.GetName()
            यदि गणः.no_संहिता_generation:
                xml_गणः.attrib["no_code_generation"] = गणः.no_संहिता_generation
            प्रति param अन्तः गणः.GetParams():
                यदि (चरमः_param_name == param.GetName() च न board_specific_param_समुच्चयः) वा चरमः_param_name != param.GetName():
                    xml_param = ET.SubElement(xml_गणः, "parameter")
                    xml_param.attrib["name"] = param.GetName()
                    xml_param.attrib["default"] = param.GetDefault()
                    xml_param.attrib["type"] = param.GetType()
                    यदि param.GetVolatile():
                        xml_param.attrib["volatile"] = "true"
                    यदि param.GetBoolean():
                        xml_param.attrib["boolean"] = "true"
                    यदि param.GetReadonly():
                        xml_param.attrib["readonly"] = "true"
                    यदि (param.GetCategory()):
                        xml_param.attrib["category"] = param.GetCategory()
                    चरमः_param_name = param.GetName()
                    प्रति संहिता अन्तः param.Get_क्षेत्रम्_Codes():
                        मानम् = param.Get_क्षेत्रम्_मानम्(संहिता)
                        यदि संहिता == "board":
                            यदि मानम् == board:
                                board_specific_param_समुच्चयः = सत्य
                                xml_क्षेत्रम् = ET.SubElement(xml_param, संहिता)
                                xml_क्षेत्रम्.text = मानम्
                            अन्यथा:
                                xml_गणः.remove(xml_param)
                        अन्यथा:
                            xml_क्षेत्रम् = ET.SubElement(xml_param, संहिता)
                            xml_क्षेत्रम्.text = मानम्
                यदि चरमः_param_name != param.GetName():
                    board_specific_param_समुच्चयः = असत्य
                
                यदि दीर्घता(param.GetEnumCodes()) > 0:
                    xml_मानानि = ET.SubElement(xml_param, "values")
                    प्रति संहिता अन्तः param.GetEnumCodes():
                        xml_मानम् = ET.SubElement(xml_मानानि, "value")
                        xml_मानम्.attrib["code"] = संहिता;
                        xml_मानम्.text = param.Get_Enum_मानम्(संहिता)

                यदि दीर्घता(param.GetBitmaskList()) > 0:
                    xml_मानानि = ET.SubElement(xml_param, "bitmask")
                    प्रति सूचकांक अन्तः param.GetBitmaskList():
                        xml_मानम् = ET.SubElement(xml_मानानि, "bit")
                        xml_मानम्.attrib["index"] = सूचकांक;
                        xml_मानम्.text = param.GetBitmaskBit(सूचकांक)

        indent(xml_parameters)
        self.xml_document = ET.Element_वृक्षः(xml_parameters)

    परिभाषा Save(self, filename):
        self.xml_document.लिख(filename, encoding="UTF-8")
