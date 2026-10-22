استورد os
استورد re
استورد codecs
استورد sys

صنف SourceScanner(object):
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
        لكل srcdir في srcdirs:
            لكل filename في os.listdir(srcdir):
                𐡄𐡍 filename.endswith(extensions1):
                    path = os.path.join(srcdir, filename)
                    𐡄𐡍 𐡋𐡀 self.ScanFile(path, parser):
                        أرجع خطأ
            لكل filename في os.listdir(srcdir):
                𐡄𐡍 filename.endswith(extensions2):
                    path = os.path.join(srcdir, filename)
                    𐡄𐡍 𐡋𐡀 self.ScanFile(path, parser):
                        أرجع خطأ
        أرجع 𐡒𐡔𐡈

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
        أرجع parser.Parse(contents)
