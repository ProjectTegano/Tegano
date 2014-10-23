IMAGES = \
	overview.svg \
	procProvider.svg \
	processing.svg \
	processing2.svg

# Generate graphic files from diagrams
.SUFFIXES: .svg .dia

.dia.svg:
	dia --export=$@ --filter=svg $<


all: $(IMAGES)
