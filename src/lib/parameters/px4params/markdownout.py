ਤਃ xml.sax.saxutils ਆਯਾਤ escape
ਆਯਾਤ codecs
ਆਯਾਤ html

ਵਰਗ MarkdownTablesOutput():
    ਪਰਿਭਾਸ਼ਾ __init__(self, groups):
        ਨਤੀਜਾ = (
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

        ਲਈ ਸਮੂਹ ਵਿੱਚ groups:
            ਨਤੀਜਾ += f'## {group.GetName()}\n\n'

            # Wrap each group, and each parameter within it, so the docs theme can skip rendering
            # off-screen content (see .param-group in docs/.vitepress/theme/style.css).
            # The group heading stays outside the wrapper so the page outline can track it.
            ਨਤੀਜਾ += f'<div class="param-group" style="--param-count: {len(group.GetParams())}">\n\n'

            ਸਮੂਹ_is_board_specific = (ਸਮੂਹ.GetName() ਵਿੱਚ BOARD_SPECIFIC_GROUPS)

            ਲਈ param ਵਿੱਚ ਸਮੂਹ.GetParams():
                name = param.GetName()

                # Apply note if either the group is board specific
                apply_note_board_specific_ਸਮੂਹ = ਸਮੂਹ_is_board_specific

                # Apply note if name is in list of timer group parameters
                apply_timer_param_note = (name ਵਿੱਚ PARAM_IS_OUTPUT_TIMER)

                short_desc = param.Get_ਖੇਤਰ_ਮੁੱਲ("short_desc") ਜਾਂ ''

                # Add fullstop to short_desc if not present
                ਜੇ short_desc:
                    ਜੇ ਨਹੀਂ short_desc.strip().endswith('.'):
                        short_desc += "."

                long_desc = param.Get_ਖੇਤਰ_ਮੁੱਲ("long_desc") ਜਾਂ ''

                #Strip out short text from start of long text, if it ends in fullstop
                ਜੇ long_desc.startswith(short_desc):
                    long_desc = long_desc[ਲੰਬਾਈ(short_desc):].lstrip()

                min_val = param.Get_ਖੇਤਰ_ਮੁੱਲ("min") ਜਾਂ ''
                max_val = param.Get_ਖੇਤਰ_ਮੁੱਲ("max") ਜਾਂ ''
                increment = param.Get_ਖੇਤਰ_ਮੁੱਲ("increment") ਜਾਂ ''
                def_val = param.GetDefault() ਜਾਂ ''
                unit = param.Get_ਖੇਤਰ_ਮੁੱਲ("unit") ਜਾਂ ''
                type = param.GetType()
                is_boolean = param.GetBoolean()
                reboot_required = param.Get_ਖੇਤਰ_ਮੁੱਲ("reboot_required") ਜਾਂ ''
                #board = param.GetFieldValue("board") or '' ## Disabled as no board values are defined in any parameters!
                #decimal = param.GetFieldValue("decimal") or '' #Disabled as is intended for GCS not people
                #field_codes = param.GetFieldCodes() ## Disabled as not needed for display.
                #boolean = param.GetFieldValue("boolean") # or '' # Disabled - does not appear useful.

                # Format values for display.
                enum_codes=param.GetEnumCodes() ਜਾਂ '' # Gets numerical values for parameter.
                enum_output=''
                # Format codes and their descriptions for display.
                ਜੇ enum_codes:
                    enum_output+='**Values:**\n\n'
                    enum_codes=sorted(enum_codes,ਕੁੰਜੀ=ਦਸ਼ਮਲਵ)
                    ਲਈ ਵਸਤੂ_2 ਵਿੱਚ enum_codes:
                        enum_output+=f"- `{item}`: {param.GetEnumValue(item)}\n"
                    enum_output+='\n\n'

                bitmask_list=param.GetBitmaskList() #Gets bitmask values for parameter
                bitmask_output=''
                #Format bitmask values
                ਜੇ bitmask_list:
                    bitmask_output+='**Bitmask:**\n\n'
                    ਲਈ ਦੋਆਧਾਰੀ_ਅੰਕ ਵਿੱਚ bitmask_list:
                        ਦੋਆਧਾਰੀ_ਅੰਕ_ਪਾਠ = param.Get_Bitmask_ਦੋਆਧਾਰੀ_ਅੰਕ(ਦੋਆਧਾਰੀ_ਅੰਕ)
                        bitmask_output+=f"- `{bit}`: {bit_text}\n"
                    bitmask_output+='\n\n'

                ਜੇ is_boolean:
                    BOOLEAN_LABELS = {
                        '0': 'Disabled',
                        '1': 'Enabled'
                    }
                    # Give def_value an explanatory string (is def_value if no match)
                    def_val = f"{BOOLEAN_LABELS[def_val]} ({def_val})" ਜੇ def_val ਵਿੱਚ BOOLEAN_LABELS ਨਹੀਂਤਾਂ def_val

                    # Format values and their descriptions for display.
                    boolean_ਮੁੱਲਾਂ = '**Values:**\n\n'
                    ਲਈ ਕੁੰਜੀ, label ਵਿੱਚ BOOLEAN_LABELS.ਚੀਜ਼ਾਂ_2():
                        boolean_ਮੁੱਲਾਂ += f"- `{key}`: {label}\n"
                    boolean_ਮੁੱਲਾਂ += '\n'

                ਨਤੀਜਾ += '<div class="param">\n\n'
                ਨਤੀਜਾ += f'### {name} (`{type}`)' + ' {#' + name + '}\n\n'
                ਜੇ apply_note_board_specific_ਸਮੂਹ:
                    ਨਤੀਜਾ += f'<Badge type="warning" text="This parameter is only present on some boards." />\n\n'
                ਜੇ apply_timer_param_note:
                    ਨਤੀਜਾ += f'<Badge type="info" text="The exact output numbers where this timer applies are board-specific." />\n\n'
                ਜੇ short_desc:
                    ਨਤੀਜਾ += f'{short_desc}\n\n'
                ਜੇ long_desc:
                    ਨਤੀਜਾ += f'{long_desc}\n\n'
                ਜੇ is_boolean:
                    ਨਤੀਜਾ += boolean_ਮੁੱਲਾਂ
                ਜੇ enum_codes:
                    ਨਤੀਜਾ += enum_output
                ਜੇ bitmask_list:
                    ਨਤੀਜਾ += bitmask_output
                # Format the ranges as a table.
                is_readonly = param.GetReadonly()
                ਨਤੀਜਾ += f"Reboot | minValue | maxValue | increment | default | unit | Read-Only\n--- | --- | --- | --- | --- | --- | ---\n{'&check;' if reboot_required else '&nbsp;' } | {min_val} | {max_val} | {increment} | {def_val} | {unit} | {'&check;' if is_readonly else '&nbsp;'}\n\n"
                ਨਤੀਜਾ += '</div>\n\n'

            ਨਤੀਜਾ += '</div>\n\n'

        self.output = ਨਤੀਜਾ

    ਪਰਿਭਾਸ਼ਾ Save(self, filename):
        ਨਾਲ codecs.open(filename, 'w', 'utf-8') ਇਵ f:
            f.ਲਿਖ(self.output)
