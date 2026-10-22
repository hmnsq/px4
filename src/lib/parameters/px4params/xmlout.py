దిగుమతి xml.etree.Element_వృక్షం ఇవ ET
దిగుమతి codecs

నిర్వచనం indent(elem, level=0):
    i = "\n" + level*"  "
    ఒకవేళ పొడవు(elem):
        ఒకవేళ కాదు elem.పాఠ్యం లేదా కాదు elem.పాఠ్యం.strip():
            elem.పాఠ్యం = i + "  "
        ఒకవేళ కాదు elem.తోక లేదా కాదు elem.తోక.strip():
            elem.తోక = i
        కొరకు elem లో elem:
            indent(elem, level+1)
        ఒకవేళ కాదు elem.తోక లేదా కాదు elem.తోక.strip():
            elem.తోక = i
    లేకపోతే:
        ఒకవేళ level మరియు (కాదు elem.తోక లేదా కాదు elem.తోక.strip()):
            elem.తోక = i

తరగతి XMLOutput():

    నిర్వచనం __init__(self, groups, board):
        xml_parameters = ET.Element("parameters")
        xml_సంస్కరణ = ET.SubElement(xml_parameters, "version")
        xml_సంస్కరణ.పాఠ్యం = "3"
        xml_సంస్కరణ = ET.SubElement(xml_parameters, "parameter_version_major")
        xml_సంస్కరణ.పాఠ్యం = "1"
        xml_సంస్కరణ = ET.SubElement(xml_parameters, "parameter_version_minor")
        xml_సంస్కరణ.పాఠ్యం = "15"
        చివరి_param_name = ""
        board_specific_param_సమితి = అబద్ధం
        కొరకు సమూహం లో groups:
            xml_సమూహం = ET.SubElement(xml_parameters, "group")
            xml_సమూహం.attrib["name"] = సమూహం.GetName()
            ఒకవేళ సమూహం.no_సంకేతం_generation:
                xml_సమూహం.attrib["no_code_generation"] = సమూహం.no_సంకేతం_generation
            కొరకు param లో సమూహం.GetParams():
                ఒకవేళ (చివరి_param_name == param.GetName() మరియు కాదు board_specific_param_సమితి) లేదా చివరి_param_name != param.GetName():
                    xml_param = ET.SubElement(xml_సమూహం, "parameter")
                    xml_param.attrib["name"] = param.GetName()
                    xml_param.attrib["default"] = param.GetDefault()
                    xml_param.attrib["type"] = param.GetType()
                    ఒకవేళ param.GetVolatile():
                        xml_param.attrib["volatile"] = "true"
                    ఒకవేళ param.GetBoolean():
                        xml_param.attrib["boolean"] = "true"
                    ఒకవేళ param.GetReadonly():
                        xml_param.attrib["readonly"] = "true"
                    ఒకవేళ (param.GetCategory()):
                        xml_param.attrib["category"] = param.GetCategory()
                    చివరి_param_name = param.GetName()
                    కొరకు సంకేతం_2 లో param.Get_క్షేత్రం_Codes():
                        విలువ = param.Get_క్షేత్రం_విలువ(సంకేతం_2)
                        ఒకవేళ సంకేతం_2 == "board":
                            ఒకవేళ విలువ == board:
                                board_specific_param_సమితి = నిజం
                                xml_క్షేత్రం = ET.SubElement(xml_param, సంకేతం_2)
                                xml_క్షేత్రం.పాఠ్యం = విలువ
                            లేకపోతే:
                                xml_సమూహం.remove(xml_param)
                        లేకపోతే:
                            xml_క్షేత్రం = ET.SubElement(xml_param, సంకేతం_2)
                            xml_క్షేత్రం.పాఠ్యం = విలువ
                ఒకవేళ చివరి_param_name != param.GetName():
                    board_specific_param_సమితి = అబద్ధం
                
                ఒకవేళ పొడవు(param.GetEnumCodes()) > 0:
                    xml_విలువలు = ET.SubElement(xml_param, "values")
                    కొరకు సంకేతం_2 లో param.GetEnumCodes():
                        xml_విలువ = ET.SubElement(xml_విలువలు, "value")
                        xml_విలువ.attrib["code"] = సంకేతం_2;
                        xml_విలువ.పాఠ్యం = param.Get_Enum_విలువ(సంకేతం_2)

                ఒకవేళ పొడవు(param.GetBitmaskList()) > 0:
                    xml_విలువలు = ET.SubElement(xml_param, "bitmask")
                    కొరకు సూచీ లో param.GetBitmaskList():
                        xml_విలువ = ET.SubElement(xml_విలువలు, "bit")
                        xml_విలువ.attrib["index"] = సూచీ;
                        xml_విలువ.పాఠ్యం = param.Get_Bitmask_బిట్టు(సూచీ)

        indent(xml_parameters)
        self.xml_document = ET.Element_వృక్షం(xml_parameters)

    నిర్వచనం Save(self, filename):
        self.xml_document.లిఖ(filename, encoding="UTF-8")
