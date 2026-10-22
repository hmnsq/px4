ਤਃ xml.sax.saxutils ਆਯਾਤ escape
ਆਯਾਤ codecs
ਆਯਾਤ json
ਆਯਾਤ sys


ਵਰਗ JsonOutput():
    ਪਰਿਭਾਸ਼ਾ __init__(self, groups, board):
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
			"decimal": ("decimalPlaces", ਪੂਰਨ),
			}
        allowed_types = { "Uint8", "Int8", "Uint16", "Int16", "Uint32", "Int32", "Float"}

        ਆਖਰੀ_param_name = ""
        board_specific_param_ਸੈੱਟ = ਝੂਠ
        ਲਈ ਸਮੂਹ ਵਿੱਚ groups:
            ਸਮੂਹ_name=ਸਮੂਹ.GetName()

            ਪਰਿਭਾਸ਼ਾ get_typed_ਮੁੱਲ(ਮੁੱਲ: str, type_name: str):
                ਜੇ type_name == 'Float': ਵਾਪਸ ਦਸ਼ਮਲਵ(ਮੁੱਲ)
                ਜੇ type_name == 'Int32': ਵਾਪਸ ਪੂਰਨ(ਮੁੱਲ)
                ਵਾਪਸ ਮੁੱਲ

            ਲਈ param ਵਿੱਚ ਸਮੂਹ.GetParams():
                ਜੇ (ਆਖਰੀ_param_name == param.GetName() ਅਤੇ ਨਹੀਂ board_specific_param_ਸੈੱਟ) ਜਾਂ ਆਖਰੀ_param_name != param.GetName():
                    curr_param=dict()
                    curr_param['name'] = param.GetName()
                    type_name = param.GetType().capitalize()
                    curr_param['type'] = type_name
                    ਜੇ ਨਹੀਂ type_name ਵਿੱਚ allowed_types:
                        ਛਾਪੋ("Error: %s type not supported: curr_param['type']" % (curr_param['name'],curr_param['type']) )
                        sys.Exit(1)
                    curr_param['default'] = get_typed_ਮੁੱਲ(param.GetDefault(), type_name)

                    curr_param['group'] = ਸਮੂਹ_name
                    ਜੇ param.GetCategory():
                        curr_param['category'] = param.GetCategory()
                    ਨਹੀਂਤਾਂ:
                        curr_param['category'] = 'Standard'

                    ਜੇ param.GetVolatile():
                        curr_param['volatile'] = ਸੱਚ
                    ਜੇ param.GetReadonly():
                        curr_param['readOnly'] = ਸੱਚ

                    ਆਖਰੀ_param_name = param.GetName()
                    ਲਈ ਕੋਡ ਵਿੱਚ param.Get_ਖੇਤਰ_Codes():
                        ਮੁੱਲ = param.Get_ਖੇਤਰ_ਮੁੱਲ(ਕੋਡ)
                        ਜੇ ਕੋਡ == "board":
                            ਜੇ ਮੁੱਲ == board:
                                board_specific_param_ਸੈੱਟ = ਸੱਚ
                                # JSON schema has no field for board_specific schema. Ignore.
                            ਨਹੀਂਤਾਂ:
                                #xml_group.remove(xml_param)
                                ਜਾਰੀ
                        ਨਹੀਂਤਾਂ:
                            #map PX4 param field names to schema names
                            ਜੇ ਕੋਡ ਵਿੱਚ schema_map:
                                curr_param[schema_map[ਕੋਡ]] = ਮੁੱਲ
                            ਜਾਂਜੇ ਕੋਡ ਵਿੱਚ schema_map_typed:
                                curr_param[schema_map_typed[ਕੋਡ]] = get_typed_ਮੁੱਲ(ਮੁੱਲ, type_name)
                            ਜਾਂਜੇ ਕੋਡ ਵਿੱਚ schema_map_fix_type:
                                curr_param[schema_map_fix_type[ਕੋਡ][0]] = schema_map_fix_type[ਕੋਡ][1](ਮੁੱਲ)
                            ਨਹੀਂਤਾਂ:
                                ਛਾਪੋ('ERROR: Field not in json schema: %s' % ਕੋਡ)
                                sys.exit(1)


                ਜੇ ਆਖਰੀ_param_name != param.GetName():
                    board_specific_param_ਸੈੱਟ = ਝੂਠ

                enum_codes=param.GetEnumCodes() ਜਾਂ '' # Gets numerical values for parameter.
                ਜੇ enum_codes:
                    enum_codes=sorted(enum_codes,ਕੁੰਜੀ=ਦਸ਼ਮਲਵ)
                    codes_list=list()
                    ਲਈ ਵਸਤੂ_2 ਵਿੱਚ enum_codes:
                        ਕੋਡ_dict=dict()
                        ਕੋਡ_dict['value']=get_typed_ਮੁੱਲ(ਵਸਤੂ_2, type_name)
                        ਕੋਡ_dict['description']=param.Get_Enum_ਮੁੱਲ(ਵਸਤੂ_2)
                        codes_list.append(ਕੋਡ_dict)
                    curr_param['values'] = codes_list
                ਜਾਂਜੇ param.GetBoolean():
                    curr_param['values'] = [
                        { 'value': 0, 'description': 'Disabled' },
                        { 'value': 1, 'description': 'Enabled' }
                    ]


                ਜੇ ਲੰਬਾਈ(param.GetBitmaskList()) > 0:
                    bitmasks_list=list()
                    ਲਈ ਸੂਚਕਾਂਕ ਵਿੱਚ param.GetBitmaskList():
                        bitmask_dict=dict()
                        bitmask_dict['index']=ਪੂਰਨ(ਸੂਚਕਾਂਕ)
                        bitmask_dict['description']=param.Get_Bitmask_ਦੋਆਧਾਰੀ_ਅੰਕ(ਸੂਚਕਾਂਕ)
                        bitmasks_list.append(bitmask_dict)
                    curr_param['bitmask'] = bitmasks_list


                all_params.append(curr_param)


        #Json string output.
        self.output = json.dumps(all_json, ਛਾਂਟੋ_ਕੁੰਜੀਆਂ=ਸੱਚ)


    ਪਰਿਭਾਸ਼ਾ Save(self, filename):
        ਨਾਲ codecs.open(filename, 'w', 'utf-8') ਇਵ f:
            f.ਲਿਖ(self.output)
