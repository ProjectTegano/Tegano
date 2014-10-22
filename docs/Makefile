XSLT_MAN_STYLESHEET = \
	/usr/share/xml/docbook/stylesheet/nwalsh/manpages/docbook.xsl

XSLT_MAN = \
	xsltproc --nonet --xinclude \
	--param man.charmap.use.subset "0" \
	--param make.year.ranges "1" \
	--param make.single.year.ranges "1" \
	$(XSLT_MAN_STYLESHEET)

XSLT_HTML = \
	xsltproc --nonet --xinclude \
	--stringparam draft.mode yes \
	--stringparam toc.section.depth 4 \
	output-html.xsl

XSLT_HTML_CHUNKED = \
	xsltproc --nonet --xinclude \
	--stringparam draft.mode yes \
	--stringparam toc.section.depth 4 \
	-o $@ output-html-chunked.xsl

XSLT_WEBPAGE_HTML = \
	xsltproc --nonet --xinclude \
	--stringparam draft.mode no \
	--stringparam toc.section.depth 4 \
	--stringparam base.dir $(@D) \
	output-html-webpage.xsl

XSLT_WEBPAGE_HTML_CHUNKED = \
	xsltproc --nonet --xinclude \
	--stringparam draft.mode no \
	--stringparam toc.section.depth 4 \
	-o $@ output-html-webpage-chunked.xsl

XSLT_PDF = \
	xsltproc --nonet --xinclude \
	--stringparam draft.mode yes \
	--stringparam toc.section.depth 4 \
	--stringparam fop1.extensions 1 \
	output-pdf.xsl

XSLT_EPUB = \
	xsltproc --nonet --xinclude \
	--stringparam draft.mode yes \
	--stringparam toc.section.depth 4 \
	output-epub.xsl

all: doc man

doc: doc-doxygen doc-html doc-html-chunked doc-pdf doc-txt

MAN_PAGES = \
	wolframed.8 \
	wolframe.conf.5 \
	wolfpasswd.1 \
	wolframe-mod_db_sqlite3.5 \
	wolframe-mod_db_postgresql.5 \

man: $(MAN_PAGES)

doc-doxygen:
	cd ..; doxygen docs/doxygen.conf

doc-doxygen-webpage:
	@-mkdir doxygen/html-webpage
	cd ..; doxygen docs/doxygen-webpage.conf

doc-html: WolfBook.html Tutorial.html

doc-html-chunked: html/index.html html-tutorial/index.html

doc-html-webpage: html-webpage/index.html html-tutorial-webpage/index.html

doc-html-webpage-chunked: html-webpage-chunked/index.html html-tutorial-webpage-chunked/index.html

doc-pdf: WolfBook.pdf Tutorial.pdf

.PHONY: depend
depend:

doc-epub: WolfBook.epub Tutorial.epub

doc-towordml: WolfBook.docx

%.html: %.xml
	$(XSLT_HTML) $< > $@

%.pdf: %.fop
	fop -fo $< -pdf $@

%.fop: %.xml
	$(XSLT_PDF) $< > $@

%.epub: %.xml
	$(XSLT_EPUB) $<

%.txt: %.html
	w3m -O ASCII -dump $< > $@
	@sed 's/Copyright ?/Copyright (C)/g' $@ > _tmp
	@mv -f _tmp $@ 

# WolfBook

WOLFBOOK_SOURCE_FILES = \
	WolfBook.xml \
	WolfBookVersion.txt \
	WolfBookDate.txt \
	WolframeVersion.txt \
	WolfclientVersion.txt \
	AppDevelopment/AAAA.xml \
	AppDevelopment/AAAAConfig.xml \
	AppDevelopment/AppDevelopment.ent \
	AppDevelopment/ApplicationServerRequests.xml \
	AppDevelopment/AuditingAndAccounting.xml \
	AppDevelopment/Authentication.xml \
	AppDevelopment/Authorization.xml \
	AppDevelopment/Appendix.xml \
	AppDevelopment/CmdHandlerDirectmap.xml \
	AppDevelopment/CmdHandlerLua.xml \
	AppDevelopment/CmdHandlers.xml \
	AppDevelopment/ConfigDatabases.xml \
	AppDevelopment/ConfigDatabaseSslOptionTable.xml \
	AppDevelopment/ConfigDatabaseSslModeTable.xml \
	AppDevelopment/ConfigDatabaseSqlite.xml \
	AppDevelopment/ConfigDatabasePostgreSQL.xml \
	AppDevelopment/ConfigGlobalSettings.xml \
	AppDevelopment/ConfigListener.xml \
	AppDevelopment/ConfigLogging.xml \
	AppDevelopment/ConfigModules.xml \
	AppDevelopment/ConfigService.xml \
	AppDevelopment/DataProcessing.xml \
	AppDevelopment/EBNF_Normalizer.xml \
	AppDevelopment/EBNF_TransactionDefinitionLanguage.xml \
	AppDevelopment/FilterJSON.xml \
	AppDevelopment/FilterXML.xml \
	AppDevelopment/FilterXSLT.xml \
	AppDevelopment/Filters.xml \
	AppDevelopment/Forms.xml \
	AppDevelopment/Functions.xml \
	AppDevelopment/Glossary.xml \
	AppDevelopment/Introduction.xml \
	AppDevelopment/ModelTypedFilter.xml \
	AppDevelopment/Preface.xml \
	AppDevelopment/ProcessorConfig.xml \
	AppDevelopment/ProgramCpp.xml \
	AppDevelopment/ProgramDotNet.xml \
	AppDevelopment/ProgramFormDataTypes.xml \
	AppDevelopment/ProgramForms.xml \
	AppDevelopment/ProgramLua.xml \
	AppDevelopment/ProgramPython.xml \
	AppDevelopment/ProgramTransactions.xml \
	AppDevelopment/ServerConfiguration.xml \
	AppDevelopment/README \
	AppDevelopment/Testing.xml \
	Client/CLI.xml \
	Client/Client.ent \
	Client/Introduction.xml \
	Client/Preface.xml \
	Client/Wolfclient.xml \
	Client/WolfclientArchitecture.xml \
	Client/WolfclientArtifacts.xml \
	Client/WolfclientDebugging.xml \
	Client/WolfclientProgrammingInterface.xml \
	Client/WolfclientRequestAnswer.xml \
	Common/Common.ent \
	Common/WolframeLegalNotice.xml \
	Common/gpl.dbk \
	ExtDevelopment/Appendix.xml \
	ExtDevelopment/BasicDataTypes.xml \
	ExtDevelopment/ExtDevelopment.ent \
	ExtDevelopment/Glossary.xml \
	ExtDevelopment/Introduction.xml \
	ExtDevelopment/NormalizerModules.xml \
	ExtDevelopment/Preface.xml \
	examples/AppDevelop/cfg_command_dotnet.conf \
	examples/AppDevelop/cfg_command_lua.conf \
	examples/AppDevelop/cfg_filter_JSON.conf \
	examples/AppDevelop/cfg_filter_libxml2.conf \
	examples/AppDevelop/cfg_filter_textwolf.conf \
	examples/AppDevelop/cfg_normalize_number_string.conf \
	examples/AppDevelop/cfg_program_dotnet.conf \
	examples/AppDevelop/cfg_program_wnmp.conf \
	examples/AppDevelop/cfg_program_xslt.conf \
	examples/AppDevelop/standardCommandHandler.conf

