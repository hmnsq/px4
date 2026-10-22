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
        ꦏꦁꦒꦺꦴ srcdir anta_h srcdirs:
            ꦏꦁꦒꦺꦴ filename anta_h os.listdir(srcdir):
                ꦪꦼꦤ꧀ filename.endswith(extensions1):
                    path = os.path.join(srcdir, filename)
                    ꦪꦼꦤ꧀ na self.ScanFile(path, parser):
                        ꦮꦭꦶ False
            ꦏꦁꦒꦺꦴ filename anta_h os.listdir(srcdir):
                ꦪꦼꦤ꧀ filename.endswith(extensions2):
                    path = os.path.join(srcdir, filename)
                    ꦪꦼꦤ꧀ na self.ScanFile(path, parser):
                        ꦮꦭꦶ False
        ꦮꦭꦶ True

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
        ꦮꦭꦶ parser.Parse(contents)
