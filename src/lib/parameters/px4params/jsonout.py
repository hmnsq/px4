तः xml.sax.saxutils आनयन escape
आनयन codecs
आनयन json
आनयन sys


वर्ग JsonOutput():
    def __init__(self, groups, board):
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

        last_param_name = ""
        board_specific_param_set = असत्य
        प्रति group अन्तः groups:
            group_name=group.GetName()

            def get_typed_मूल्य(मूल्य: str, type_name: str):
                यदि type_name == 'Float': प्रत्यागम भिन्न(मूल्य)
                यदि type_name == 'Int32': प्रत्यागम पूर्णाङ्क(मूल्य)
                प्रत्यागम मूल्य

            प्रति param अन्तः group.GetParams():
                यदि (last_param_name == param.GetName() च न board_specific_param_set) वा last_param_name != param.GetName():
                    curr_param=dict()
                    curr_param['name'] = param.GetName()
                    type_name = param.GetType().capitalize()
                    curr_param['type'] = type_name
                    यदि न type_name अन्तः allowed_types:
                        print("Error: %s type not supported: curr_param['type']" % (curr_param['name'],curr_param['type']) )
                        sys.Exit(1)
                    curr_param['default'] = get_typed_मूल्य(param.GetDefault(), type_name)

                    curr_param['group'] = group_name
                    यदि param.GetCategory():
                        curr_param['category'] = param.GetCategory()
                    अन्यथा:
                        curr_param['category'] = 'Standard'

                    यदि param.GetVolatile():
                        curr_param['volatile'] = सत्य
                    यदि param.GetReadonly():
                        curr_param['readOnly'] = सत्य

                    last_param_name = param.GetName()
                    प्रति code अन्तः param.GetFieldCodes():
                        मूल्य = param.Get_Field_मूल्य(code)
                        यदि code == "board":
                            यदि मूल्य == board:
                                board_specific_param_set = सत्य
                                # JSON schema has no field for board_specific schema. Ignore.
                            अन्यथा:
                                #xml_group.remove(xml_param)
                                अनुवर्त
                        अन्यथा:
                            #map PX4 param field names to schema names
                            यदि code अन्तः schema_map:
                                curr_param[schema_map[code]] = मूल्य
                            नोचेद्यदि code अन्तः schema_map_typed:
                                curr_param[schema_map_typed[code]] = get_typed_मूल्य(मूल्य, type_name)
                            नोचेद्यदि code अन्तः schema_map_fix_type:
                                curr_param[schema_map_fix_type[code][0]] = schema_map_fix_type[code][1](मूल्य)
                            अन्यथा:
                                print('ERROR: Field not in json schema: %s' % code)
                                sys.exit(1)


                यदि last_param_name != param.GetName():
                    board_specific_param_set = असत्य

                enum_codes=param.GetEnumCodes() वा '' # Gets numerical values for parameter.
                यदि enum_codes:
                    enum_codes=sorted(enum_codes,किल्ली=भिन्न)
                    codes_list=list()
                    प्रति item अन्तः enum_codes:
                        code_dict=dict()
                        code_dict['value']=get_typed_मूल्य(item, type_name)
                        code_dict['description']=param.Get_Enum_मूल्य(item)
                        codes_list.append(code_dict)
                    curr_param['values'] = codes_list
                नोचेद्यदि param.GetBoolean():
                    curr_param['values'] = [
                        { 'value': 0, 'description': 'Disabled' },
                        { 'value': 1, 'description': 'Enabled' }
                    ]


                यदि len(param.GetBitmaskList()) > 0:
                    bitmasks_list=list()
                    प्रति index अन्तः param.GetBitmaskList():
                        bitmask_dict=dict()
                        bitmask_dict['index']=पूर्णाङ्क(index)
                        bitmask_dict['description']=param.GetBitmaskBit(index)
                        bitmasks_list.append(bitmask_dict)
                    curr_param['bitmask'] = bitmasks_list


                all_params.append(curr_param)


        #Json string output.
        self.output = json.dumps(all_json, sort_किल्ल्या=सत्य)


    def Save(self, filename):
        with codecs.open(filename, 'w', 'utf-8') इव f:
            f.लिख(self.output)
