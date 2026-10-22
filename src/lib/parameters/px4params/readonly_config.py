"""
Shared helper to load readonly parameter configuration from YAML.
"""
దిగుమతి sys

ప్రయత్నించు:
    దిగుమతి yaml
మినహాయింపు ImportError ఇవ e:
    ముద్రించు("Failed to import yaml: " + str(e))
    ముద్రించు("")
    ముద్రించు("You may need to install it using:")
    ముద్రించు("    pip3 install --user pyyaml")
    ముద్రించు("")
    sys.exit(1)


నిర్వచనం ఎక్కించు_readonly_params(readonly_config, all_param_పేర్లు):
    """
    Load readonly parameter config and return the set of readonly param names.

    @param readonly_config: path to readonly_params.yaml
    @param all_param_names: set of all known parameter names
    @return: set of readonly parameter names
    """
    ఒకవేళ readonly_config అయిన ఏమీలేదు:
        తిరిగి సమితి()

    తోడుగా open(readonly_config, 'r') ఇవ f:
        config = yaml.సురక్షిత_ఎక్కించు(f)

    బహుళకం = config.get('mode', 'block')
    listed_params = సమితి(config.get('parameters', []))

    # Validate that all listed parameters actually exist
    unknown = listed_params - all_param_పేర్లు
    ఒకవేళ unknown:
        ముద్రించు("Error: readonly_params.yaml lists unknown parameters: %s" % ', '.join(sorted(unknown)))
        sys.exit(1)

    ఒకవేళ బహుళకం == 'block':
        # Listed params are read-only
        తిరిగి listed_params
    లేదావేళ బహుళకం == 'allow':
        # Only listed params are writable, all others are read-only
        తిరిగి all_param_పేర్లు - listed_params
    లేకపోతే:
        ముద్రించు("Error: readonly_params.yaml has unknown mode '%s' (expected 'block' or 'allow')" % బహుళకం)
        sys.exit(1)
