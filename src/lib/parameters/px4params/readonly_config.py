"""
Shared helper to load readonly parameter configuration from YAML.
"""
ಆಮದು sys

ಪ್ರಯತ್ನಿಸು:
    ಆಮದು yaml
ಅಪವಾದ ImportError ಇವ e:
    ಮುದ್ರಿಸು("Failed to import yaml: " + str(e))
    ಮುದ್ರಿಸು("")
    ಮುದ್ರಿಸು("You may need to install it using:")
    ಮುದ್ರಿಸು("    pip3 install --user pyyaml")
    ಮುದ್ರಿಸು("")
    sys.exit(1)


ವ್ಯಾಖ್ಯಾನ ತುಂಬು_readonly_params(readonly_config, all_param_ಹೆಸರುಗಳು):
    """
    Load readonly parameter config and return the set of readonly param names.

    @param readonly_config: path to readonly_params.yaml
    @param all_param_names: set of all known parameter names
    @return: set of readonly parameter names
    """
    ಆದರೆ readonly_config ಆಗಿರುವ ಏನೂಇಲ್ಲ:
        ಹಿಂತಿರುಗಿ ಗಣ()

    ಜೊತೆ open(readonly_config, 'r') ಇವ f:
        config = yaml.ಸುರಕ್ಷಿತ_ತುಂಬು(f)

    ಬಹುಲಕ = config.get('mode', 'block')
    listed_params = ಗಣ(config.get('parameters', []))

    # Validate that all listed parameters actually exist
    unknown = listed_params - all_param_ಹೆಸರುಗಳು
    ಆದರೆ unknown:
        ಮುದ್ರಿಸು("Error: readonly_params.yaml lists unknown parameters: %s" % ', '.join(sorted(unknown)))
        sys.exit(1)

    ಆದರೆ ಬಹುಲಕ == 'block':
        # Listed params are read-only
        ಹಿಂತಿರುಗಿ listed_params
    ಅಥವಾಆದರೆ ಬಹುಲಕ == 'allow':
        # Only listed params are writable, all others are read-only
        ಹಿಂತಿರುಗಿ all_param_ಹೆಸರುಗಳು - listed_params
    ಇಲ್ಲದಿದ್ದರೆ:
        ಮುದ್ರಿಸು("Error: readonly_params.yaml has unknown mode '%s' (expected 'block' or 'allow')" % ಬಹುಲಕ)
        sys.exit(1)
