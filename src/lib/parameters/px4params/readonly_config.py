"""
Shared helper to load readonly parameter configuration from YAML.
"""
आनयन sys

यत्न:
    आनयन yaml
अपवाद ImportError इव e:
    मुद्रय("Failed to import yaml: " + str(e))
    मुद्रय("")
    मुद्रय("You may need to install it using:")
    मुद्रय("    pip3 install --user pyyaml")
    मुद्रय("")
    sys.exit(1)


परिभाषा आरोपय_readonly_params(readonly_config, all_param_नामानि):
    """
    Load readonly parameter config and return the set of readonly param names.

    @param readonly_config: path to readonly_params.yaml
    @param all_param_names: set of all known parameter names
    @return: set of readonly parameter names
    """
    यदि readonly_config अस्ति किमपि_न:
        प्रत्यागम समुच्चयः()

    सह open(readonly_config, 'r') इव f:
        config = yaml.safe_आरोपय(f)

    बहुलकम् = config.get('mode', 'block')
    listed_params = समुच्चयः(config.get('parameters', []))

    # Validate that all listed parameters actually exist
    unknown = listed_params - all_param_नामानि
    यदि unknown:
        मुद्रय("Error: readonly_params.yaml lists unknown parameters: %s" % ', '.join(sorted(unknown)))
        sys.exit(1)

    यदि बहुलकम् == 'block':
        # Listed params are read-only
        प्रत्यागम listed_params
    नोचेद्यदि बहुलकम् == 'allow':
        # Only listed params are writable, all others are read-only
        प्रत्यागम all_param_नामानि - listed_params
    अन्यथा:
        मुद्रय("Error: readonly_params.yaml has unknown mode '%s' (expected 'block' or 'allow')" % बहुलकम्)
        sys.exit(1)
