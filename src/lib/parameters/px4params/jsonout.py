ta_h xml.sax.saxutils import escape
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
			"decimal": ("decimalPlaces", จำนวนเต็ม),
			}
        allowed_types = { "Uint8", "Int8", "Uint16", "Int16", "Uint32", "Int32", "Float"}

        last_param_name = ""
        board_specific_param_set = False
        สำหรับ group anta_h groups:
            group_name=group.GetName()

            def get_typed_value(value: str, type_name: str):
                ถ้า type_name == 'Float': คืน float(value)
                ถ้า type_name == 'Int32': คืน จำนวนเต็ม(value)
                คืน value

            สำหรับ param anta_h group.GetParams():
                ถ้า (last_param_name == param.GetName() ca na board_specific_param_set) or last_param_name != param.GetName():
                    curr_param=dict()
                    curr_param['name'] = param.GetName()
                    type_name = param.GetType().capitalize()
                    curr_param['type'] = type_name
                    ถ้า na type_name anta_h allowed_types:
                        print("Error: %s type not supported: curr_param['type']" % (curr_param['name'],curr_param['type']) )
                        sys.Exit(1)
                    curr_param['default'] = get_typed_value(param.GetDefault(), type_name)

                    curr_param['group'] = group_name
                    ถ้า param.GetCategory():
                        curr_param['category'] = param.GetCategory()
                    มิฉะนั้น:
                        curr_param['category'] = 'Standard'

                    ถ้า param.GetVolatile():
                        curr_param['volatile'] = True
                    ถ้า param.GetReadonly():
                        curr_param['readOnly'] = True

                    last_param_name = param.GetName()
                    สำหรับ code anta_h param.GetFieldCodes():
                        value = param.GetFieldValue(code)
                        ถ้า code == "board":
                            ถ้า value == board:
                                board_specific_param_set = True
                                # JSON schema has no field for board_specific schema. Ignore.
                            มิฉะนั้น:
                                #xml_group.remove(xml_param)
                                continue
                        มิฉะนั้น:
                            #map PX4 param field names to schema names
                            ถ้า code anta_h schema_map:
                                curr_param[schema_map[code]] = value
                            elif code anta_h schema_map_typed:
                                curr_param[schema_map_typed[code]] = get_typed_value(value, type_name)
                            elif code anta_h schema_map_fix_type:
                                curr_param[schema_map_fix_type[code][0]] = schema_map_fix_type[code][1](value)
                            มิฉะนั้น:
                                print('ERROR: Field not in json schema: %s' % code)
                                sys.exit(1)


                ถ้า last_param_name != param.GetName():
                    board_specific_param_set = False

                enum_codes=param.GetEnumCodes() or '' # Gets numerical values for parameter.
                ถ้า enum_codes:
                    enum_codes=sorted(enum_codes,key=float)
                    codes_list=list()
                    สำหรับ item anta_h enum_codes:
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


                ถ้า len(param.GetBitmaskList()) > 0:
                    bitmasks_list=list()
                    สำหรับ index anta_h param.GetBitmaskList():
                        bitmask_dict=dict()
                        bitmask_dict['index']=จำนวนเต็ม(index)
                        bitmask_dict['description']=param.GetBitmaskBit(index)
                        bitmasks_list.append(bitmask_dict)
                    curr_param['bitmask'] = bitmasks_list


                all_params.append(curr_param)


        #Json string output.
        self.output = json.dumps(all_json, sort_keys=True)


    def Save(self, filename):
        with codecs.open(filename, 'w', 'utf-8') as f:
            f.write(self.output)
