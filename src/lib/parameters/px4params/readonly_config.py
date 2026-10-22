"""
Shared helper to load readonly parameter configuration from YAML.
"""
استورد sys

حاول:
    استورد yaml
استثناء ImportError كـ e:
    چھاپو("Failed to import yaml: " + str(e))
    چھاپو("")
    چھاپو("You may need to install it using:")
    چھاپو("    pip3 install --user pyyaml")
    چھاپو("")
    sys.exit(1)


تعریف لدو_readonly_params(readonly_config, all_param_ناں):
    """
    Load readonly parameter config and return the set of readonly param names.

    @param readonly_config: path to readonly_params.yaml
    @param all_param_names: set of all known parameter names
    @return: set of readonly parameter names
    """
    إذا readonly_config اے کجھ_نئیں:
        أرجع سیٹ()

    نال open(readonly_config, 'r') كـ f:
        config = yaml.محفوظ_لدو(f)

    منوال = config.get('mode', 'block')
    listed_params = سیٹ(config.get('parameters', []))

    # Validate that all listed parameters actually exist
    unknown = listed_params - all_param_ناں
    إذا unknown:
        چھاپو("Error: readonly_params.yaml lists unknown parameters: %s" % ', '.join(sorted(unknown)))
        sys.exit(1)

    إذا منوال == 'block':
        # Listed params are read-only
        أرجع listed_params
    وإلاإذا منوال == 'allow':
        # Only listed params are writable, all others are read-only
        أرجع all_param_ناں - listed_params
    وإلا:
        چھاپو("Error: readonly_params.yaml has unknown mode '%s' (expected 'block' or 'allow')" % منوال)
        sys.exit(1)
