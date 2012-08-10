SNAP=	20120415
REL=	20120426
ARC=	subc-$(SNAP).tgz
DIST=	subc-$(REL).tgz

csums:
	csum -u <_sums >_newsums ; mv -f _newsums _sums

sums:	clean
	find . -type f | grep -v _sums | csum >_sums

clean:
	cd src && make clean
	rm -f $(ARC) $(DIST)

arc:	clean
	tar cvfz $(ARC) *

dist:	clean
	tar cvfz $(DIST) *
