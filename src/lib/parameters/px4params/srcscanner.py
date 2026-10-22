استورد os
استورد re
استورد codecs
استورد sys

صنف سروت_Scanner(object):
    """
    Traverses directory tree, reads all source files, and passes their contents
    to the Parser.
    """

    تعریف جانچ_Dir(self, srcdirs, parser):
        """
        Scans provided path and passes all found contents to the parser using
        parser.Parse method.
        """
        extensions1 = tuple([".h"])
        extensions2 = tuple([".c"])
        لكل srcdir في srcdirs:
            لكل filename في os.listdir(srcdir):
                إذا filename.endswith(extensions1):
                    رستہ = os.رستہ.join(srcdir, filename)
                    إذا ليس self.جانچ_File(رستہ, parser):
                        أرجع خطأ
            لكل filename في os.listdir(srcdir):
                إذا filename.endswith(extensions2):
                    رستہ = os.رستہ.join(srcdir, filename)
                    إذا ليس self.جانچ_File(رستہ, parser):
                        أرجع خطأ
        أرجع سچ

    تعریف جانچ_File(self, رستہ, parser):
        """
        Scans provided file and passes its contents to the parser using
        parser.Parse method.
        """

        نال codecs.open(رستہ, 'r', 'utf-8') كـ f:
            حاول:
                contents = f.اقرأ()
            استثناء:
                contents = ''
                چھاپو('Failed reading file: %s, skipping content.' % رستہ)
                لنگھو
        أرجع parser.Parse(contents)
