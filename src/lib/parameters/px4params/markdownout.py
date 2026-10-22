ತಃ xml.sax.saxutils ಆಮದು escape
ಆಮದು codecs
ಆಮದು html

ವರ್ಗ MarkdownTablesOutput():
    ವ್ಯಾಖ್ಯಾನ __init__(self, groups):
        ಫಲಿತಾಂಶ = (
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

        ಗಾಗಿ ಗುಂಪು ಒಳಗೆ groups:
            ಫಲಿತಾಂಶ += f'## {group.GetName()}\n\n'

            # Wrap each group, and each parameter within it, so the docs theme can skip rendering
            # off-screen content (see .param-group in docs/.vitepress/theme/style.css).
            # The group heading stays outside the wrapper so the page outline can track it.
            ಫಲಿತಾಂಶ += f'<div class="param-group" style="--param-count: {len(group.GetParams())}">\n\n'

            ಗುಂಪು_is_board_specific = (ಗುಂಪು.GetName() ಒಳಗೆ BOARD_SPECIFIC_GROUPS)

            ಗಾಗಿ param ಒಳಗೆ ಗುಂಪು.GetParams():
                name = param.GetName()

                # Apply note if either the group is board specific
                apply_note_board_specific_ಗುಂಪು = ಗುಂಪು_is_board_specific

                # Apply note if name is in list of timer group parameters
                apply_timer_param_note = (name ಒಳಗೆ PARAM_IS_OUTPUT_TIMER)

                short_desc = param.Get_ಕ್ಷೇತ್ರ_ಮೌಲ್ಯ("short_desc") ಅಥವಾ ''

                # Add fullstop to short_desc if not present
                ಆದರೆ short_desc:
                    ಆದರೆ ಅಲ್ಲ short_desc.strip().endswith('.'):
                        short_desc += "."

                long_desc = param.Get_ಕ್ಷೇತ್ರ_ಮೌಲ್ಯ("long_desc") ಅಥವಾ ''

                #Strip out short text from start of long text, if it ends in fullstop
                ಆದರೆ long_desc.startswith(short_desc):
                    long_desc = long_desc[ಉದ್ದ(short_desc):].lstrip()

                min_val = param.Get_ಕ್ಷೇತ್ರ_ಮೌಲ್ಯ("min") ಅಥವಾ ''
                max_val = param.Get_ಕ್ಷೇತ್ರ_ಮೌಲ್ಯ("max") ಅಥವಾ ''
                increment = param.Get_ಕ್ಷೇತ್ರ_ಮೌಲ್ಯ("increment") ಅಥವಾ ''
                def_val = param.GetDefault() ಅಥವಾ ''
                unit = param.Get_ಕ್ಷೇತ್ರ_ಮೌಲ್ಯ("unit") ಅಥವಾ ''
                type = param.GetType()
                is_boolean = param.GetBoolean()
                reboot_required = param.Get_ಕ್ಷೇತ್ರ_ಮೌಲ್ಯ("reboot_required") ಅಥವಾ ''
                #board = param.GetFieldValue("board") or '' ## Disabled as no board values are defined in any parameters!
                #decimal = param.GetFieldValue("decimal") or '' #Disabled as is intended for GCS not people
                #field_codes = param.GetFieldCodes() ## Disabled as not needed for display.
                #boolean = param.GetFieldValue("boolean") # or '' # Disabled - does not appear useful.

                # Format values for display.
                enum_codes=param.GetEnumCodes() ಅಥವಾ '' # Gets numerical values for parameter.
                enum_output=''
                # Format codes and their descriptions for display.
                ಆದರೆ enum_codes:
                    enum_output+='**Values:**\n\n'
                    enum_codes=sorted(enum_codes,ಕೀಲಿ=ದಶಮಾಂಶ)
                    ಗಾಗಿ ಅಂಶ ಒಳಗೆ enum_codes:
                        enum_output+=f"- `{item}`: {param.GetEnumValue(item)}\n"
                    enum_output+='\n\n'

                bitmask_list=param.GetBitmaskList() #Gets bitmask values for parameter
                bitmask_output=''
                #Format bitmask values
                ಆದರೆ bitmask_list:
                    bitmask_output+='**Bitmask:**\n\n'
                    ಗಾಗಿ ಬಿಟ್ ಒಳಗೆ bitmask_list:
                        ಬಿಟ್_ಪಠ್ಯ = param.Get_Bitmask_ಬಿಟ್(ಬಿಟ್)
                        bitmask_output+=f"- `{bit}`: {bit_text}\n"
                    bitmask_output+='\n\n'

                ಆದರೆ is_boolean:
                    BOOLEAN_LABELS = {
                        '0': 'Disabled',
                        '1': 'Enabled'
                    }
                    # Give def_value an explanatory string (is def_value if no match)
                    def_val = f"{BOOLEAN_LABELS[def_val]} ({def_val})" ಆದರೆ def_val ಒಳಗೆ BOOLEAN_LABELS ಇಲ್ಲದಿದ್ದರೆ def_val

                    # Format values and their descriptions for display.
                    boolean_ಮೌಲ್ಯಗಳು = '**Values:**\n\n'
                    ಗಾಗಿ ಕೀಲಿ, label ಒಳಗೆ BOOLEAN_LABELS.ವಸ್ತುಗಳು_2():
                        boolean_ಮೌಲ್ಯಗಳು += f"- `{key}`: {label}\n"
                    boolean_ಮೌಲ್ಯಗಳು += '\n'

                ಫಲಿತಾಂಶ += '<div class="param">\n\n'
                ಫಲಿತಾಂಶ += f'### {name} (`{type}`)' + ' {#' + name + '}\n\n'
                ಆದರೆ apply_note_board_specific_ಗುಂಪು:
                    ಫಲಿತಾಂಶ += f'<Badge type="warning" text="This parameter is only present on some boards." />\n\n'
                ಆದರೆ apply_timer_param_note:
                    ಫಲಿತಾಂಶ += f'<Badge type="info" text="The exact output numbers where this timer applies are board-specific." />\n\n'
                ಆದರೆ short_desc:
                    ಫಲಿತಾಂಶ += f'{short_desc}\n\n'
                ಆದರೆ long_desc:
                    ಫಲಿತಾಂಶ += f'{long_desc}\n\n'
                ಆದರೆ is_boolean:
                    ಫಲಿತಾಂಶ += boolean_ಮೌಲ್ಯಗಳು
                ಆದರೆ enum_codes:
                    ಫಲಿತಾಂಶ += enum_output
                ಆದರೆ bitmask_list:
                    ಫಲಿತಾಂಶ += bitmask_output
                # Format the ranges as a table.
                is_readonly = param.GetReadonly()
                ಫಲಿತಾಂಶ += f"Reboot | minValue | maxValue | increment | default | unit | Read-Only\n--- | --- | --- | --- | --- | --- | ---\n{'&check;' if reboot_required else '&nbsp;' } | {min_val} | {max_val} | {increment} | {def_val} | {unit} | {'&check;' if is_readonly else '&nbsp;'}\n\n"
                ಫಲಿತಾಂಶ += '</div>\n\n'

            ಫಲಿತಾಂಶ += '</div>\n\n'

        self.output = ಫಲಿತಾಂಶ

    ವ್ಯಾಖ್ಯಾನ Save(self, filename):
        ಜೊತೆ codecs.open(filename, 'w', 'utf-8') ಇವ f:
            f.ಲಿಖ(self.output)
