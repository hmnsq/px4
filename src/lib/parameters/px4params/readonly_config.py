"""
Shared helper to load readonly parameter configuration from YAML.
"""
استورد sys

حاول:
    استورد yaml
استثناء ImportError كـ e:
    چاپ("Failed to import yaml: " + str(e))
    چاپ("")
    چاپ("You may need to install it using:")
    چاپ("    pip3 install --user pyyaml")
    چاپ("")
    sys.exit(1)


تعریف بارول_readonly_params(readonly_config, all_param_نومونه):
    """
    Load readonly parameter config and return the set of readonly param names.

    @param readonly_config: path to readonly_params.yaml
    @param all_param_names: set of all known parameter names
    @return: set of readonly parameter names
    """
    إذا readonly_config دی هیڅ:
        أرجع سېټ()

    سره open(readonly_config, 'r') كـ f:
        config = yaml.خوندي_بارول(f)

    ډېرپېښ = config.get('mode', 'block')
    listed_params = سېټ(config.get('parameters', []))

    # Validate that all listed parameters actually exist
    unknown = listed_params - all_param_نومونه
    إذا unknown:
        چاپ("Error: readonly_params.yaml lists unknown parameters: %s" % ', '.join(sorted(unknown)))
        sys.exit(1)

    إذا ډېرپېښ == 'block':
        # Listed params are read-only
        أرجع listed_params
    وإلاإذا ډېرپېښ == 'allow':
        # Only listed params are writable, all others are read-only
        أرجع all_param_نومونه - listed_params
    وإلا:
        چاپ("Error: readonly_params.yaml has unknown mode '%s' (expected 'block' or 'allow')" % ډېرپېښ)
        sys.exit(1)
