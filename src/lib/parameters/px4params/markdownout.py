من xml.sax.saxutils استورد escape
استورد codecs
استورد html

صنف MarkdownTablesOutput():
    تعریف __init__(self, groups):
        نتیجه = (
"""# Parameter Reference

::: info
This documentation was auto-generated from the source code for this PX4 version (using `make parameters_metadata`).
:::

::: tip
If a listed parameter is missing from the Firmware see: [Finding/Updating Parameters](../advanced_config/parameters.md#parameter-not-in-firmware).
:::

<!-- markdown generator: src/lib/parameters/px4params/markdownout.py -->

"""
                  )

        # Define the groups and parameters that are specific to certain boards, to allow for a note to be added to the documentation.
        BOARD_SPECIFIC_GROUPS = ['Actuator Outputs']
        PARAM_IS_OUTPUT_TIMER = ['PWM_MAIN_TIM0', 'PWM_MAIN_TIM1', 'PWM_MAIN_TIM2', 'PWM_AUX_TIM0', 'PWM_AUX_TIM1', 'PWM_AUX_TIM2', 'PWM_AUX_TIM3']

        لكل گروه في groups:
            نتیجه += f'## {group.GetName()}\n\n'

            # Wrap each group, and each parameter within it, so the docs theme can skip rendering
            # off-screen content (see .param-group in docs/.vitepress/theme/style.css).
            # The group heading stays outside the wrapper so the page outline can track it.
            نتیجه += f'<div class="param-group" style="--param-count: {len(group.GetParams())}">\n\n'

            گروه_is_board_specific = (گروه.GetName() في BOARD_SPECIFIC_GROUPS)

            لكل param في گروه.GetParams():
                name = param.GetName()

                # Apply note if either the group is board specific
                apply_note_board_specific_گروه = گروه_is_board_specific

                # Apply note if name is in list of timer group parameters
                apply_timer_param_note = (name في PARAM_IS_OUTPUT_TIMER)

                short_desc = param.Get_فیلد_مقدار("short_desc") أو ''

                # Add fullstop to short_desc if not present
                إذا short_desc:
                    إذا ليس short_desc.strip().endswith('.'):
                        short_desc += "."

                long_desc = param.Get_فیلد_مقدار("long_desc") أو ''

                #Strip out short text from start of long text, if it ends in fullstop
                إذا long_desc.startswith(short_desc):
                    long_desc = long_desc[طول(short_desc):].lstrip()

                min_val = param.Get_فیلد_مقدار("min") أو ''
                max_val = param.Get_فیلد_مقدار("max") أو ''
                increment = param.Get_فیلد_مقدار("increment") أو ''
                def_val = param.GetDefault() أو ''
                unit = param.Get_فیلد_مقدار("unit") أو ''
                type = param.GetType()
                is_boolean = param.GetBoolean()
                reboot_required = param.Get_فیلد_مقدار("reboot_required") أو ''
                #board = param.GetFieldValue("board") or '' ## Disabled as no board values are defined in any parameters!
                #decimal = param.GetFieldValue("decimal") or '' #Disabled as is intended for GCS not people
                #field_codes = param.GetFieldCodes() ## Disabled as not needed for display.
                #boolean = param.GetFieldValue("boolean") # or '' # Disabled - does not appear useful.

                # Format values for display.
                enum_codes=param.GetEnumCodes() أو '' # Gets numerical values for parameter.
                enum_output=''
                # Format codes and their descriptions for display.
                إذا enum_codes:
                    enum_output+='**Values:**\n\n'
                    enum_codes=sorted(enum_codes,کلید=عائم)
                    لكل قلم في enum_codes:
                        enum_output+=f"- `{item}`: {param.GetEnumValue(item)}\n"
                    enum_output+='\n\n'

                bitmask_list=param.GetBitmaskList() #Gets bitmask values for parameter
                bitmask_output=''
                #Format bitmask values
                إذا bitmask_list:
                    bitmask_output+='**Bitmask:**\n\n'
                    لكل بیت في bitmask_list:
                        بیت_متن = param.Get_Bitmask_بیت(بیت)
                        bitmask_output+=f"- `{bit}`: {bit_text}\n"
                    bitmask_output+='\n\n'

                إذا is_boolean:
                    BOOLEAN_LABELS = {
                        '0': 'Disabled',
                        '1': 'Enabled'
                    }
                    # Give def_value an explanatory string (is def_value if no match)
                    def_val = f"{BOOLEAN_LABELS[def_val]} ({def_val})" إذا def_val في BOOLEAN_LABELS وإلا def_val

                    # Format values and their descriptions for display.
                    boolean_مقادیر = '**Values:**\n\n'
                    لكل کلید, label في BOOLEAN_LABELS.اقلام_2():
                        boolean_مقادیر += f"- `{key}`: {label}\n"
                    boolean_مقادیر += '\n'

                نتیجه += '<div class="param">\n\n'
                نتیجه += f'### {name} (`{type}`)' + ' {#' + name + '}\n\n'
                إذا apply_note_board_specific_گروه:
                    نتیجه += f'<Badge type="warning" text="This parameter is only present on some boards." />\n\n'
                إذا apply_timer_param_note:
                    نتیجه += f'<Badge type="info" text="The exact output numbers where this timer applies are board-specific." />\n\n'
                إذا short_desc:
                    نتیجه += f'{short_desc}\n\n'
                إذا long_desc:
                    نتیجه += f'{long_desc}\n\n'
                إذا is_boolean:
                    نتیجه += boolean_مقادیر
                إذا enum_codes:
                    نتیجه += enum_output
                إذا bitmask_list:
                    نتیجه += bitmask_output
                # Format the ranges as a table.
                is_readonly = param.GetReadonly()
                نتیجه += f"Reboot | minValue | maxValue | increment | default | unit | Read-Only\n--- | --- | --- | --- | --- | --- | ---\n{'&check;' if reboot_required else '&nbsp;' } | {min_val} | {max_val} | {increment} | {def_val} | {unit} | {'&check;' if is_readonly else '&nbsp;'}\n\n"
                نتیجه += '</div>\n\n'

            نتیجه += '</div>\n\n'

        self.output = نتیجه

    تعریف Save(self, filename):
        با codecs.open(filename, 'w', 'utf-8') كـ f:
            f.اكتب(self.output)
