درآمد os
درآمد re
درآمد codecs
درآمد sys

جماعت ماخذ_Scanner(object):
    """
    Traverses directory tree, reads all source files, and passes their contents
    to the Parser.
    """

    تعریف جائزہ_Dir(self, srcdirs, parser):
        """
        Scans provided path and passes all found contents to the parser using
        parser.Parse method.
        """
        extensions1 = tuple([".h"])
        extensions2 = tuple([".c"])
        برائے srcdir میں srcdirs:
            برائے filename میں os.listdir(srcdir):
                اگر filename.endswith(extensions1):
                    راستہ = os.راستہ.join(srcdir, filename)
                    اگر نہیں self.جائزہ_File(راستہ, parser):
                        واپس جھوٹ
            برائے filename میں os.listdir(srcdir):
                اگر filename.endswith(extensions2):
                    راستہ = os.راستہ.join(srcdir, filename)
                    اگر نہیں self.جائزہ_File(راستہ, parser):
                        واپس جھوٹ
        واپس سچ

    تعریف جائزہ_File(self, راستہ, parser):
        """
        Scans provided file and passes its contents to the parser using
        parser.Parse method.
        """

        ساتھ codecs.open(راستہ, 'r', 'utf-8') بطور f:
            کوشش:
                contents = f.اقرأ()
            استثنا:
                contents = ''
                چھاپو('Failed reading file: %s, skipping content.' % راستہ)
                گزرو
        واپس parser.Parse(contents)
