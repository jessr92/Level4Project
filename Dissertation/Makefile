all: 1002536r-Document-Filtering.pdf

1002536r-Document-Filtering.pdf:
	latexmk -pdf -pvc 1002536r-Document-Filtering
	rm -f *.log *.aux *.toc *.out *.bbl *.blg *.fdb_latexmk *.fls

clean:
	latexmk -C
	rm -f 1002536r-Document-Filtering.bbl
