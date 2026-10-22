ਆਯਾਤ xml.etree.Element_ਰੁੱਖ ਇਵ ET
ਆਯਾਤ codecs

ਪਰਿਭਾਸ਼ਾ indent(elem, level=0):
    i = "\n" + level*"  "
    ਜੇ ਲੰਬਾਈ(elem):
        ਜੇ ਨਹੀਂ elem.ਪਾਠ ਜਾਂ ਨਹੀਂ elem.ਪਾਠ.strip():
            elem.ਪਾਠ = i + "  "
        ਜੇ ਨਹੀਂ elem.ਪੂਛ ਜਾਂ ਨਹੀਂ elem.ਪੂਛ.strip():
            elem.ਪੂਛ = i
        ਲਈ elem ਵਿੱਚ elem:
            indent(elem, level+1)
        ਜੇ ਨਹੀਂ elem.ਪੂਛ ਜਾਂ ਨਹੀਂ elem.ਪੂਛ.strip():
            elem.ਪੂਛ = i
    ਨਹੀਂਤਾਂ:
        ਜੇ level ਅਤੇ (ਨਹੀਂ elem.ਪੂਛ ਜਾਂ ਨਹੀਂ elem.ਪੂਛ.strip()):
            elem.ਪੂਛ = i

ਵਰਗ XMLOutput():

    ਪਰਿਭਾਸ਼ਾ __init__(self, groups, board):
        xml_parameters = ET.Element("parameters")
        xml_ਸੰਸਕਰਣ = ET.SubElement(xml_parameters, "version")
        xml_ਸੰਸਕਰਣ.ਪਾਠ = "3"
        xml_ਸੰਸਕਰਣ = ET.SubElement(xml_parameters, "parameter_version_major")
        xml_ਸੰਸਕਰਣ.ਪਾਠ = "1"
        xml_ਸੰਸਕਰਣ = ET.SubElement(xml_parameters, "parameter_version_minor")
        xml_ਸੰਸਕਰਣ.ਪਾਠ = "15"
        ਆਖਰੀ_param_name = ""
        board_specific_param_ਸੈੱਟ = ਝੂਠ
        ਲਈ ਸਮੂਹ ਵਿੱਚ groups:
            xml_ਸਮੂਹ = ET.SubElement(xml_parameters, "group")
            xml_ਸਮੂਹ.attrib["name"] = ਸਮੂਹ.GetName()
            ਜੇ ਸਮੂਹ.no_ਕੋਡ_generation:
                xml_ਸਮੂਹ.attrib["no_code_generation"] = ਸਮੂਹ.no_ਕੋਡ_generation
            ਲਈ param ਵਿੱਚ ਸਮੂਹ.GetParams():
                ਜੇ (ਆਖਰੀ_param_name == param.GetName() ਅਤੇ ਨਹੀਂ board_specific_param_ਸੈੱਟ) ਜਾਂ ਆਖਰੀ_param_name != param.GetName():
                    xml_param = ET.SubElement(xml_ਸਮੂਹ, "parameter")
                    xml_param.attrib["name"] = param.GetName()
                    xml_param.attrib["default"] = param.GetDefault()
                    xml_param.attrib["type"] = param.GetType()
                    ਜੇ param.GetVolatile():
                        xml_param.attrib["volatile"] = "true"
                    ਜੇ param.GetBoolean():
                        xml_param.attrib["boolean"] = "true"
                    ਜੇ param.GetReadonly():
                        xml_param.attrib["readonly"] = "true"
                    ਜੇ (param.GetCategory()):
                        xml_param.attrib["category"] = param.GetCategory()
                    ਆਖਰੀ_param_name = param.GetName()
                    ਲਈ ਕੋਡ ਵਿੱਚ param.Get_ਖੇਤਰ_Codes():
                        ਮੁੱਲ = param.Get_ਖੇਤਰ_ਮੁੱਲ(ਕੋਡ)
                        ਜੇ ਕੋਡ == "board":
                            ਜੇ ਮੁੱਲ == board:
                                board_specific_param_ਸੈੱਟ = ਸੱਚ
                                xml_ਖੇਤਰ = ET.SubElement(xml_param, ਕੋਡ)
                                xml_ਖੇਤਰ.ਪਾਠ = ਮੁੱਲ
                            ਨਹੀਂਤਾਂ:
                                xml_ਸਮੂਹ.remove(xml_param)
                        ਨਹੀਂਤਾਂ:
                            xml_ਖੇਤਰ = ET.SubElement(xml_param, ਕੋਡ)
                            xml_ਖੇਤਰ.ਪਾਠ = ਮੁੱਲ
                ਜੇ ਆਖਰੀ_param_name != param.GetName():
                    board_specific_param_ਸੈੱਟ = ਝੂਠ
                
                ਜੇ ਲੰਬਾਈ(param.GetEnumCodes()) > 0:
                    xml_ਮੁੱਲਾਂ = ET.SubElement(xml_param, "values")
                    ਲਈ ਕੋਡ ਵਿੱਚ param.GetEnumCodes():
                        xml_ਮੁੱਲ = ET.SubElement(xml_ਮੁੱਲਾਂ, "value")
                        xml_ਮੁੱਲ.attrib["code"] = ਕੋਡ;
                        xml_ਮੁੱਲ.ਪਾਠ = param.Get_Enum_ਮੁੱਲ(ਕੋਡ)

                ਜੇ ਲੰਬਾਈ(param.GetBitmaskList()) > 0:
                    xml_ਮੁੱਲਾਂ = ET.SubElement(xml_param, "bitmask")
                    ਲਈ ਸੂਚਕਾਂਕ ਵਿੱਚ param.GetBitmaskList():
                        xml_ਮੁੱਲ = ET.SubElement(xml_ਮੁੱਲਾਂ, "bit")
                        xml_ਮੁੱਲ.attrib["index"] = ਸੂਚਕਾਂਕ;
                        xml_ਮੁੱਲ.ਪਾਠ = param.Get_Bitmask_ਦੋਆਧਾਰੀ_ਅੰਕ(ਸੂਚਕਾਂਕ)

        indent(xml_parameters)
        self.xml_document = ET.Element_ਰੁੱਖ(xml_parameters)

    ਪਰਿਭਾਸ਼ਾ Save(self, filename):
        self.xml_document.ਲਿਖ(filename, encoding="UTF-8")
