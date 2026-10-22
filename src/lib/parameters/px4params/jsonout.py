from xml.sax.saxutils import escape
import codecs
import json
import sys


class JsonOutput():
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
			"decimal": ("decimalPlaces", 整数),
			}
        allowed_types = { "Uint8", "Int8", "Uint16", "Int16", "Uint32", "Int32", "Float"}

        last_param_name = ""
        board_specific_param_set = False
        为 group in groups:
            group_name=group.GetName()

            def get_typed_value(value: str, type_name: str):
                如果 type_name == 'Float': 返回 float(value)
                如果 type_name == 'Int32': 返回 整数(value)
                返回 value

            为 param in group.GetParams():
                如果 (last_param_name == param.GetName() and not board_specific_param_set) or last_param_name != param.GetName():
                    curr_param=dict()
                    curr_param['name'] = param.GetName()
                    type_name = param.GetType().capitalize()
                    curr_param['type'] = type_name
                    如果 not type_name in allowed_types:
                        print("Error: %s type not supported: curr_param['type']" % (curr_param['name'],curr_param['type']) )
                        sys.Exit(1)
                    curr_param['default'] = get_typed_value(param.GetDefault(), type_name)

                    curr_param['group'] = group_name
                    如果 param.GetCategory():
                        curr_param['category'] = param.GetCategory()
                    否则:
                        curr_param['category'] = 'Standard'

                    如果 param.GetVolatile():
                        curr_param['volatile'] = True
                    如果 param.GetReadonly():
                        curr_param['readOnly'] = True

                    last_param_name = param.GetName()
                    为 code in param.GetFieldCodes():
                        value = param.GetFieldValue(code)
                        如果 code == "board":
                            如果 value == board:
                                board_specific_param_set = True
                                # JSON schema has no field for board_specific schema. Ignore.
                            否则:
                                #xml_group.remove(xml_param)
                                continue
                        否则:
                            #map PX4 param field names to schema names
                            如果 code in schema_map:
                                curr_param[schema_map[code]] = value
                            elif code in schema_map_typed:
                                curr_param[schema_map_typed[code]] = get_typed_value(value, type_name)
                            elif code in schema_map_fix_type:
                                curr_param[schema_map_fix_type[code][0]] = schema_map_fix_type[code][1](value)
                            否则:
                                print('ERROR: Field not in json schema: %s' % code)
                                sys.exit(1)


                如果 last_param_name != param.GetName():
                    board_specific_param_set = False

                enum_codes=param.GetEnumCodes() or '' # Gets numerical values for parameter.
                如果 enum_codes:
                    enum_codes=sorted(enum_codes,key=float)
                    codes_list=list()
                    为 item in enum_codes:
                        code_dict=dict()
                        code_dict['value']=get_typed_value(item, type_name)
                        code_dict['description']=param.GetEnumValue(item)
                        codes_list.append(code_dict)
                    curr_param['values'] = codes_list
                elif param.GetBoolean():
                    curr_param['values'] = [
                        { 'value': 0, 'description': 'Disabled' },
                        { 'value': 1, 'description': 'Enabled' }
                    ]


                如果 len(param.GetBitmaskList()) > 0:
                    bitmasks_list=list()
                    为 index in param.GetBitmaskList():
                        bitmask_dict=dict()
                        bitmask_dict['index']=整数(index)
                        bitmask_dict['description']=param.GetBitmaskBit(index)
                        bitmasks_list.append(bitmask_dict)
                    curr_param['bitmask'] = bitmasks_list


                all_params.append(curr_param)


        #Json string output.
        self.output = json.dumps(all_json, sort_keys=True)


    def Save(self, filename):
        with codecs.open(filename, 'w', 'utf-8') as f:
            f.write(self.output)
