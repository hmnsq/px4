من xml.sax.saxutils استورد escape
استورد codecs
استورد json
استورد sys


صنف JsonOutput():
    وصف __init__(self, groups, board):
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

        آخري_param_name = ""
        board_specific_param_سيٽ = خطأ
        لكل گروهه في groups:
            گروهه_name=گروهه.GetName()

            وصف get_typed_قدر(قدر: str, type_name: str):
                إذا type_name == 'Float': أرجع عائم(قدر)
                إذا type_name == 'Int32': أرجع صحيح(قدر)
                أرجع قدر

            لكل param في گروهه.GetParams():
                إذا (آخري_param_name == param.GetName() و ليس board_specific_param_سيٽ) أو آخري_param_name != param.GetName():
                    curr_param=dict()
                    curr_param['name'] = param.GetName()
                    type_name = param.GetType().capitalize()
                    curr_param['type'] = type_name
                    إذا ليس type_name في allowed_types:
                        ڇاپيو("Error: %s type not supported: curr_param['type']" % (curr_param['name'],curr_param['type']) )
                        sys.Exit(1)
                    curr_param['default'] = get_typed_قدر(param.GetDefault(), type_name)

                    curr_param['group'] = گروهه_name
                    إذا param.GetCategory():
                        curr_param['category'] = param.GetCategory()
                    وإلا:
                        curr_param['category'] = 'Standard'

                    إذا param.GetVolatile():
                        curr_param['volatile'] = سچ
                    إذا param.GetReadonly():
                        curr_param['readOnly'] = سچ

                    آخري_param_name = param.GetName()
                    لكل ڪوڊ في param.Get_ميدان_Codes():
                        قدر = param.Get_ميدان_قدر(ڪوڊ)
                        إذا ڪوڊ == "board":
                            إذا قدر == board:
                                board_specific_param_سيٽ = سچ
                                # JSON schema has no field for board_specific schema. Ignore.
                            وإلا:
                                #xml_group.remove(xml_param)
                                استمر
                        وإلا:
                            #map PX4 param field names to schema names
                            إذا ڪوڊ في schema_map:
                                curr_param[schema_map[ڪوڊ]] = قدر
                            وإلاإذا ڪوڊ في schema_map_typed:
                                curr_param[schema_map_typed[ڪوڊ]] = get_typed_قدر(قدر, type_name)
                            وإلاإذا ڪوڊ في schema_map_fix_type:
                                curr_param[schema_map_fix_type[ڪوڊ][0]] = schema_map_fix_type[ڪوڊ][1](قدر)
                            وإلا:
                                ڇاپيو('ERROR: Field not in json schema: %s' % ڪوڊ)
                                sys.exit(1)


                إذا آخري_param_name != param.GetName():
                    board_specific_param_سيٽ = خطأ

                enum_codes=param.GetEnumCodes() أو '' # Gets numerical values for parameter.
                إذا enum_codes:
                    enum_codes=sorted(enum_codes,ڪنجي=عائم)
                    codes_list=list()
                    لكل شي في enum_codes:
                        ڪوڊ_dict=dict()
                        ڪوڊ_dict['value']=get_typed_قدر(شي, type_name)
                        ڪوڊ_dict['description']=param.Get_Enum_قدر(شي)
                        codes_list.append(ڪوڊ_dict)
                    curr_param['values'] = codes_list
                وإلاإذا param.GetBoolean():
                    curr_param['values'] = [
                        { 'value': 0, 'description': 'Disabled' },
                        { 'value': 1, 'description': 'Enabled' }
                    ]


                إذا ڊيگهه(param.GetBitmaskList()) > 0:
                    bitmasks_list=list()
                    لكل اشاريو في param.GetBitmaskList():
                        bitmask_dict=dict()
                        bitmask_dict['index']=صحيح(اشاريو)
                        bitmask_dict['description']=param.Get_Bitmask_ٻٽو_انگ(اشاريو)
                        bitmasks_list.append(bitmask_dict)
                    curr_param['bitmask'] = bitmasks_list


                all_params.append(curr_param)


        #Json string output.
        self.output = json.dumps(all_json, ترتيب_ڏيو_ڪنجيون=سچ)


    وصف Save(self, filename):
        سان codecs.open(filename, 'w', 'utf-8') كـ f:
            f.اكتب(self.output)
