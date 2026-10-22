استورد os
استورد re
استورد codecs
استورد sys

מחלקה SourceScanner(object):
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
        עבור srcdir في srcdirs:
            עבור filename في os.listdir(srcdir):
                אם filename.endswith(extensions1):
                    path = os.path.join(srcdir, filename)
                    אם לא self.ScanFile(path, parser):
                        החזר שקר
            עבור filename في os.listdir(srcdir):
                אם filename.endswith(extensions2):
                    path = os.path.join(srcdir, filename)
                    אם לא self.ScanFile(path, parser):
                        החזר שקר
        החזר אמת

    def ScanFile(self, path, parser):
        """
        Scans provided file and passes its contents to the parser using
        parser.Parse method.
        """

        with codecs.open(path, 'r', 'utf-8') كـ f:
            حاول:
                contents = f.اقرأ()
            استثناء:
                contents = ''
                print('Failed reading file: %s, skipping content.' % path)
                pass
        החזר parser.Parse(contents)
