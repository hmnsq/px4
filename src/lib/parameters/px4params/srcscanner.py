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
        for srcdir anta_h srcdirs:
            for filename anta_h os.listdir(srcdir):
                ᱡᱩᱫᱤ filename.endswith(extensions1):
                    path = os.path.join(srcdir, filename)
                    ᱡᱩᱫᱤ na self.ScanFile(path, parser):
                        ᱨᱩᱣᱟᱹᱲ False
            for filename anta_h os.listdir(srcdir):
                ᱡᱩᱫᱤ filename.endswith(extensions2):
                    path = os.path.join(srcdir, filename)
                    ᱡᱩᱫᱤ na self.ScanFile(path, parser):
                        ᱨᱩᱣᱟᱹᱲ False
        ᱨᱩᱣᱟᱹᱲ True

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
        ᱨᱩᱣᱟᱹᱲ parser.Parse(contents)
