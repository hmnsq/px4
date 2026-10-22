আনযন os
আনযন re
আনযন codecs
আনযন sys

শ্ৰেণী SourceScanner(object):
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
        বাবে srcdir অন্তঃ srcdirs:
            বাবে filename অন্তঃ os.listdir(srcdir):
                যদি filename.endswith(extensions1):
                    path = os.path.join(srcdir, filename)
                    যদি নহয় self.ScanFile(path, parser):
                        ঘূৰাই মিছা
            বাবে filename অন্তঃ os.listdir(srcdir):
                যদি filename.endswith(extensions2):
                    path = os.path.join(srcdir, filename)
                    যদি নহয় self.ScanFile(path, parser):
                        ঘূৰাই মিছা
        ঘূৰাই সঁচা

    def ScanFile(self, path, parser):
        """
        Scans provided file and passes its contents to the parser using
        parser.Parse method.
        """

        with codecs.open(path, 'r', 'utf-8') ইৱ f:
            যত্ন:
                contents = f.পঠ()
            অপৱাদ:
                contents = ''
                print('Failed reading file: %s, skipping content.' % path)
                pass
        ঘূৰাই parser.Parse(contents)
