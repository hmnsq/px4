ಆನಯನ os
ಆನಯನ re
ಆನಯನ codecs
ಆನಯನ sys

ವರ್ಗ SourceScanner(object):
    """
    Traverses directory tree, reads all source files, and passes their contents
    to the Parser.
    """

    def ScanDir(self, srcdirs, parser):
        """
        Scans provided path and passes all found contents to the parser using
        parser.Parse method.
        """
        extensions1 = tuple([".h"])
        extensions2 = tuple([".c"])
        ಪ್ರತಿ srcdir ಅನ್ತಃ srcdirs:
            ಪ್ರತಿ filename ಅನ್ತಃ os.listdir(srcdir):
                ಯದಿ filename.endswith(extensions1):
                    path = os.path.join(srcdir, filename)
                    ಯದಿ ನ self.ScanFile(path, parser):
                        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ
            ಪ್ರತಿ filename ಅನ್ತಃ os.listdir(srcdir):
                ಯದಿ filename.endswith(extensions2):
                    path = os.path.join(srcdir, filename)
                    ಯದಿ ನ self.ScanFile(path, parser):
                        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ

    def ScanFile(self, path, parser):
        """
        Scans provided file and passes its contents to the parser using
        parser.Parse method.
        """

        with codecs.open(path, 'r', 'utf-8') ಇವ f:
            ಯತ್ನ:
                contents = f.ಪಠ()
            ಅಪವಾದ:
                contents = ''
                print('Failed reading file: %s, skipping content.' % path)
                pass
        ಪ್ರತ್ಯಾಗಮ parser.Parse(contents)
