இறக்குமதி os
இறக்குமதி re
இறக்குமதி codecs
இறக்குமதி sys

வகுப்பு SourceScanner(object):
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
        க்கு srcdir உள் srcdirs:
            க்கு filename உள் os.listdir(srcdir):
                எனில் filename.endswith(extensions1):
                    path = os.path.join(srcdir, filename)
                    எனில் இல்லை self.ScanFile(path, parser):
                        திரும்பு பொய்
            க்கு filename உள் os.listdir(srcdir):
                எனில் filename.endswith(extensions2):
                    path = os.path.join(srcdir, filename)
                    எனில் இல்லை self.ScanFile(path, parser):
                        திரும்பு பொய்
        திரும்பு உண்மை

    def ScanFile(self, path, parser):
        """
        Scans provided file and passes its contents to the parser using
        parser.Parse method.
        """

        with codecs.open(path, 'r', 'utf-8') இவ f:
            முயல்:
                contents = f.பட()
            விதிவிலக்கு:
                contents = ''
                print('Failed reading file: %s, skipping content.' % path)
                pass
        திரும்பு parser.Parse(contents)
