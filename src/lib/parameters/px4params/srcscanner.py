దిగుమతి os
దిగుమతి re
దిగుమతి codecs
దిగుమతి sys

తరగతి ఆధారం_Scanner(object):
    """
    Traverses directory tree, reads all source files, and passes their contents
    to the Parser.
    """

    నిర్వచనం పరిశీలన_Dir(self, srcdirs, parser):
        """
        Scans provided path and passes all found contents to the parser using
        parser.Parse method.
        """
        extensions1 = tuple([".h"])
        extensions2 = tuple([".c"])
        కొరకు srcdir లో srcdirs:
            కొరకు filename లో os.listdir(srcdir):
                ఒకవేళ filename.endswith(extensions1):
                    మార్గం = os.మార్గం.join(srcdir, filename)
                    ఒకవేళ కాదు self.పరిశీలన_File(మార్గం, parser):
                        తిరిగి అబద్ధం
            కొరకు filename లో os.listdir(srcdir):
                ఒకవేళ filename.endswith(extensions2):
                    మార్గం = os.మార్గం.join(srcdir, filename)
                    ఒకవేళ కాదు self.పరిశీలన_File(మార్గం, parser):
                        తిరిగి అబద్ధం
        తిరిగి నిజం

    నిర్వచనం పరిశీలన_File(self, మార్గం, parser):
        """
        Scans provided file and passes its contents to the parser using
        parser.Parse method.
        """

        తోడుగా codecs.open(మార్గం, 'r', 'utf-8') ఇవ f:
            ప్రయత్నించు:
                contents = f.పఠ()
            మినహాయింపు:
                contents = ''
                ముద్రించు('Failed reading file: %s, skipping content.' % మార్గం)
                దాటు
        తిరిగి parser.Parse(contents)
