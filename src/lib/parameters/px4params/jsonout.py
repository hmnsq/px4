از xml.sax.saxutils واردکن escape
واردکن codecs
واردکن json
واردکن sys


رده JsonOutput():
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
			"decimal": ("decimalPlaces", صحیح),
			}
        allowed_types = { "Uint8", "Int8", "Uint16", "Int16", "Uint32", "Int32", "Float"}

        last_param_name = ""
        board_specific_param_set = نادرست
        برای group در groups:
            group_name=group.GetName()

            def get_typed_مقدار(مقدار: str, type_name: str):
                اگر type_name == 'Float': بازگشت اعشاری(مقدار)
                اگر type_name == 'Int32': بازگشت صحیح(مقدار)
                بازگشت مقدار

            برای param در group.GetParams():
                اگر (last_param_name == param.GetName() و نه board_specific_param_set) یا last_param_name != param.GetName():
                    curr_param=dict()
                    curr_param['name'] = param.GetName()
                    type_name = param.GetType().capitalize()
                    curr_param['type'] = type_name
                    اگر نه type_name در allowed_types:
                        print("Error: %s type not supported: curr_param['type']" % (curr_param['name'],curr_param['type']) )
                        sys.Exit(1)
                    curr_param['default'] = get_typed_مقدار(param.GetDefault(), type_name)

                    curr_param['group'] = group_name
                    اگر param.GetCategory():
                        curr_param['category'] = param.GetCategory()
                    وگرنه:
                        curr_param['category'] = 'Standard'

                    اگر param.GetVolatile():
                        curr_param['volatile'] = درست
                    اگر param.GetReadonly():
                        curr_param['readOnly'] = درست

                    last_param_name = param.GetName()
                    برای code در param.GetFieldCodes():
                        مقدار = param.Get_Field_مقدار(code)
                        اگر code == "board":
                            اگر مقدار == board:
                                board_specific_param_set = درست
                                # JSON schema has no field for board_specific schema. Ignore.
                            وگرنه:
                                #xml_group.remove(xml_param)
                                ادامه
                        وگرنه:
                            #map PX4 param field names to schema names
                            اگر code در schema_map:
                                curr_param[schema_map[code]] = مقدار
                            وگرنهاگر code در schema_map_typed:
                                curr_param[schema_map_typed[code]] = get_typed_مقدار(مقدار, type_name)
                            وگرنهاگر code در schema_map_fix_type:
                                curr_param[schema_map_fix_type[code][0]] = schema_map_fix_type[code][1](مقدار)
                            وگرنه:
                                print('ERROR: Field not in json schema: %s' % code)
                                sys.exit(1)


                اگر last_param_name != param.GetName():
                    board_specific_param_set = نادرست

                enum_codes=param.GetEnumCodes() یا '' # Gets numerical values for parameter.
                اگر enum_codes:
                    enum_codes=sorted(enum_codes,کلید=اعشاری)
                    codes_list=list()
                    برای item در enum_codes:
                        code_dict=dict()
                        code_dict['value']=get_typed_مقدار(item, type_name)
                        code_dict['description']=param.Get_Enum_مقدار(item)
                        codes_list.append(code_dict)
                    curr_param['values'] = codes_list
                وگرنهاگر param.GetBoolean():
                    curr_param['values'] = [
                        { 'value': 0, 'description': 'Disabled' },
                        { 'value': 1, 'description': 'Enabled' }
                    ]


                اگر len(param.GetBitmaskList()) > 0:
                    bitmasks_list=list()
                    برای index در param.GetBitmaskList():
                        bitmask_dict=dict()
                        bitmask_dict['index']=صحیح(index)
                        bitmask_dict['description']=param.GetBitmaskBit(index)
                        bitmasks_list.append(bitmask_dict)
                    curr_param['bitmask'] = bitmasks_list


                all_params.append(curr_param)


        #Json string output.
        self.output = json.dumps(all_json, sort_کلیدها=درست)


    def Save(self, filename):
        with codecs.open(filename, 'w', 'utf-8') بهعنوان f:
            f.اكتب(self.output)