WolfBook.html: $(WOLFBOOK_SOURCE_FILES)

html/index.html: $(WOLFBOOK_SOURCE_FILES)
	$(XSLT_HTML_CHUNKED) $<

html-webpage/index.html: $(WOLFBOOK_SOURCE_FILES)
	$(XSLT_WEBPAGE_HTML) $<

html-webpage-chunked/index.html: $(WOLFBOOK_SOURCE_FILES)
	$(XSLT_WEBPAGE_HTML_CHUNKED) $<

WolfBook.epub: $(WOLFBOOK_SOURCE_FILES)
	mkdir -p OEBPS/images
	cp images/draft.png images/*.svg OEBPS/images
	echo "application/epub+zip" > mimetype
	zip -0Xq  WolfBook.epub mimetype
	zip -Xr9D WolfBook.epub META-INF OEBPS
	rm -rf mimetype META-INF OEBPS

# tutorial

TUTORIAL_SOURCE_FILES = \
	Tutorial.xml \
	TutorialVersion.txt \
	TutorialDate.txt \
	Tutorial/Tutorial.ent \
	Tutorial/Preface.xml \
	Tutorial/Introduction.xml \
	Tutorial/Step1.xml \
	Tutorial/Step2.xml \
	Tutorial/Step3.xml \
	Tutorial/Step4.xml \
	Tutorial/Step5.xml \
	Tutorial/Step6.xml \
	Tutorial/Step7.xml \
	Tutorial/Step8.xml \
	Tutorial/Step9.xml

Tutorial.html: $(TUTORIAL_SOURCE_FILES)
Tutorial.pdf: $(TUTORIAL_SOURCE_FILES)

html-tutorial/index.html: $(TUTORIAL_SOURCE_FILES)
	$(XSLT_HTML_CHUNKED) $<

html-tutorial-webpage/index.html: $(TUTORIAL_SOURCE_FILES)
	$(XSLT_WEBPAGE_HTML) $<

html-tutorial-webpage-chunked/index.html: $(TUTORIAL_SOURCE_FILES)
	$(XSLT_WEBPAGE_HTML_CHUNKED) $<

Tutorial.epub: $(TUTORIAL_SOURCE_FILES)
	mkdir -p OEBPS/images
	cp images/draft.png images/*.svg OEBPS/images
	echo "application/epub+zip" > mimetype
	zip -0Xq  Tutorial.epub mimetype
	zip -Xr9D Tutorial.epub META-INF OEBPS
	rm -rf mimetype META-INF OEBPS

# man pages

%.8: %-man.xml
	$(XSLT_MAN) $<

%.5: %-man.xml
	$(XSLT_MAN) $<

%.1: %-man.xml
	$(XSLT_MAN) $<

clean:
	-rm -rf doxygen/html/* doxygen/man/*  doxygen/perlmod/*
	-rm -f WolfBook.html WolfBook.fop WolfBook.pdf WolfBook.epub WolfBook.docx
	-rm -f Tutorial.html Tutorial.pdf
	-rm -rf html/*.html
	-rm -rf html-webpage/*.html
	-rm -rf html-tutorial/*.html
	-rm -rf html-webpage-chunked/*.html
	-rm -rf html-tutorial-webpage/*.html
	-rm -rf html-tutorial-webpage-chunked/*.html
	-rm -rf doxygen/html-webpage/*
	
check:
	xmllint --noout --xinclude --postvalid WolfBook.xml
	xmllint --noout --xinclude --postvalid Tutorial.xml
	xmllint --noout --xinclude --postvalid wolframe.conf-man.xml
	xmllint --noout --xinclude --postvalid wolframe-mod_db_sqlite3-man.xml
	xmllint --noout --xinclude --postvalid wolframe-mod_db_postgresql-man.xml
	xmllint --noout --xinclude --postvalid wolframed-man.xml
	xmllint --noout --xinclude --postvalid wolfpasswd-man.xml

help:
	@cat help.mk
