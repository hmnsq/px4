import os
import re
import codecs
import sys

class SourceScanner(object):
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
        cho srcdir in srcdirs:
            cho filename in os.listdir(srcdir):
                nếu filename.endswith(extensions1):
                    path = os.path.join(srcdir, filename)
                    nếu not self.ScanFile(path, parser):
                        trả_về False
            cho filename in os.listdir(srcdir):
                nếu filename.endswith(extensions2):
                    path = os.path.join(srcdir, filename)
                    nếu not self.ScanFile(path, parser):
                        trả_về False
        trả_về True

    def ScanFile(self, path, parser):
        """
        Scans provided file and passes its contents to the parser using
        parser.Parse method.
        """

        with codecs.open(path, 'r', 'utf-8') as f:
            try:
                contents = f.read()
            except:
                contents = ''
                print('Failed reading file: %s, skipping content.' % path)
                pass
        trả_về parser.Parse(contents)
