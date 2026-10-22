ਆਯਾਤ os
ਆਯਾਤ re
ਆਯਾਤ codecs
ਆਯਾਤ sys

ਵਰਗ ਸਰੋਤ_Scanner(object):
    """
    Traverses directory tree, reads all source files, and passes their contents
    to the Parser.
    """

    ਪਰਿਭਾਸ਼ਾ ਜਾਂਚ_Dir(self, srcdirs, parser):
        """
        Scans provided path and passes all found contents to the parser using
        parser.Parse method.
        """
        extensions1 = tuple([".h"])
        extensions2 = tuple([".c"])
        ਲਈ srcdir ਵਿੱਚ srcdirs:
            ਲਈ filename ਵਿੱਚ os.listdir(srcdir):
                ਜੇ filename.endswith(extensions1):
                    ਰਸਤਾ = os.ਰਸਤਾ.join(srcdir, filename)
                    ਜੇ ਨਹੀਂ self.ਜਾਂਚ_File(ਰਸਤਾ, parser):
                        ਵਾਪਸ ਝੂਠ
            ਲਈ filename ਵਿੱਚ os.listdir(srcdir):
                ਜੇ filename.endswith(extensions2):
                    ਰਸਤਾ = os.ਰਸਤਾ.join(srcdir, filename)
                    ਜੇ ਨਹੀਂ self.ਜਾਂਚ_File(ਰਸਤਾ, parser):
                        ਵਾਪਸ ਝੂਠ
        ਵਾਪਸ ਸੱਚ

    ਪਰਿਭਾਸ਼ਾ ਜਾਂਚ_File(self, ਰਸਤਾ, parser):
        """
        Scans provided file and passes its contents to the parser using
        parser.Parse method.
        """

        ਨਾਲ codecs.open(ਰਸਤਾ, 'r', 'utf-8') ਇਵ f:
            ਕੋਸ਼ਿਸ਼:
                contents = f.ਪਠ()
            ਅਪਵਾਦ:
                contents = ''
                ਛਾਪੋ('Failed reading file: %s, skipping content.' % ਰਸਤਾ)
                ਲੰਘੋ
        ਵਾਪਸ parser.Parse(contents)
