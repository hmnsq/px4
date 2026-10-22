"""
Shared helper to load readonly parameter configuration from YAML.
"""
ਆਯਾਤ sys

ਕੋਸ਼ਿਸ਼:
    ਆਯਾਤ yaml
ਅਪਵਾਦ ImportError ਇਵ e:
    ਛਾਪੋ("Failed to import yaml: " + str(e))
    ਛਾਪੋ("")
    ਛਾਪੋ("You may need to install it using:")
    ਛਾਪੋ("    pip3 install --user pyyaml")
    ਛਾਪੋ("")
    sys.exit(1)


ਪਰਿਭਾਸ਼ਾ ਲੱਦੋ_readonly_params(readonly_config, all_param_ਨਾਂ):
    """
    Load readonly parameter config and return the set of readonly param names.

    @param readonly_config: path to readonly_params.yaml
    @param all_param_names: set of all known parameter names
    @return: set of readonly parameter names
    """
    ਜੇ readonly_config ਹੈ ਕੁਝ_ਨਹੀਂ:
        ਵਾਪਸ ਸੈੱਟ()

    ਨਾਲ open(readonly_config, 'r') ਇਵ f:
        config = yaml.ਸੁਰੱਖਿਅਤ_ਲੱਦੋ(f)

    ਬਹੁਲਕ = config.get('mode', 'block')
    listed_params = ਸੈੱਟ(config.get('parameters', []))

    # Validate that all listed parameters actually exist
    unknown = listed_params - all_param_ਨਾਂ
    ਜੇ unknown:
        ਛਾਪੋ("Error: readonly_params.yaml lists unknown parameters: %s" % ', '.join(sorted(unknown)))
        sys.exit(1)

    ਜੇ ਬਹੁਲਕ == 'block':
        # Listed params are read-only
        ਵਾਪਸ listed_params
    ਜਾਂਜੇ ਬਹੁਲਕ == 'allow':
        # Only listed params are writable, all others are read-only
        ਵਾਪਸ all_param_ਨਾਂ - listed_params
    ਨਹੀਂਤਾਂ:
        ਛਾਪੋ("Error: readonly_params.yaml has unknown mode '%s' (expected 'block' or 'allow')" % ਬਹੁਲਕ)
        sys.exit(1)
