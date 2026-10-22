من xml.sax.saxutils استورد escape
استورد codecs
استورد html

صنف MarkdownTablesOutput():
    وصف __init__(self, groups):
        نتيجو = (
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

        لكل گروهه في groups:
            نتيجو += f'## {group.GetName()}\n\n'

            # Wrap each group, and each parameter within it, so the docs theme can skip rendering
            # off-screen content (see .param-group in docs/.vitepress/theme/style.css).
            # The group heading stays outside the wrapper so the page outline can track it.
            نتيجو += f'<div class="param-group" style="--param-count: {len(group.GetParams())}">\n\n'

            گروهه_is_board_specific = (گروهه.GetName() في BOARD_SPECIFIC_GROUPS)

            لكل param في گروهه.GetParams():
                name = param.GetName()

                # Apply note if either the group is board specific
                apply_note_board_specific_گروهه = گروهه_is_board_specific

                # Apply note if name is in list of timer group parameters
                apply_timer_param_note = (name في PARAM_IS_OUTPUT_TIMER)

                short_desc = param.Get_ميدان_قدر("short_desc") أو ''

                # Add fullstop to short_desc if not present
                إذا short_desc:
                    إذا ليس short_desc.strip().endswith('.'):
                        short_desc += "."

                long_desc = param.Get_ميدان_قدر("long_desc") أو ''

                #Strip out short text from start of long text, if it ends in fullstop
                إذا long_desc.startswith(short_desc):
                    long_desc = long_desc[ڊيگهه(short_desc):].lstrip()

                min_val = param.Get_ميدان_قدر("min") أو ''
                max_val = param.Get_ميدان_قدر("max") أو ''
                increment = param.Get_ميدان_قدر("increment") أو ''
                def_val = param.GetDefault() أو ''
                unit = param.Get_ميدان_قدر("unit") أو ''
                type = param.GetType()
                is_boolean = param.GetBoolean()
                reboot_required = param.Get_ميدان_قدر("reboot_required") أو ''
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
                    enum_codes=sorted(enum_codes,ڪنجي=عائم)
                    لكل شي في enum_codes:
                        enum_output+=f"- `{item}`: {param.GetEnumValue(item)}\n"
                    enum_output+='\n\n'

                bitmask_list=param.GetBitmaskList() #Gets bitmask values for parameter
                bitmask_output=''
                #Format bitmask values
                إذا bitmask_list:
                    bitmask_output+='**Bitmask:**\n\n'
                    لكل ٻٽو_انگ في bitmask_list:
                        ٻٽو_انگ_متن = param.Get_Bitmask_ٻٽو_انگ(ٻٽو_انگ)
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
                    boolean_قدرون = '**Values:**\n\n'
                    لكل ڪنجي, label في BOOLEAN_LABELS.شيون_2():
                        boolean_قدرون += f"- `{key}`: {label}\n"
                    boolean_قدرون += '\n'

                نتيجو += '<div class="param">\n\n'
                نتيجو += f'### {name} (`{type}`)' + ' {#' + name + '}\n\n'
                إذا apply_note_board_specific_گروهه:
                    نتيجو += f'<Badge type="warning" text="This parameter is only present on some boards." />\n\n'
                إذا apply_timer_param_note:
                    نتيجو += f'<Badge type="info" text="The exact output numbers where this timer applies are board-specific." />\n\n'
                إذا short_desc:
                    نتيجو += f'{short_desc}\n\n'
                إذا long_desc:
                    نتيجو += f'{long_desc}\n\n'
                إذا is_boolean:
                    نتيجو += boolean_قدرون
                إذا enum_codes:
                    نتيجو += enum_output
                إذا bitmask_list:
                    نتيجو += bitmask_output
                # Format the ranges as a table.
                is_readonly = param.GetReadonly()
                نتيجو += f"Reboot | minValue | maxValue | increment | default | unit | Read-Only\n--- | --- | --- | --- | --- | --- | ---\n{'&check;' if reboot_required else '&nbsp;' } | {min_val} | {max_val} | {increment} | {def_val} | {unit} | {'&check;' if is_readonly else '&nbsp;'}\n\n"
                نتيجو += '</div>\n\n'

            نتيجو += '</div>\n\n'

        self.output = نتيجو

    وصف Save(self, filename):
        سان codecs.open(filename, 'w', 'utf-8') كـ f:
            f.اكتب(self.output)
