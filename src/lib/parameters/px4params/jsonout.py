तः xml.sax.saxutils आनयन escape
आनयन codecs
आनयन json
आनयन sys


वर्ग JsonOutput():
    परिभाषा __init__(self, groups, board):
        all_json=dict()
        all_json['version']=1
        all_params=[]
        all_json['parameters']=all_params

        all_json["translation"] = {
            "items": {
                "parameters": {
                    "list": {
                        "key": "name",
                        "translate": [ "shortDesc", "longDesc" ],
                        "translate-global": ["category", "group"],
                        "items": {
                            "bitmask": {
                                "list": {
                                    "key": "index",
                                    "translate": [ "description" ]
                                }
                            },
                            "values": {
                                "list": {
                                    "key": "value",
                                    "translate": [ "description" ]
                                }
                            }
                        }
                    }
                }
            }
        }

        schema_map = {
                        "short_desc": "shortDesc",
			"long_desc": "longDesc",
			"unit": "units",
			}
        schema_map_typed = {
			"min": "min",
			"max": "max",
			"increment": "increment",
			}
        schema_map_fix_type = {
			"reboot_required": ("rebootRequired", bool),
			"decimal": ("decimalPlaces", पूर्णाङ्क),
			}
        allowed_types = { "Uint8", "Int8", "Uint16", "Int16", "Uint32", "Int32", "Float"}

        चरमः_param_name = ""
        board_specific_param_समुच्चयः = असत्य
        प्रति गणः अन्तः groups:
            गणः_name=गणः.GetName()

            परिभाषा get_typed_मानम्(मानम्: str, type_name: str):
                यदि type_name == 'Float': प्रत्यागम भिन्न(मानम्)
                यदि type_name == 'Int32': प्रत्यागम पूर्णाङ्क(मानम्)
                प्रत्यागम मानम्

            प्रति param अन्तः गणः.GetParams():
                यदि (चरमः_param_name == param.GetName() च न board_specific_param_समुच्चयः) वा चरमः_param_name != param.GetName():
                    curr_param=dict()
                    curr_param['name'] = param.GetName()
                    type_name = param.GetType().capitalize()
                    curr_param['type'] = type_name
                    यदि न type_name अन्तः allowed_types:
                        मुद्रय("Error: %s type not supported: curr_param['type']" % (curr_param['name'],curr_param['type']) )
                        sys.Exit(1)
                    curr_param['default'] = get_typed_मानम्(param.GetDefault(), type_name)

                    curr_param['group'] = गणः_name
                    यदि param.GetCategory():
                        curr_param['category'] = param.GetCategory()
                    अन्यथा:
                        curr_param['category'] = 'Standard'

                    यदि param.GetVolatile():
                        curr_param['volatile'] = सत्य
                    यदि param.GetReadonly():
                        curr_param['readOnly'] = सत्य

                    चरमः_param_name = param.GetName()
                    प्रति संहिता अन्तः param.Get_क्षेत्रम्_Codes():
                        मानम् = param.Get_क्षेत्रम्_मानम्(संहिता)
                        यदि संहिता == "board":
                            यदि मानम् == board:
                                board_specific_param_समुच्चयः = सत्य
                                # JSON schema has no field for board_specific schema. Ignore.
                            अन्यथा:
                                #xml_group.remove(xml_param)
                                अनुवर्त
                        अन्यथा:
                            #map PX4 param field names to schema names
                            यदि संहिता अन्तः schema_map:
                                curr_param[schema_map[संहिता]] = मानम्
                            नोचेद्यदि संहिता अन्तः schema_map_typed:
                                curr_param[schema_map_typed[संहिता]] = get_typed_मानम्(मानम्, type_name)
                            नोचेद्यदि संहिता अन्तः schema_map_fix_type:
                                curr_param[schema_map_fix_type[संहिता][0]] = schema_map_fix_type[संहिता][1](मानम्)
                            अन्यथा:
                                मुद्रय('ERROR: Field not in json schema: %s' % संहिता)
                                sys.exit(1)


                यदि चरमः_param_name != param.GetName():
                    board_specific_param_समुच्चयः = असत्य

                enum_codes=param.GetEnumCodes() वा '' # Gets numerical values for parameter.
                यदि enum_codes:
                    enum_codes=sorted(enum_codes,कुञ्चिका=भिन्न)
                    codes_list=list()
                    प्रति item अन्तः enum_codes:
                        संहिता_dict=dict()
                        संहिता_dict['value']=get_typed_मानम्(item, type_name)
                        संहिता_dict['description']=param.Get_Enum_मानम्(item)
                        codes_list.append(संहिता_dict)
                    curr_param['values'] = codes_list
                नोचेद्यदि param.GetBoolean():
                    curr_param['values'] = [
                        { 'value': 0, 'description': 'Disabled' },
                        { 'value': 1, 'description': 'Enabled' }
                    ]


                यदि दीर्घता(param.GetBitmaskList()) > 0:
                    bitmasks_list=list()
                    प्रति सूचकांक अन्तः param.GetBitmaskList():
                        bitmask_dict=dict()
                        bitmask_dict['index']=पूर्णाङ्क(सूचकांक)
                        bitmask_dict['description']=param.GetBitmaskBit(सूचकांक)
                        bitmasks_list.append(bitmask_dict)
                    curr_param['bitmask'] = bitmasks_list


                all_params.append(curr_param)


        #Json string output.
        self.output = json.dumps(all_json, क्रमय_कुञ्चिकाः=सत्य)


    परिभाषा Save(self, filename):
        सह codecs.open(filename, 'w', 'utf-8') इव f:
            f.लिख(self.output)
