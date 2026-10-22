من xml.sax.saxutils استورد escape
استورد codecs
استورد json
استورد sys


صنف JsonOutput():
    تعریف __init__(self, groups, board):
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
			"decimal": ("decimalPlaces", صحيح),
			}
        allowed_types = { "Uint8", "Int8", "Uint16", "Int16", "Uint32", "Int32", "Float"}

        آخرین_param_name = ""
        board_specific_param_مجموعه = خطأ
        لكل گروه في groups:
            گروه_name=گروه.GetName()

            تعریف get_typed_مقدار(مقدار: str, type_name: str):
                إذا type_name == 'Float': أرجع عائم(مقدار)
                إذا type_name == 'Int32': أرجع صحيح(مقدار)
                أرجع مقدار

            لكل param في گروه.GetParams():
                إذا (آخرین_param_name == param.GetName() و ليس board_specific_param_مجموعه) أو آخرین_param_name != param.GetName():
                    curr_param=dict()
                    curr_param['name'] = param.GetName()
                    type_name = param.GetType().capitalize()
                    curr_param['type'] = type_name
                    إذا ليس type_name في allowed_types:
                        چاپ("Error: %s type not supported: curr_param['type']" % (curr_param['name'],curr_param['type']) )
                        sys.Exit(1)
                    curr_param['default'] = get_typed_مقدار(param.GetDefault(), type_name)

                    curr_param['group'] = گروه_name
                    إذا param.GetCategory():
                        curr_param['category'] = param.GetCategory()
                    وإلا:
                        curr_param['category'] = 'Standard'

                    إذا param.GetVolatile():
                        curr_param['volatile'] = درست
                    إذا param.GetReadonly():
                        curr_param['readOnly'] = درست

                    آخرین_param_name = param.GetName()
                    لكل کد في param.Get_فیلد_Codes():
                        مقدار = param.Get_فیلد_مقدار(کد)
                        إذا کد == "board":
                            إذا مقدار == board:
                                board_specific_param_مجموعه = درست
                                # JSON schema has no field for board_specific schema. Ignore.
                            وإلا:
                                #xml_group.remove(xml_param)
                                استمر
                        وإلا:
                            #map PX4 param field names to schema names
                            إذا کد في schema_map:
                                curr_param[schema_map[کد]] = مقدار
                            وإلاإذا کد في schema_map_typed:
                                curr_param[schema_map_typed[کد]] = get_typed_مقدار(مقدار, type_name)
                            وإلاإذا کد في schema_map_fix_type:
                                curr_param[schema_map_fix_type[کد][0]] = schema_map_fix_type[کد][1](مقدار)
                            وإلا:
                                چاپ('ERROR: Field not in json schema: %s' % کد)
                                sys.exit(1)


                إذا آخرین_param_name != param.GetName():
                    board_specific_param_مجموعه = خطأ

                enum_codes=param.GetEnumCodes() أو '' # Gets numerical values for parameter.
                إذا enum_codes:
                    enum_codes=sorted(enum_codes,کلید=عائم)
                    codes_list=list()
                    لكل قلم في enum_codes:
                        کد_dict=dict()
                        کد_dict['value']=get_typed_مقدار(قلم, type_name)
                        کد_dict['description']=param.Get_Enum_مقدار(قلم)
                        codes_list.append(کد_dict)
                    curr_param['values'] = codes_list
                وإلاإذا param.GetBoolean():
                    curr_param['values'] = [
                        { 'value': 0, 'description': 'Disabled' },
                        { 'value': 1, 'description': 'Enabled' }
                    ]


                إذا طول(param.GetBitmaskList()) > 0:
                    bitmasks_list=list()
                    لكل شاخص في param.GetBitmaskList():
                        bitmask_dict=dict()
                        bitmask_dict['index']=صحيح(شاخص)
                        bitmask_dict['description']=param.Get_Bitmask_بیت(شاخص)
                        bitmasks_list.append(bitmask_dict)
                    curr_param['bitmask'] = bitmasks_list


                all_params.append(curr_param)


        #Json string output.
        self.output = json.dumps(all_json, مرتب_سازی_کلیدها=درست)


    تعریف Save(self, filename):
        با codecs.open(filename, 'w', 'utf-8') كـ f:
            f.اكتب(self.output)
