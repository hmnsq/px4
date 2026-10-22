ಆಮದು os
ಆಮದು re
ಆಮದು codecs
ಆಮದು sys

ವರ್ಗ ಮೂಲ_Scanner(object):
    """
    Traverses directory tree, reads all source files, and passes their contents
    to the Parser.
    """

    ವ್ಯಾಖ್ಯಾನ ಪರಿಶೀಲನೆ_Dir(self, srcdirs, parser):
        """
        Scans provided path and passes all found contents to the parser using
        parser.Parse method.
        """
        extensions1 = tuple([".h"])
        extensions2 = tuple([".c"])
        ಗಾಗಿ srcdir ಒಳಗೆ srcdirs:
            ಗಾಗಿ filename ಒಳಗೆ os.listdir(srcdir):
                ಆದರೆ filename.endswith(extensions1):
                    ಮಾರ್ಗ = os.ಮಾರ್ಗ.join(srcdir, filename)
                    ಆದರೆ ಅಲ್ಲ self.ಪರಿಶೀಲನೆ_File(ಮಾರ್ಗ, parser):
                        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು
            ಗಾಗಿ filename ಒಳಗೆ os.listdir(srcdir):
                ಆದರೆ filename.endswith(extensions2):
                    ಮಾರ್ಗ = os.ಮಾರ್ಗ.join(srcdir, filename)
                    ಆದರೆ ಅಲ್ಲ self.ಪರಿಶೀಲನೆ_File(ಮಾರ್ಗ, parser):
                        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು
        ಹಿಂತಿರುಗಿ ನಿಜ

    ವ್ಯಾಖ್ಯಾನ ಪರಿಶೀಲನೆ_File(self, ಮಾರ್ಗ, parser):
        """
        Scans provided file and passes its contents to the parser using
        parser.Parse method.
        """

        ಜೊತೆ codecs.open(ಮಾರ್ಗ, 'r', 'utf-8') ಇವ f:
            ಪ್ರಯತ್ನಿಸು:
                contents = f.ಪಠ()
            ಅಪವಾದ:
                contents = ''
                ಮುದ್ರಿಸು('Failed reading file: %s, skipping content.' % ಮಾರ್ಗ)
                ದಾಟು
        ಹಿಂತಿರುಗಿ parser.Parse(contents)
