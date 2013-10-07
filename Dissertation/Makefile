all: 1002536r-Document-Filtering.pdf

1002536r-Document-Filtering.pdf: 1002536r-Document-Filtering.tex 1002536r-Document-Filtering.bib $(wildcard includes/*.tex)
	pdflatex 1002536r-Document-Filtering
	bibtex 1002536r-Document-Filtering
	pdflatex 1002536r-Document-Filtering
	pdflatex 1002536r-Document-Filtering
	rm -f *.log *.aux *.toc *.out
	rm -f *.bbl *.blg

clean:
	rm -f *.log *.aux *.toc *.out
	rm -f *.bbl *.blg
	rm -f 1002536r-Document-Filtering.pdf