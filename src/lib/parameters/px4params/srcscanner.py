আনযন os
আনযন re
আনযন codecs
আনযন sys

বর্গ SourceScanner(object):
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
        প্রতি srcdir অন্তঃ srcdirs:
            প্রতি filename অন্তঃ os.listdir(srcdir):
                যদি filename.endswith(extensions1):
                    path = os.path.join(srcdir, filename)
                    যদি ন self.ScanFile(path, parser):
                        প্রত্যাগম অসত্য
            প্রতি filename অন্তঃ os.listdir(srcdir):
                যদি filename.endswith(extensions2):
                    path = os.path.join(srcdir, filename)
                    যদি ন self.ScanFile(path, parser):
                        প্রত্যাগম অসত্য
        প্রত্যাগম সত্য

    def ScanFile(self, path, parser):
        """
        Scans provided file and passes its contents to the parser using
        parser.Parse method.
        """

        with codecs.open(path, 'r', 'utf-8') ইব f:
            যত্ন:
                contents = f.পঠ()
            অপবাদ:
                contents = ''
                print('Failed reading file: %s, skipping content.' % path)
                pass
        প্রত্যাগম parser.Parse(contents)
